#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>
#include <cmath>

void 
FFShaperProcessor::Process(int moduleSlot, FFPluginBlock& block)
{
  for(int channel = 0; channel < FB_CHANNELS_STEREO; channel++)
    for (int s = 0; s < FF_BLOCK_SIZE; s++)
      block.shaperOut[moduleSlot][channel][s] = std::tanh(block.shaperIn[moduleSlot][channel][s]);
}