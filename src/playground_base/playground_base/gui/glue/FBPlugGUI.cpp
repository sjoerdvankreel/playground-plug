#include <playground_base/gui/glue/FBPlugGUI.hpp>

using namespace juce;

FBPlugGUI::
FBPlugGUI(IFBHostGUIContext* hostContext) : 
_hostContext(hostContext)
{
  setOpaque(true);
  setVisible(true);
}

void 
FBPlugGUI::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

int 
FBPlugGUI::DefaultHeight() const
{
  int w = DefaultWidth();
  int arW = AspectRatioWidth();
  int arH = AspectRatioHeight();
  return w * arH / arW;
}