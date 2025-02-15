#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGraph.hpp>
#include <playground_base/base/topo/static/FBStaticModule.hpp>

static FBStaticModuleGraph
MakeGraphTopo()
{
  FBStaticModuleGraph result = {};
  result.enabled = true;
  result.renderer = FFGLFORenderGraph;
  return result;
}

std::unique_ptr<FBStaticModule>
FFMakeGLFOTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = false;
  result->name = "GLFO";
  result->slotCount = FFGLFOCount;
  result->graph = MakeGraphTopo();
  result->id = "{D89A9DCA-6A8F-48E5-A317-071E688D729E}";
  result->params.resize((int)FFGLFOParam::Count);
  result->addrSelectors.globalModuleExchange = FFSelectGlobalModuleExchangeAddr([](auto& state) { return &state.gLFO; });
  auto selectModule = [](auto& state) { return &state.global.gLFO; };

  auto& on = result->params[(int)FFGLFOParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{A9741F9B-5E07-40D9-8FC1-73F90363EF0C}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectOn);
  on.addrSelectors.globalBlockProc = FFSelectProcParamAddr(selectModule, selectOn);
  on.addrSelectors.globalExchange = FFSelectExchangeParamAddr(selectModule, selectOn);

  auto& rate = result->params[(int)FFGLFOParam::Rate];
  rate.acc = true;
  rate.defaultText = "1";
  rate.name = "Rate";
  rate.slotCount = 1;
  rate.unit = "Hz";
  rate.id = "{79BFD05E-98FA-48D4-8D07-C009285EACA7}";
  rate.type = FBParamType::Linear;
  rate.Linear().min = 0.1f;
  rate.Linear().max = 20.0f;
  auto selectRate = [](auto& module) { return &module.acc.rate; };
  rate.addrSelectors.scalar = FFSelectScalarParamAddr(selectModule, selectRate);
  rate.addrSelectors.globalAccProc = FFSelectProcParamAddr(selectModule, selectRate);
  rate.addrSelectors.globalExchange = FFSelectExchangeParamAddr(selectModule, selectRate);
  return result;
}