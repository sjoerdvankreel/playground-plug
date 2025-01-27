#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <vector>
#include <unordered_map>
#include <unordered_set>

struct FBRuntimeTopo;
class FBHostGUIContext;

class FBPlugGUI:
public juce::Component
{
public:
  template <class TComponent, class... Args>
  TComponent* StoreComponent(Args&&... args);

  FBRuntimeTopo const* Topo() const { return _topo; }
  FBHostGUIContext* HostContext() const { return _hostContext; }

  void ShowHostMenuForParam(int index);
  void SteppedParamNormalizedChanged(int index);
  void SetParamNormalizedFromHost(int index, float normalized);
  void ShowPopupMenuFor(
    juce::Component* target, 
    juce::PopupMenu menu, 
    std::function<void(int)> callback);

protected:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);
  FBPlugGUI(FBRuntimeTopo const* topo, FBHostGUIContext* hostContext);

  void InitAllDependencies();
  juce::Component* StoreComponent(std::unique_ptr<juce::Component>&& component);

private:
  FBRuntimeTopo const* const _topo;
  FBHostGUIContext* const _hostContext;
  std::unordered_map<int, int> _paramIndexToComponent = {};
  std::vector<std::unique_ptr<juce::Component>> _store = {};
  std::unordered_map<int, std::unordered_set<FBParamsDependent*>> _paramsDependents = {};
};

template <class TComponent, class... Args>
TComponent* FBPlugGUI::StoreComponent(Args&&... args)
{
  auto component = std::make_unique<TComponent>(std::forward<Args>(args)...);
  TComponent* result = component.get();
  StoreComponent(std::move(component));
  return result;
}