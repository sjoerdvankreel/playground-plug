#include <playground_plug/plug/shared/FFPluginTopo.hpp>
#include <playground_plug_vst3/FFVST3PluginProcessor.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <algorithm>

static FFBlockParamEvent
MakeBlockParamEvent(int tag, ParamValue value)
{
  FFBlockParamEvent result;
  result.tag = tag;
  result.normalized = value;
  return result;
}

static FFAccParamEvent
MakeAccParamEvent(int tag, int position, ParamValue value)
{
  FFAccParamEvent result;
  result.tag = tag;
  result.normalized = value;
  result.position = position;
  return result;
}

static FFNoteEvent
MakeNoteOnEvent(Event const& event)
{
  FFNoteEvent result;
  result.on = true;
  result.velo = event.noteOn.velocity;
  result.position = event.sampleOffset;
  result.note.id = event.noteOn.noteId;
  result.note.key = event.noteOn.pitch;
  result.note.channel = event.noteOn.channel;
  return result;
}

static FFNoteEvent
MakeNoteOffEvent(Event const& event)
{
  FFNoteEvent result;
  result.on = false;
  result.position = event.sampleOffset;
  result.velo = event.noteOff.velocity;
  result.note.id = event.noteOff.noteId;
  result.note.key = event.noteOff.pitch;
  result.note.channel = event.noteOff.channel;
  return result;
}

static FFRawStereoBlockView
MakeRawStereoBlockView(AudioBusBuffers& buffers, int sampleCount)
{
  return FFRawStereoBlockView(
    buffers.channelBuffers32[FF_CHANNEL_L],
    buffers.channelBuffers32[FF_CHANNEL_R],
    sampleCount);
}

FFVST3PluginProcessor::
FFVST3PluginProcessor(FUID const& controllerId):
_topo(FFMakeTopo())
{
  setControllerClass(controllerId);
}

tresult PLUGIN_API
FFVST3PluginProcessor::initialize(FUnknown* context)
{
  if (AudioEffect::initialize(context) != kResultTrue)
    return kResultFalse;
  addEventInput(STR16("Event In"));
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3PluginProcessor::canProcessSampleSize(int32 symbolicSize)
{
  if (symbolicSize == kSample32) 
    return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API
FFVST3PluginProcessor::setupProcessing(ProcessSetup& setup)
{
  _input.reset(new FFHostInputBlock(nullptr, nullptr, 0));
  _zeroIn.reset(new FFDynamicStereoBlock(setup.maxSamplesPerBlock));
  _processor.reset(new FFPluginProcessor(setup.maxSamplesPerBlock, setup.sampleRate));
  return kResultTrue;
}

tresult PLUGIN_API
FFVST3PluginProcessor::setBusArrangements(
  SpeakerArrangement* inputs, int32 numIns, SpeakerArrangement* outputs, int32 numOuts)
{
  if (numIns != 0 || numOuts != 1 || outputs[0] != SpeakerArr::kStereo)
    return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
}

tresult PLUGIN_API
FFVST3PluginProcessor::process(ProcessData& data)
{
  _input->audio = _zeroIn->GetRawBlockView();
  _output = MakeRawStereoBlockView(data.outputs[0], data.numSamples);
  if (data.numInputs == 1)
    MakeRawStereoBlockView(data.inputs[0], data.numSamples);

  Event event;
  _input->events.note.clear();
  if (data.inputEvents != nullptr)
    for (int i = 0; i < data.inputEvents->getEventCount(); i++)
      if (data.inputEvents->getEvent(i, event) == kResultOk)
        if (event.type == Event::kNoteOnEvent)
          _input->events.note.push_back(MakeNoteOnEvent(event));
        else if (event.type == Event::kNoteOffEvent)
          _input->events.note.push_back(MakeNoteOffEvent(event));

  int position;
  ParamValue value;
  IParamValueQueue* queue;
  std::map<int, int>::const_iterator iter;
  _input->events.accParam.clear();
  _input->events.blockParam.clear();
  if(data.inputParameterChanges != nullptr)
    for (int p = 0; p < data.inputParameterChanges->getParameterCount(); p++)
      if ((queue = data.inputParameterChanges->getParameterData(p)) != nullptr)
        if ((iter = _topo.tagToBlockParam.find(queue->getParameterId())) != _topo.tagToBlockParam.end())
        {
          if (queue->getPoint(queue->getPointCount() - 1, position, value) == kResultTrue)
            _input->events.blockParam.push_back(MakeBlockParamEvent(queue->getParameterId(), value));
        } else if ((iter = _topo.tagToAccParam.find(queue->getParameterId())) != _topo.tagToAccParam.end())
        {
          for (int point = 0; point < queue->getPointCount(); point++)
            if (queue->getPoint(point, position, value) == kResultTrue)
              _input->events.accParam.push_back(MakeAccParamEvent(queue->getParameterId(), position, value));
        }     

  auto compare = [](auto const& l, auto const& r) { 
    if (l.position == r.position)
      return l.tag < r.tag;
    return l.position < r.position; };
  std::sort(_input->events.accParam.begin(), _input->events.accParam.end(), compare);
  _processor->ProcessHost(*_input, _output);  
  return kResultTrue;
}