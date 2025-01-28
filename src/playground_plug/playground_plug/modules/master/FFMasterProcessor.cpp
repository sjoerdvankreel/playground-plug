#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

void
FFMasterProcessor::Process(FBModuleProcState const& state)
{
  auto* procState = state.ProcState<FFProcState>();
  auto& output = procState->dsp.global.master.output;
  auto const& input = procState->dsp.global.master.input;
  auto const& params = procState->param.global.master[state.moduleSlot];
  output.Transform([&](int ch, int v) {
    auto gain = params.acc.gain[0].Global().CV(v);
    return input[ch][v] * gain; });
  auto const* voicesParam = state.topo->ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Voices, 0 });
  (*state.outputParamsNormalized)[voicesParam->runtimeParamIndex] = 
    voicesParam->static_.discrete.PlainToNormalized(state.input->voiceManager->VoiceCount());
}