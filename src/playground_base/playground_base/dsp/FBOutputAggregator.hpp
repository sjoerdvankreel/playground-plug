# pragma once

#include <playground_base/shared/FBHostBlock.hpp>

class FBRawStereoBlockView;

class FBOutputAggregator
{
  FBAccumulatedOutputBlock _accumulated;

public:
  FBOutputAggregator(int maxHostSampleCount);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBOutputAggregator);

  void Aggregate(FBRawStereoBlockView& output);
  void Accumulate(FBFixedStereoBlock const& input);
};