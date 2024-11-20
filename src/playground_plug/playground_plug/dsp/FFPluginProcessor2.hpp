#pragma once

#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug/base/dsp/FBPluginProcessor.hpp>
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

#include <array>
#include <vector>

class FFPluginProcessor:
public FBPluginProcessor<FFPluginProcessor, FFProcessorMemory>
{
  FFPluginProcessors _processors = {};

public:
  FFPluginProcessor(FFRuntimeTopo const* topo, int maxSampleCount, float sampleRate);
  void ProcessPluginBlock(FBProcessorContext const& context);
};