#pragma once

#include <firefly_base/gui/shared/FBAutoSize.hpp>
#include <firefly_base/gui/shared/FBParamsDependent.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;

class FBParamsDependentComponent:
public juce::Component,
public FBParamsDependent,
public IFBHorizontalAutoSize
{
public:
  void resized() override;
  void parentHierarchyChanged() override;
  int FixedWidth(int height) const override;
  
  FBParamsDependentComponent(
    FBPlugGUI* plugGUI, juce::Component* content, int slot,
    FBTopoIndices const& moduleIndices, FBParamsDependencies const& dependencies);
};