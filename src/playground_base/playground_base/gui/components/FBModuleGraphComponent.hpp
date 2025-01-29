#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>
#include <playground_base/gui/components/FBModuleGraphComponentData.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBGraphProcState;

class FBModuleGraphComponent final:
public juce::Component
{
  int _tweakedParamByUI = -1;
  FBModuleGraphRenderer _renderer = {};
  FBModuleGraphComponentData _data = {};
  
  bool PrepareForRender(int paramIndex);
  std::string GetActiveModuleName() const;

public:
  void RequestRerender(int paramIndex);
  void paint(juce::Graphics& g) override;
  FBModuleGraphComponent(FBGraphProcState* state);
  FB_NOCOPY_MOVE_NODEFCTOR(FBModuleGraphComponent);
};