#include <playground_base/base/shared/FBDenormal.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessor.hpp>
#include <playground_base/dsp/pipeline/host/FBHostInputBlock.hpp>
#include <playground_base/dsp/pipeline/host/FBHostOutputBlock.hpp>
#include <playground_base/dsp/pipeline/host/FBHostProcessContext.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothingProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBHostBufferProcessor.hpp>
#include <playground_base/dsp/pipeline/buffer/FBFixedBufferProcessor.hpp>

#include <utility>

FBHostProcessor::
~FBHostProcessor() {}

FBHostProcessor::
FBHostProcessor(
  IFBHostProcessContext* hostContext,
  FBStaticTopo const& topo,
  std::unique_ptr<IFBPlugProcessor>&& plug,
  FBProcStateContainer* state, float sampleRate):
_topo(topo),
_sampleRate(sampleRate),
_state(state),
_hostContext(hostContext),
_plug(std::move(plug)),
_voiceManager(std::make_unique<FBVoiceManager>(state)),
_hostBuffer(std::make_unique<FBHostBufferProcessor>()),
_fixedBuffer(std::make_unique<FBFixedBufferProcessor>(_voiceManager.get())),
_smoothing(std::make_unique<FBSmoothingProcessor>(_voiceManager.get(), (int)state->Params().size()))
{
  _fixedOut.state = state;
  _plugIn.voiceManager = _voiceManager.get();
}

void
FBHostProcessor::ProcessAllVoices()
{
  for (int v = 0; v < FBMaxVoices; v++)
    ProcessVoice(v);
}

void 
FBHostProcessor::ProcessVoice(int slot)
{
  if (_plugIn.voiceManager->IsActive(slot))
    _plug->ProcessVoice(_plugIn, slot);
}

void 
FBHostProcessor::ProcessHost(
  FBHostInputBlock const& input, FBHostOutputBlock& output)
{
  auto denormalState = FBDisableDenormal(); 
  for (auto const& be : input.block)
    _fixedOut.state->Params()[be.param].Value(be.normalized);

  auto const& smoothing = _state->Special().smoothing;
  float smoothingSeconds = smoothing.PlainLinear(_topo);
  int smoothingSamples = (int)std::ceil(smoothingSeconds * _sampleRate);
  _state->SetSmoothingCoeffs(_sampleRate, smoothingSeconds);

  FBFixedInputBlock const* fixedIn;
  _hostBuffer->BufferFromHost(input);
  while ((fixedIn = _hostBuffer->ProcessToFixed()) != nullptr)
  {
    _plugIn.note = &fixedIn->note;
    _plugIn.audio = &fixedIn->audio;
    _plug->LeaseVoices(_plugIn);
    _smoothing->ProcessSmoothing(*fixedIn, _fixedOut, smoothingSamples);
    _plug->ProcessPreVoice(_plugIn);
    _hostContext->ProcessVoices();
    _plug->ProcessPostVoice(_plugIn, _fixedOut.audio);
    _plug->ReturnVoices(_plugIn);
    _fixedBuffer->BufferFromFixed(_fixedOut.audio);
  }
  _fixedBuffer->ProcessToHost(output);  
  FBRestoreDenormal(denormalState);
}