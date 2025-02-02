#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBGraphRenderState.hpp>
#include <playground_base/base/state/FBModuleProcState.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/components/FBModuleGraphComponent.hpp>

#include <cmath>
#include <numbers>

using namespace juce;

FBModuleGraphComponent::
FBModuleGraphComponent(FBGraphRenderState* renderState):
Component()
{
  _data.renderState = renderState;
}

void
FBModuleGraphComponent::RequestRerender(int paramIndex)
{
  if (!PrepareForRender(paramIndex))
    return;
  _tweakedParamByUI = paramIndex;
  repaint();
}

juce::Point<float>
FBModuleGraphComponent::PointLocation(
  std::vector<float> const& points, int point) const
{
  float x = (float)point / points.size() * getWidth();
  float y = (1.0f - points[point]) * getHeight();
  return { x, y };
}

void 
FBModuleGraphComponent::PaintMarker(
  Graphics& g, std::vector<float> const& points, int marker)
{
  g.setColour(Colours::white);
  auto markerXY = PointLocation(points, marker);
  g.fillEllipse(markerXY.getX() - 4.0f, markerXY.getY() - 4.0f, 8.0f, 8.0f);
}

void
FBModuleGraphComponent::PaintSeries(
  Graphics& g, Colour color, std::vector<float> const& points)
{
  if (points.empty())
    return;

  Path path;
  path.startNewSubPath(PointLocation(points, 0));
  for (int i = 1; i < points.size(); i++)
    path.lineTo(PointLocation(points, i));
  g.setColour(color);
  g.strokePath(path, PathStrokeType(1.0f));
}

bool 
FBModuleGraphComponent::PrepareForRender(int paramIndex)
{
  auto& moduleState = _data.renderState->ModuleState();
  auto const& topoIndices = moduleState.topo->params[paramIndex].topoIndices.module;
  auto const& staticModule = moduleState.topo->static_.modules[topoIndices.index];
  if (staticModule.renderGraph == nullptr)
    return false;
  _renderer = staticModule.renderGraph;
  moduleState.moduleSlot = topoIndices.slot;
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
  _data.primarySeries.clear();
  _data.primaryMarkers.clear();
  _data.secondarySeries.clear();
  _renderer(&_data);

  g.fillAll(Colours::black);
  g.setColour(Colours::darkgrey);
  auto const* runtimeTopo = _data.renderState->ModuleState().topo;
  std::string moduleName = runtimeTopo->ModuleAtParamIndex(_tweakedParamByUI)->name;
  g.drawText(moduleName + " " + _data.text, getLocalBounds(), Justification::centred, false);
  for (int i = 0; i < _data.secondarySeries.size(); i++)
  {
    int marker = _data.secondarySeries[i].marker;
    auto const& points = _data.secondarySeries[i].points;
    PaintSeries(g, Colours::grey, points);
    if (marker != -1)
      PaintMarker(g, points, marker);
  }
  
  PaintSeries(g, Colours::white, _data.primarySeries);
  for (int i = 0; i < _data.primaryMarkers.size(); i++)
    PaintMarker(g, _data.primarySeries, _data.primaryMarkers[i]);
}