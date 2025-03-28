#include <playground_base/base/topo/static/FBStaticModule.hpp>
#include <playground_base/base/topo/runtime/FBTopoDetail.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParamBase.hpp>

static std::string
MakeRuntimeParamId(
  FBStaticModule const& module, 
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramId = param.id + "-" + std::to_string(indices.param.slot);
  auto moduleId = module.id + "-" + std::to_string(indices.module.slot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& module,
  FBStaticParamBase const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeName(param.name, param.slotCount, indices.param.slot, param.slotFormatter);
  auto moduleName = FBMakeRuntimeName(module.name, module.slotCount, indices.module.slot, {});
  return moduleName + " " + paramName;
}

FBRuntimeParamBase::
FBRuntimeParamBase(
  FBStaticModule const& staticModule,
  FBStaticParamBase const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
runtimeModuleIndex(runtimeModuleIndex),
runtimeParamIndex(runtimeParamIndex),
topoIndices(topoIndices),
longName(MakeRuntimeParamLongName(staticModule, staticParam, topoIndices)),
shortName(FBMakeRuntimeName(staticParam.name, staticParam.slotCount, topoIndices.param.slot, staticParam.slotFormatter)),
tooltip(FBMakeRuntimeTooltip(staticParam.name, staticParam.tooltip, staticParam.slotCount, topoIndices.param.slot, staticParam.slotFormatter)),
id(MakeRuntimeParamId(staticModule, staticParam, topoIndices)),
tag(FBMakeStableHash(id)) {}