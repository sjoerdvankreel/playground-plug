#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <playground_base/base/topo/param/FBLog2Param.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBBarsParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>

#include <playground_base/base/topo/static/FBParamType.hpp>
#include <playground_base/base/topo/static/FBTextDisplay.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

#include <juce_gui_basics/juce_gui_basics.h>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParamBase
{
private:
  FBLog2Param log2 = {};
  FBListParam list = {};
  FBNoteParam note = {};
  FBBarsParam bars = {};
  FBBoolParam boolean = {};
  FBLinearParam linear = {};
  FBDiscreteParam discrete = {};

public:
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string tooltip = {};
  std::string defaultText = {};
  FBParamType type = (FBParamType)-1;
  FBParamsDependencies dependencies = {};

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParamBase);

  int ValueCount() const;

  FBListParam& List();
  FBNoteParam& Note();
  FBBarsParam& Bars();
  FBLog2Param& Log2();
  FBBoolParam& Boolean();
  FBLinearParam& Linear();
  FBDiscreteParam& Discrete();

  FBListParam const& List() const;
  FBNoteParam const& Note() const;
  FBBarsParam const& Bars() const;
  FBLog2Param const& Log2() const;
  FBBoolParam const& Boolean() const;
  FBLinearParam const& Linear() const;
  FBDiscreteParam const& Discrete() const;

  juce::PopupMenu MakePopupMenu() const;
  float DefaultNormalizedByText() const;
  float AnyDiscreteToNormalizedSlow(int discrete) const;
  int NormalizedToAnyDiscreteSlow(float normalized) const;

  std::optional<float> TextToNormalized(bool io, std::string const& text) const;
  std::string NormalizedToText(FBValueTextDisplay display, float normalized) const;
  std::string NormalizedToText(FBParamTextDisplay display, float normalized) const;
};

inline FBListParam&
FBStaticParamBase::List()
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam&
FBStaticParamBase::Note()
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBarsParam&
FBStaticParamBase::Bars()
{
  assert(type == FBParamType::Bars);
  return bars;
}

inline FBBoolParam&
FBStaticParamBase::Boolean()
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam&
FBStaticParamBase::Linear()
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBLog2Param&
FBStaticParamBase::Log2()
{
  assert(type == FBParamType::Log2);
  return log2;
}

inline FBDiscreteParam&
FBStaticParamBase::Discrete()
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline FBListParam const&
FBStaticParamBase::List() const
{
  assert(type == FBParamType::List);
  return list;
}

inline FBNoteParam const&
FBStaticParamBase::Note() const
{
  assert(type == FBParamType::Note);
  return note;
}

inline FBBarsParam const&
FBStaticParamBase::Bars() const
{
  assert(type == FBParamType::Bars);
  return bars;
}

inline FBBoolParam const&
FBStaticParamBase::Boolean() const
{
  assert(type == FBParamType::Boolean);
  return boolean;
}

inline FBLinearParam const&
FBStaticParamBase::Linear() const
{
  assert(type == FBParamType::Linear);
  return linear;
}

inline FBLog2Param const&
FBStaticParamBase::Log2() const
{
  assert(type == FBParamType::Log2);
  return log2;
}

inline FBDiscreteParam const&
FBStaticParamBase::Discrete() const
{
  assert(type == FBParamType::Discrete);
  return discrete;
}

inline int
FBStaticParamBase::ValueCount() const
{
  switch (type)
  {
  case FBParamType::List: return list.ValueCount();
  case FBParamType::Note: return note.ValueCount();
  case FBParamType::Bars: return bars.ValueCount();
  case FBParamType::Log2: return log2.ValueCount();
  case FBParamType::Linear: return linear.ValueCount();
  case FBParamType::Boolean: return boolean.ValueCount();
  case FBParamType::Discrete: return discrete.ValueCount();
  default: assert(false); return {};
  }
}