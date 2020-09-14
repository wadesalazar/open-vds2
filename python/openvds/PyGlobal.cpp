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

#include "PyGlobal.h"

using namespace native;

void 
PyGlobal::initModule(py::module& m)
{
  // These are opaque pointers, so they must not be destructed from pybind11 code
  py::class_<VDS, std::unique_ptr<VDS, py::nodelete>>
    VDS_(m, "VDS");
  VDS_.def("__int__", [](VDS* self) { return (int64_t)self; });
  
  py::class_<IOManager, std::unique_ptr<IOManager, py::nodelete>>(m, "IOManager");

//AUTOGEN-BEGIN
  // OpenOptions
  py::class_<OpenOptions, std::unique_ptr<OpenOptions>> 
    OpenOptions_(m,"OpenOptions", OPENVDS_DOCSTRING(OpenOptions));

  OpenOptions_.def_readwrite("connectionType"              , &OpenOptions::connectionType   , OPENVDS_DOCSTRING(OpenOptions_connectionType));

  py::enum_<OpenOptions::ConnectionType> 
    OpenOptions_ConnectionType_(OpenOptions_,"ConnectionType", OPENVDS_DOCSTRING(OpenOptions_ConnectionType));

  OpenOptions_ConnectionType_.value("AWS"                         , OpenOptions::ConnectionType::AWS        , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AWS));
  OpenOptions_ConnectionType_.value("Azure"                       , OpenOptions::ConnectionType::Azure      , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Azure));
  OpenOptions_ConnectionType_.value("AzurePresigned"              , OpenOptions::ConnectionType::AzurePresigned, OPENVDS_DOCSTRING(OpenOptions_ConnectionType_AzurePresigned));
  OpenOptions_ConnectionType_.value("GoogleStorage"               , OpenOptions::ConnectionType::GoogleStorage, OPENVDS_DOCSTRING(OpenOptions_ConnectionType_GoogleStorage));
  OpenOptions_ConnectionType_.value("Http"                        , OpenOptions::ConnectionType::Http       , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Http));
  OpenOptions_ConnectionType_.value("VDSFile"                     , OpenOptions::ConnectionType::VDSFile    , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_VDSFile));
  OpenOptions_ConnectionType_.value("InMemory"                    , OpenOptions::ConnectionType::InMemory   , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_InMemory));
  OpenOptions_ConnectionType_.value("Other"                       , OpenOptions::ConnectionType::Other      , OPENVDS_DOCSTRING(OpenOptions_ConnectionType_Other));
  OpenOptions_ConnectionType_.value("ConnectionTypeCount"         , OpenOptions::ConnectionType::ConnectionTypeCount, OPENVDS_DOCSTRING(OpenOptions_ConnectionType_ConnectionTypeCount));

  // AWSOpenOptions
  py::class_<AWSOpenOptions, OpenOptions, std::unique_ptr<AWSOpenOptions>> 
    AWSOpenOptions_(m,"AWSOpenOptions", OPENVDS_DOCSTRING(AWSOpenOptions));

  AWSOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions));
  AWSOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &, const std::string &>(), py::arg("bucket").none(false), py::arg("key").none(false), py::arg("region").none(false), py::arg("endpointOverride").none(false), OPENVDS_DOCSTRING(AWSOpenOptions_AWSOpenOptions_2));
  AWSOpenOptions_.def_readwrite("bucket"                      , &AWSOpenOptions::bucket        , OPENVDS_DOCSTRING(AWSOpenOptions_bucket));
  AWSOpenOptions_.def_readwrite("key"                         , &AWSOpenOptions::key           , OPENVDS_DOCSTRING(AWSOpenOptions_key));
  AWSOpenOptions_.def_readwrite("region"                      , &AWSOpenOptions::region        , OPENVDS_DOCSTRING(AWSOpenOptions_region));
  AWSOpenOptions_.def_readwrite("endpointOverride"            , &AWSOpenOptions::endpointOverride, OPENVDS_DOCSTRING(AWSOpenOptions_endpointOverride));
  AWSOpenOptions_.def_readwrite("accessKeyId"                 , &AWSOpenOptions::accessKeyId   , OPENVDS_DOCSTRING(AWSOpenOptions_accessKeyId));
  AWSOpenOptions_.def_readwrite("secretKey"                   , &AWSOpenOptions::secretKey     , OPENVDS_DOCSTRING(AWSOpenOptions_secretKey));
  AWSOpenOptions_.def_readwrite("sessionToken"                , &AWSOpenOptions::sessionToken  , OPENVDS_DOCSTRING(AWSOpenOptions_sessionToken));
  AWSOpenOptions_.def_readwrite("expiration"                  , &AWSOpenOptions::expiration    , OPENVDS_DOCSTRING(AWSOpenOptions_expiration));
  AWSOpenOptions_.def_readwrite("logFilenamePrefix"           , &AWSOpenOptions::logFilenamePrefix, OPENVDS_DOCSTRING(AWSOpenOptions_logFilenamePrefix));
  AWSOpenOptions_.def_readwrite("loglevel"                    , &AWSOpenOptions::loglevel      , OPENVDS_DOCSTRING(AWSOpenOptions_loglevel));

  // AzureOpenOptions
  py::class_<AzureOpenOptions, OpenOptions, std::unique_ptr<AzureOpenOptions>> 
    AzureOpenOptions_(m,"AzureOpenOptions", OPENVDS_DOCSTRING(AzureOpenOptions));

  AzureOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AzureOpenOptions_AzureOpenOptions));
  AzureOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &>(), py::arg("connectionString").none(false), py::arg("container").none(false), py::arg("blob").none(false), OPENVDS_DOCSTRING(AzureOpenOptions_AzureOpenOptions_2));
  AzureOpenOptions_.def(py::init<const std::string &, const std::string &, const std::string &, int &, int &>(), py::arg("connectionString").none(false), py::arg("container").none(false), py::arg("blob").none(false), py::arg("parallelism_factor").none(false), py::arg("max_execution_time").none(false), OPENVDS_DOCSTRING(AzureOpenOptions_AzureOpenOptions_3));
  AzureOpenOptions_.def_readwrite("connectionString"            , &AzureOpenOptions::connectionString, OPENVDS_DOCSTRING(AzureOpenOptions_connectionString));
  AzureOpenOptions_.def_readwrite("container"                   , &AzureOpenOptions::container   , OPENVDS_DOCSTRING(AzureOpenOptions_container));
  AzureOpenOptions_.def_readwrite("blob"                        , &AzureOpenOptions::blob        , OPENVDS_DOCSTRING(AzureOpenOptions_blob));
  AzureOpenOptions_.def_readwrite("parallelism_factor"          , &AzureOpenOptions::parallelism_factor, OPENVDS_DOCSTRING(AzureOpenOptions_parallelism_factor));
  AzureOpenOptions_.def_readwrite("max_execution_time"          , &AzureOpenOptions::max_execution_time, OPENVDS_DOCSTRING(AzureOpenOptions_max_execution_time));

  // AzurePresignedOpenOptions
  py::class_<AzurePresignedOpenOptions, OpenOptions, std::unique_ptr<AzurePresignedOpenOptions>> 
    AzurePresignedOpenOptions_(m,"AzurePresignedOpenOptions", OPENVDS_DOCSTRING(AzurePresignedOpenOptions));

  AzurePresignedOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(AzurePresignedOpenOptions_AzurePresignedOpenOptions));
  AzurePresignedOpenOptions_.def(py::init<const std::string &, const std::string &>(), py::arg("baseUrl").none(false), py::arg("urlSuffix").none(false), OPENVDS_DOCSTRING(AzurePresignedOpenOptions_AzurePresignedOpenOptions_2));
  AzurePresignedOpenOptions_.def_readwrite("baseUrl"                     , &AzurePresignedOpenOptions::baseUrl, OPENVDS_DOCSTRING(AzurePresignedOpenOptions_baseUrl));
  AzurePresignedOpenOptions_.def_readwrite("urlSuffix"                   , &AzurePresignedOpenOptions::urlSuffix, OPENVDS_DOCSTRING(AzurePresignedOpenOptions_urlSuffix));

  // GoogleOpenOptions
  py::class_<GoogleOpenOptions, OpenOptions, std::unique_ptr<GoogleOpenOptions>> 
    GoogleOpenOptions_(m,"GoogleOpenOptions", OPENVDS_DOCSTRING(GoogleOpenOptions));

  GoogleOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(GoogleOpenOptions_GoogleOpenOptions));
  GoogleOpenOptions_.def(py::init<const std::string &, const std::string &>(), py::arg("bucket").none(false), py::arg("pathPrefix").none(false), OPENVDS_DOCSTRING(GoogleOpenOptions_GoogleOpenOptions_2));
  GoogleOpenOptions_.def_readwrite("bucket"                      , &GoogleOpenOptions::bucket     , OPENVDS_DOCSTRING(GoogleOpenOptions_bucket));
  GoogleOpenOptions_.def_readwrite("pathPrefix"                  , &GoogleOpenOptions::pathPrefix , OPENVDS_DOCSTRING(GoogleOpenOptions_pathPrefix));

  // HttpOpenOptions
  py::class_<HttpOpenOptions, OpenOptions, std::unique_ptr<HttpOpenOptions>> 
    HttpOpenOptions_(m,"HttpOpenOptions", OPENVDS_DOCSTRING(HttpOpenOptions));

  HttpOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(HttpOpenOptions_HttpOpenOptions));
  HttpOpenOptions_.def(py::init<const std::string &           >(), py::arg("url").none(false), OPENVDS_DOCSTRING(HttpOpenOptions_HttpOpenOptions_2));
  HttpOpenOptions_.def_readwrite("url"                         , &HttpOpenOptions::url          , OPENVDS_DOCSTRING(HttpOpenOptions_url));

  // InMemoryOpenOptions
  py::class_<InMemoryOpenOptions, OpenOptions, std::unique_ptr<InMemoryOpenOptions>> 
    InMemoryOpenOptions_(m,"InMemoryOpenOptions", OPENVDS_DOCSTRING(InMemoryOpenOptions));

  InMemoryOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(InMemoryOpenOptions_InMemoryOpenOptions));
  InMemoryOpenOptions_.def(py::init<const char *                  >(), py::arg("name").none(false), OPENVDS_DOCSTRING(InMemoryOpenOptions_InMemoryOpenOptions_2));
  InMemoryOpenOptions_.def(py::init<const std::string &           >(), py::arg("name").none(false), OPENVDS_DOCSTRING(InMemoryOpenOptions_InMemoryOpenOptions_3));
  InMemoryOpenOptions_.def_readwrite("name"                        , &InMemoryOpenOptions::name     , OPENVDS_DOCSTRING(InMemoryOpenOptions_name));

  // VDSFileOpenOptions
  py::class_<VDSFileOpenOptions, OpenOptions, std::unique_ptr<VDSFileOpenOptions>> 
    VDSFileOpenOptions_(m,"VDSFileOpenOptions", OPENVDS_DOCSTRING(VDSFileOpenOptions));

  VDSFileOpenOptions_.def(py::init<                              >(), OPENVDS_DOCSTRING(VDSFileOpenOptions_VDSFileOpenOptions));
  VDSFileOpenOptions_.def(py::init<const std::string &           >(), py::arg("fileName").none(false), OPENVDS_DOCSTRING(VDSFileOpenOptions_VDSFileOpenOptions_2));
  VDSFileOpenOptions_.def_readwrite("fileName"                    , &VDSFileOpenOptions::fileName  , OPENVDS_DOCSTRING(VDSFileOpenOptions_fileName));

  // Error
  py::class_<Error, std::unique_ptr<Error>> 
    Error_(m,"Error", OPENVDS_DOCSTRING(Error));

  Error_.def_readwrite("code"                        , &Error::code                   , OPENVDS_DOCSTRING(Error_code));
  Error_.def_readwrite("string"                      , &Error::string                 , OPENVDS_DOCSTRING(Error_string));

  // StringWrapper
  py::class_<StringWrapper, std::unique_ptr<StringWrapper>> 
    StringWrapper_(m,"StringWrapper", OPENVDS_DOCSTRING(StringWrapper));

  StringWrapper_.def(py::init<const std::string &           >(), py::arg("toWrap").none(false), OPENVDS_DOCSTRING(StringWrapper_StringWrapper));
  StringWrapper_.def_readwrite("data"                        , &StringWrapper::data           , OPENVDS_DOCSTRING(StringWrapper_data));
  StringWrapper_.def_readwrite("size"                        , &StringWrapper::size           , OPENVDS_DOCSTRING(StringWrapper_size));

  m.def("createOpenOptions"           , static_cast<native::OpenOptions *(*)(native::StringWrapper, native::StringWrapper, native::Error &)>(&CreateOpenOptions), py::arg("url").none(false), py::arg("connectionString").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(CreateOpenOptions));
  m.def("open"                        , static_cast<native::VDSHandle(*)(native::StringWrapper, native::StringWrapper, native::Error &)>(&Open), py::arg("url").none(false), py::arg("connectionString").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , static_cast<native::VDSHandle(*)(native::StringWrapper, native::Error &)>(&Open), py::arg("url").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Open_2));
  m.def("open"                        , static_cast<native::VDSHandle(*)(const native::OpenOptions &, native::Error &)>(&Open), py::arg("options").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Open_3));
  m.def("open"                        , static_cast<native::VDSHandle(*)(native::IOManager *, native::Error &)>(&Open), py::arg("ioManager").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Open_4));
  m.def("create"                      , static_cast<native::VDSHandle(*)(native::StringWrapper, native::StringWrapper, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), py::arg("url").none(false), py::arg("connectionString").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , static_cast<native::VDSHandle(*)(native::StringWrapper, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), py::arg("url").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Create_2));
  m.def("create"                      , static_cast<native::VDSHandle(*)(const native::OpenOptions &, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), py::arg("options").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Create_3));
  m.def("create"                      , static_cast<native::VDSHandle(*)(native::IOManager *, const native::VolumeDataLayoutDescriptor &, VectorWrapper<native::VolumeDataAxisDescriptor>, VectorWrapper<native::VolumeDataChannelDescriptor>, const native::MetadataReadAccess &, native::Error &)>(&Create), py::arg("ioManager").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Create_4));
  m.def("getLayout"                   , static_cast<native::VolumeDataLayout *(*)(native::VDSHandle)>(&GetLayout), py::arg("handle").none(false), OPENVDS_DOCSTRING(GetLayout));
  m.def("getAccessManager"            , static_cast<native::VolumeDataAccessManager *(*)(native::VDSHandle)>(&GetAccessManager), py::arg("handle").none(false), OPENVDS_DOCSTRING(GetAccessManager));
  m.def("close"                       , static_cast<void(*)(native::VDSHandle)>(&Close), py::arg("handle").none(false), OPENVDS_DOCSTRING(Close));
  m.def("getGlobalState"              , static_cast<native::GlobalState *(*)()>(&GetGlobalState), OPENVDS_DOCSTRING(GetGlobalState));
//AUTOGEN-END
  Error_.def(py::init<>());
  Error_.def("__repr__", [](native::Error const& self){ std::string tmp = std::to_string(self.code); return std::string("Error(code=") + tmp + ", string='" + self.string + "')"; });

  OpenOptions_.def("__repr__", [](OpenOptions const& self)
    {
      std::string conn = "Unknown";
      switch(self.connectionType)
      {
      case OpenOptions::ConnectionType::AWS            : conn = std::string("AWS"            ); break;
      case OpenOptions::ConnectionType::Azure          : conn = std::string("Azure"          ); break;
      case OpenOptions::ConnectionType::AzurePresigned : conn = std::string("AzurePresigned" ); break;
      case OpenOptions::ConnectionType::GoogleStorage  : conn = std::string("GoogleStorage"  ); break;
      case OpenOptions::ConnectionType::Http           : conn = std::string("Http"           ); break;
      case OpenOptions::ConnectionType::VDSFile        : conn = std::string("VDSFile"        ); break;
      case OpenOptions::ConnectionType::InMemory       : conn = std::string("InMemory"       ); break;
      case OpenOptions::ConnectionType::Other          : conn = std::string("Other"          ); break;
      case OpenOptions::ConnectionType::ConnectionTypeCount : conn = std::string("ConnectionTypeCount"); break;
      }
      return std::string("OpenOptions(connectionType='" + conn + "')");
    });

  // Overloads for "open" and "create" that raise an exception when an error occurs
  m.def("open"                        , [](const std::string &url, const std::string &connection, Error &error)
    {
      return native::Open(url, connection, error);
    }, py::arg("url").none(false), py::arg("connectionString").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , [](const std::string &url, const std::string &connection)
    {
      native::Error err;
      auto handle = native::Open(url, connection, err);
      if (err.code)
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("url").none(false), py::arg("connectionString").none(false), OPENVDS_DOCSTRING(Open));
  m.def("open"                        , [](const std::string &url, Error &error)
    {
      return native::Open(url, error);
    }, py::arg("url").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Open_2));
  m.def("open"                        , [](const std::string &url)
    {
      native::Error err;
      auto handle = native::Open(url, err);
      if (err.code)
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("url").none(false), OPENVDS_DOCSTRING(Open_2));
  m.def("open"                        , [](const native::OpenOptions &opt)
    {
      native::Error err;
      auto handle = native::Open(opt, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("options").none(false), OPENVDS_DOCSTRING(Open_3));
  m.def("open"                        , [](native::IOManager *mgr)
    {
      native::Error err;
      auto handle = native::Open(mgr, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("ioManager").none(false), OPENVDS_DOCSTRING(Open_4));
  m.def("create"                      , [](const std::string &url, const std::string &connectionString, const native::VolumeDataLayoutDescriptor &layout, std::vector<native::VolumeDataAxisDescriptor> axisdesc, std::vector<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata, native::Error &error)
    {
      return native::Create(url, connectionString, layout , axisdesc, channeldesc, metadata, error);
    }, py::arg("url").none(false), py::arg("connectionString").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , [](const std::string &url, const std::string &connectionString, const native::VolumeDataLayoutDescriptor &layout, std::vector<native::VolumeDataAxisDescriptor> axisdesc, std::vector<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata)
    {
      native::Error err;
      auto handle = native::Create(url, connectionString, layout , axisdesc, channeldesc, metadata, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("url").none(false), py::arg("connectionString").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), OPENVDS_DOCSTRING(Create));
  m.def("create"                      , [](const std::string &url, const native::VolumeDataLayoutDescriptor &layout, std::vector<native::VolumeDataAxisDescriptor> axisdesc, std::vector<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata, native::Error &error)
    {
      return native::Create(url, layout , axisdesc, channeldesc, metadata, error);
    }, py::arg("url").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), py::arg("error").none(false), OPENVDS_DOCSTRING(Create_2));
  m.def("create"                      , [](const std::string &url, const native::VolumeDataLayoutDescriptor &layout, std::vector<native::VolumeDataAxisDescriptor> axisdesc, std::vector<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata)
    {
      native::Error err;
      auto handle = native::Create(url, layout , axisdesc, channeldesc, metadata, err);
      if (err.code)
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("url").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), OPENVDS_DOCSTRING(Create_2));
  m.def("create"                      , [](const native::OpenOptions &opt, const native::VolumeDataLayoutDescriptor &layout, std::vector<native::VolumeDataAxisDescriptor> axisdesc, std::vector<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata)
    {
      native::Error err;
      auto handle = native::Create(opt, layout , axisdesc, channeldesc, metadata, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("options").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), OPENVDS_DOCSTRING(Create_3));
  m.def("create"                      , [](native::IOManager *mgr, const native::VolumeDataLayoutDescriptor &layout, std::vector<native::VolumeDataAxisDescriptor> axisdesc, std::vector<native::VolumeDataChannelDescriptor> channeldesc, const native::MetadataReadAccess &metadata)
    {
      native::Error err;
      auto handle = native::Create(mgr, layout , axisdesc, channeldesc, metadata, err);
      if (err.code) 
      {
        throw std::runtime_error(err.string);
      }
      return handle;
    }, py::arg("ioManager").none(false), py::arg("layoutDescriptor").none(false), py::arg("axisDescriptors").none(false), py::arg("channelDescriptors").none(false), py::arg("metadata").none(false), OPENVDS_DOCSTRING(Create_4));
}

