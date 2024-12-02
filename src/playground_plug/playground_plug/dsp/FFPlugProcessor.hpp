#pragma once

#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_base/base/plug/FBPlugTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

struct FBRuntimeTopo;
class FBFixedAudioBlock;

class FFPlugProcessor:
public IFBPlugProcessor
{
  float _phase = 0;
  FBStaticTopo _topo;
  FFProcState _state = {}; // TODO make private and const accessor?
  float const _sampleRate;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(FBRuntimeTopo const& topo, float sampleRate);

  FBStateAddrs StateAddrs() override;
  void ProcessPlug(FBFixedAudioBlock const& input, FBFixedAudioBlock& output) override;
};