#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/env/FFEnvGraph.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>
#include <playground_base/gui/shared/FBGUIGraphing.hpp>

#include <algorithm>

void
FFEnvRenderGraph(FBModuleGraphComponentData* graphData)
{
  FBModuleGraphRenderData<FFEnvProcessor> renderData = {};
  renderData.graphData = graphData;
  renderData.staticModuleIndex = (int)FFModuleType::Env;
  renderData.voiceExchangeSelector = [](void const* exchangeState, int voice, int slot) {
    return &static_cast<FFExchangeState const*>(exchangeState)->voice[voice].env[slot]; };
  renderData.voiceOutputSelector = [](void const* procState, int voice, int slot) {
    return &static_cast<FFProcState const*>(procState)->dsp.voice[voice].env[slot].output; };
  FBRenderModuleGraph<false>(renderData);
}