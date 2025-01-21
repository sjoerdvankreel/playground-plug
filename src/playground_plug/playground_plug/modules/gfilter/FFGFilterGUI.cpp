#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterGUI.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0 });
  auto on = plugGUI->Topo()->ParamAtTopo({(int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::On, 0});
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(on));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamToggleButton>(topo, on, hostContext));
  auto type = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Type, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(type));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamComboBox>(topo, type, hostContext));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}

static Component*
MakeSectionParams(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 1, 0, 1, 0, 1 });
  auto freq = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Freq, 0 });
  grid->Add(0, 0, plugGUI->AddComponent<FBParamLabel>(freq));
  grid->Add(0, 1, plugGUI->AddComponent<FBParamSlider>(topo, freq, hostContext, plugGUI, Slider::SliderStyle::LinearHorizontal));
  auto res = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Res, 0 });
  grid->Add(0, 2, plugGUI->AddComponent<FBParamLabel>(res));
  grid->Add(0, 3, plugGUI->AddComponent<FBParamSlider>(topo, res, hostContext, plugGUI, Slider::SliderStyle::LinearHorizontal));
  auto gain = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::GFilter, moduleSlot, (int)FFGFilterParam::Gain, 0 });
  grid->Add(0, 4, plugGUI->AddComponent<FBParamLabel>(gain));
  grid->Add(0, 5, plugGUI->AddComponent<FBParamSlider>(topo, gain, hostContext, plugGUI, Slider::SliderStyle::LinearHorizontal));
  return plugGUI->AddComponent<FBSectionComponent>(grid);
}
  
static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto result = plugGUI->AddComponent<FBGridComponent>(1, std::vector<int> { 0, 1 });
  result->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  result->Add(0, 1, MakeSectionParams(plugGUI, moduleSlot));
  return result;
}

Component*
FFMakeGFilterGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(
    plugGUI, (int)FFModuleType::GFilter, TabFactory);
}