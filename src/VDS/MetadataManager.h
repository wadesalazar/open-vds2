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

#ifndef METADATAMANAGER_H
#define METADATAMANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <list>


#include <OpenVDS/VolumeData.h>

#include "VolumeDataChunk.h"


namespace OpenVDS
{
  struct MetadataStatus
  {
    enum
    {
      WAVELET_ADAPTIVE_LEVELS = 16
    };

    int                 m_chunkIndexCount;
    int                 m_chunkMetadataPageSize;
    int                 m_chunkMetadataByteSize;
    float               m_compressionTolerance;
    CompressionMethod   m_compressionMethod;
    int64_t             m_uncompressedSize;
    int64_t             m_adaptiveLevelSizes[WAVELET_ADAPTIVE_LEVELS];
  };

  class ObjectRequester;
  class MetadataManager;
  class MetadataPage
  {
    friend MetadataManager;

    MetadataManager *m_manager;

    int m_pageIndex;
    bool m_valid;
    int m_lockCount;

    std::shared_ptr<ObjectRequester> m_activeTransfer;

    std::vector<uint8_t> m_data;
  public:
    MetadataManager *GetManager() { return m_manager; }
    int PageIndex() const { return m_pageIndex; }
    bool IsValid()   const { return m_valid; }

    MetadataPage(MetadataManager *manager, int pageIndex)
      : m_manager(manager)
      , m_pageIndex(pageIndex)
      , m_valid(false)
      , m_lockCount(0)
      , m_activeTransfer(nullptr)
    {}
  };
  typedef std::list<MetadataPage> MetadataPageList;

  class IOManager;
  class MetadataManager
  {
    IOManager *m_iomanager;
    std::string m_layerUrl;

    MetadataStatus m_metadataStatus;

    std::mutex m_mutex;

    int m_pageLimit;

    typedef std::unordered_map<int, MetadataPageList::iterator> MetadataPageMap;

    MetadataPageMap m_pageMap;
    MetadataPageList m_pageList;

    void limitPages();
  public:
    MetadataManager(IOManager *iomanager, std::string const &layerURL, MetadataStatus const &metadataStatus, int pageLimit);
    ~MetadataManager();

    const char *layerUrl() const { return m_layerUrl.c_str(); }
    const std::string &layerUrlStr() const { return m_layerUrl; }

    MetadataPage *lockPage(int pageIndex, bool *initiateTransfer);

    void pageTransferError(MetadataPage *page, const char *msg);

    void pageTransferCompleted(MetadataPage *page);

    void initiateTransfer(MetadataPage *page, std::string const &url, bool verbose, const std::vector<std::string>& headers);

    uint8_t const *GetPageEntry(MetadataPage *page, int entry) const;

    void unlockPage(MetadataPage *page);

    MetadataStatus const &metadataStatus() const { return m_metadataStatus; }
  };
}

#endif //METADATAMANAGER_H
