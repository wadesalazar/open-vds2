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

#ifndef VOLUMEDATASTORE_H
#define VOLUMEDATASOTRE_H

#include <OpenVDS/VolumeDataChannelDescriptor.h>
#include <OpenVDS/VolumeData.h>
#include <OpenVDS/OpenVDS.h>

#include "MetadataManager.h"
#include "DataBlock.h"

#include <vector>

namespace OpenVDS
{

class WaveletAdaptiveLevelsMetadata
{
public:
  uint8_t m_levels[MetadataStatus::WAVELET_ADAPTIVE_LEVELS];

  //void          encode(int32_t totalSize, int32_t const (&adaptiveLevels)[MetadataStatus::WAVELET_ADAPTIVE_LEVELS]);
  //int32_t       decode(int32_t totalSize, int32_t targetLevel) const;
  //void          accumulateSizes(int32_t totalSize, int64_t (&adaptiveLevelSizes)[MetadataStatus::WAVELET_ADAPTIVE_LEVELS], bool isSubtract) const;
  //void          dropLosslessData();
  //void          shift(int32_t nLevels);
  //void          clear() { memset(m_levels, 0x00, sizeof(m_levels)); }
};

struct VolumeDataStore
{
  static bool verify(const VolumeDataChunk& volumeDataChunk, const std::vector<uint8_t>& serializedData, CompressionMethod compressionMethod, bool isFullyRead);
  static bool deserializeVolumeData(const VolumeDataChunk& chunk, const std::vector<uint8_t>& serializedData, const std::vector<uint8_t>& metadata, CompressionMethod compressionMethod, int32_t adaptiveLevel, VolumeDataChannelDescriptor::Format loadFormat, std::vector<uint8_t>& target, Error& error);
};

}

#endif //VOLUMEDATASTORE_H