#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FFModuleProcState;

class FFShaperProcessor
{
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperProcessor);
  void Process(FFModuleProcState const& state, int voice);
};