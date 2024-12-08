#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

class FBProcStatePtrs;

struct FBFixedOutputBlock final
{
  FBFixedAudioBlock audio = {};
  FBProcStatePtrs const* state = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};