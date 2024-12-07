#pragma once

#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

class FBProcStatePtrs;

struct FBFixedOutputBlock final
{
  FBProcStatePtrs* state = {};
  FBFixedAudioBlock audio = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedOutputBlock);
};