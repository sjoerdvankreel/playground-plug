#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/env/FFEnvGUI.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>

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
  auto grid = plugGUI->StoreComponent<FBGridComponent>(1, std::vector<int> { 0, 0, 0, 0, 0, 0 });
  auto on = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::On, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, on));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, on));
  auto exp = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Exp, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, exp));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, exp));
  auto type = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::Type, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
MakeSectionDAHDSR(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto grid = plugGUI->StoreComponent<FBGridComponent>(1, std::vector<int> { 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 });
  auto delayTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DelayTime, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, delayTime));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, delayTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto attackTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackTime, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, attackTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto attackSlope = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::AttackSlope, 0 });
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamLabel>(plugGUI, attackSlope));
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, attackSlope, Slider::SliderStyle::RotaryVerticalDrag));
  auto holdTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::HoldTime, 0 });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, holdTime));
  grid->Add(0, 7, plugGUI->StoreComponent<FBParamSlider>(plugGUI, holdTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto decayTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecayTime, 0 });
  grid->Add(0, 8, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decayTime));
  grid->Add(0, 9, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decayTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto decaySlope = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::DecaySlope, 0 });
  grid->Add(0, 10, plugGUI->StoreComponent<FBParamLabel>(plugGUI, decaySlope));
  grid->Add(0, 11, plugGUI->StoreComponent<FBParamSlider>(plugGUI, decaySlope, Slider::SliderStyle::RotaryVerticalDrag));
  auto sustainLevel = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::SustainLevel, 0 });
  grid->Add(0, 12, plugGUI->StoreComponent<FBParamLabel>(plugGUI, sustainLevel));
  grid->Add(0, 13, plugGUI->StoreComponent<FBParamSlider>(plugGUI, sustainLevel, Slider::SliderStyle::RotaryVerticalDrag));
  auto releaseTime = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseTime, 0 });
  grid->Add(0, 14, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseTime));
  grid->Add(0, 15, plugGUI->StoreComponent<FBParamSlider>(plugGUI, releaseTime, Slider::SliderStyle::RotaryVerticalDrag));
  auto releaseSlope = plugGUI->Topo()->ParamAtTopo({ (int)FFModuleType::Env, moduleSlot, (int)FFEnvParam::ReleaseSlope, 0 });
  grid->Add(0, 16, plugGUI->StoreComponent<FBParamLabel>(plugGUI, releaseSlope));
  grid->Add(0, 17, plugGUI->StoreComponent<FBParamSlider>(plugGUI, releaseSlope, Slider::SliderStyle::RotaryVerticalDrag));
  return plugGUI->StoreComponent<FBSectionComponent>(plugGUI, grid);
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto result = plugGUI->StoreComponent<FBGridComponent>(1, std::vector<int> { 0, 1 });
  result->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  result->Add(0, 1, MakeSectionDAHDSR(plugGUI, moduleSlot));
  return result;
}

Component*
FFMakeEnvGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Env, TabFactory);
}