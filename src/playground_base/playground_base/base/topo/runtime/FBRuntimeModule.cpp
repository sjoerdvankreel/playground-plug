#include <playground_base/base/topo/static/FBStaticModule.hpp>
#include <playground_base/base/topo/runtime/FBTopoDetail.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeModule.hpp>

template <class RuntimeParam, class StaticParam>
static std::vector<RuntimeParam>
MakeRuntimeParams(
  FBStaticModule const& staticModule, 
  FBTopoIndices const& topoIndices,
  int runtimeModuleIndex, int runtimeParamStart,
  std::vector<StaticParam> const& staticParams)
{
  std::vector<RuntimeParam> result;
  for (int p = 0; p < staticParams.size(); p++)
    for (int s = 0; s < staticParams[p].slotCount; s++)
    {
      FBParamTopoIndices indices = {};
      indices.param.slot = s;
      indices.param.index = p;
      indices.module = topoIndices;
      result.push_back(RuntimeParam(
        staticModule, staticParams[p],
        indices, runtimeModuleIndex, runtimeParamStart++));
    }
  return result;
}

FBRuntimeModule::
FBRuntimeModule(
  FBStaticModule const& staticModule, FBTopoIndices const& topoIndices,
  int runtimeIndex, int runtimeParamStart, int runtimeGUIParamStart):
topoIndices(topoIndices),
runtimeModuleIndex(runtimeIndex),
name(FBMakeRuntimeName(staticModule.name, staticModule.slotCount, topoIndices.slot, {})),
params(MakeRuntimeParams<FBRuntimeParam>(staticModule, topoIndices, runtimeIndex, runtimeParamStart, staticModule.params)),
guiParams(MakeRuntimeParams<FBRuntimeGUIParam>(staticModule, topoIndices, runtimeIndex, runtimeGUIParamStart, staticModule.guiParams)) {}