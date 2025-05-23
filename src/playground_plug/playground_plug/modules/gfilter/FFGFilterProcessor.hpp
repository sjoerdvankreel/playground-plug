#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/shared/FBCytomicFilter.hpp>

#include <array>

struct FBModuleProcState;

// https://www.cytomic.com/files/dsp/SvfLinearTrapOptimised2.pdf
class FFGFilterProcessor final
{
  FBCytomicFilter<2> _filter = {};

public:
  void Process(FBModuleProcState& state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterProcessor);
};