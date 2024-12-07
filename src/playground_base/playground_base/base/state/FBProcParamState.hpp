#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

struct FBProcParamState final
{
  float current = 0.0f;
  FBFixedCVBlock smoothed = {};
  FBOnePoleFilter smoother = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBProcParamState);
};