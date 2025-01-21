#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

#include <cassert>

using namespace juce;

FBPlugGUI::
FBPlugGUI(FBRuntimeTopo const* topo, IFBHostGUIContext* hostContext):
_topo(topo), _hostContext(hostContext) {}

void
FBPlugGUI::SteppedParamNormalizedChanged(int index, float normalized)
{
  for (auto target : _paramsDependents[index])
    dynamic_cast<Component&>(*target).setEnabled(target->Evaluate());
}

void
FBPlugGUI::SetParamNormalizedFromHost(int index, float value)
{
  auto iter = _paramIndexToComponent.find(index);
  assert(iter != _paramIndexToComponent.end());
  auto& paramControl = dynamic_cast<FBParamControl&>(*_store[iter->second].get());
  paramControl.SetValueNormalizedFromHost(value);
  if(FBParamTypeIsStepped(paramControl.Param()->static_.type))
    SteppedParamNormalizedChanged(index, value);
}

Component*
FBPlugGUI::AddComponent(std::unique_ptr<Component>&& component)
{
  FBParamControl* paramControl;
  FBParamsDependent* paramsDependent;
  int componentIndex = _store.size();
  Component* result = component.get();
  _store.emplace_back(std::move(component));
  if ((paramControl = dynamic_cast<FBParamControl*>(result)) != nullptr)
    _paramIndexToComponent[paramControl->Param()->runtimeParamIndex] = componentIndex;
  if ((paramsDependent = dynamic_cast<FBParamsDependent*>(result)) != nullptr)
    for (int p : paramsDependent->EvaluateWhen())
      _paramsDependents[p].insert(paramsDependent);
  return result;
}