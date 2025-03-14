#pragma once

#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>

#include <string>
#include <optional>

struct FBBoolParam
{
  bool NormalizedToPlainFast(float normalized) const;
}; 

struct FBBoolParamNonRealTime final:
public FBBoolParam,
public FBParamNonRealTime
{
  bool IsItems() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  FBEditType GUIEditType() const override;
  FBEditType AutomationEditType() const override;

  double PlainToNormalized(double plain) const override;
  double NormalizedToPlain(double normalized) const override;
  std::string PlainToText(FBTextDisplay display, double plain) const override;
  std::optional<double> TextToPlain(FBTextDisplay display, std::string const& text) const override;
};

inline bool
FBBoolParam::NormalizedToPlainFast(float normalized) const
{
  return normalized >= 0.5f;
}