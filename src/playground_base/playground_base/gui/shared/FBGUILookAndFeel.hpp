#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class FBParamSlider;

class FBGUILookAndFeel:
public juce::LookAndFeel_V4
{
  void DrawLinearSliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider, 
    int y, int height, float exchangeValue);

  void DrawRotarySliderExchangeThumb(
    juce::Graphics& g, FBParamSlider& slider,
    int x, int y, int width, int height,
    float rotaryStartAngle, float rotaryEndAngle, 
    float exchangeValue);

public:
  void drawLinearSlider(
    juce::Graphics&, 
    int x, int y, int width, int height, 
    float sliderPos, float minSliderPos, float maxSliderPos, 
    juce::Slider::SliderStyle, juce::Slider&) override;

  void drawRotarySlider(
    juce::Graphics&,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
};