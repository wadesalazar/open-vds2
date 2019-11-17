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

#include "PyVolumeDataLayoutDescriptor.h"

void 
PyVolumeDataLayoutDescriptor::initModule(py::module& m)
{
#if 0
//AUTOGEN-BEGIN
  // VolumeDataLayoutDescriptor
  py::class_<native::VolumeDataLayoutDescriptor> 
    VolumeDataLayoutDescriptor_(m,"VolumeDataLayoutDescriptor", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor));

  VolumeDataLayoutDescriptor_.def(py::init<                              >(), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_VolumeDataLayoutDescriptor));
  VolumeDataLayoutDescriptor_.def(py::init<OpenVDS::VolumeDataLayoutDescriptor::BrickSize, int, int, int, OpenVDS::VolumeDataLayoutDescriptor::LODLevels, OpenVDS::VolumeDataLayoutDescriptor::Options, int>(), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_VolumeDataLayoutDescriptor_2));
  VolumeDataLayoutDescriptor_.def("isValid"                     , static_cast<bool(*)()>(&native::IsValid), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_IsValid));
  VolumeDataLayoutDescriptor_.def("getBrickSize"                , static_cast<OpenVDS::VolumeDataLayoutDescriptor::BrickSize(*)()>(&native::GetBrickSize), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetBrickSize));
  VolumeDataLayoutDescriptor_.def("getNegativeMargin"           , static_cast<int(*)()>(&native::GetNegativeMargin), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetNegativeMargin));
  VolumeDataLayoutDescriptor_.def("getPositiveMargin"           , static_cast<int(*)()>(&native::GetPositiveMargin), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetPositiveMargin));
  VolumeDataLayoutDescriptor_.def("getBrickSizeMultiplier2D"    , static_cast<int(*)()>(&native::GetBrickSizeMultiplier2D), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetBrickSizeMultiplier2D));
  VolumeDataLayoutDescriptor_.def("getLODLevels"                , static_cast<OpenVDS::VolumeDataLayoutDescriptor::LODLevels(*)()>(&native::GetLODLevels), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetLODLevels));
  VolumeDataLayoutDescriptor_.def("isCreate2DLODs"              , static_cast<bool(*)()>(&native::IsCreate2DLODs), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_IsCreate2DLODs));
  VolumeDataLayoutDescriptor_.def("isForceFullResolutionDimension", static_cast<bool(*)()>(&native::IsForceFullResolutionDimension), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_IsForceFullResolutionDimension));
  VolumeDataLayoutDescriptor_.def("getFullResolutionDimension"  , static_cast<int(*)()>(&native::GetFullResolutionDimension), OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_GetFullResolutionDimension));

  py::enum_<native::VolumeDataLayoutDescriptor::BrickSize> 
    VolumeDataLayoutDescriptor_BrickSize_(VolumeDataLayoutDescriptor_,"BrickSize", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize));

  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_32"                , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_32, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_32));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_64"                , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_64, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_64));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_128"               , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_128, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_128));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_256"               , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_256, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_256));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_512"               , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_512, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_512));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_1024"              , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_1024, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_1024));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_2048"              , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_2048, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_2048));
  VolumeDataLayoutDescriptor_BrickSize_.value("BrickSize_4096"              , native::VolumeDataLayoutDescriptor::BrickSize::BrickSize_4096, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_BrickSize_BrickSize_4096));

  py::enum_<native::VolumeDataLayoutDescriptor::LODLevels> 
    VolumeDataLayoutDescriptor_LODLevels_(VolumeDataLayoutDescriptor_,"LODLevels", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels));

  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_None"              , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_None, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_None));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_1"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_1, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_1));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_2"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_2, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_2));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_3"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_3, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_3));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_4"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_4, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_4));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_5"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_5, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_5));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_6"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_6, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_6));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_7"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_7, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_7));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_8"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_8, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_8));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_9"                 , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_9, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_9));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_10"                , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_10, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_10));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_11"                , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_11, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_11));
  VolumeDataLayoutDescriptor_LODLevels_.value("LODLevels_12"                , native::VolumeDataLayoutDescriptor::LODLevels::LODLevels_12, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_LODLevels_LODLevels_12));

  py::enum_<native::VolumeDataLayoutDescriptor::Options> 
    VolumeDataLayoutDescriptor_Options_(VolumeDataLayoutDescriptor_,"Options", OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options));

  VolumeDataLayoutDescriptor_Options_.value("Options_None"                , native::VolumeDataLayoutDescriptor::Options::Options_None, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options_Options_None));
  VolumeDataLayoutDescriptor_Options_.value("Options_Create2DLODs"        , native::VolumeDataLayoutDescriptor::Options::Options_Create2DLODs, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options_Options_Create2DLODs));
  VolumeDataLayoutDescriptor_Options_.value("Options_ForceFullResolutionDimension", native::VolumeDataLayoutDescriptor::Options::Options_ForceFullResolutionDimension, OPENVDS_DOCSTRING(VolumeDataLayoutDescriptor_Options_Options_ForceFullResolutionDimension));

  m.def("operator|"                   , static_cast<VolumeDataLayoutDescriptor::Options(*)(VolumeDataLayoutDescriptor::Options, VolumeDataLayoutDescriptor::Options)>(&native::operator|), OPENVDS_DOCSTRING(operator|));
//AUTOGEN-END
#endif
}

