#include <firefly_synth/shared/FFPlugState.hpp>
#include <firefly_synth/modules/effect/FFEffectProcessor.hpp>
#include <firefly_synth/modules/effect/FFEffectGraph.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/shared/FBGraphing.hpp>
#include <firefly_base/gui/glue/FBPlugGUIContext.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>

#include <bit>
#include <algorithm>

struct EffectGraphRenderData final:
public FBModuleGraphRenderData<EffectGraphRenderData>
{
  int totalSamples = {};
  std::array<int, FFEffectBlockCount + 1> samplesProcessed = {};

  FFEffectProcessor& GetProcessor(FBModuleProcState& state);
  int DoProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoBeginVoice(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice);
  void DoProcessIndicators(int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points) {}
  void DoPostProcess(FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice, FBModuleGraphPoints& points);
};

static FBModuleGraphPlotParams
PlotParams(FBModuleGraphComponentData const* data)
{
  // Need to know SR for graphing because filters.
  // Plot a bit more than exact pixel width to make it look prettier.
  FBModuleGraphPlotParams result = {};
  result.autoSampleRate = false;
  result.sampleCount = data->pixelWidth * 4;
  result.sampleRate = data->pixelWidth * 4.0f / FFEffectPlotLengthSeconds;
  return result;
}

FFEffectProcessor&
EffectGraphRenderData::GetProcessor(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& processor = *procState->dsp.voice[state.voice->slot].effect[state.moduleSlot].processor;
  processor.InitializeBuffers(true, state.input->sampleRate);
  return processor;
}

void 
EffectGraphRenderData::DoBeginVoice(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{ 
  samplesProcessed[graphIndex] = 0;
  auto* moduleProcState = state->ModuleProcState();
  GetProcessor(*moduleProcState).BeginVoice(true, graphIndex, totalSamples, *moduleProcState);
}

void
EffectGraphRenderData::DoPostProcess(
  FBGraphRenderState* state, int graphIndex,
  bool exchange, int exchangeVoice, FBModuleGraphPoints& points)
{
  if (graphIndex == FFEffectBlockCount)
    return;

  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Kind, graphIndex };
  auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
  if (kind != FFEffectKind::StVar && kind != FFEffectKind::Comb)
    return;

  auto nextPow2 = std::bit_ceil(points.l.size());
  int order = std::bit_width(nextPow2) - 1;
  // todo reuse?
  juce::dsp::FFT fft(order);
  points.l.resize(nextPow2 * 2);
  fft.performFrequencyOnlyForwardTransform(points.l.data(), true);
  points.l.resize(points.l.size() / 4);

  float min = 0.0f;
  float max = 1.0f;
  for (int i = 0; i < points.l.size(); i++)
  {
    min = std::min(min, points.l[i]);
    max = std::max(max, points.l[i]);
  }

  for (int i = 0; i < points.l.size(); i++)
  {
    points.l[i] = FBToBipolar(points.l[i] / (max - min) - min);
    assert(-1.01f <= points.l[i] && points.l[i] <= 1.01f);
  }
}

int 
EffectGraphRenderData::DoProcess(
  FBGraphRenderState* state, int graphIndex, bool exchange, int exchangeVoice)
{
  bool plotSpecificFilter = false;
  auto* moduleProcState = state->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::On, 0 };
  bool on = state->AudioParamBool(indices, false, -1);
  if (!on)
    return 0;

  if (graphIndex != FFEffectBlockCount)
  {
    indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Kind, graphIndex };
    auto kind = state->AudioParamList<FFEffectKind>(indices, exchange, exchangeVoice);
    plotSpecificFilter = kind == FFEffectKind::StVar || kind == FFEffectKind::Comb;
    if (kind == FFEffectKind::Off)
      return 0;
  }

  auto* procState = moduleProcState->ProcAs<FFProcState>();
  auto& input = procState->dsp.voice[moduleProcState->voice->slot].effect[moduleProcState->moduleSlot].input;
  for (int c = 0; c < 2; c++)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      if (plotSpecificFilter)
        input[c].Set(s, (samplesProcessed[graphIndex] + s) == 0 ? 1.0f : 0.0f);
      else
        input[c].Set(s, ((samplesProcessed[graphIndex] + s) / static_cast<float>(totalSamples)) * 2.0f - 1.0f);
  
  samplesProcessed[graphIndex] += FBFixedBlockSamples;
  return GetProcessor(*moduleProcState).Process(*moduleProcState);
}

void
FFEffectRenderGraph(FBModuleGraphComponentData* graphData)
{
  EffectGraphRenderData renderData = {};
  graphData->bipolar = true;
  graphData->drawClipBoundaries = true;
  graphData->skipDrawOnEqualsPrimary = false; // midi note dependent
  renderData.graphData = graphData;
  renderData.plotParamsSelector = PlotParams;
  renderData.totalSamples = PlotParams(graphData).sampleCount;
  renderData.staticModuleIndex = (int)FFModuleType::Effect;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].effect[slot]; };
  renderData.voiceMonoOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].effect[slot].output[0]; };

  auto* renderState = graphData->renderState;
  auto* moduleProcState = renderState->ModuleProcState();
  int moduleSlot = moduleProcState->moduleSlot;
  FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::On, 0 };
  bool on = renderState->AudioParamBool(indices, false, -1);
  for (int i = 0; i <= FFEffectBlockCount; i++)
  {
    FBRenderModuleGraph<false, false>(renderData, i);
    if (i == FFEffectBlockCount)
      graphData->graphs[i].text = on? "ALL": "ALL OFF";
    else
    {
      FBParamTopoIndices indices = { (int)FFModuleType::Effect, moduleSlot, (int)FFEffectParam::Kind, i };
      auto kind = renderState->AudioParamList<FFEffectKind>(indices, false, -1);
      bool blockOn = on && kind != FFEffectKind::Off;
      graphData->graphs[i].text = std::to_string(i + 1);
      if (!blockOn)
        graphData->graphs[i].text += " OFF";
    }
  }
}