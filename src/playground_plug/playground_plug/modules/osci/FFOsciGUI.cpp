#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/modules/osci/FFOsciGUI.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamLabel.hpp>
#include <playground_base/gui/controls/FBParamSlider.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>
#include <playground_base/gui/controls/FBParamToggleButton.hpp>
#include <playground_base/gui/components/FBGridComponent.hpp>
#include <playground_base/gui/components/FBSectionComponent.hpp>
#include <playground_base/gui/components/FBModuleTabComponent.hpp>
#include <playground_base/gui/components/FBParamsDependentSectionComponent.hpp>

using namespace juce;

static Component*
MakeSectionMain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 0, 0, 0 });
  auto type = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Type, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, type));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, type));
  auto gain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Gain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto note = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Note, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, note));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamComboBox>(plugGUI, note));
  auto cent = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::Cent, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, cent));
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, cent, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 1, 4 });
  grid->MarkSection({ 1, 0, 1, 4 });
  return grid;
}

static Component*
MakeSectionGLFOToGain(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 0, -1, 2, std::vector<int> { 0 });
  auto gLFOToGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::GLFOToGain, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gLFOToGain));
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gLFOToGain, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 1 });
  grid->MarkSection({ 1, 0, 1, 1 });
  return grid;
}

static Component*
MakeSectionBasic(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 0, 1, 0, 0, 1, 0 });
  auto basicSinOn = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSinOn, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, basicSinOn));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, basicSinOn));
  auto basicSinGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSinGain, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicSinGain, Slider::SliderStyle::LinearHorizontal));
  auto basicSawOn = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSawOn, 0 });
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, basicSawOn));
  grid->Add(0, 4, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, basicSawOn));
  auto basicSawGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSawGain, 0 });
  grid->Add(0, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicSawGain, Slider::SliderStyle::LinearHorizontal));
  auto basicTriOn = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicTriOn, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, basicTriOn));
  grid->Add(1, 1, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, basicTriOn));
  auto basicTriGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicTriGain, 0 });
  grid->Add(1, 2, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicTriGain, Slider::SliderStyle::LinearHorizontal));
  auto basicSqrOn = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSqrOn, 0 });
  grid->Add(1, 3, plugGUI->StoreComponent<FBParamLabel>(plugGUI, basicSqrOn));
  grid->Add(1, 4, plugGUI->StoreComponent<FBParamToggleButton>(plugGUI, basicSqrOn));
  auto basicSqrGain = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSqrGain, 0 });
  grid->Add(1, 5, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicSqrGain, Slider::SliderStyle::LinearHorizontal));
  auto basicSqrPW = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::BasicSqrPW, 0 });
  grid->Add(0, 6, plugGUI->StoreComponent<FBParamLabel>(plugGUI, basicSqrPW));
  grid->Add(1, 6, plugGUI->StoreComponent<FBParamSlider>(plugGUI, basicSqrPW, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 1, 7 });
  grid->MarkSection({ 1, 0, 1, 7 });

  // TODO helper function
  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::Off || vs[0] == (int)FFOsciType::Basic; });
  return plugGUI->StoreComponent<FBParamsDependentSectionComponent>(plugGUI, grid, indices, dependencies);
}

static Component*
MakeSectionDSF(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 2, std::vector<int> { 0, 1, 0, 1});
  auto dsfOvertones = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFOvertones, 0 });
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dsfOvertones));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfOvertones, Slider::SliderStyle::LinearHorizontal));
  auto dsfDistance = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFDistance, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dsfDistance));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfDistance, Slider::SliderStyle::LinearHorizontal));
  auto dsfDecay = topo->audio.ParamAtTopo({ (int)FFModuleType::Osci, moduleSlot, (int)FFOsciParam::DSFDecay, 0 });
  grid->Add(1, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, dsfDecay));
  grid->Add(1, 1, 1, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, dsfDecay, Slider::SliderStyle::LinearHorizontal));
  grid->MarkSection({ 0, 0, 1, 4 });
  grid->MarkSection({ 1, 0, 1, 4 });

  // TODO helper function
  FBParamsDependencies dependencies = {};
  FBTopoIndices indices = { (int)FFModuleType::Osci, moduleSlot };
  dependencies.visible.audio.When({ (int)FFOsciParam::Type }, [](auto const& vs) { return vs[0] == (int)FFOsciType::DSF; });
  return plugGUI->StoreComponent<FBParamsDependentSectionComponent>(plugGUI, grid, indices, dependencies);
}

static Component*
TabFactory(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto result = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, 1, std::vector<int> { 0, 0, 1 });
  result->Add(0, 0, MakeSectionMain(plugGUI, moduleSlot));
  result->Add(0, 1, MakeSectionGLFOToGain(plugGUI, moduleSlot));
  result->Add(0, 2, MakeSectionBasic(plugGUI, moduleSlot));
  result->Add(0, 2, MakeSectionDSF(plugGUI, moduleSlot));
  return result;
}

Component*
FFMakeOsciGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Osci, TabFactory);
}