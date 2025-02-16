#pragma once

#include <playground_base/gui/shared/FBParamComponent.hpp>
#include <playground_base/gui/controls/FBAutoSizeLabel.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI;
struct FBRuntimeAudioParam;

class FBParamLabel final:
public FBAutoSizeLabel,
public FBParamComponent
{
public:
  void parentHierarchyChanged() override;
  FBParamLabel(FBPlugGUI* plugGUI, FBRuntimeAudioParam const* param);
};