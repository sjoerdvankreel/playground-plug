#pragma once

#include <firefly_base/dsp/plug/FBPlugProcessor.hpp>
#include <firefly_base/base/state/proc/FBModuleProcState.hpp>

struct FFProcState;
struct FBRuntimeTopo;
struct FFExchangeState;
struct FBPlugOutputBlock;
class IFBHostDSPContext;

class FFPlugProcessor final:
public IFBPlugProcessor
{
  float const _sampleRate;
  FBRuntimeTopo const* const _topo;
  FFProcState* const _procState;
  FFExchangeState* const _exchangeState;

  FBModuleProcState MakeModuleState(FBPlugInputBlock const& input) const;
  FBModuleProcState MakeModuleVoiceState(FBPlugInputBlock const& input, int voice) const;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFPlugProcessor);
  FFPlugProcessor(IFBHostDSPContext* hostContext);

  void LeaseVoices(FBPlugInputBlock const& input) override;
  void ProcessPreVoice(FBPlugInputBlock const& /*input*/) override {}
  void ProcessVoice(FBPlugInputBlock const& input, int voice) override;
  void ProcessPostVoice(FBPlugInputBlock const& input, FBPlugOutputBlock& output) override;
};