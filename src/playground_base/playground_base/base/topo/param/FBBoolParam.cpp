#include <playground_base/base/topo/param/FBBoolParam.hpp>

bool
FBBoolParamNonRealTime::IsItems() const
{
  return false;
}

int
FBBoolParamNonRealTime::ValueCount() const
{
  return 2;
}

double
FBBoolParamNonRealTime::PlainToNormalized(double plain) const
{
  return plain >= 0.5 ? 1.0 : 0.0;
}

double
FBBoolParamNonRealTime::NormalizedToPlain(double normalized) const
{
  return normalized >= 0.5 ? 1.0 : 0.0;
}

std::string
FBBoolParamNonRealTime::PlainToText(FBValueTextDisplay display, double plain) const
{
  return plain >= 0.5 ? "On" : "Off";
}

std::optional<double>
FBBoolParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  if (text == "On")
    return 1.0;
  if (text == "Off")
    return 0.0;
  return {};
}