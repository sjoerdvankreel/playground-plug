#include <playground_plug/dsp/FFPlugProcessor.hpp>
#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(
FBRuntimeTopo const& topo, float sampleRate) :
_sampleRate(sampleRate)
{
  topo.InitProcAddrs(_memory);
  topo.InitScalarAddrs(_memory);
}

FBStateAddrs 
FFPlugProcessor::StateAddrs()
{
  FBStateAddrs result;
  result.proc = &_memory;
  result.scalar = &_memory;
  return result;
}

void 
FFPlugProcessor::ProcessPlug(
  FBFixedAudioBlock const& input, FBFixedAudioBlock& output)
{
  bool on = FBNormalizedToBool(_memory.block.osci.on[0][FFOsciBlockOn]);
  if (!on)
  {
    output.Fill(0, output.Count(), 0.0f);
    return;
  }

  auto const& gain = _memory.cv.osci.gain[0][0];
  for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
  {
    float sample = std::sin(2.0f * std::numbers::pi_v<float> * _phase);
    output[0][s] = sample * gain[s];
    output[1][s] = sample * gain[s];
    _phase += 440.0f / _sampleRate;
    _phase -= std::floor(_phase);
  }
}