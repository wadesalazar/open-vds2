/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#include "OpenVDS/OpenVDS.h"

#include <OpenVDSHandle.h>

#include <VDS/ParseVDSJson.h>

#include <memory>

#include <OpenVDS/VolumeDataAccess.h>

#include "VDS/VolumeDataLayoutImpl.h"
#include "VDS/VolumeDataPageAccessorImpl.h"
#include "VDS/VolumeDataAccessManagerImpl.h"
#include "VDS/VolumeDataRequestProcessor.h"

#include <fmt/format.h>

namespace OpenVDS
{
VDSHandle *open(const OpenOptions &options, Error &error)
{
  error = Error();
  std::unique_ptr<VDSHandle> ret(new VDSHandle(options, error));
  if (error.code)
    return nullptr;

  if (!downloadAndParseVolumeDataLayoutAndLayerStatus(*ret.get(), error))
  {
    return nullptr;
  }
  ret->dataAccessManager.reset(new VolumeDataAccessManagerImpl(*ret.get()));
  ret->requestProcessor.reset(new VolumeDataRequestProcessor(*ret->dataAccessManager.get()));
  return ret.release();
}

VolumeDataLayout *getLayout(VDSHandle *handle)
{
  if (!handle)
    return nullptr;
  return handle->volumeDataLayout.get();
}

VolumeDataAccessManager *getDataAccessManager(VDSHandle *handle)
{
  if (!handle)
    return nullptr;
  return handle->dataAccessManager.get();
}

const char *addDescriptorString(std::string const &descriptorString, VDSHandle &handle)
{
  char *data = new char[descriptorString.size() + 1];
  memcpy(data, descriptorString.data(), descriptorString.size());
  data[descriptorString.size()] = 0;
  handle.descriptorStrings.emplace_back(data);
  return data;
}

static int32_t getInternalCubeSizeLOD0(const VolumeDataLayoutDescriptor &desc)
{
  int32_t size = int32_t(1) << desc.getBrickSize();

  size -= desc.getNegativeMargin();
  size -= desc.getPositiveMargin();

  assert(size > 0);

  return size;
}

static int32_t getLODCount(const VolumeDataLayoutDescriptor &desc)
{
  return desc.getLODLevels() + 1;
}

std::string getLayerName(VolumeDataLayer const &volumeDataLayer)
{
  if(volumeDataLayer.getChannelIndex() == 0)
  {
    return fmt::format("{}LOD{}", DimensionGroupUtil::getDimensionGroupName(volumeDataLayer.getChunkDimensionGroup()), volumeDataLayer.getLOD());
  }
  else
  {
    assert(std::string(volumeDataLayer.getVolumeDataChannelDescriptor().getName()) != "");
    return fmt::format("{}{}LOD{}", volumeDataLayer.getVolumeDataChannelDescriptor().getName(), DimensionGroupUtil::getDimensionGroupName(volumeDataLayer.getPrimaryChannelLayer().getChunkDimensionGroup()), volumeDataLayer.getLOD());
  }
}

MetadataManager *findMetadataManager(LayerMetadataContainer const &layerMetadataContainer, std::string const &layerName)
{
  std::unique_lock<std::mutex> metadataManagersMutexLock(layerMetadataContainer.mutex);
  auto it = layerMetadataContainer.managers.find(layerName);
  return (it != layerMetadataContainer.managers.end()) ? it->second.get() : nullptr;
}

MetadataManager *createMetadataManager(VDSHandle &handle, std::string const &layerName, MetadataStatus const &metadataStatus)
{
  std::unique_lock<std::mutex> metadataManagersMutexLock(handle.layerMetadataContainer.mutex);

  assert(handle.layerMetadataContainer.managers.find(layerName) == handle.layerMetadataContainer.managers.end());
  int pageLimit = handle.axisDescriptors.size() <= 3 ? 64 : 1024;
  return handle.layerMetadataContainer.managers.insert(std::make_pair(layerName, std::unique_ptr<MetadataManager>(new MetadataManager(handle.ioManager.get(), layerName, metadataStatus, pageLimit)))).first->second.get();
}

void createVolumeDataLayout(VDSHandle &handle)
{
  //handle.volumeDataLayout.reset(new VolumeDataLayout(handle.channelDescriptors)
  int32_t dimensionality = int32_t(handle.axisDescriptors.size());

  // Check if input layouts are valid so we can create a new layout
  if (dimensionality < 2)
  {
    handle.volumeDataLayout.reset();
    return;
  }

  handle.volumeDataLayout.reset(
    new VolumeDataLayoutImpl(
      handle,
      handle.layoutDescriptor,
      handle.axisDescriptors,
      handle.channelDescriptors,
      0, //MIA for now
      { 1, 0 }, //MIA for now
      VolumeDataHash::getUniqueHash(),
      CompressionMethod::None,
      0,
      false,
      0));

  for(int32_t dimensionGroupIndex = 0; dimensionGroupIndex < DimensionGroup_3D_Max; dimensionGroupIndex++)
  {
    DimensionGroup dimensionGroup = (DimensionGroup)dimensionGroupIndex;

    int32_t nChunkDimensionality = DimensionGroupUtil::getDimensionality(dimensionGroup);

        // Check if highest dimension in chunk is higher than the highest dimension in the dataset or 1D
    if(DimensionGroupUtil::getDimension(dimensionGroup, nChunkDimensionality - 1) >= dimensionality ||
       nChunkDimensionality == 1)
    {
      continue;
    }

    assert(nChunkDimensionality == 2 || nChunkDimensionality == 3);

    int32_t physicalLODLevels = (nChunkDimensionality == 3 || handle.layoutDescriptor.isCreate2DLODs()) ? getLODCount(handle.layoutDescriptor) : 1;
    int32_t brickSize = getInternalCubeSizeLOD0(handle.layoutDescriptor) * (nChunkDimensionality == 2 ? handle.layoutDescriptor.getBrickSizeMultiplier2D() : 1);

    handle.volumeDataLayout->createLayers(dimensionGroup, brickSize, physicalLODLevels, handle.produceStatuses[DimensionGroupUtil::getDimensionsNDFromDimensionGroup(dimensionGroup)]);
  }
}

VDSHandle* create(const OpenOptions& options, VolumeDataLayoutDescriptor const &layoutDescriptor, std::vector<VolumeDataAxisDescriptor> const &axisDescriptors, std::vector<VolumeDataChannelDescriptor> const &channelDescriptors, MetadataContainer const &metadataContainer, Error &error)
{
  error = Error();
  std::unique_ptr<VDSHandle> handle(new VDSHandle(options, error));

  handle->layoutDescriptor = layoutDescriptor;

  for(auto axisDescriptor : axisDescriptors)
  {
    handle->axisDescriptors.push_back(VolumeDataAxisDescriptor(axisDescriptor.getNumSamples(), addDescriptorString(axisDescriptor.getName(), *handle), addDescriptorString(axisDescriptor.getUnit(), *handle), axisDescriptor.getCoordinateMin(), axisDescriptor.getCoordinateMax()));
  }

  for(auto channelDescriptor : channelDescriptors)
  {
    VolumeDataChannelDescriptor::Flags flags = VolumeDataChannelDescriptor::Default;

    if(channelDescriptor.isDiscrete())                     flags = flags | VolumeDataChannelDescriptor::DiscreteData;
    if(!channelDescriptor.isAllowLossyCompression())       flags = flags | VolumeDataChannelDescriptor::NoLossyCompression;
    if(channelDescriptor.isUseZipForLosslessCompression()) flags = flags | VolumeDataChannelDescriptor::NoLossyCompressionUseZip;
    if(!channelDescriptor.isRenderable())                  flags = flags | VolumeDataChannelDescriptor::NotRenderable;

    if(channelDescriptor.isUseNoValue())
    {
      handle->channelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.getFormat(), channelDescriptor.getComponents(), addDescriptorString(channelDescriptor.getName(), *handle), addDescriptorString(channelDescriptor.getUnit(), *handle), channelDescriptor.getValueRangeMin(), channelDescriptor.getValueRangeMax(), channelDescriptor.getMapping(), channelDescriptor.getMappedValueCount(), flags, channelDescriptor.getNoValue(), channelDescriptor.getIntegerScale(), channelDescriptor.getIntegerOffset()));
    }
    else
    {
      handle->channelDescriptors.push_back(VolumeDataChannelDescriptor(channelDescriptor.getFormat(), channelDescriptor.getComponents(), addDescriptorString(channelDescriptor.getName(), *handle), addDescriptorString(channelDescriptor.getUnit(), *handle), channelDescriptor.getValueRangeMin(), channelDescriptor.getValueRangeMax(), channelDescriptor.getMapping(), channelDescriptor.getMappedValueCount(), flags, channelDescriptor.getIntegerScale(), channelDescriptor.getIntegerOffset()));
    }
  }

  handle->metadataContainer = metadataContainer;

  handle->produceStatuses.clear();
  handle->produceStatuses.resize(int(Dimensions_45) + 1, VolumeDataLayer::ProduceStatus_Unavailable);

  createVolumeDataLayout(*handle);

  if (error.code)
    return nullptr;

  if (!serializeAndUploadVolumeDataLayout(*handle, error))
    return nullptr;

  handle->dataAccessManager.reset(new VolumeDataAccessManagerImpl(*handle.get()));
  handle->requestProcessor.reset(new VolumeDataRequestProcessor(*handle->dataAccessManager.get()));

  return handle.release();
}

void destroy(VDSHandle *handle)
{
  delete handle;
}

}
