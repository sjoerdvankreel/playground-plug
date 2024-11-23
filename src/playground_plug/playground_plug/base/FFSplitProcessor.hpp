#pragma once

#include <playground_plug/base/FFDSPBlock.hpp>
#include <playground_plug/base/FFHostBlock.hpp>
#include <playground_plug/base/FFInputSplitter.hpp>
#include <playground_plug/base/FFOutputSplitter.hpp>

class FFSplitProcessor
{
  FFFixedInputBlock _input;
  FFFixedStereoBlock _output;
  FFInputSplitter _inputSplitter;
  FFOutputSplitter _outputSplitter;

protected:
  virtual void ProcessFixedBlock(
    FFFixedInputBlock const& input,
    FFFixedStereoBlock& output) = 0;

public:
  FFSplitProcessor(int maxHostSampleCount);

  void ProcessHostBlock(
    FFHostInputBlock const& input, 
    FFRawStereoBlock& output);
};