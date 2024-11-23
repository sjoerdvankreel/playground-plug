#include <playground_plug/base/FFSplitProcessor.hpp>

FFSplitProcessor::
FFSplitProcessor(int maxHostSampleCount):
_output(),
_inputSplitter(maxHostSampleCount),
_outputSplitter(maxHostSampleCount) {}

void 
FFSplitProcessor::ProcessHostBlock(
  FFHostInputBlock const& input,
  FFRawStereoBlock& output)
{
  _inputSplitter.Accumulate(input);
  FFFixedInputBlock const* splitted = nullptr;
  while ((splitted = _inputSplitter.Split()) != nullptr)
  {
    ProcessFixedBlock(*splitted, _output);
    _outputSplitter.AccumulateFixedBlock(_output);
  }

  auto const& accumulatedOutput = _outputSplitter.GetAccumulatedBlock();
  int samplesUsed = std::min(output.Count(), accumulatedOutput.sampleCount);
  int samplesPadded = std::max(0, output.Count() - accumulatedOutput.sampleCount);
  output.Fill(0, samplesPadded, 0.0f);
  accumulatedOutput.audio.CopyTo(output, 0, samplesPadded, samplesUsed);
  _outputSplitter.RemoveSamples(samplesUsed);
}