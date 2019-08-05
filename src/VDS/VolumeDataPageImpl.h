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

#ifndef VOLUMEDATAPAGEIMPL_H
#define VOLUMEDATAPAGEIMPL_H

#include <OpenVDS/VolumeDataAccess.h>

#include <mutex>
#include <vector>

namespace OpenVDS
{
class VolumeDataLayer;
class VolumeDataPageAccessorImpl;
class VolumeDataPageImpl : public VolumeDataPage
{
private:
  const VolumeDataPageAccessor * m_volumeDataPageAccessor;

  int64_t m_chunk;

  std::vector<uint8_t> m_blob;

  int32_t _nPins;

  void *m_buffer;

  bool _isDirty;

  int32_t m_pitch[Dimensionality_Max];

  int32_t m_writtenMin[Dimensionality_Max];
  int32_t m_writtenMax[Dimensionality_Max];

  int64_t m_copiedToChunkIndexes[26];

  int32_t m_chunksCopiedTo;

public:
  VolumeDataPageImpl(VolumeDataPageImpl const &) = delete;

  VolumeDataPageImpl(VolumeDataPageAccessorImpl *volumeDataPageAccessor, int64_t chunk);
  ~VolumeDataPageImpl();

  int64_t getChunkIndex() const { return m_chunk; }

  // All these methods require the caller to hold a lock
  bool          isPinned();
  void          pin();
  void          unPin();

  bool          isEmpty();
  bool          isDirty();
  bool          isWritten();
  void          makeDirty();

  void          setBufferData(std::vector<uint8_t> &&blob, const int (&pitch)[Dimensionality_Max]);
  void          writeBack(VolumeDataLayer *volumeDataLayer, std::unique_lock<std::mutex> &pageListMutexLock);
  void *        getBufferInternal(int (&anPitch)[Dimensionality_Max], bool isReadWrite);
  bool          isCopyMarginNeeded(VolumeDataPage *targetPage);
  void          copyMargin(VolumeDataPage *targetPage);

  // Implementation of Hue::HueSpaceLib::VolumeDataPage interface, these methods aquire a lock (except the GetMinMax methods which don't need to)
  void  getMinMax(int (&min)[Dimensionality_Max], int (&max)[Dimensionality_Max]) const override;
  void  getMinMaxExcludingMargin(int (&minExcludingMargin)[Dimensionality_Max], int (&maxExcludingMargin)[Dimensionality_Max]) const override;
  const void *getBuffer(int (&pitch)[Dimensionality_Max]) override; // Getting the buffer will block if the page is currently being read from the VolumeDataCache
  void *getWritableBuffer(int (&pitch)[Dimensionality_Max]) override;
  void  updateWrittenRegion(const int (&writtenMin)[Dimensionality_Max], const int (&writtenMax)[Dimensionality_Max]) override;
  void  release() override;
};
}
#endif //VOLUMEDATAPAGEIMPL_H