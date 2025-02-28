#include <playground_base/base/shared/FBFormat.hpp>
#include <playground_base/base/topo/param/FBLog2Param.hpp>

void
FBLog2ParamRealTime::Init(float offset, float curveStart, float curveEnd)
{
  _offset = offset;
  _curveStart = curveStart;
  _expo = std::log(curveEnd / curveStart) / std::log(2.0f);
}

float 
FBLog2ParamNonRealTime::PlainToNormalized(float plain) const
{
  float result = std::log2((plain - _offset) / _curveStart) / _expo;
  assert(0.0f <= result && result <= 1.0f);
  return result;
}

float
FBLog2ParamNonRealTime::NormalizedToPlain(float normalized) const
{
  return FBLog2ParamRealTime::NormalizedToPlain(normalized);
}

std::string
FBLog2ParamNonRealTime::PlainToText(FBValueTextDisplay display, float plain) const
{
  if (display == FBValueTextDisplay::IO)
    return std::to_string(plain);
  return FBFormatFloat(plain, FBDefaultDisplayPrecision);
}

std::optional<float>
FBLog2ParamNonRealTime::TextToPlain(FBValueTextDisplay display, std::string const& text) const
{
  char* end;
  float result = std::strtof(text.c_str(), &end);
  if (end != text.c_str() + text.size())
    return {};
  if (result < NormalizedToPlain(0.0f) || result > NormalizedToPlain(1.0f))
    return {};
  return { result };
}