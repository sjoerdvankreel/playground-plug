#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

class FBPlugGUI:
public juce::Component  
{
protected:
  FBPlugGUI();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBPlugGUI);

public:
  void paint(juce::Graphics& g) override;

  // TODO scale factor
  int DefaultHeight() const;
  virtual int MinWidth() const = 0;
  virtual int MaxWidth() const = 0;
  virtual int DefaultWidth() const = 0;
  virtual int AspectRatioWidth() const = 0;
  virtual int AspectRatioHeight() const = 0;
  virtual void SetParamNormalized(int index, float normalized) = 0;
};