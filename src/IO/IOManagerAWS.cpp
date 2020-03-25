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

#include "IOManagerAWS.h"

#include <aws/core/Aws.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetBucketLocationRequest.h>
#include <aws/s3/model/GetBucketLocationResult.h>
#include <aws/s3/model/BucketLocationConstraint.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/core/utils/memory/AWSMemory.h>
#include <aws/core/utils/memory/stl/AWSString.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/sts/STSClient.h>
#include <aws/sts/model/AssumeRoleRequest.h>
#include <mutex>
#include <functional>

#include <fmt/format.h>

namespace OpenVDS
{

  static int initialize_sdk = 0;
  static std::mutex initialize_sdk_mutex;
  static Aws::SDKOptions initialize_sdk_options;

  static Aws::String convertStdString(const std::string &s)
  {
    return Aws::String(s.begin(), s.end());
  }

  static std::string convertAwsString(const Aws::String &s)
  {
    return std::string(s.begin(), s.end());
  }

  static void initializeAWSSDK()
  {

    std::unique_lock<std::mutex> lock(initialize_sdk_mutex);
    initialize_sdk++;
    if (initialize_sdk == 1)
    {

      Aws::Utils::Logging::InitializeAWSLogging(
        Aws::MakeShared<Aws::Utils::Logging::DefaultLogSystem>(
          "OpenVDS-S3 Integration", Aws::Utils::Logging::LogLevel::Trace, "aws_sdk_"));
      Aws::InitAPI(initialize_sdk_options);
    }
  }

  static void deinitializeAWSSDK()
  {
    std::unique_lock<std::mutex> lock(initialize_sdk_mutex);
    initialize_sdk--;
    if (!initialize_sdk)
    {
      Aws::Utils::Logging::ShutdownAWSLogging();
      Aws::ShutdownAPI(initialize_sdk_options);
    }

  }

  template<typename T>
  struct AsyncContext
  {
    AsyncContext(T *back)
      : back(back)
    {}
    T *back;
    std::mutex mutex;
  };

  GetOrHeadRequestAWS::GetOrHeadRequestAWS(const std::string& id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : Request(id)
    , m_handler(handler)
    , m_cancelled(false)
    , m_done(false)
  {
  }

  GetOrHeadRequestAWS::~GetOrHeadRequestAWS()
  {
    GetOrHeadRequestAWS::Cancel();
  }

  void GetOrHeadRequestAWS::WaitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_waitForFinish.wait(lock, [this]{ return m_done; });
  }
  bool GetOrHeadRequestAWS::IsDone() const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }

  bool GetOrHeadRequestAWS::IsSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_done)
    {
      error.code = -1;
      error.string = "Download not done.";
      return false;
    }
    error = m_error;
    return m_error.code == 0;
  }

  void GetOrHeadRequestAWS::Cancel()
  {
    m_cancelled = true;
  }

  static void readobjectinfo_callback(const Aws::S3::S3Client *client, const Aws::S3::Model::HeadObjectRequest& objreq, const Aws::S3::Model::HeadObjectOutcome &getObjectOutcome, std::weak_ptr<ReadObjectInfoRequestAWS> weak_request)
  {
    auto objReq =  weak_request.lock();

    if (!objReq || objReq->m_cancelled)
      return;

    std::unique_lock<std::mutex> lock(objReq->m_mutex, std::defer_lock);
    if (getObjectOutcome.IsSuccess())
    {
      Aws::S3::Model::HeadObjectResult result = const_cast<Aws::S3::Model::HeadObjectOutcome&>(getObjectOutcome).GetResultWithOwnership();

      int64_t content_length = int64_t(result.GetContentLength());
      objReq->m_handler->HandleObjectSize(content_length);
      for (auto it : result.GetMetadata())
      {
        objReq->m_handler->HandleMetadata(convertAwsString(it.first), convertAwsString(it.second));
      }

      lock.lock();
    }
    else
    {
      lock.lock();
      auto s3error = getObjectOutcome.GetError();
      objReq->m_error.code = int(s3error.GetResponseCode());
      objReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
    }

    objReq->m_done = true;
    objReq->m_waitForFinish.notify_all();
    lock.unlock();
    objReq->m_handler->Completed(*objReq, objReq->m_error);
  }

  ReadObjectInfoRequestAWS::ReadObjectInfoRequestAWS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : GetOrHeadRequestAWS(id, handler)
  {

  }

  void ReadObjectInfoRequestAWS::run(Aws::S3::S3Client& client, const std::string& bucket, std::weak_ptr<ReadObjectInfoRequestAWS> readObjectInfoRequest)
  {
    Aws::S3::Model::HeadObjectRequest object_request;
    object_request.SetBucket(convertStdString(bucket));
    object_request.SetKey(convertStdString(GetObjectName()));

    Aws::S3::HeadObjectResponseReceivedHandler bounded_callback = [readObjectInfoRequest](const Aws::S3::S3Client *client, const Aws::S3::Model::HeadObjectRequest &request, const Aws::S3::Model::HeadObjectOutcome &outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&)
    {
      readobjectinfo_callback(client, request, outcome, readObjectInfoRequest);
    };
    client.HeadObjectAsync(object_request, bounded_callback);
  }

  static void download_callback(const Aws::S3::S3Client *client, const Aws::S3::Model::GetObjectRequest& objreq, const Aws::S3::Model::GetObjectOutcome &getObjectOutcome, std::weak_ptr<DownloadRequestAWS> weak_request)
  {
    auto objReq =  weak_request.lock();

    if (!objReq || objReq->m_cancelled)
      return;

    std::unique_lock<std::mutex> lock(objReq->m_mutex, std::defer_lock);
    if (getObjectOutcome.IsSuccess())
    {
      Aws::S3::Model::GetObjectResult result = const_cast<Aws::S3::Model::GetObjectOutcome&>(getObjectOutcome).GetResultWithOwnership();
      int64_t content_length = int64_t(result.GetContentLength());
      objReq->m_handler->HandleObjectSize(content_length);
      for (auto it : result.GetMetadata())
      {
        objReq->m_handler->HandleMetadata(convertAwsString(it.first), convertAwsString(it.second));
      }
      auto& retrieved_object = result.GetBody();
      std::vector<uint8_t> data;

      if (content_length > 0)
      {
        data.resize(content_length);
        retrieved_object.read((char*)&data[0], content_length);
        objReq->m_handler->HandleData(std::move(data));
      }
      lock.lock();
    }
    else
    {
      lock.lock();
      auto s3error = getObjectOutcome.GetError();
      objReq->m_error.code = int(s3error.GetResponseCode());
      objReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
    }

    objReq->m_done = true;
    objReq->m_waitForFinish.notify_all();
    lock.unlock();
    objReq->m_handler->Completed(*objReq, objReq->m_error);
  }

  DownloadRequestAWS::DownloadRequestAWS(const std::string &id, const std::shared_ptr<TransferDownloadHandler>& handler)
    : GetOrHeadRequestAWS(id, handler)
  {
  }
  void DownloadRequestAWS::run(Aws::S3::S3Client& client, const std::string& bucket, const IORange& range, std::weak_ptr<DownloadRequestAWS> downloadRequest)
  {
    Aws::S3::Model::GetObjectRequest object_request;
    object_request.SetBucket(convertStdString(bucket));
    object_request.SetKey(convertStdString(GetObjectName()));
    if (range.end)
    {
      object_request.SetRange(convertStdString(fmt::format("bytes={}-{}", range.start, range.end)));
    }
    Aws::S3::GetObjectResponseReceivedHandler bounded_callback = [downloadRequest](const Aws::S3::S3Client *client, const Aws::S3::Model::GetObjectRequest &request, const Aws::S3::Model::GetObjectOutcome &outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&)
    {
      download_callback(client, request, outcome, downloadRequest);
    };
    client.GetObjectAsync(object_request, bounded_callback);
  }

  static void upload_callback(const Aws::S3::S3Client* client, const Aws::S3::Model::PutObjectRequest&putRequest, const Aws::S3::Model::PutObjectOutcome &outcome, std::weak_ptr<UploadRequestAWS> weak_upload)
  {
    auto objReq =  weak_upload.lock();
    if (!objReq || objReq->m_cancelled)
      return;

    std::unique_lock<std::mutex> lock(objReq->m_mutex);
    if (!outcome.IsSuccess())
    {
      auto s3error = outcome.GetError();
      objReq->m_error.code = int(s3error.GetResponseCode());
      objReq->m_error.string = (s3error.GetExceptionName() + " : " + s3error.GetMessage()).c_str();
    }

    objReq->m_done = true;
    objReq->m_waitForFinish.notify_all();
    Error error = objReq->m_error;
    lock.unlock();
    if (objReq->m_completedCallback)
      objReq->m_completedCallback(*objReq, error);
  }

  UploadRequestAWS::UploadRequestAWS(const std::string& id, std::function<void(const Request & request, const Error & error)> completedCallback)
    : Request(id)
    , m_completedCallback(completedCallback)
    , m_cancelled(false)
    , m_done(false)
  {
  }
 
  void UploadRequestAWS::run(Aws::S3::S3Client& client, const std::string& bucket, const std::string& contentDispostionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::weak_ptr<UploadRequestAWS> uploadRequest)
  {
    m_stream = std::make_shared<IOStream>(data);

    Aws::S3::Model::PutObjectRequest put;
    put.SetBucket(convertStdString(bucket));
    put.SetKey(convertStdString(GetObjectName()));
    put.SetBody(m_stream);
    put.SetContentType(convertStdString(contentType));
    put.SetContentLength(data->size());
    if (contentDispostionFilename.size())
      put.SetContentDisposition(Aws::String("attachment; filename=" + convertStdString(contentDispostionFilename)));
    for (auto &metaPair : metadataHeader)
    {
      put.AddMetadata(convertStdString(metaPair.first), convertStdString(metaPair.second.c_str()));
    }
    
    Aws::S3::PutObjectResponseReceivedHandler bounded_callback = [uploadRequest] (const Aws::S3::S3Client* client, const Aws::S3::Model::PutObjectRequest&putRequest, const Aws::S3::Model::PutObjectOutcome &outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>&) { upload_callback(client, putRequest, outcome, uploadRequest);};
    client.PutObjectAsync(put, bounded_callback);

  }

  void UploadRequestAWS::WaitForFinish()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_waitForFinish.wait(lock, [this]{ return this->m_done; });
  }
  bool UploadRequestAWS::IsDone() const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_done;
  }
  bool UploadRequestAWS::IsSuccess(Error& error) const
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_done)
    {
      error.code = -1;
      error.string = "Download not done.";
      return false;
    }
    error = m_error;
    return m_error.code == 0;
  }
  void UploadRequestAWS::Cancel()
  {
    m_cancelled = true;
  }

  IOManagerAWS::IOManagerAWS(const AWSOpenOptions& openOptions, Error &error)
    : m_region(openOptions.region)
    , m_bucket(openOptions.bucket)
    , m_objectId(openOptions.key)
  {
    if (m_region.empty() || m_bucket.empty())
    {
      error.code = -1;
      error.string = "AWS Config error. Empty bucket or region";
      return;
    }

    if (m_objectId.size() && m_objectId[m_objectId.size() -1] == '/')
      m_objectId.resize(m_objectId.size() - 1);
    initializeAWSSDK();

    Aws::String profileName = "";

    Aws::Auth::AWSCredentials
      credentials;

    // If the default profile uses a role, we need to resolve the role ourselves
    if(profileName.empty() && !Aws::Config::GetCachedConfigProfile(Aws::Auth::GetConfigProfileName()).GetRoleArn().empty())
    {
      profileName = Aws::Auth::GetConfigProfileName();
    }

    // If there is no profile name set we use the default credentials provider chain
    if(profileName.empty())
    {
      Aws::Auth::DefaultAWSCredentialsProviderChain provider;
      credentials = provider.GetAWSCredentials();
    }
    else
    {
      auto profile = Aws::Config::GetCachedConfigProfile(profileName);

      // If the profile is using roles we need to resolve the role ourselves as the AWS C++ SDK doesn't do this correctly
      if(!profile.GetRoleArn().empty())
      {
        auto sourceProfileName = profile.GetSourceProfile();
        Aws::Auth::ProfileConfigFileAWSCredentialsProvider sourceCredentialsProvider(sourceProfileName.c_str());
        auto sourceProfileCredentials = sourceCredentialsProvider.GetAWSCredentials();

        Aws::STS::Model::AssumeRoleRequest request;
        request.SetRoleArn(profile.GetRoleArn());
        request.SetRoleSessionName("OpenVDS");

        Aws::STS::STSClient stsClient(sourceProfileCredentials);
        auto result = stsClient.AssumeRole(request);
        if (result.IsSuccess())
        {
          auto stsCredentials = result.GetResult().GetCredentials();
          credentials = Aws::Auth::AWSCredentials(stsCredentials.GetAccessKeyId(), stsCredentials.GetSecretAccessKey(), stsCredentials.GetSessionToken(), stsCredentials.GetExpiration());
        }
      }
      else
      {
        Aws::Auth::ProfileConfigFileAWSCredentialsProvider credentialsProvider(profileName.c_str());
        credentials = credentialsProvider.GetAWSCredentials();
      }
    }

    Aws::Client::ClientConfiguration clientConfig;
    clientConfig.scheme = Aws::Http::Scheme::HTTPS;
    clientConfig.region = m_region.c_str();
    clientConfig.connectTimeoutMs = 3000;
    clientConfig.requestTimeoutMs = 6000;

    m_s3Client.reset(new Aws::S3::S3Client(credentials, clientConfig));
  }

  IOManagerAWS::~IOManagerAWS()
  {
    deinitializeAWSSDK();
  }

  std::shared_ptr<Request> IOManagerAWS::ReadObjectInfo(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<ReadObjectInfoRequestAWS>(id, handler);
    ret->run(*m_s3Client.get(), m_bucket, ret);
    return ret;
  }

  std::shared_ptr<Request> IOManagerAWS::Download(const std::string &objectName, std::shared_ptr<TransferDownloadHandler> handler, const IORange &range)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<DownloadRequestAWS>(id, handler);
    ret->run(*m_s3Client.get(), m_bucket, range, ret);
    return ret;
  }

  std::shared_ptr<Request> IOManagerAWS::Upload(const std::string &objectName, const std::string& contentDispositionFilename, const std::string& contentType, const std::vector<std::pair<std::string, std::string>>& metadataHeader, std::shared_ptr<std::vector<uint8_t>> data, std::function<void(const Request & request, const Error & error)> completedCallback)
  {
    std::string id = objectName.empty()? m_objectId : m_objectId + "/" + objectName;
    auto ret = std::make_shared<UploadRequestAWS>(id, completedCallback);
    ret->run(*m_s3Client.get(), m_bucket, contentDispositionFilename, contentType, metadataHeader, data, ret);
    return ret;
  }
}
