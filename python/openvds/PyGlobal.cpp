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
#include "PyGlobal.h"

void 
PyGlobal::initModule(py::module& m)
{
  py::class_<native::OpenOptions> openOptions(m, "OpenOptions");
  openOptions.def_readwrite("connectionType", &native::OpenOptions::connectionType);

  py::enum_<native::OpenOptions::ConnectionType>(openOptions, "ConnectionType")
    .value("AWS", native::OpenOptions::AWS)
    .value("Azure", native::OpenOptions::Azure)
    .value("File", native::OpenOptions::File)
    .export_values();

  py::class_<native::AWSOpenOptions, native::OpenOptions>(m, "AWSOpenOptions")
    .def(py::init<>())
    .def(py::init<const std::string&, const std::string&, const std::string&>())
    .def_readwrite("bucket", &native::AWSOpenOptions::bucket)
    .def_readwrite("key",    &native::AWSOpenOptions::key)
    .def_readwrite("region", &native::AWSOpenOptions::region);

  py::class_<native::Error> error(m, "Error");
  error
    .def(py::init<>())
    .def_readwrite("code", &native::Error::code)
    .def_readwrite("string", &native::Error::string);
  error.def("__repr__", [](native::Error const& self){ char tmp[256]; _itoa_s(self.code, tmp, 16); return std::string("Error(code=") + tmp + ", string='" + self.string + "')"; });

  enum class AllAccess
  {
    None      = 0,
    Read      = (int)native::Access::Read,
    Write     = (int)native::Access::Write,
    ReadWrite = (int)native::Access::Read | (int)native::Access::Write
  };

  py::enum_<AllAccess> access(m, "Access", py::arithmetic());
  access.def("__or__",[](AllAccess const& lhs, AllAccess const& rhs){ return AllAccess((int)lhs | (int)rhs); });
  access
    .value("NONE",      AllAccess::None)
    .value("Read",      AllAccess::Read)
    .value("Write",     AllAccess::Write)
    .value("ReadWrite", AllAccess::ReadWrite);

  py::class_<native::VDSHandle>(m, "VDSHandle");

  m.def("open", native::open);
}

