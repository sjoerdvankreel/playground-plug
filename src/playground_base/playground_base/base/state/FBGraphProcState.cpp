#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphProcState.hpp>

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

FBGraphProcState::
FBGraphProcState(FBPlugGUI const* plugGUI):
plugGUI(plugGUI),
container(*plugGUI->Topo()),
voiceManager(&container),
audio(), notes(), input(), moduleState()
{
  input.note = &notes;
  input.audio = &audio;
  input.voiceManager = &voiceManager;
  for (int i = 0; i < plugGUI->Topo()->params.size(); i++)
    container.InitProcessing(i, plugGUI->HostContext()->GetParamNormalized(i));  

  moduleState.input = &input;
  moduleState.moduleSlot = -1;
  moduleState.sampleRate = -1;
  moduleState.topo = plugGUI->Topo();
  moduleState.procRaw = container.Raw();
  moduleState.outputParamsNormalized = nullptr;

  PrepareForRender();
}

void
FBGraphProcState::PrepareForRender()
{
  voiceManager.Lease(MakeNoteC4On());
  moduleState.voice = &voiceManager.Voices()[0];
}