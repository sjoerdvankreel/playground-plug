#include <firefly_base/gui/shared/FBGUI.hpp>
#include <firefly_base/gui/controls/FBMultiLineLabel.hpp>

using namespace juce;

FBAutoSizeMultiLineLabel::
FBAutoSizeMultiLineLabel(std::string const& text, int vOffset):
Component(),
IFBHorizontalAutoSize(),
_vOffset(vOffset),
_text(text),
_textSize(FBGUIGetStringSizeCached(text)) {}

int 
FBAutoSizeMultiLineLabel::FixedWidth(int /*height*/) const
{
  return _textSize.x + 2;
}

void
FBAutoSizeMultiLineLabel::paint(Graphics& g)
{
  Label dummy;
  g.setFont(getLookAndFeel().getLabelFont(dummy));
  g.setColour(getLookAndFeel().findColour(Label::ColourIds::textColourId));
  int y = static_cast<int>(std::round((getBounds().getHeight() - _textSize.y) * 0.5f) + _vOffset);
  g.drawMultiLineText(_text, 1, y, _textSize.x, Justification::centred);
}