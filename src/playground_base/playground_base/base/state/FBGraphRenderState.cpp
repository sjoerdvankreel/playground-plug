#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/base/state/FBProcStateContainer.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

#include <cassert>

static FBNoteEvent
MakeNoteC4On()
{
  FBNoteEvent result;
  result.pos = 0;
  result.on = true;
  result.velo = 1.0f;
  result.note.id = 0;
  result.note.key = 60;
  result.note.channel = 0;
  return result;
}

FBGraphRenderState::
~FBGraphRenderState() {}

FBGraphRenderState::
FBGraphRenderState(FBPlugGUI const* plugGUI):
_plugGUI(plugGUI),
_input(std::make_unique<FBPlugInputBlock>()),
_moduleState(std::make_unique<FBModuleProcState>()),
_procState(std::make_unique<FBProcStateContainer>(*plugGUI->HostContext()->Topo())),
_scalarState(std::make_unique<FBScalarStateContainer>(*plugGUI->HostContext()->Topo())),
_voiceManager(std::make_unique<FBVoiceManager>(_procState.get()))
{
  _input->note = &_notes;
  _input->audio = &_audio;
  _input->voiceManager = _voiceManager.get();

  auto hostContext = plugGUI->HostContext();
  for (int i = 0; i < hostContext->Topo()->params.size(); i++)
    _procState->InitProcessing(i, hostContext->GetParamNormalized(i));

  _moduleState->moduleSlot = -1;
  _moduleState->sampleRate = -1;
  _moduleState->input = _input.get();
  _moduleState->topo = hostContext->Topo();
  _moduleState->procRaw = _procState->Raw();
  _moduleState->exchangeInputRaw = nullptr; // TODO
  _moduleState->outputParamsNormalized = nullptr;
}

FBExchangeStateContainer const*
FBGraphRenderState::ExchangeContainer() const
{
  return _plugGUI->HostContext()->ExchangeState();
}

void
FBGraphRenderState::PrimaryParamChanged(int index, float normalized)
{
  *_scalarState->Params()[index] = normalized;
}

void
FBGraphRenderState::PrepareForRender(bool primary, int voice)
{
  // TODO figure out to handle global
  assert(!primary || voice == -1);
  if (_voiceManager->VoiceCount() > 0)
  {
    _voiceManager->Return(0);
    _voiceManager->ResetReturnedVoices();
  }
  if (primary)
    _procState->InitProcessing(*_scalarState);
  else
    // TODO initproc for 1 voice only?
    _procState->InitProcessing(*ExchangeContainer(), voice);
  _voiceManager->Lease(MakeNoteC4On());
  _moduleState->voice = &_voiceManager->Voices()[0];
}