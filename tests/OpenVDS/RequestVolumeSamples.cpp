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

#include <OpenVDS/OpenVDS.h>
#include <OpenVDS/VolumeDataAccess.h>
#include <OpenVDS/VolumeDataLayout.h>
#include <OpenVDS/VolumeData.h>

#include <cstdlib>

#include <random>
#include <array>

#include <gtest/gtest.h>

static inline void generateRandomPosition(std::mt19937 &gen, const std::vector<std::uniform_real_distribution<float>> dimensionDistribution, int dimension, float (&pos)[OpenVDS::Dimensionality_Max])
{
  for (int i = 0; i < dimension; i++)
  {
    pos[i] = dimensionDistribution[i](gen);
  }
}
GTEST_TEST(OpenVDS_integration, SimpleRequestVolumeSamples)
{
  OpenVDS::Error error;
  OpenVDS::AWSOpenOptions options;

  options.region = TEST_AWS_REGION;
  options.bucket = TEST_AWS_BUCKET;
  options.key = TEST_AWS_OBJECTID;

  if(options.region.empty() || options.bucket.empty() || options.key.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  ASSERT_TRUE(options.region.size() && options.bucket.size() && options.key.size());
  std::unique_ptr<OpenVDS::VDSHandle, decltype(&OpenVDS::destroy)> handle(OpenVDS::open(options, error), &OpenVDS::destroy);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager *dataAccessManager = OpenVDS::getDataAccessManager(handle.get());
  ASSERT_TRUE(dataAccessManager);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::getLayout(handle.get());

  int32_t dimension = layout->getDimensionality();

  std::mt19937 gen(5746);
  std::vector<std::uniform_real_distribution<float>> dimensionDistribution;
  dimensionDistribution.reserve(OpenVDS::Dimensionality_Max);
  for (int i = 0; i < dimension; i++)
  {
    float min = layout->getDimensionMin(i) + 1; 
    float max = layout->getDimensionMax(i);
    dimensionDistribution.emplace_back(min, max);
  }
  
  float positions[100][OpenVDS::Dimensionality_Max];

  for(int i = 0; i < 100; i++)
  {
    generateRandomPosition(gen, dimensionDistribution, dimension, positions[i]);
  }
  float buffer[100];
  int request = dataAccessManager->requestVolumeSamples(buffer, layout, OpenVDS::Dimensions_012, 0, 0, positions, 100, OpenVDS::InterpolationMethod::Linear);
  dataAccessManager->waitForCompletion(request);

  OpenVDS::VolumeDataPageAccessor *pageAccessor = dataAccessManager->createVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 100, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  OpenVDS::VolumeDataReadAccessor<OpenVDS::FloatVector3, float >* readAccessor = dataAccessManager->create3DInterpolatingVolumeDataAccessorR32(pageAccessor, 0.0f, OpenVDS::InterpolationMethod::Linear);

  float verifyBuffer[100];
  for (int i = 0; i < 100; i++)
  {
    auto &p = positions[i];
    OpenVDS::FloatVector3 pos(p[2], p[1], p[0]);
    verifyBuffer[i] = readAccessor->getValue(pos);
  }

  for (int i = 0; i < 100; i++)
  {
    float diff = std::abs(buffer[i] - verifyBuffer[i]);
    ASSERT_EQ(diff, 0.0f);
  }
}
