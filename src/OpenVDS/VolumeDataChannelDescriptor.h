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

#ifndef VOLUMEDATACHANNELDESCRIPTOR_H
#define VOLUMEDATACHANNELDESCRIPTOR_H

#include <string>
#include <Math/Range.h>

#include <OpenVDS/VolumeData.h>

namespace OpenVDS
{
/// \class VolumeDataChannelDescriptor
/// \brief Describes a channel of a VDS
class VolumeDataChannelDescriptor
{
public:
  /// Flags for this channel
  enum Flags
  {
    Default = 0,
    DiscreteData = (1 << 0),                                      ///< This channel contains discrete data
    NoLossyCompression = (1 << 1),                                ///< Do not allow lossy compression on this channel
    NotRenderable = (1 << 2),                                     ///< This channel is not renderable
    NoLossyCompressionUseZip = NoLossyCompression | (1 << 3),     ///< Use Zip when compressing this channel
  };
  
  enum Format
  {
    FormatAny = -1, ///< The DataBlock can be in any format
    Format1Bit,     ///< data is in packed 1-bit format
    FormatU8,       ///< data is in unsigned 8 bit
    FormatU16,      ///< data is in unsigned 16 bit
    FormatR32,      ///< data is in 32 bit float
    FormatU32,      ///< data is in unsigned 32 bit
    FormatR64,      ///< data is in 64 bit double
    FormatU64       ///< data is in unsigned 64 bit
  };

  enum Components
  {
    Components_1 = 1,
    Components_2 = 2,
    Components_4 = 4,
    ComponentsMax = Components_4
  };


private:
  Format m_format;
  Components m_components;
  const char *m_name;
  const char *m_unit;
  FloatRange m_valueRange;

  VolumeDataMapping m_mapping;
  int         m_mappedValueCount;

  Flags       m_flags;

  bool        m_useNoValue;
  float       m_noValue;

  float       m_integerScale;
  float       m_integerOffset;

public:
  VolumeDataChannelDescriptor()
    : m_format(FormatAny), m_components(Components_1), m_name(nullptr), m_unit(nullptr), m_valueRange({0.0f,0.0f}), m_mapping(VolumeDataMapping::Direct), m_mappedValueCount(1), m_flags(Default), m_useNoValue(false), m_noValue(0.0f), m_integerScale(1.0f), m_integerOffset(0.0f) {}

  /// The minimum constructor for a VolumeDataChannelDescriptor. This will use direct mapping, default flags, and no No Value
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(VolumeDataMapping::Direct), m_mappedValueCount(1), m_flags(Default), m_useNoValue(false), m_noValue(0.0f), m_integerScale(1.0f), m_integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(mapping), m_mappedValueCount(1), m_flags(Default), m_useNoValue(false), m_noValue(0.0f), m_integerScale(1.0f), m_integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param flags the flags for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum Flags flags)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(VolumeDataMapping::Direct), m_mappedValueCount(1), m_flags(flags), m_useNoValue(false), m_noValue(0.0f), m_integerScale(1.0f), m_integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  /// \param flags the flags for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping, enum Flags flags)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(mapping), m_mappedValueCount(1), m_flags(flags), m_useNoValue(false), m_noValue(0.0f), m_integerScale(1.0f), m_integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \param integerScale the scale to use for integer types
  /// \param integerOffset the offset to use for integer types
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping, int mappedValueCount, enum Flags flags, float integerScale, float integerOffset)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(mapping), m_mappedValueCount(mappedValueCount), m_flags(flags), m_useNoValue(false), m_noValue(0.0f), m_integerScale(integerScale), m_integerOffset(integerOffset) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param noValue the No Value for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, float noValue)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(VolumeDataMapping::Direct), m_mappedValueCount(1), m_flags(Default), m_useNoValue(true), m_noValue(noValue), m_integerScale(1.0f), m_integerOffset(0.0f) {}

  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param noValue the No Value for this channel
  /// \param mapping the mapping for this channel
  /// \param flags the flags for this channel
  VolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, float noValue, enum VolumeDataMapping mapping, enum Flags flags)
    : m_format(format), m_components(components), m_name(pName), m_unit(pUnit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(mapping), m_mappedValueCount(1), m_flags(flags), m_useNoValue(true), m_noValue(noValue), m_integerScale(1.0f), m_integerOffset(0.0f) {}
  
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mapping the mapping for this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \param noValue the No Value for this channel
  /// \param integerScale the scale to use for integer types
  /// \param integerOffset the offset to use for integer types
  VolumeDataChannelDescriptor(Format format, Components components, const char *name, const char *unit, float valueRangeMin, float valueRangeMax, enum VolumeDataMapping mapping, int mappedValueCount, enum Flags flags, float noValue, float integerScale, float integerOffset)
    : m_format(format), m_components(components), m_name(name), m_unit(unit), m_valueRange({valueRangeMin, valueRangeMax}), m_mapping(mapping), m_mappedValueCount(mappedValueCount), m_flags(flags), m_useNoValue(true), m_noValue(noValue), m_integerScale(integerScale), m_integerOffset(integerOffset) {}

  Format      getFormat()                       const { return m_format; }
  Components  getComponents()                   const { return m_components; }
  bool        isDiscrete()                      const { return (m_flags & DiscreteData) || m_format == Format1Bit; }
  bool        isRenderable()                    const { return !(m_flags & NotRenderable); }
  bool        isAllowLossyCompression()         const { return !(m_flags & NoLossyCompression) && !isDiscrete(); }
  bool        isUseZipForLosslessCompression()  const { return (m_flags & NoLossyCompressionUseZip) == NoLossyCompressionUseZip; }
  const char *getName()                         const { return m_name; }
  const char *getUnit()                         const { return m_unit; }
  const FloatRange &getValueRange()           const { return m_valueRange; }
  float       getValueRangeMin()                const { return m_valueRange.min; }
  float       getValueRangeMax()                const { return m_valueRange.max; }

  VolumeDataMapping getMapping()                const { return m_mapping; }
  int         getMappedValueCount()             const { return m_mappedValueCount; }

  bool        isUseNoValue()                    const { return m_useNoValue; }
  float       getNoValue()                      const { return m_noValue; }

  float       getIntegerScale()                 const { return m_integerScale; }
  float       getIntegerOffset()                const { return m_integerOffset; }

  /// Named constructor for a trace mapped channel
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \return a trace mapped descriptor
  static VolumeDataChannelDescriptor
    TraceMappedVolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, int mappedValueCount, enum Flags flags)
  {
    return VolumeDataChannelDescriptor(format, components, pName, pUnit, valueRangeMin, valueRangeMax, VolumeDataMapping::PerTrace, mappedValueCount, flags, 1.0f, 0.0f);
  }

  /// Named constructor for a trace mapped channel
  /// \param format the data format for this channel
  /// \param components the vector count for this channel
  /// \param pName the name of this channel
  /// \param pUnit the unit of this channel
  /// \param valueRangeMin the value range minimum of this channel
  /// \param valueRangeMax the value range maximum of this channel
  /// \param mappedValueCount When using per trace mapping, the number of values to store per trace
  /// \param flags the flags for this channel
  /// \param noValue the No Value for this channel
  /// \return a trace mapped descriptor
  static VolumeDataChannelDescriptor
    TraceMappedVolumeDataChannelDescriptor(Format format, Components components, const char *pName, const char *pUnit, float valueRangeMin, float valueRangeMax, int mappedValueCount, enum Flags flags, float noValue)
  {
    return VolumeDataChannelDescriptor(format, components, pName, pUnit, valueRangeMin, valueRangeMax, VolumeDataMapping::PerTrace, mappedValueCount, flags, noValue, 1.0f, 0.0f);
  }
};
}
#endif //VOLUMEDATACHANNELDESCRIPTOR_H