#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBParamControl.hpp>
#include <playground_base/gui/glue/FBPlugGUIContainer.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

using namespace juce;

FBPlugGUIContainer::
~FBPlugGUIContainer() {}

FBPlugGUIContainer::
FBPlugGUIContainer(FBHostGUIContext* hostContext):
FBPlugGUIContext(hostContext),
_gui(hostContext->Topo()->static_.gui.factory(hostContext))
{
  setOpaque(true);
  setVisible(true);
  int plugWidth = hostContext->Topo()->static_.gui.plugWidth;
  int plugHeight = GetHeightForAspectRatio(plugWidth);
  setSize(plugWidth, plugHeight);
  _gui->setSize(plugWidth, plugHeight);
  addAndMakeVisible(_gui.get());
}

void 
FBPlugGUIContainer::paint(Graphics& g)
{
  g.fillAll(Colours::black);
}

void
FBPlugGUIContainer::RemoveFromDesktop()
{
  removeFromDesktop();
}

void
FBPlugGUIContainer::UpdateExchangeState()
{
  _gui->UpdateExchangeState();
}

void
FBPlugGUIContainer::AddToDesktop(void* parent)
{
  addToDesktop(0, parent);
}

void
FBPlugGUIContainer::SetVisible(bool visible)
{
  setVisible(visible);
}

void
FBPlugGUIContainer::SetParamNormalized(int index, float normalized)
{
  _gui->SetParamNormalizedFromHost(index, normalized);
}

void
FBPlugGUIContainer::RequestRescale(float combinedScale)
{
  auto hostSize = GetHostSize();
  setSize(hostSize.first, hostSize.second);
  _gui->setTransform(AffineTransform::scale(combinedScale));
}