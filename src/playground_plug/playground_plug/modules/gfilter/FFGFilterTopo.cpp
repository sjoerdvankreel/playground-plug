#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFTopoDetail.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

FBStaticModule
FFMakeGFilterTopo()
{
  FBStaticModule result = {};
  result.voice = false;
  result.name = "GFilter";
  result.slotCount = FFGFilterCount;
  result.id = "{290E86EF-DFE9-4A3C-B6B2-9063643DD0E8}";
  result.params.resize(FFGFilterParamCount);
  auto selectModule = [](auto& state) { return &state.global.gFilter; };

  auto& on = result.params[FFGFilterBlockOn];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.valueCount = 2;
  on.id = "{B9DF9543-5115-4D9C-89DD-62D5D495DBF8}";
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  on.globalBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& type = result.params[FFGFilterBlockType];
  type.acc = false;
  type.name = "Type";
  type.slotCount = 1;
  type.valueCount = 2;
  type.id = "{503DECBB-EE24-4EC9-8AA2-DC865A38A70C}";
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectType);
  type.globalBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectType);

  auto& res = result.params[FFGFilterAccRes];
  res.acc = true;
  res.defaultText = "0";
  res.displayMultiplier = 100.0f;
  res.name = "Res";
  res.slotCount = 1;
  res.valueCount = 0;
  res.unit = "%";
  res.id = "{ED140CF2-52C6-40A6-9F39-44E8069FFC77}";
  auto selectRes = [](auto& module) { return &module.acc.res; };
  res.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectRes);
  res.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectRes);

  auto& freq = result.params[FFGFilterAccFreq];
  freq.acc = true;
  freq.defaultText = "1000";
  freq.name = "Freq";
  freq.plainMin = 20.0f;
  freq.plainMax = 20000.0f;
  freq.slotCount = 1;
  freq.valueCount = 0;
  freq.unit = "Hz";
  freq.id = "{24E988C5-7D41-4064-9212-111D1C3D2AF7}";
  auto selectFreq = [](auto& module) { return &module.acc.freq; };
  freq.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectFreq);
  freq.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectFreq);

  auto& gain = result.params[FFGFilterAccFreq];
  gain.acc = true;
  gain.defaultText = "0";
  gain.name = "Gain";
  gain.plainMin = -24.0f;
  gain.plainMax = 24.0f;
  gain.slotCount = 1;
  gain.valueCount = 0;
  gain.unit = "dB";
  gain.id = "{8A4C5073-CE26-44CF-A244-425824596540}";
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.globalAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  return result;
}