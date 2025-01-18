#include <playground_base/base/topo/FBTopoDetail.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

static std::string
MakeRuntimeParamId(
  FBStaticModule const& module, 
  FBStaticParam const& param,
  FBParamTopoIndices const& indices)
{
  auto paramId = param.id + "-" + std::to_string(indices.staticParamSlot);
  auto moduleId = module.id + "-" + std::to_string(indices.staticModuleSlot);
  return moduleId + "-" + paramId;
}

static std::string
MakeRuntimeParamLongName(
  FBStaticModule const& module,
  FBStaticParam const& param,
  FBParamTopoIndices const& indices)
{
  auto paramName = FBMakeRuntimeName(param.name, param.slotCount, indices.staticParamSlot);
  auto moduleName = FBMakeRuntimeName(module.name, module.slotCount, indices.staticModuleSlot);
  return moduleName + " " + paramName;
}

FBRuntimeParam::
FBRuntimeParam(
  FBStaticModule const& staticModule,
  FBStaticParam const& staticParam,
  FBParamTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamIndex):
runtimeModuleIndex(runtimeModuleIndex),
runtimeParamIndex(runtimeParamIndex),
static_(staticParam),
topoIndices(topoIndices),
longName(MakeRuntimeParamLongName(staticModule, staticParam, topoIndices)),
shortName(FBMakeRuntimeName(staticParam.name, staticParam.slotCount, topoIndices.staticParamSlot)),
id(MakeRuntimeParamId(staticModule, staticParam, topoIndices)),
tag(FBMakeStableHash(id)) {}