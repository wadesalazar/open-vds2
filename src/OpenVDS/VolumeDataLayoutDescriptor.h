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

#ifndef VOLUMEDATALAYOUTDESCRIPTOR_H
#define VOLUMEDATALAYOUTDESCRIPTOR_H

namespace OpenVDS
{
class VolumeDataLayoutDescriptor
{
public:
  enum BrickSize
  {
    BrickSize_32   =  5,
    BrickSize_64   =  6,
    BrickSize_128  =  7,
    BrickSize_256  =  8,
    BrickSize_512  =  9,
    BrickSize_1024 = 10,
    BrickSize_2048 = 11,
    BrickSize_4096 = 12
  };

  enum LODLevels
  {
    LODLevels_None =  0,
    LODLevels_1    =  1,
    LODLevels_2    =  2,
    LODLevels_3    =  3,
    LODLevels_4    =  4,
    LODLevels_5    =  5,
    LODLevels_6    =  6,
    LODLevels_7    =  7,
    LODLevels_8    =  8,
    LODLevels_9    =  9,
    LODLevels_10   = 10,
    LODLevels_11   = 11,
    LODLevels_12   = 12
  };

  enum Options
  {
    Options_None                         = 0,
    Options_Create2DLods                 = (1 << 0),
    Options_ForceFullResolutionDimension = (1 << 1)
  };

private:
  BrickSize           m_brickSize;

  int                 m_negativeMargin;
  int                 m_positiveMargin;
  int                 m_brickSizeMultiplier2D;

  LODLevels           m_lodLevels;

  Options             m_options;

  int                 m_fullResolutionDimension;

public:
  VolumeDataLayoutDescriptor() : m_brickSize(), m_negativeMargin(), m_positiveMargin(), m_brickSizeMultiplier2D(), m_lodLevels(), m_options(), m_fullResolutionDimension() {}
  VolumeDataLayoutDescriptor(BrickSize brickSize, int negativeMargin, int positiveMargin, int brickSizeMultiplier2D, LODLevels lodLevels, Options options, int fullResolutionDimension = 0) : m_brickSize(brickSize), m_negativeMargin(negativeMargin), m_positiveMargin(positiveMargin), m_brickSizeMultiplier2D(brickSizeMultiplier2D), m_lodLevels(lodLevels), m_options(options), m_fullResolutionDimension(fullResolutionDimension) {}

  bool                isValid()        const { return m_brickSize != 0; }

  BrickSize           getBrickSize()      const { return m_brickSize; }
  int                 getNegativeMargin() const { return m_negativeMargin; }
  int                 getPositiveMargin() const { return m_positiveMargin; }
  int                 getBrickSizeMultiplier2D() const { return m_brickSizeMultiplier2D; }
  LODLevels           getLODLevels()      const { return m_lodLevels; }

  bool                isCreate2DLods()                 const { return (m_options & Options_Create2DLods) != 0; }
  bool                isForceFullResolutionDimension() const { return (m_options & Options_ForceFullResolutionDimension) != 0; }

  int                 getFullResolutionDimension() const { return m_fullResolutionDimension; }
};
}
#endif //VOLUMEDATALAYOUTDESCRIPTOR_H