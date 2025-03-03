#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/shared/FBGUIUtility.hpp>
#include <playground_base/gui/controls/FBOutputParamLabel.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeParam.hpp>

using namespace juce;

FBOutputParamLabel::
FBOutputParamLabel(
  FBPlugGUI* plugGUI, 
  FBRuntimeParam const* param, 
  std::string const& defaultText,
  std::string const& maxWidthText):
Label(),
FBParamControl(plugGUI, param),
_maxTextWidth(FBGetStringWidthCached(maxWidthText))
{
  setText(defaultText, dontSendNotification);
}

void
FBOutputParamLabel::parentHierarchyChanged()
{
  ParentHierarchyChanged();
}

int
FBOutputParamLabel::FixedWidth(int height) const
{
  return _maxTextWidth + getBorderSize().getLeftAndRight();
}

String
FBOutputParamLabel::getTooltip()
{
  return _plugGUI->GetTooltipForAudioParam(_param->runtimeParamIndex);
}

void
FBOutputParamLabel::SetValueNormalizedFromHost(float normalized)
{
  setText(_param->static_.NormalizedToText(FBValueTextDisplay::Text, normalized), dontSendNotification);
}