#pragma once

#include <playground_plug/base/FFHostBlock.hpp>

class FFInputSplitter
{
  FFHostInputBlock _accumulatedBlock;

public:
  FFInputSplitter(int maxHostSampleCount);
  FF_NOCOPY_NOMOVE_NODEFCTOR(FFInputSplitter);

  void RemoveFirstFixedBlock();
  bool GetFirstFixedBlock(FFHostInputBlock const* fixedBlock);
  void AccumulateHostBlock(FFHostInputBlock const& hostBlock);
};