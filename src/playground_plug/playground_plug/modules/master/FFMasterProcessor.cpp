#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/master/FFMasterTopo.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <playground_base/dsp/plug/FBPlugBlock.hpp>
#include <playground_base/dsp/voice/FBVoiceManager.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

void
FFMasterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto& output = procState->dsp.global.master.output;
  auto const& input = procState->dsp.global.master.input;
  auto const& procParams = procState->param.global.master[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Master];
  auto const& gainNorm = procParams.acc.gain[0].Global();

  for (int s = 0; s < FBFixedBlockSamples; s += FBSIMDFloatCount)
  {
    auto gainPlain = topo.NormalizedToIdentityFast(FFMasterParam::Gain, gainNorm, s);
    for (int ch = 0; ch < 2; ch++)
      output[ch].Store(s, input[ch].Load(s) * gainPlain);
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeDSP = exchangeToGUI->global.master[state.moduleSlot];
  exchangeDSP.active = true;

  auto& exchangeParams = exchangeToGUI->param.global.master[state.moduleSlot];
  exchangeParams.acc.gain[0] = gainNorm.Last();
}