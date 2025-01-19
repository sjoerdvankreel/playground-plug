#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>

using namespace juce;

static Component&
MakeOsciGUI(
  FBRuntimeTopo const* topo, FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext, int moduleSlot)
{
  auto& result = plugGUI->AddComponent<FBGridComponent>(1, 7);
  auto const* on = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::On, 0 });
  result.Add(plugGUI->AddParamControl<FBParamToggleButton>(on, hostContext));
  auto const* type = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  result.Add(plugGUI->AddParamControl<FBParamComboBox>(type, hostContext));
  auto const* note = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  result.Add(plugGUI->AddParamControl<FBParamComboBox>(note, hostContext));
  auto const* gain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  result.Add(plugGUI->AddParamControl<FBParamSlider>(gain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* cent = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  result.Add(plugGUI->AddParamControl<FBParamSlider>(cent, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* pw = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::PW, 0 });
  result.Add(plugGUI->AddParamControl<FBParamSlider>(pw, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  auto const* gLFOToGain = topo->ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  result.Add(plugGUI->AddParamControl<FBParamSlider>(gLFOToGain, plugGUI, hostContext, Slider::SliderStyle::Rotary));
  return result;
}

Component&
FFMakeOsciGUI(
  FBRuntimeTopo const* topo, 
  FBPlugGUI* plugGUI,
  IFBHostGUIContext* hostContext)
{
  return plugGUI->AddComponent<FBModuleTabComponent>(topo, plugGUI, hostContext, (int)FFModuleType::Osci, MakeOsciGUI);
}