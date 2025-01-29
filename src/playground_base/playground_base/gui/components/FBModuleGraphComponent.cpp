#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphProcState.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphComponent::
FBModuleGraphComponent(FBGraphProcState* state):
Component()
{
  _data.state = state;
}

void
FBModuleGraphComponent::RequestRerender(int paramIndex)
{
  if (!PrepareForRender(paramIndex))
    return;
  _tweakedParamByUI = paramIndex;
  repaint();
}

bool 
FBModuleGraphComponent::PrepareForRender(int paramIndex)
{
  auto const* runtimeTopo = _data.state->moduleState.topo;
  auto const& topoIndices = runtimeTopo->params[paramIndex].topoIndices.module;
  auto const& staticModule = runtimeTopo->static_.modules[topoIndices.index];
  if (staticModule.renderGraph == nullptr)
    return false;
  _renderer = staticModule.renderGraph;
  _data.state->moduleState.moduleSlot = topoIndices.slot;
  return true;
}

void
FBModuleGraphComponent::paint(Graphics& g)
{
  if (_tweakedParamByUI == -1)
    return;
  if (!PrepareForRender(_tweakedParamByUI))
    return;

  _data.text.clear();
  _data.points.clear();
  _renderer(&_data);

  g.fillAll(Colours::black);
  g.setColour(Colours::grey);
  auto const* runtimeTopo = _data.state->moduleState.topo;
  std::string moduleName = runtimeTopo->ModuleAtParamIndex(_tweakedParamByUI)->name;
  g.drawText(moduleName + " " + _data.text, getLocalBounds(), Justification::centred, false);

  if (_data.points.empty())
    return;

  Path p;
  float y0 = (1.0f - _data.points[0]) * getHeight();
  p.startNewSubPath(0.0, y0);
  for (int i = 1; i < _data.points.size(); i++)
  {
    float x = (float)i / _data.points.size() * getWidth();
    float y = (1.0f - _data.points[i]) * getHeight();
    p.lineTo(x, y);
  }
  g.setColour(Colours::white);
  g.strokePath(p, PathStrokeType(1.0f));
}