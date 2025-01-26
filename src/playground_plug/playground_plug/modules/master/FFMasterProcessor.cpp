#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

void
FFMasterProcessor::Process(
  FFModuleProcState const& state, 
  std::vector<float>& outputParamsNormalized)
{
  auto& output = state.proc->dsp.global.master.output;
  auto const& input = state.proc->dsp.global.master.input;
  auto const& params = state.proc->param.global.master[state.moduleSlot];
  output.Transform([&](int ch, int v) {
    auto gain = params.acc.gain[0].Global().CV(v);
    return input[ch][v] * gain; });
  auto const* voicesParam = state.topo->ParamAtTopo({ (int)FFModuleType::Master, 0, (int)FFMasterParam::Voices, 0 });
  outputParamsNormalized[voicesParam->runtimeParamIndex] = 
    voicesParam->static_.discrete.PlainToNormalized(state.input->voiceManager->VoiceCount());
}