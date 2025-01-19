#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/glfo/FFGLFOGUI.hpp>
#include <playground_plug/modules/glfo/FFGLFOTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBGUIStore.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>

using namespace juce;

Component&
FFMakeGLFOGUI(
  FBRuntimeTopo const* topo, int moduleSlot,
  FBGUIStore* store, IFBHostGUIContext* hostContext)
{
  auto& grid = store->AddComponent<FBGridComponent>(1, 2);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::On, 0 });
  grid.AddItemAndChild(GridItem(store->AddParamControl<FBParamToggleButton>(on, hostContext)));
  auto const* rate = topo->ParamAtTopo({ (int)FFModuleType::GLFO, moduleSlot, (int)FFGLFOParam::Rate, 0 });
  grid.AddItemAndChild(GridItem(store->AddParamControl<FBParamSlider>(rate, hostContext, Slider::SliderStyle::Rotary)));
  return grid;
}