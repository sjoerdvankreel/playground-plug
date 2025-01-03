#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugProcessor.hpp>

#include <clap/helpers/plugin.hxx>
#include <algorithm>

static FBBlockEvent
MakeBlockEvent(
  FBStaticParam const& topo, int param, double value)
{
  FBBlockEvent result;
  result.param = param;
  result.normalized = FBCLAPToNormalized(topo, value);
  return result;
}

static FBAccAutoEvent
MakeAccAutoEvent(
  int param, clap_event_param_value const* event)
{
  FBAccAutoEvent result;
  result.param = param;
  result.pos = event->header.time;
  result.value = static_cast<float>(event->value);
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
  result.value = static_cast<float>(event->amount);
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
  result.velo = static_cast<float>(event->velocity);
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
_state(*_topo) {}

void 
FBCLAPPlugin::ProcessVoices()
{
  if (!_host.canUseThreadPool() ||
    !_host.threadPoolRequestExec(FBMaxVoices))
    _hostProcessor->ProcessAllVoices();
}

bool 
FBCLAPPlugin::isValidParamId(
  clap_id paramId) const noexcept
{
  return _topo->tagToParam.find(paramId) != _topo->tagToParam.end();
}

int32_t 
FBCLAPPlugin::getParamIndexForParamId(
  clap_id paramId) const noexcept
{
  auto iter = _topo->tagToParam.find(paramId);
  return iter == _topo->tagToParam.end() ? -1 : iter->second;
}

bool 
FBCLAPPlugin::getParamInfoForParamId(
  clap_id paramId, clap_param_info* info) const noexcept
{
  auto iter = _topo->tagToParam.find(paramId);
  if (iter == _topo->tagToParam.end())
    return false;
  return paramsInfo(iter->second, info);
}

bool
FBCLAPPlugin::activate(
  double sampleRate, uint32_t minFrameCount, uint32_t maxFrameCount) noexcept 
{
  float fSampleRate = static_cast<float>(sampleRate);
  for (int ch = 0; ch < 2; ch++)
    _zeroIn[ch] = std::vector<float>(maxFrameCount, 0.0f);
  auto plug = MakePlugProcessor(_topo->static_, _state.Raw(), fSampleRate);
  _hostProcessor.reset(new FBHostProcessor(this, _topo->static_, std::move(plug), &_state, fSampleRate));
  return true;
}

clap_process_status 
FBCLAPPlugin::process(
  const clap_process* process) noexcept 
{
  auto events = process->in_events;
  int eventCount = events->size(events);
  auto& accAuto = _input.accAutoByParamThenSample;
  auto& accMod = _input.accModByParamThenNoteThenSample;

  clap_event_param_mod const* mod;
  clap_event_param_value const* value;
  std::unordered_map<int, int>::const_iterator iter;

  accMod.clear();
  accAuto.clear();
  _input.note.clear();
  _input.block.clear();

  for (int i = 0; i < eventCount; i++)
  {
    auto header = events->get(events, i);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID)
      continue;
    switch (header->type)
    {
    case CLAP_EVENT_NOTE_ON: 
    case CLAP_EVENT_NOTE_OFF: 
      _input.note.push_back(MakeNoteEvent(header)); 
      break;
    case CLAP_EVENT_PARAM_MOD:
      mod = reinterpret_cast<clap_event_param_mod const*>(header);
      if ((iter = _topo->tagToParam.find(mod->param_id)) != _topo->tagToParam.end())
        if (_topo->params[iter->second].static_.acc)
          accMod.push_back(MakeAccModEvent(iter->second, mod));
      break;
    case CLAP_EVENT_PARAM_VALUE:
      value = reinterpret_cast<clap_event_param_value const*>(header);
      if ((iter = _topo->tagToParam.find(value->param_id)) != _topo->tagToParam.end())
      {
        auto const& static_ = _topo->params[iter->second].static_;
        if (static_.acc)
          accAuto.push_back(MakeAccAutoEvent(iter->second, value));
        else
          _input.block.push_back(MakeBlockEvent(static_, iter->second, value->value));
      }
      break;
    default:
      break;
    }
  }

  std::sort(accAuto.begin(), accAuto.end(), FBAccAutoEventOrderByParamThenPos);
  std::sort(accMod.begin(), accMod.end(), FBAccModEventOrderByParamThenNoteThenPos);

  float* zeroIn[2] = { _zeroIn[0].data(), _zeroIn[1].data() };
  if (process->audio_inputs_count != 1)
    _input.audio = FBHostAudioBlock(zeroIn, process->frames_count);
  else
    _input.audio = FBHostAudioBlock(process->audio_inputs[0].data32, process->frames_count);
  _output.audio = FBHostAudioBlock(process->audio_outputs[0].data32, process->frames_count);
  _hostProcessor->ProcessHost(_input, _output);
  
  auto const& rvs = _output.returnedVoices;
  for (int rv = 0; rv < rvs.size(); rv++)
  {
    clap_event_note event = {};
    event.port_index = 0;
    event.velocity = 0.0f;
    event.key = rvs[rv].key;
    event.note_id = rvs[rv].id;
    event.channel = rvs[rv].channel;
    event.header.flags = 0;
    event.header.type = CLAP_EVENT_NOTE_END;
    event.header.size = sizeof(clap_event_note);
    event.header.time = process->frames_count - 1;
    event.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
    process->out_events->try_push(process->out_events, &(event.header));
  }

  return true;
}