#include <playground_plug/plug/dsp/FFPluginProcessor.hpp>

#include <cmath>
#include <numbers>

FFPluginProcessor::
FFPluginProcessor(int maxHostSampleCount, float sampleRate) :
_phase(),
_sampleRate(sampleRate),
_fixedProcessor(maxHostSampleCount) {}

void 
FFPluginProcessor::ProcessHost(FFHostInputBlock const& input, FFRawStereoBlockView& output)
{
  _fixedProcessor.Process(input, output, *this);
}

void 
FFPluginProcessor::ProcessFixed(FFFixedInputBlock const& input, FFFixedStereoBlock& output)
{
  for (int s = 0; s < FF_FIXED_BLOCK_SIZE; s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> * _phase.Current());
    output[FF_CHANNEL_L][s] = sample;
    output[FF_CHANNEL_R][s] = sample;
    _phase.Next(_sampleRate, 440.0f);
  }
}