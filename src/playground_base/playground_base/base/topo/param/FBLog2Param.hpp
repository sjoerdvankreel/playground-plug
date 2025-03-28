#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

#include <cmath>
#include <string>
#include <cassert>
#include <optional>
#include <algorithm>

struct FBLog2Param
{
protected:
  float _expo = {};
  float _offset = {};
  float _curveStart = {};

public:
  float NormalizedToPlainFast(float normalized) const;
  void Init(float offset, float curveStart, float curveEnd);
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;

  FBFloatVector NormalizedToPlainFast(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlainFast(FBDoubleVector normalized) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatBlock& plain) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleBlock& plain) const;
};

struct FBLog2ParamNonRealTime final :
public FBLog2Param,
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

inline float
FBLog2Param::NormalizedToPlainFast(float normalized) const
{
  float result = _offset + _curveStart * std::pow(2.0f, _expo * normalized);
  assert(result >= _curveStart + _offset);
  return result;
}

inline FBFloatVector 
FBLog2Param::NormalizedToPlainFast(FBFloatVector normalized) const
{
  return _offset + _curveStart * xsimd::pow(FBFloatVector(2.0f), _expo * normalized);
}

inline FBDoubleVector 
FBLog2Param::NormalizedToPlainFast(FBDoubleVector normalized) const
{
#pragma warning(push)
#pragma warning(disable : 4244)
  return _offset + _curveStart * xsimd::pow(FBDoubleVector(2.0), _expo * normalized);
#pragma warning(pop)
}

inline int
FBLog2Param::NormalizedTimeToSamplesFast(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline int
FBLog2Param::NormalizedFreqToSamplesFast(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline void
FBLog2Param::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatBlock& plain) const
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    plain[v] = NormalizedToPlainFast(normalized.CV(v));
}

inline void
FBLog2Param::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleBlock& plain) const
{
  FBFixedDoubleBlock normDouble;
  normDouble.LoadCastFromFloatArray(normalized.CV());
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    plain[v] = NormalizedToPlainFast(normDouble[v]);
}