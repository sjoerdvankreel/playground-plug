#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/shared/FBGUIConfig.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/shared/FBParamsDependent.hpp>

#include <cassert>

using namespace juce;

FBPlugGUI::
FBPlugGUI(FBHostGUIContext* hostContext) :
_hostContext(hostContext)
{
  _tooltipWindow = StoreComponent<TooltipWindow>();
  addAndMakeVisible(_tooltipWindow);
}

void
FBPlugGUI::SteppedParamNormalizedChanged(int index)
{
  for (auto target : _paramsDependents[index])
    target->DependenciesChanged();
}

void 
FBPlugGUI::InitAllDependencies()
{
  for (int i = 0; i < HostContext()->Topo()->params.size(); i++)
    if (FBParamTypeIsStepped(HostContext()->Topo()->params[i].static_.type))
      SteppedParamNormalizedChanged(i);
}

void
FBPlugGUI::UpdateExchangeStateTick()
{
  // TODO the param sliders
}

void
FBPlugGUI::UpdateExchangeState()
{
  using std::chrono::milliseconds;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;

  auto now = high_resolution_clock::now();
  auto elapsedMillis = duration_cast<milliseconds>(now - _exchangeUpdated);
  if (elapsedMillis.count() < 1000.0 / FBGUIFPS) // TODO the clap todo
    return;
  _exchangeUpdated = now;
  UpdateExchangeStateTick();
}

void
FBPlugGUI::ShowPopupMenuFor(
  Component* target,
  PopupMenu menu,
  std::function<void(int)> callback)
{
  PopupMenu::Options options;
  options = options.withParentComponent(this);
  options = options.withTargetComponent(target);
  options = options.withMousePosition();
  menu.showMenuAsync(options, callback);
}

std::string
FBPlugGUI::GetTooltipForParam(int index)
{
  auto const& param = HostContext()->Topo()->params[index];
  float normalized = HostContext()->GetParamNormalized(index);
  auto result = param.tooltip + ": ";
  result += param.static_.NormalizedToText(FBTextDisplay::Tooltip, normalized);
  if (param.static_.unit.empty())
    return result;
  return result + " " + param.static_.unit;
}

void
FBPlugGUI::ShowHostMenuForParam(int index)
{
  auto menuItems = HostContext()->MakeParamContextMenu(index);
  if (menuItems.empty())
    return;
  auto hostMenu = FBMakeHostContextMenu(menuItems);
  auto clicked = [this, index](int tag) {
    if (tag > 0)
      HostContext()->ParamContextMenuClicked(index, tag); };
  ShowPopupMenuFor(this, *hostMenu, clicked);
}

void
FBPlugGUI::SetParamNormalizedFromHost(int index, float value)
{
  auto iter = _paramIndexToComponent.find(index);
  assert(iter != _paramIndexToComponent.end());
  auto& paramControl = dynamic_cast<FBParamControl&>(*_store[iter->second].get());
  paramControl.SetValueNormalizedFromHost(value);
  if(FBParamTypeIsStepped(paramControl.Param()->static_.type))
    SteppedParamNormalizedChanged(index);
}

Component*
FBPlugGUI::StoreComponent(std::unique_ptr<Component>&& component)
{
  FBParamControl* paramControl;
  FBParamsDependent* paramsDependent;
  int componentIndex = _store.size();
  Component* result = component.get();
  _store.emplace_back(std::move(component));
  if ((paramControl = dynamic_cast<FBParamControl*>(result)) != nullptr)
    _paramIndexToComponent[paramControl->Param()->runtimeParamIndex] = componentIndex;
  if ((paramsDependent = dynamic_cast<FBParamsDependent*>(result)) != nullptr)
    for (int p : paramsDependent->RuntimeDependencies())
      _paramsDependents[p].insert(paramsDependent);
  return result;
}