#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBParamsDependencies.hpp>

#include <playground_base/base/topo/param/FBLog2Param.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBNoteParam.hpp>
#include <playground_base/base/topo/param/FBBarsParam.hpp>
#include <playground_base/base/topo/param/FBParamType.hpp>
#include <playground_base/base/topo/param/FBTextDisplay.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteLog2Param.hpp>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParamBase
{
private:
  FBLog2ParamNonRealTime log2 = {};
  FBListParamNonRealTime list = {};
  FBNoteParamNonRealTime note = {};
  FBBarsParamNonRealTime bars = {};
  FBBoolParamNonRealTime boolean = {};
  FBLinearParamNonRealTime linear = {};
  FBDiscreteParamNonRealTime discrete = {};
  FBDiscreteLog2ParamNonRealTime discreteLog2 = {};

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

  FBListParam& List();
  FBNoteParam& Note();
  FBBarsParam& Bars();
  FBLog2Param& Log2();
  FBBoolParam& Boolean();
  FBLinearParam& Linear();
  FBDiscreteParam& Discrete();
  FBDiscreteLog2Param& DiscreteLog2();
  
  FBListParam const& List() const;
  FBNoteParam const& Note() const;
  FBBarsParam const& Bars() const;
  FBLog2Param const& Log2() const;
  FBBoolParam const& Boolean() const;
  FBLinearParam const& Linear() const;
  FBDiscreteParam const& Discrete() const;
  FBDiscreteLog2Param const& DiscreteLog2() const;
  
  FBParamNonRealTime const& NonRealTime() const;
  FBItemsParamNonRealTime const& ItemsNonRealTime() const;

  double DefaultNormalizedByText() const;
  std::string NormalizedToTextWithUnit(FBTextDisplay display, double normalized) const;
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

inline FBDiscreteLog2Param&
FBStaticParamBase::DiscreteLog2()
{
  assert(type == FBParamType::DiscreteLog2);
  return discreteLog2;
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

inline FBDiscreteLog2Param const&
FBStaticParamBase::DiscreteLog2() const
{
  assert(type == FBParamType::DiscreteLog2);
  return discreteLog2;
}