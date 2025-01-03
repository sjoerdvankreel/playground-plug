#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedUtility.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatBlock.hpp>

#include <array>
#include <algorithm>

class alignas(sizeof(FBFloatVector)) FBAccParamState final
{
  friend class FBSmoothingProcessor;
  friend class FBVoiceAccParamState;
  friend class FBGlobalAccParamState;

  FBFixedFloatArray _cv = {};
  float _modulation = {};
  FBOnePoleFilter _smoother = {};

  void SmoothNext(int sample, float automation);
  void Modulate(float offset) { _modulation = offset; }

  void InitProcessing(float value);
  void SetSmoothingCoeffs(float sampleRate, float durationSecs);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBAccParamState);
  FBFloatVector CV(int v) const;
  FBFixedFloatArray const& CV() const { return _cv; }
};

inline FBFloatVector
FBAccParamState::CV(int v) const
{
  return FBFloatVector::load_aligned(_cv.data.data() + v * FBVectorFloatCount);
}

inline void
FBAccParamState::InitProcessing(float value)
{
  std::fill(_cv.data.begin(), _cv.data.end(), value);
}

inline void 
FBAccParamState::SetSmoothingCoeffs(float sampleRate, float durationSecs) 
{ 
  _smoother.SetCoeffs(sampleRate, durationSecs); 
}

inline void
FBAccParamState::SmoothNext(int sample, float automation) 
{ 
  _cv.data[sample] = _smoother.Next(std::clamp(automation + _modulation, 0.0f, 1.0f));
}