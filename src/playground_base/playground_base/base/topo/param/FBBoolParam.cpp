#include <playground_base/base/topo/param/FBBoolParam.hpp>

int 
FBBoolParamNonRealTime::ValueCount() const
{
  return 2;
}

float 
FBBoolParamNonRealTime::PlainToNormalized(int plain) const
{
  return plain ? 1.0f : 0.0f;
}

int
FBBoolParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBBoolParamRealTime::NormalizedToPlain(normalized);
}

std::string
FBBoolParamNonRealTime::PlainToText(FBValueTextDisplay display, int plain) const
{
  return plain ? "On" : "Off";
}

std::optional<int>
FBBoolParamNonRealTime::TextToPlain(bool io, std::string const& text) const
{
  if (text == "On")
    return true;
  if (text == "Off")
    return false;
  return {};
}