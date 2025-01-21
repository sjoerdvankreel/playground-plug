#include <playground_base/base/topo/FBRuntimeParam.hpp>
#include <playground_base/gui/shared/FBPlugGUI.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>
#include <playground_base/gui/controls/FBParamComboBox.hpp>

using namespace juce;

FBParamComboBox::
FBParamComboBox(FBPlugGUI* plugGUI, FBRuntimeParam const* param):  
ComboBox(),
FBParamControl(plugGUI, param)
{
  for (int i = 0; i < param->static_.ValueCount(); i++)
  {
    float normalized = param->static_.AnyDiscreteToNormalizedSlow(i);
    addItem(param->static_.NormalizedToText(false, normalized), i + 1);
  }
  SetValueNormalizedFromHost(plugGUI->HostContext()->GetParamNormalized(param->runtimeParamIndex));
}

int
FBParamComboBox::FixedWidth(int height) const
{
  Font font(12.0f); // TODO
  int maxTextWidth = 0;
  for (int i = 0; i < getNumItems(); i++)
    maxTextWidth = std::max(maxTextWidth, (int)std::ceil(
      TextLayout::getStringWidth(font, getItemText(i))));
  return maxTextWidth + 48; // TODO
}

void
FBParamComboBox::SetValueNormalizedFromHost(float normalized)
{
  int plain = _param->static_.NormalizedToAnyDiscreteSlow(normalized);
  setSelectedItemIndex(plain, dontSendNotification);
}

void
FBParamComboBox::valueChanged(Value& value)
{
  float normalized = _param->static_.AnyDiscreteToNormalizedSlow(getSelectedItemIndex());
  _plugGUI->SteppedParamNormalizedChanged(_param->runtimeParamIndex);
  _plugGUI->HostContext()->PerformParamEdit(_param->runtimeParamIndex, normalized);
}