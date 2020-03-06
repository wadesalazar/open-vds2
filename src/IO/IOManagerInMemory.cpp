#include "IOManagerInMemory.h"

#include <fmt/format.h>

namespace OpenVDS
{

class InMemoryRequest : public Request
{
public:
  InMemoryRequest(const std::string &objectName)
    : Request(objectName)
    , m_done(false)
  {

  }
  ~InMemoryRequest() override
  {

  }
  void WaitForFinish() override
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_wait.wait(lock, [this] { return m_done; });
  }
  bool IsDone() const override
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }
  bool IsSuccess(Error &error) const override
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    error = m_error;
    return m_error.code == 0;
  }
  void Cancel() override
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_done = true;
    m_wait.notify_all();
  }


  bool m_done;
  std::condition_variable m_wait;
  mutable std::mutex m_mutex;
  Error m_error;
};

IOManagerInMemory::IOManagerInMemory(const InMemoryOpenOptions &openOptions, Error &error)
  : m_threadPool(std::thread::hardware_concurrency())
{

}

IOManagerInMemory::~IOManagerInMemory()
{

}

HeadInfo IOManagerInMemory::Head(const std::string &objectName, Error &error, const IORange& range)
{
  std::unique_lock<std::mutex> lock(m_mutex);
  auto it = m_data.find(objectName);
  HeadInfo ret;
  if (it == m_data.end())
  {
    error.code = -2;
    error.string = fmt::format("Object {} not found\n", objectName);
    ret.contentLength = 0;
    return ret;
  }
  ret.contentLength = it->second.data.size();
  return ret;
}

std::shared_ptr<OpenVDS::Request> IOManagerInMemory::Download(const std::string objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
{
  auto request = std::make_shared<InMemoryRequest>(objectName);
  m_threadPool.Enqueue([this, objectName, handler, request]
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      auto it = m_data.find(objectName);
      Error error;
      if (it != m_data.end())
      {
        auto object = it->second;
        lock.unlock(); 
        for (auto& meta : object.metaHeader)
        {
          handler->HandleMetadata(meta.first, meta.second);
        }
        handler->HandleData(std::move(object.data));
      }
      else
      {
        lock.unlock();
        error.string = std::string("Object: ") + objectName + " not found.";
        error.code = 404;
      }

      {
        std::unique_lock<std::mutex> request_lock(request->m_mutex);
        request->m_done = true;
        request->m_error = error;
      }

      handler->Completed(*request, error); 

      request->m_wait.notify_all();
    });
  return request;
}

std::shared_ptr<Request> IOManagerInMemory::Upload(const std::string objectName, const std::string &contentDispostionFilename, const std::string &contentType, const std::vector<std::pair<std::string, std::string> > &metadataHeader, std::shared_ptr<std::vector<uint8_t> > data, std::function<void (const Request &, const Error &)> completedCallback)
{
  auto request = std::make_shared<InMemoryRequest>(objectName);
  m_threadPool.Enqueue([objectName, metadataHeader, data, completedCallback, request, this]
    {
      Object object;
      object.metaHeader = metadataHeader;
      object.data = *data;
      std::unique_lock<std::mutex> lock(m_mutex);
      m_data[objectName] = std::move(object);
      lock.unlock();
     
      Error error;
      {
        std::unique_lock<std::mutex> request_lock(request->m_mutex);
        request->m_done = true;
        request->m_error = error;
      }

      if (completedCallback)
        completedCallback(*request, error);

      request->m_wait.notify_all();
    });
  return request;
}

}
