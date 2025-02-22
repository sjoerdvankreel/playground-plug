#pragma once

#include <string>
#include <optional>
#include <algorithm>
#include <functional>

struct FBDiscreteParam
{
  int valueCount = {};

  int ValueCount() const { return valueCount; }
  float PlainToNormalized(int plain) const;
  int NormalizedToPlain(float normalized) const;
  std::string PlainToText(int plain) const;
  std::optional<int> TextToPlain(std::string const& text) const;
};

inline int
FBDiscreteParam::NormalizedToPlain(float normalized) const
{
  return std::clamp((int)(normalized * valueCount), 0, valueCount - 1);
}