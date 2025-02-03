#include <playground_base/base/topo/FBStaticParam.hpp>

float
FBStaticParam::DefaultNormalizedByText() const
{
  if (defaultText.size() == 0)
    return 0.0f;
  return TextToNormalized(false, defaultText).value();
}

FBAutomationType 
FBStaticParam::AutomationType() const
{
  if (acc)
    return FBAutomationType::Modulate;
  if (!FBParamTypeIsStepped(type) && voiceBlockAddr != nullptr)
    return FBAutomationType::Automate;
  return FBAutomationType::None;
}

float
FBStaticParam::AnyDiscreteToNormalizedSlow(int plain) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.PlainToNormalized(plain);
  case FBParamType::Boolean:
    return boolean.PlainToNormalized(plain);
  case FBParamType::Discrete:
    return discrete.PlainToNormalized(plain);
  default:
    assert(false);
    return {};
  }
}

int 
FBStaticParam::NormalizedToAnyDiscreteSlow(float normalized) const
{
  switch (type)
  {
  case FBParamType::List:
    return list.NormalizedToPlain(normalized);
  case FBParamType::Boolean:
    return boolean.NormalizedToPlain(normalized);
  case FBParamType::Discrete:
    return discrete.NormalizedToPlain(normalized);
  default:
    assert(false);
    return {};
  }
}

std::string 
FBStaticParam::NormalizedToText(FBTextDisplay display, float normalized) const
{
  switch (type)
  {
  case FBParamType::Boolean:
    return boolean.PlainToText(boolean.NormalizedToPlain(normalized));
  case FBParamType::Discrete:
    return discrete.PlainToText(discrete.NormalizedToPlain(normalized));
  case FBParamType::List:
    return list.PlainToText(display, list.NormalizedToPlain(normalized));
  case FBParamType::Linear:
    return linear.PlainToText(display, linear.NormalizedToPlain(normalized));
  case FBParamType::FreqOct:
    return freqOct.PlainToText(display, freqOct.NormalizedToPlain(normalized));
  default:
    assert(false);
    return {};
  }
}

std::optional<float> 
FBStaticParam::TextToNormalized(bool io, std::string const& text) const
{
  switch (type)
  {
    case FBParamType::List:
    {
      auto plain = list.TextToPlain(io, text);
      if (!plain) return {};
      return list.PlainToNormalized(plain.value());
    }
    case FBParamType::Boolean:
    {
      auto plain = boolean.TextToPlain(text);
      if (!plain) return {};
      return boolean.PlainToNormalized(plain.value());
    }
    case FBParamType::Linear:
    {
      auto plain = linear.TextToPlain(text);
      if (!plain) return {};
      return linear.PlainToNormalized(plain.value());
    }
    case FBParamType::FreqOct:
    {
      auto plain = freqOct.TextToPlain(text);
      if (!plain) return {};
      return freqOct.PlainToNormalized(plain.value());
    }
    case FBParamType::Discrete:
    {
      auto plain = discrete.TextToPlain(text);
      if (!plain) return {};
      return discrete.PlainToNormalized(plain.value());
    }
    default:
    {
      assert(false);
      return {};
    }
  }
}