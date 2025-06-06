#include <firefly_synth/shared/FFPlugTopo.hpp>
#include <firefly_synth/modules/master/FFMasterGUI.hpp>
#include <firefly_synth/modules/master/FFMasterTopo.hpp>

#include <firefly_base/gui/shared/FBPlugGUI.hpp>
#include <firefly_base/gui/glue/FBHostGUIContext.hpp>
#include <firefly_base/gui/controls/FBLabel.hpp>
#include <firefly_base/gui/controls/FBSlider.hpp>
#include <firefly_base/gui/controls/FBComboBox.hpp>
#include <firefly_base/gui/controls/FBOutputLabel.hpp>
#include <firefly_base/gui/components/FBTabComponent.hpp>
#include <firefly_base/gui/components/FBGridComponent.hpp>
#include <firefly_base/gui/components/FBSectionComponent.hpp>
#include <firefly_base/base/topo/runtime/FBRuntimeTopo.hpp>

using namespace juce;

static Component*
MakeSectionAll(FBPlugGUI* plugGUI, int moduleSlot)
{
  auto topo = plugGUI->HostContext()->Topo();
  auto grid = plugGUI->StoreComponent<FBGridComponent>(FBGridType::Module, std::vector<int> { 1 }, std::vector<int> { 0, 0, 0, 0 } );
  auto gain = topo->audio.ParamAtTopo({(int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::Gain, 0});
  grid->Add(0, 0, plugGUI->StoreComponent<FBParamLabel>(plugGUI, gain));
  grid->Add(0, 1, plugGUI->StoreComponent<FBParamSlider>(plugGUI, gain, Slider::SliderStyle::RotaryVerticalDrag));
  auto hostSmoothTime = topo->audio.ParamAtTopo({ (int)FFModuleType::Master, moduleSlot, (int)FFMasterParam::HostSmoothTime, 0 });
  grid->Add(0, 2, plugGUI->StoreComponent<FBParamLabel>(plugGUI, hostSmoothTime));
  grid->Add(0, 3, plugGUI->StoreComponent<FBParamSlider>(plugGUI, hostSmoothTime, Slider::SliderStyle::RotaryVerticalDrag));
  grid->MarkSection({ 0, 0, 1, 4 });
  return plugGUI->StoreComponent<FBSectionComponent>(grid);
}

Component*
FFMakeMasterGUI(FBPlugGUI* plugGUI)
{
  return plugGUI->StoreComponent<FBModuleTabComponent>(plugGUI, (int)FFModuleType::Master, MakeSectionAll);
}