#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/param/FBParamType.hpp>
#include <playground_base/base/topo/param/FBBoolParam.hpp>
#include <playground_base/base/topo/param/FBListParam.hpp>
#include <playground_base/base/topo/param/FBLinearParam.hpp>
#include <playground_base/base/topo/param/FBFreqOctParam.hpp>
#include <playground_base/base/topo/param/FBDiscreteParam.hpp>
#include <playground_base/base/topo/param/FBParamAddrSelector.hpp>

#include <string>
#include <vector>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBStaticParam final
{
  bool acc = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  std::string unit = {};
  std::string defaultText = {};

  FBParamType type = {};
  FBListParam list = {};
  FBBoolParam boolean = {};
  FBLinearParam linear = {};
  FBFreqOctParam freqOct = {};
  FBDiscreteParam discrete = {};

  FBScalarAddrSelector scalarAddr = {};
  FBVoiceAccAddrSelector voiceAccAddr = {};
  FBGlobalAccAddrSelector globalAccAddr = {};
  FBVoiceBlockAddrSelector voiceBlockAddr = {};
  FBGlobalBlockAddrSelector globalBlockAddr = {};    

  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticParam);

  int ValueCount() const;
  double ListOrDiscreteToNormalizedSlow(int discrete) const;
  int NormalizedToListOrDiscreteSlow(double normalized) const;

  double DefaultNormalizedByText() const;
  std::string NormalizedToText(bool io, double normalized) const;
  std::optional<double> TextToNormalized(bool io, std::string const& text) const;
};

inline int
FBStaticParam::ValueCount() const
{
  switch (type)
  {
  case FBParamType::List: return list.ValueCount();
  case FBParamType::Linear: return linear.ValueCount();
  case FBParamType::FreqOct: return freqOct.ValueCount();
  case FBParamType::Boolean: return boolean.ValueCount();
  case FBParamType::Discrete: return discrete.ValueCount();
  default: assert(false); return {};
  }
}