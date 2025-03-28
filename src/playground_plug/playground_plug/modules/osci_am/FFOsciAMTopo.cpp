#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci_am/FFOsciAMTopo.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule> 
FFMakeOsciAMTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc AM";
  result->slotCount = 1;
  result->id = "{DA00F5AC-B2EF-49DB-A8C6-81145ACA2803}";
  result->params.resize((int)FFOsciAMParam::Count);
  result->addrSelectors.voiceModuleExchange = FFSelectVoiceModuleExchangeAddr([](auto& state) { return &state.osciAM; });
  auto selectModule = [](auto& state) { return &state.voice.osciAM; };

  auto& on = result->params[(int)FFOsciAMParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = FFOsciModSlotCount;
  on.id = "{F10BBAB2-F179-496F-9A55-68545E734EF6}";
  on.type = FBParamType::Boolean;
  on.slotFormatter = [](int slot) { return FFOsciModMakeSourceAndTargetText(slot); };
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceBlockProc = FFSelectProcParamAddr(selectModule, selectOn);
  on.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& mix = result->params[(int)FFOsciAMParam::Mix];
  mix.acc = true;
  mix.defaultText = "100";
  mix.name = "Mix";
  mix.slotCount = FFOsciModSlotCount;
  mix.unit = "%";
  mix.id = "{84CF7CE6-CFC4-43F7-8EF7-18D9C279D205}";
  mix.type = FBParamType::Identity;
  mix.slotFormatter = [name = mix.name](int slot) { return name + " " + FFOsciModMakeSourceAndTargetText(slot); };
  auto selectMix = [](auto& module) { return &module.acc.mix; };
  mix.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectMix);
  mix.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectMix);
  mix.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectMix);
  mix.dependencies.enabled.audio.When({ (int)FFOsciAMParam::On }, [](auto const& vs) { return vs[0] != 0; });

  auto& ring = result->params[(int)FFOsciAMParam::Ring];
  ring.acc = true;
  ring.defaultText = "0";
  ring.name = "Ring";
  ring.slotCount = FFOsciModSlotCount;
  ring.unit = "%";
  ring.id = "{B17630C5-6FA6-410B-84F3-EEAC4A947658}";
  ring.type = FBParamType::Identity;
  ring.slotFormatter = [name = ring.name](int slot) { return name + " " + FFOsciModMakeSourceAndTargetText(slot); };
  auto selectRing = [](auto& module) { return &module.acc.ring; };
  ring.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectRing);
  ring.addrSelectors.voiceAccProc = FFSelectProcParamAddr(selectModule, selectRing);
  ring.addrSelectors.voiceExchange = FFSelectExchangeParamAddr(selectModule, selectRing);
  ring.dependencies.enabled.audio.When({ (int)FFOsciAMParam::On }, [](auto const& vs) { return vs[0] != 0; });

  return result;
}