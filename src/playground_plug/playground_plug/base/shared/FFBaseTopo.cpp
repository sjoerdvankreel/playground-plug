#include <playground_plug/base/shared/FFBaseTopo.hpp>
#include <cstdint>

static std::string
MakeRuntimeName(
  std::string const& staticName, int slotCount, int slot)
{
  std::string result = staticName;
  if (slotCount > 1)
    result += std::to_string(slot + 1);
  return result;
}

int
MakeRuntimeParamHash(
  std::string const& id)
{
  std::uint32_t result = 0;
  int const multiplier = 33;
  for (char c : id)
    result = multiplier * result + static_cast<std::uint32_t>(c);
  return std::abs(static_cast<int>(result + (result >> 5)));
}

static std::string
MakeRuntimeParamId(
  FFStaticModule const& staticModule, int moduleSlot, 
  FFStaticParam const& staticParam, int paramSlot)
{
  auto moduleId = staticModule.id + "-" + std::to_string(moduleSlot);
  auto paramId = staticParam.id + "-" + std::to_string(paramSlot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FFStaticModule const& staticModule, int moduleSlot,
  FFStaticParam const& staticParam, int paramSlot)
{
  auto moduleName = MakeRuntimeName(staticModule.name, staticModule.slotCount, moduleSlot);
  auto paramName = MakeRuntimeName(staticParam.name, staticParam.slotCount, paramSlot);
  return moduleName + " " + paramName;
}

static std::map<int, int>
MakeTagToParam(
  std::vector<FFRuntimeParam> const& runtimeParams)
{
  std::map<int, int> result;
  for (int p = 0; p < runtimeParams.size(); p++)
    result[runtimeParams[p].tag] = p;
  return result;
}

static std::vector<FFRuntimeParam>
MakeRuntimeAccParams(
  std::vector<FFRuntimeModule> const& runtimeModules)
{
  std::vector<FFRuntimeParam> result;
  for (int m = 0; m < runtimeModules.size(); m++)
    for (int ap = 0; ap < runtimeModules[m].accParams.size(); ap++)
      result.push_back(runtimeModules[m].accParams[ap]);
  return result;
}

static std::vector<FFRuntimeParam>
MakeRuntimeBlockParams(
  std::vector<FFRuntimeModule> const& runtimeModules)
{
  std::vector<FFRuntimeParam> result;
  for (int m = 0; m < runtimeModules.size(); m++)
    for (int bp = 0; bp < runtimeModules[m].blockParams.size(); bp++)
      result.push_back(runtimeModules[m].blockParams[bp]);
  return result;
}

static std::vector<FFRuntimeModule>
MakeRuntimeModules(FFStaticTopo const& staticTopo)
{
  std::vector<FFRuntimeModule> result;
  for (int mi = 0; mi < staticTopo.modules.size(); mi++)
    for (int ms = 0; ms < staticTopo.modules[mi].slotCount; ms++)
      result.push_back(FFRuntimeModule(staticTopo.modules[mi], ms));
  return result;
}

static std::vector<FFRuntimeParam>
MakeRuntimeParams(
  FFStaticModule const& staticModule, int moduleSlot,
  std::vector<FFStaticParam> const& staticParams)
{
  std::vector<FFRuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[s].slotCount; s++)
      result.push_back(FFRuntimeParam(staticModule, moduleSlot, staticParams[p], s));
  return result;
}

FFRuntimeTopo::
FFRuntimeTopo(FFStaticTopo const& staticTopo):
modules(MakeRuntimeModules(staticTopo)),
accParams(MakeRuntimeAccParams(modules)),
blockParams(MakeRuntimeBlockParams(modules)),
tagToAccParam(MakeTagToParam(accParams)),
tagToBlockParam(MakeTagToParam(blockParams)) {}

FFRuntimeParam::
FFRuntimeParam(
  FFStaticModule const& staticModule, int moduleSlot,
  FFStaticParam const& staticParam, int paramSlot) :
moduleSlot(moduleSlot),
paramSlot(paramSlot),
staticParam(staticParam),
longName(MakeRuntimeParamLongName(staticModule, moduleSlot, staticParam, paramSlot)),
shortName(MakeRuntimeName(staticParam.name, staticParam.slotCount, paramSlot)),
id(MakeRuntimeParamId(staticModule, moduleSlot, staticParam, paramSlot)),
tag(MakeRuntimeParamHash(id)) {}

FFRuntimeModule::
FFRuntimeModule(
  FFStaticModule const& staticModule, int moduleSlot):
name(MakeRuntimeName(staticModule.name, staticModule.slotCount, moduleSlot)),
accParams(MakeRuntimeParams(staticModule, moduleSlot, staticModule.accParams)),
blockParams(MakeRuntimeParams(staticModule, moduleSlot, staticModule.blockParams)) {}