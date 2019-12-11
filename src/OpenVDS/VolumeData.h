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

#ifndef VOLUMEDATA_H
#define VOLUMEDATA_H

#include <cstdint>

namespace OpenVDS
{

/// Interpolation method used for sampling
enum class InterpolationMethod
{
  Nearest,    ///< Snap to the closest sample
  Linear,     ///< Linear interpolation
  Cubic,      ///< Cubic interpolation
  Angular,    ///< Angular interpolation (same as linear but wraps around the value range)
  Triangular, ///< Triangular interpolation used to interpolate heightmap data
};

/// 2D or 3D dimension group\n
enum DimensionsND
{
  Dimensions_012,
  Dimensions_013,
  Dimensions_014,
  Dimensions_015,
  Dimensions_023,
  Dimensions_024,
  Dimensions_025,
  Dimensions_034,
  Dimensions_035,
  Dimensions_045,
  Dimensions_123,
  Dimensions_124,
  Dimensions_125,
  Dimensions_134,
  Dimensions_135,
  Dimensions_145,
  Dimensions_234,
  Dimensions_235,
  Dimensions_245,
  Dimensions_345,
  Dimensions_01,
  Dimensions_02,
  Dimensions_03,
  Dimensions_04,
  Dimensions_05,
  Dimensions_12,
  Dimensions_13,
  Dimensions_14,
  Dimensions_15,
  Dimensions_23,
  Dimensions_24,
  Dimensions_25,
  Dimensions_34,
  Dimensions_35,
  Dimensions_45
};

/// Mapping volume data channels
enum class VolumeDataMapping : uint64_t
{
  Direct   = 0xFFFFFFFFFFFFFFFFULL, ///< Each voxel in the volume has a value 
  PerTrace = 0x1B6F015EB8864888ULL  ///< Each trace in the volume has a specified number of values. A trace is the entire length of dimension 0
};

enum class CompressionMethod
{
  None,
  Wavelet,
  RLE,
  Zip,
  WaveletNormalizeBlock,
  WaveletLossless,
  WaveletNormalizeBlockLossless
};
 
class CompressionInfo
{
  CompressionMethod   compressionMethod;
  int                 adaptiveLevel;
public:
  CompressionInfo() : compressionMethod(CompressionMethod::None), adaptiveLevel(0)
  {
  }
  CompressionInfo(CompressionMethod compressionMethod, int adaptiveLevel) : compressionMethod(compressionMethod), adaptiveLevel(adaptiveLevel)
  {
  }

  CompressionMethod   GetCompressionMethod() const
  {
    return compressionMethod;
  }
  int                 GetAdaptiveLevel()     const
  {
    return adaptiveLevel;
  }
};

enum Dimensionality
{
  Dimensionality_1 = 1,
  Dimensionality_2,
  Dimensionality_3,
  Dimensionality_4,
  Dimensionality_5,
  Dimensionality_6,
  Dimensionality_Max = Dimensionality_6
};

/// <summary>
/// Get the number of voxels at a particular LOD from a voxel range (ranges are exclusive).
/// </summary>
/// <param name="voxelMin">
/// The minimum voxel index of the range.
/// </param>
/// <param name="voxelMax">
/// The maximum voxel index of the range (ranges are exclusive, so the range does not include voxelMax).
/// </param>
/// <param name="includePartialUpperVoxel">
/// This controls the rounding. Usually you want the size of the range to be all voxels in the range, but when we are copying from multiple adjacent ranges we only want to round up for the last one.
/// </param>
/// <returns>
/// The number of voxels at the given LOD, at LOD 0 the result is voxelMax - voxelMin.
/// </returns>

inline int GetLODSize(int voxelMin, int voxelMax, int lod, bool includePartialUpperVoxel = true)
{
  return ((voxelMax - includePartialUpperVoxel) >> lod) + includePartialUpperVoxel - (voxelMin >> lod);
}

} /* namespace OpenVDS */

#endif //VOLUMEDATA_H_INCLUDED
