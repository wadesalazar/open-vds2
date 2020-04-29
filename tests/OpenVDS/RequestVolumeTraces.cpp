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
#include <vector>

#include <gtest/gtest.h>
#include <fmt/format.h>

GTEST_TEST(OpenVDS_integration, SimpleRequestVolumeTraces)
{
  OpenVDS::Error error;
  OpenVDS::AWSOpenOptions options;

  options.region = TEST_AWS_REGION;
  options.bucket = TEST_AWS_BUCKET;
  options.endpointOverride = TEST_AWS_ENDPOINT_OVERRIDE;
  options.key = TEST_AWS_OBJECTID;

  if(options.bucket.empty() || options.key.empty())
  {
    GTEST_SKIP() << "Environment variables not set";
  }

  std::unique_ptr<OpenVDS::VDS, decltype(&OpenVDS::Close)> handle(OpenVDS::Open(options, error), &OpenVDS::Close);
  ASSERT_TRUE(handle);

  OpenVDS::VolumeDataAccessManager *accessManager = OpenVDS::GetAccessManager(handle.get());
  ASSERT_TRUE(accessManager);

  OpenVDS::VolumeDataLayout *layout = OpenVDS::GetLayout(handle.get());
 
  int sampleCount0 = layout->GetDimensionNumSamples(0);
  std::vector<float> buffer(10 * sampleCount0);

  int sampleCount1 = layout->GetDimensionNumSamples(1);
  int sampleCount2 = layout->GetDimensionNumSamples(2);

  float tracePos[10][6];

  for (int trace = 0; trace < 10; trace++)
  {
    tracePos[trace][0] = 0;
    tracePos[trace][1] = (float)(trace * (sampleCount1 / 10));
    tracePos[trace][2] = (float)(trace * (sampleCount2 / 10));
    tracePos[trace][3] = 0;
    tracePos[trace][4] = 0;
    tracePos[trace][5] = 0;
  }

  int64_t requestId = accessManager->RequestVolumeTraces(buffer.data(), layout, OpenVDS::Dimensions_012, 0, 0, tracePos, 10, OpenVDS::InterpolationMethod::Nearest, 0);

  float previousProgress = -1;
  while(!accessManager->WaitForCompletion(requestId,1000)){
      ASSERT_FALSE(accessManager->IsCanceled(requestId));

      float progress = accessManager->GetCompletionFactor(requestId);
      if (progress != previousProgress) {
          previousProgress = progress;
          GTEST_LOG_(INFO) << "Request progress : " << progress * 100. << " %";
      }
  }

  auto pageAccessor = accessManager->CreateVolumeDataPageAccessor(layout, OpenVDS::Dimensions_012, 0, 0, 1000, OpenVDS::VolumeDataAccessManager::AccessMode_ReadOnly);
  auto valueReader = accessManager->Create3DInterpolatingVolumeDataAccessorR32(pageAccessor, 0.0f, OpenVDS::InterpolationMethod::Nearest);

  std::vector<float> verify(10 * sampleCount0);
  for (int trace = 0; trace < 10; trace++)
  {
    for (int i = 0; i < sampleCount0; i++)
    {
      verify[trace * sampleCount0 + i] = valueReader->GetValue(OpenVDS::FloatVector3(tracePos[trace][2], tracePos[trace][1], float(i)));
    }
  }
  ASSERT_EQ(buffer, verify);
}
