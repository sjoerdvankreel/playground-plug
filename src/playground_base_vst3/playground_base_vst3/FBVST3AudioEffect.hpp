#pragma once

#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <public.sdk/source/vst/vstaudioeffect.h>

#include <array>
#include <vector>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;

class FBHostProcessor;
class IFBPlugProcessor;

class FBVST3AudioEffect:
public AudioEffect
{
  std::unique_ptr<FBRuntimeTopo> _topo;
  void* _state;
  FBProcStatePtrs _statePtrs;

  FBHostInputBlock _input = {};
  std::array<std::vector<float>, 2> _zeroIn = {};
  std::unique_ptr<FBHostProcessor> _hostProcessor = {};

protected:
  virtual std::unique_ptr<IFBPlugProcessor> 
  MakePlugProcessor(FBStaticTopo const& topo, void* state, float sampleRate) const = 0;

public:
  ~FBVST3AudioEffect();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3AudioEffect);
  FBVST3AudioEffect(FBStaticTopo const& topo, FUID const& controllerId);
  uint32 PLUGIN_API getLatencySamples() override { return FBFixedBlockSamples; }

  tresult PLUGIN_API setState(IBStream* state) override;
  tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolicSize) override;
  tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts) override;
};