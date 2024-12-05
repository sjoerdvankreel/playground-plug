#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
~FBHostProcessor() {}

FBHostProcessor::
FBHostProcessor(
  std::unique_ptr<IFBPlugProcessor>&& plug,
  FBProcStatePtrs const* state, float sampleRate):
_plug(std::move(plug)),
_ramp(std::make_unique<FBRampProcessor>()),
_smooth(std::make_unique<FBSmoothProcessor>()),
_voiceManager(std::make_unique<FBVoiceManager>()),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>())
{
  _fixedOut.state = state;
  _plugIn.voiceManager = _voiceManager.get();
  for (int p = 0; p < state->isAcc.size(); p++)
    if (state->isAcc[p])
    {
      state->single.acc[p]->smooth = FBOnePoleFilter(
        sampleRate, FB_PARAM_SMOOTH_SEC);
      if(state->isVoice[p])
        for(int v = 0; v < FB_MAX_VOICES; v++)
          state->voice[v].acc[p]->smooth = FBOnePoleFilter(
            sampleRate, FB_PARAM_SMOOTH_SEC);
    }
}

void 
FBHostProcessor::ProcessVoices()
{
  auto const& voices = _plugIn.voiceManager->Voices();
  for (int v = 0; v < voices.size(); v++)
    if (voices[v].active)
    {
      // TODO 
      // for now just copy the single (not per voice) cv blocks
      // also TODO
      // block valued params should be fixed at voice start      
    }
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostAudioBlock& output)
{
  for (auto const& be : input.block)
    *_fixedOut.state->single.block[be.index] = be.normalized;

  FBFixedInputBlock const* fixedIn;
  _hostBuffer->BufferFromHost(input);
  while ((fixedIn = _hostBuffer->ProcessToFixed()) != nullptr)
  {
    _plugIn.note = &fixedIn->note;
    _plugIn.audio = &fixedIn->audio;
    _ramp->ProcessRamping(*fixedIn, _fixedOut);
    _smooth->ProcessSmoothing(_fixedOut);
    _plug->ProcessPreVoice(_plugIn);
    ProcessVoices();
    _plug->ProcessPostVoice(_fixedOut.audio);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);
}