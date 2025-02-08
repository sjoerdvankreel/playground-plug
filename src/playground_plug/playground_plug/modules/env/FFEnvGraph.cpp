#include <playground_plug/gui/FFGUIUtility.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/base/state/main/FBGraphRenderState.hpp>
#include <playground_base/base/state/exchange/FBExchangeStateContainer.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <algorithm>

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  // todo probably need to share some of this

  float const sampleRate = 100.0f;
  auto renderState = graphData->renderState;
  auto& moduleState = renderState->ModuleState();
  moduleState.sampleRate = sampleRate;
  int moduleSlot = moduleState.moduleSlot;

  FFModuleGraphRenderData<FFEnvProcessor> renderData;
  renderData.graphData = graphData;
  renderData.voiceOutputSelector = [](auto const& dsp, int v, int slot) { 
    return &dsp.voice[v].env[slot].output; };

  renderState->PrepareForRenderPrimary();
  renderState->PrepareForRenderPrimaryVoice();
  FFRenderModuleGraph<false>(renderData, graphData->primarySeries);

  renderState->PrepareForRenderExchange();
  auto exchangeState = renderState->ExchangeState<FFExchangeState>();
  for (int v = 0; v < FBMaxVoices; v++)
  {
    auto const& envExchange = exchangeState->voice[v].env[moduleSlot];
    if (!envExchange.active || envExchange.positionSamples >= envExchange.lengthSamples)
      continue;
    renderState->PrepareForRenderExchangeVoice(v);
    float positionNormalized = envExchange.positionSamples / (float)envExchange.lengthSamples;
    if (renderState->VoiceModuleExchangeStateEqualsPrimary(v, (int)FFModuleType::Env, moduleSlot))
    {
      graphData->primaryMarkers.push_back((int)(positionNormalized * graphData->primarySeries.size()));
      assert(graphData->primaryMarkers[graphData->primaryMarkers.size() - 1] < graphData->primarySeries.size());
      continue;
    }
    auto& secondary = graphData->secondarySeries.emplace_back();
    FFRenderModuleGraph<false>(renderData, secondary.points);
    secondary.marker = (int)(positionNormalized * secondary.points.size());
    assert(secondary.marker < secondary.points.size());
  }

  float durationSections = renderData.graphData->primarySeries.size() / sampleRate;
  renderData.graphData->text = FBFormatFloat(durationSections, FBDefaultDisplayPrecision) + " Sec";
}