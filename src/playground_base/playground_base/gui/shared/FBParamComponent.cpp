#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamComponent.hpp>

static std::vector<int>
EvaluateWhen(
  FBRuntimeTopo const* topo, 
  FBRuntimeParam const* param)
{
  std::vector<int> result;
  for (int i = 0; i < param->static_.relevant.staticParamIndices.size(); i++)
  {
    FBParamTopoIndices indices;
    indices.staticParamIndex = param->static_.relevant.staticParamIndices[i];
    indices.staticParamSlot = param->topoIndices.staticParamSlot;
    indices.staticModuleSlot = param->topoIndices.staticModuleSlot;
    indices.staticModuleIndex = param->topoIndices.staticModuleIndex;
    result.push_back(topo->ParamAtTopo(indices)->runtimeParamIndex);
  }
  return result;
}

FBParamComponent::
FBParamComponent(FBPlugGUI* plugGUI, FBRuntimeParam const* param):
FBParamsDependent(plugGUI, ::EvaluateWhen(plugGUI->Topo(), param)),
_param(param) {}

bool 
FBParamComponent::Evaluate(std::vector<int> const& vals) const
{
  return _param->static_.relevant.evaluate(vals);
}