#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/dsp/FFPlugProcessor.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <cmath>
#include <numbers>

FFPlugProcessor::
FFPlugProcessor(
FBStaticTopo const& topo, FFProcState* state, float sampleRate) :
_topo(topo),
_state(state),
_sampleRate(sampleRate) {}

FFModuleProcState 
FFPlugProcessor::MakeModuleState(FBPlugInputBlock const& input) const
{
  FFModuleProcState result = {};
  result.proc = _state;
  result.topo = &_topo;
  result.input = &input;
  result.sampleRate = _sampleRate;
  return result;
}

void
FFPlugProcessor::ProcessPreVoice(
  FBPlugInputBlock const& input)
{
  auto moduleState = MakeModuleState(input);
  for (int s = 0; s < FF_GLFO_COUNT; s++)
  {
    moduleState.moduleSlot = s;
    _state->dsp.global.glfo[s].processor.Process(moduleState);
  }

  for (int n = 0; n < input.note->size(); n++)
    if((*input.note)[n].on)
      input.voiceManager->Lease((*input.note)[n]);
}

void
FFPlugProcessor::ProcessPostVoice(
  FBPlugInputBlock const& input, FBFixedAudioBlock& output)
{
  for (int n = 0; n < input.note->size(); n++)
    if (!(*input.note)[n].on)
      input.voiceManager->ReturnOldest((*input.note)[n]);

  output.Fill(0, output.Count(), 0.0f);
  for (int i = 0; i < FB_MAX_VOICES; i++)
    if (input.voiceManager->Voices()[i].active)
      output += _state->dsp.voice[i].output;
}

void
FFPlugProcessor::ProcessVoice(FBPlugInputBlock const& input, int voice)
{
  auto moduleState = MakeModuleState(input);
  moduleState.voice = &input.voiceManager->Voices()[voice];

  auto& voiceDSP = _state->dsp.voice[voice];
  voiceDSP.output.Fill(0, voiceDSP.output.Count(), 0.0f);
  for (int i = 0; i < FF_OSCI_COUNT; i++)
  {
    moduleState.moduleSlot = i;
    voiceDSP.osci[i].processor.Process(moduleState, voice);
    voiceDSP.output += voiceDSP.osci[i].output;
  }

  // todo shaper
}