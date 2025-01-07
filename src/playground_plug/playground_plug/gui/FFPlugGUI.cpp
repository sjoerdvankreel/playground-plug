#include <playground_plug/gui/FFPlugGUI.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/components/FBParamSlider.hpp>
#include <playground_base/gui/components/FBParamComboBox.hpp>
#include <playground_base/gui/components/FBParamToggleButton.hpp>

using namespace juce;

FFPlugGUI::
FFPlugGUI(
  FBRuntimeTopo const* topo, 
  IFBHostGUIContext* hostContext):
FBPlugGUI(),
_topo(topo),
_hostContext(hostContext)
{
  for (int i = 0; i < topo->params.size(); i++)
    switch (topo->params[i].static_.type)
    {
    case FBParamType::List:
    case FBParamType::Discrete:
      _controls.emplace_back(std::make_unique<FBParamComboBox>(
        &_topo->params[i], _hostContext));
      break;
    case FBParamType::Boolean:
      _controls.emplace_back(std::make_unique<FBParamToggleButton>(
        &_topo->params[i], _hostContext));
      break;
    case FBParamType::Linear:
    case FBParamType::FreqOct:
      _controls.emplace_back(std::make_unique<FBParamSlider>(
        &_topo->params[i], _hostContext, Slider::SliderStyle::Rotary));
      break;
    default:
      assert(false);
      break;
    }
  for (int i = 0; i < topo->params.size(); i++)
    addAndMakeVisible(_controls[i].get());
  setSize(DefaultWidth(), DefaultHeight());
  resized();
}

void
FFPlugGUI::SetParamNormalized(int index, float normalized)
{
  auto& control = dynamic_cast<IFBParamControl&>(*_controls[index]);
  control.SetValueNormalized(normalized);
}

void 
FFPlugGUI::resized()
{
  auto bounds = getLocalBounds();
  int rowColCount = std::ceil(std::sqrt((float)_topo->params.size()));
  int w = bounds.getWidth();
  int h = bounds.getHeight();
  int rh = h / rowColCount;
  int cw = w / rowColCount;
  for(int r = 0; r < rowColCount; r++)
    for (int c = 0; c < rowColCount; c++)
    {
      int p = r * rowColCount + c;
      if (p < _topo->params.size())
      {
        int y = r * rh;
        int x = c * cw;
        getChildComponent(p)->setBounds(x, y, cw, rh);
        getChildComponent(p)->resized();
      }
    }
}