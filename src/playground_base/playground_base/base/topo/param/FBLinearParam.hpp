#pragma once

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBParamNonRealTime.hpp>
#include <playground_base/base/state/proc/FBAccParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

#include <cmath>
#include <string>
#include <optional>
#include <algorithm>

struct FBLinearParam
{
  float min = 0.0f;
  float max = 1.0f;
  double editSkewFactor = 1.0;
  double displayMultiplier = 1.0;

  float NormalizedToPlainFast(float normalized) const;
  int NormalizedTimeToSamplesFast(float normalized, float sampleRate) const;
  int NormalizedFreqToSamplesFast(float normalized, float sampleRate) const;

  FBFloatVector NormalizedToPlainFast(FBFloatVector normalized) const;
  FBDoubleVector NormalizedToPlainFast(FBDoubleVector normalized) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatBlock& plain) const;
  void NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleBlock& plain) const;
};

struct FBLinearParamNonRealTime final :
public FBLinearParam,
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
FBLinearParam::NormalizedToPlainFast(float normalized) const
{
  return min + (max - min) * normalized;
}

inline FBFloatVector 
FBLinearParam::NormalizedToPlainFast(FBFloatVector normalized) const
{
  return min + (max - min) * normalized;
}

inline FBDoubleVector 
FBLinearParam::NormalizedToPlainFast(FBDoubleVector normalized) const
{
  return min + (max - min) * normalized; 
}

inline int
FBLinearParam::NormalizedTimeToSamplesFast(float normalized, float sampleRate) const
{
  return FBTimeToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline int
FBLinearParam::NormalizedFreqToSamplesFast(float normalized, float sampleRate) const
{
  return FBFreqToSamples(NormalizedToPlainFast(normalized), sampleRate);
}

inline void 
FBLinearParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedFloatBlock& plain) const
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    plain[v] = NormalizedToPlainFast(normalized.CV(v));
}

inline void
FBLinearParam::NormalizedToPlainFast(FBAccParamState const& normalized, FBFixedDoubleBlock& plain) const
{
  FBFixedDoubleBlock normDouble;
  normDouble.LoadCastFromFloatArray(normalized.CV());
  for (int v = 0; v < FBFixedDoubleVectors; v++)
    plain[v] = NormalizedToPlainFast(normDouble[v]);
}