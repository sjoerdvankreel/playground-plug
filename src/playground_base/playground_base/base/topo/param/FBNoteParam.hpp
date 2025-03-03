#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBListItem.hpp>
#include <playground_base/base/topo/static/FBListParamNonRealTime.hpp>

#include <vector>
#include <string>
#include <optional>
#include <algorithm>

struct FBNoteParamRealTime
{
  static inline std::string const C4Name = "C4";
  static inline int constexpr MidiNoteCount = 128;

  int NormalizedToPlain(float normalized) const;
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteParamRealTime);
};

struct FBNoteParamNonRealTime:
public FBNoteParamRealTime,
public IFBListParamNonRealTime
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FBNoteParamNonRealTime);
  bool IsList() const override;
  bool IsStepped() const override;
  int ValueCount() const override;
  juce::PopupMenu MakePopupMenu() const override;
  float PlainToNormalized(int plain) const override;
  int NormalizedToPlain(float normalized) const override;
  std::string PlainToText(FBValueTextDisplay display, int plain) const override;
  std::optional<int> TextToPlain(FBValueTextDisplay display, std::string const& text) const override;
};

inline int
FBNoteParamRealTime::NormalizedToPlain(float normalized) const
{
  return std::clamp((int)(normalized * MidiNoteCount), 0, MidiNoteCount - 1);
}