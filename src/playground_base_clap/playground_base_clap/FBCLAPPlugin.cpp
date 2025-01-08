#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/gui/glue/FBPlugGUI.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

#include <clap/helpers/plugin.hxx>
#include <algorithm>

static FBAccAutoEvent
MakeAccAutoEvent(
  int param, double value, int pos)
{
  FBAccAutoEvent result;
  result.pos = pos;
  result.param = param;
  result.value = (float)value;
  return result;
}

static FBAccModEvent
MakeAccModEvent(
  int param, clap_event_param_mod const* event)
{
  FBAccModEvent result;
  result.param = param;
  result.pos = event->header.time;
  result.note.key = event->key;
  result.note.id = event->note_id;
  result.note.channel = event->channel;
  result.value = (float)event->amount;
  return result;
}

static FBBlockEvent
MakeBlockEvent(
  FBStaticParam const& topo, int param, double value)
{
  FBBlockEvent result;
  result.param = param;
  result.normalized = FBCLAPToNormalized(topo, value);
  return result;
}

static FBNoteEvent
MakeNoteEvent(
  clap_event_header_t const* header)
{
  FBNoteEvent result;
  auto event = reinterpret_cast<clap_event_note_t const*>(header);
  result.pos = header->time;
  result.note.key = event->key;
  result.note.id = event->note_id;
  result.note.channel = event->channel;
  result.on = header->type == CLAP_EVENT_NOTE_ON;
  result.velo = (float)event->velocity;
  return result;
}

static clap_event_note 
MakeNoteEndEvent(FBNote const& note, int time)
{
  clap_event_note result = {};
  result.port_index = 0;
  result.velocity = 0.0f;
  result.key = note.key;
  result.note_id = note.id;
  result.channel = note.channel;
  result.header.flags = 0;
  result.header.time = time;
  result.header.type = CLAP_EVENT_NOTE_END;
  result.header.size = sizeof(clap_event_note);
  result.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
  return result;
}

static clap_event_param_value
MakeValueEvent(int paramTag, float value)
{
  clap_event_param_value result = {};
  result.value = value;
  result.param_id = paramTag;
  result.header.time = 0;
  result.header.flags = 0;
  result.header.type = CLAP_EVENT_PARAM_VALUE;
  result.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
  result.header.size = sizeof(clap_event_param_value);
  return result;
}

static clap_event_param_gesture
MakeGestureEvent(int paramTag, bool begin)
{
  clap_event_param_gesture result = {};
  result.param_id = paramTag;
  result.header.time = 0;
  result.header.flags = 0;
  result.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
  result.header.size = sizeof(clap_event_param_gesture);
  result.header.type = CLAP_EVENT_PARAM_GESTURE_BEGIN;
  if (!begin) result.header.type = CLAP_EVENT_PARAM_GESTURE_END;
  return result;
}

FBCLAPPlugin::
~FBCLAPPlugin() {}

FBCLAPPlugin::
FBCLAPPlugin(
  FBStaticTopo const& topo,
  clap_plugin_descriptor const* desc, 
  clap_host const* host):
Plugin(desc, host),
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_procState(*_topo),
_guiState(*_topo),
_audioToMainEvents(FBCLAPSyncEventReserve - 1),
_mainToAudioEvents(FBCLAPSyncEventReserve - 1) {}

float
FBCLAPPlugin::GetParamNormalized(int index) const
{
  return *_guiState.Params()[index];
}

void 
FBCLAPPlugin::ProcessVoices()
{
  // todo maybe drop this ?
  if (!_host.canUseThreadPool() ||
    !_host.threadPoolRequestExec(FBMaxVoices))
    _hostProcessor->ProcessAllVoices();
}

void 
FBCLAPPlugin::EndParamChange(int index)
{
  FBCLAPSyncToAudioEvent event;
  event.value = 0.0f;
  event.paramIndex = index;
  event.type = FBCLAPSyncEventType::EndChange;
  _mainToAudioEvents.enqueue(event);
}

void 
FBCLAPPlugin::BeginParamChange(int index)
{
  FBCLAPSyncToAudioEvent event;
  event.value = 0.0f;
  event.paramIndex = index;
  event.type = FBCLAPSyncEventType::BeginChange;
  _mainToAudioEvents.enqueue(event);
}

void 
FBCLAPPlugin::PerformParamEdit(int index, float normalized)
{  
  FBCLAPSyncToAudioEvent event;
  event.value = normalized;
  event.paramIndex = index;
  event.type = FBCLAPSyncEventType::PerformEdit;
  _mainToAudioEvents.enqueue(event);
  *_guiState.Params()[index] = normalized;
}

int32_t 
FBCLAPPlugin::getParamIndexForParamId(
  clap_id paramId) const noexcept
{
  auto iter = _topo->paramTagToIndex.find(paramId);
  return iter == _topo->paramTagToIndex.end() ? -1 : iter->second;
}

bool 
FBCLAPPlugin::getParamInfoForParamId(
  clap_id paramId, clap_param_info* info) const noexcept
{
  auto iter = _topo->paramTagToIndex.find(paramId);
  if (iter == _topo->paramTagToIndex.end())
    return false;
  return paramsInfo(iter->second, info);
}

bool
FBCLAPPlugin::isValidParamId(
  clap_id paramId) const noexcept
{
  return _topo->paramTagToIndex.find(paramId) != _topo->paramTagToIndex.end();
}

bool
FBCLAPPlugin::activate(
  double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept 
{
  float fSampleRate = (float)sampleRate;
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(maxFrameCount, 0.0f);
  auto plug = MakePlugProcessor(_topo->static_, _procState.Raw(), fSampleRate);
  _hostProcessor.reset(new FBHostProcessor(this, _topo->static_, std::move(plug), &_procState, fSampleRate));
  return true;
}

clap_process_status 
FBCLAPPlugin::process(
  const clap_process* process) noexcept 
{
  clap_event_param_mod const* modFromHost;
  clap_event_param_value const* valueFromHost;
  std::unordered_map<int, int>::const_iterator iter;

  auto inEvents = process->in_events;
  auto outEvents = process->out_events;
  int inEventCount = inEvents->size(inEvents);
  auto& accAuto = _input.accAutoByParamThenSample;
  auto& accMod = _input.accModByParamThenNoteThenSample;

  accMod.clear();
  accAuto.clear();
  _input.note.clear();
  _input.block.clear();

  auto pushParamChangeToProcessor = [&](int index, double value, int pos) {
    auto const& static_ = _topo->params[index].static_;
    if (static_.acc)
      accAuto.push_back(MakeAccAutoEvent(index, value, pos));
    else
      _input.block.push_back(MakeBlockEvent(static_, index, value)); };

  int paramTag;
  bool gestureBegin;
  FBCLAPSyncToAudioEvent uiEvent;
  clap_event_param_value valueToHost;
  clap_event_param_gesture gestureToHost;
  while (_mainToAudioEvents.try_dequeue(uiEvent))
  {
    int paramTag = _topo->params[uiEvent.paramIndex].tag;
    switch (uiEvent.type)
    {
    case FBCLAPSyncEventType::EndChange:
    case FBCLAPSyncEventType::BeginChange:
      gestureBegin = uiEvent.type == FBCLAPSyncEventType::BeginChange;
      gestureToHost = MakeGestureEvent(paramTag, gestureBegin);
      outEvents->try_push(outEvents, &gestureToHost.header);
      break;
    case FBCLAPSyncEventType::PerformEdit:
      valueToHost = MakeValueEvent(paramTag, uiEvent.value);
      outEvents->try_push(outEvents, &valueToHost.header);
      pushParamChangeToProcessor(uiEvent.paramIndex, uiEvent.value, 0);
      break;
    default:
      assert(false);
      break;
    }
  }

  for (int i = 0; i < inEventCount; i++)
  {
    auto header = inEvents->get(inEvents, i);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID)
      continue;
    switch (header->type)
    {
    case CLAP_EVENT_NOTE_ON: 
    case CLAP_EVENT_NOTE_OFF: 
      _input.note.push_back(MakeNoteEvent(header)); 
      break;
    case CLAP_EVENT_PARAM_MOD:
      modFromHost = reinterpret_cast<clap_event_param_mod const*>(header);
      if ((iter = _topo->paramTagToIndex.find(modFromHost->param_id)) != _topo->paramTagToIndex.end())
        if (_topo->params[iter->second].static_.acc)
          accMod.push_back(MakeAccModEvent(iter->second, modFromHost));
      break;
    case CLAP_EVENT_PARAM_VALUE:
      valueFromHost = reinterpret_cast<clap_event_param_value const*>(header);
      if ((iter = _topo->paramTagToIndex.find(valueFromHost->param_id)) != _topo->paramTagToIndex.end())
        pushParamChangeToProcessor(iter->second, valueFromHost->value, valueFromHost->header.time);
      break;
    default:
      break;
    }
  }

  float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
  if (process->audio_inputs_count != 1)
    _input.audio = FBHostAudioBlock(zeroIn, process->frames_count);
  else
    _input.audio = FBHostAudioBlock(process->audio_inputs[0].data32, process->frames_count);
  _output.audio = FBHostAudioBlock(process->audio_outputs[0].data32, process->frames_count);
  std::sort(accAuto.begin(), accAuto.end(), FBAccAutoEventOrderByParamThenPos);
  std::sort(accMod.begin(), accMod.end(), FBAccModEventOrderByParamThenNoteThenPos);

  _hostProcessor->ProcessHost(_input, _output);
  
  auto const& rvs = _output.returnedVoices;
  for (int rv = 0; rv < rvs.size(); rv++)
  {
    clap_event_note endEvent = MakeNoteEndEvent(rvs[rv], process->frames_count - 1);
    process->out_events->try_push(process->out_events, &(endEvent.header));
  }
  return true;
}