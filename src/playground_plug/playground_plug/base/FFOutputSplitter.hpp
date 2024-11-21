# pragma once

#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/FFHostBlock.hpp>

class FFOutputSplitter
{
  FFHostOutputBlock _accumulatedBlock;

public:
  FFOutputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFOutputSplitter);

  void AccumulateFixedBlock(FFFixedStereoBlock const& audioOut);
};