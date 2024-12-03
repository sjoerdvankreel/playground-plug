#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_base/base/topo/FBStaticTopo.hpp>

template <class State>
static auto SelectAddr(auto selectModule, auto selectParam)
{
  return [selectModule, selectParam](int moduleSlot, int paramSlot, void* state) {
    auto moduleState = selectModule(*static_cast<State*>(state));
    return &(*selectParam((*moduleState)[moduleSlot]))[paramSlot]; };
}

std::unique_ptr<FBStaticTopo>
FFMakeTopo()
{
  auto result = std::make_unique<FBStaticTopo>();
  result->modules.resize(FFModuleCount);
  
  auto& osci = result->modules[FFModuleOsci];
  auto selectOsci = [](auto& state) { return &state.osci; };
  osci.name = "Osc";
  osci.slotCount = FF_OSCI_COUNT;
  osci.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  osci.acc.resize(FFOsciAccCount);
  osci.block.resize(FFOsciBlockCount);

  auto& osciOn = osci.block[FFOsciBlockOn];
  osciOn.name = "On";
  osciOn.slotCount = 1;
  osciOn.valueCount = 2;
  osciOn.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";
  auto selectOsciOn = [](auto& module) { return &module.block.on; };
  osciOn.scalarAddr = SelectAddr<FFScalarState>(selectOsci, selectOsciOn);
  osciOn.procBlockAddr = SelectAddr<FFProcState>(selectOsci, selectOsciOn);

  auto& osciType = osci.block[FFOsciBlockType];
  osciType.name = "Type";
  osciType.slotCount = 1;
  osciType.valueCount = FFOsciTypeCount;
  osciType.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  osciType.list = {
    { "{2400822D-BFA9-4A43-91E8-2849756DE659}", "Sine" },
    { "{ECE0331E-DD96-446E-9CCA-5B89EE949EB4}", "Saw" } };
  auto selectOsciType = [](auto& module) { return &module.block.type; };
  osciType.scalarAddr = SelectAddr<FFScalarState>(selectOsci, selectOsciType);
  osciType.procBlockAddr = SelectAddr<FFProcState>(selectOsci, selectOsciType);

  auto& osciGain = osci.acc[FFOsciAccGain];
  osciGain.name = "Gain";
  osciGain.slotCount = FF_OSCI_GAIN_COUNT;
  osciGain.valueCount = 0;
  osciGain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  auto selectOsciGain = [](auto& module) { return &module.acc.gain; };
  osciGain.procAccAddr = SelectAddr<FFProcState>(selectOsci, selectOsciGain);
  osciGain.scalarAddr = SelectAddr<FFScalarState>(selectOsci, selectOsciGain);

  auto& osciPitch = osci.acc[FFOsciAccPitch];
  osciPitch.name = "Pitch";
  osciPitch.slotCount = 1;
  osciPitch.valueCount = 0;
  osciPitch.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  auto selectOsciPitch = [](auto& module) { return &module.acc.pitch; };
  osciPitch.procAccAddr = SelectAddr<FFProcState>(selectOsci, selectOsciPitch);
  osciPitch.scalarAddr = SelectAddr<FFScalarState>(selectOsci, selectOsciPitch);

  auto& shaper = result->modules[FFModuleShaper];
  auto selectShaper = [](auto& state) { return &state.shaper; };
  shaper.name = "Shaper";
  shaper.slotCount = FF_SHAPER_COUNT;
  shaper.id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  shaper.acc.resize(FFShaperAccCount);
  shaper.block.resize(FFShaperBlockCount);

  auto& shaperOn = shaper.block[FFShaperBlockOn];
  shaperOn.name = "On";
  shaperOn.slotCount = 1;
  shaperOn.valueCount = 2;
  shaperOn.id = "{BF67A27A-97E9-4640-9E57-B1E04D195ACC}";
  auto selectShaperOn = [](auto& module) { return &module.block.on; };
  shaperOn.scalarAddr = SelectAddr<FFScalarState>(selectShaper, selectShaperOn);
  shaperOn.procBlockAddr = SelectAddr<FFProcState>(selectShaper, selectShaperOn);

  auto& shaperClip = shaper.block[FFShaperBlockClip];
  shaperClip.name = "Clip";
  shaperClip.slotCount = 1;
  shaperClip.valueCount = 2;
  shaperClip.id = "{81C7442E-4064-4E90-A742-FDDEA84AE1AC}";
  auto selectShaperClip = [](auto& module) { return &module.block.clip; };
  shaperClip.scalarAddr = SelectAddr<FFScalarState>(selectShaper, selectShaperClip);
  shaperClip.procBlockAddr = SelectAddr<FFProcState>(selectShaper, selectShaperClip);

  auto& shaperGain = shaper.acc[FFShaperAccGain];
  shaperGain.name = "Gain";
  shaperGain.slotCount = 1;
  shaperGain.valueCount = 2;
  shaperGain.id = "{12989CF4-2941-4E76-B8CF-B3F4E2F73B68}";
  auto selectShaperGain = [](auto& module) { return &module.acc.gain; };
  shaperGain.procAccAddr = SelectAddr<FFProcState>(selectShaper, selectShaperGain);
  shaperGain.scalarAddr = SelectAddr<FFScalarState>(selectShaper, selectShaperGain);

  return result;
}
