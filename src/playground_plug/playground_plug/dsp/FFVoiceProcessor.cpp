#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/dsp/FFVoiceProcessor.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>
#include <playground_plug/modules/glfo/FFGLFOProcessor.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>
#include <playground_plug/modules/output/FFOutputProcessor.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>
#include <playground_plug/modules/osci_mod/FFOsciModProcessor.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>

void 
FFVoiceProcessor::BeginVoice(FBModuleProcState state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].env[i].processor->BeginVoice(state);
  }
  state.moduleSlot = 0;
  procState->dsp.voice[voice].osciMod.processor->BeginVoice(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].osci[i].processor->BeginVoice(state);
  }
  for (int i = 0; i < FFPhysCount; i++)
  {
    state.moduleSlot = i;
    procState->dsp.voice[voice].phys[i].processor->BeginVoice(false, state);
  }
}

bool 
FFVoiceProcessor::Process(FBModuleProcState state)
{
  bool voiceFinished = false;
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto& voiceDSP = procState->dsp.voice[voice];
  voiceDSP.output.Fill(0.0f);

  for (int i = 0; i < FFEnvCount; i++)
  {
    state.moduleSlot = i;
    int envProcessed = voiceDSP.env[i].processor->Process(state);
    if (i == 0)
      voiceFinished = envProcessed != FBFixedBlockSamples;
  }
  state.moduleSlot = 0;
  voiceDSP.osciMod.processor->Process(state);
  for (int i = 0; i < FFOsciCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.osci[i].processor->Process(state);
    voiceDSP.output.Add(voiceDSP.osci[i].output);
  }
  for (int i = 0; i < FFPhysCount; i++)
  {
    state.moduleSlot = i;
    voiceDSP.phys[i].processor->Process(state);
    voiceDSP.output.Add(voiceDSP.phys[i].output);
  }

  // TODO dont hardcode this to voice amp?
  voiceDSP.output.Mul(voiceDSP.env[0].output);
  return voiceFinished;
}