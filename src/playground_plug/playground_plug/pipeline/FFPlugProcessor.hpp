#pragma once

#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

struct FFProcState;
struct FBRuntimeTopo;
struct FBFixedOutputBlock;
class FBFixedFloatAudioBlock;

class FFPlugProcessor final:
public IFBPlugProcessor
{
  FFProcState* _state;
  float const _sampleRate;
  FBRuntimeTopo const* _topo;

  FFModuleProcState MakeModuleState(FBPlugInputBlock const& input) const;
  FFModuleProcState MakeModuleVoiceState(FBPlugInputBlock const& input, int voice) const;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(FBRuntimeTopo const* topo, FFProcState* state, float sampleRate);

  void LeaseVoices(FBPlugInputBlock const& input) override;
  void ProcessPreVoice(FBPlugInputBlock const& input) override;
  void ProcessVoice(FBPlugInputBlock const& input, int voice) override;
  void ProcessPostVoice(FBPlugInputBlock const& input, FBFixedOutputBlock& output) override;
};