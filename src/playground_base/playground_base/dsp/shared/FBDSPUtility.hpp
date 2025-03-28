#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/topo/param/FBBarsItem.hpp>

#include <cmath>
#include <array>
#include <numbers>
#include <cassert>

inline constexpr int FBNoteCount = 128;
inline constexpr int FBCentCount = 100;
inline constexpr int FBNoteCentCount = FBNoteCount * FBCentCount;

inline constexpr float FBPi = std::numbers::pi_v<float>;
inline constexpr float FBTwoPi = 2.0f * FBPi;

std::array<float, FBNoteCentCount> const&
FBPitchToFreqTable();

inline float
FBToUnipolar(float v)
{
  return (v + 1.0f) * 0.5f;
}

inline float
FBToBipolar(float v)
{
  return (v * 2.0f) - 1.0f;
}

inline FBFloatVector
FBToUnipolar(FBFloatVector v)
{
  return (v + 1.0f) * 0.5f;
}

inline FBFloatVector
FBToBipolar(FBFloatVector v)
{
  return (v * 2.0f) - 1.0f;
}

inline int
FBTimeToSamples(float time, float sampleRate)
{
  return static_cast<int>(std::round(time * sampleRate));
}

inline int
FBFreqToSamples(float freq, float sampleRate)
{
  assert(freq > 0.0f);
  return static_cast<int>(std::round(1.0f / freq * sampleRate));
}

inline int
FBBarsToSamples(FBBarsItem const& bars, float sampleRate, float bpm)
{
  return FBTimeToSamples((bars.num * 240.0f) / (bars.denom * bpm), sampleRate);
}

inline float
FBPitchToFreqAccurateRaw(float pitch)
{
  return 440.0f * std::pow(2.0f, (pitch - 69.0f) / 12.0f);
}

inline float
FBPitchToFreqAccurate(float pitch, float sampleRate)
{
  return std::min(sampleRate * 0.5f, FBPitchToFreqAccurateRaw(pitch));
}

inline float
FBPitchToFreqFastAndInaccurate(float pitch)
{
  float cent = std::clamp(pitch * 100.0f, 0.0f, static_cast<float>(FBNoteCentCount));
  float floor = std::floor(cent);
  float mix = cent - floor;
  int low = static_cast<int>(floor);
  int high = low + 1;
  return (1.0f - mix) * FBPitchToFreqTable()[low] + mix * FBPitchToFreqTable()[high];
}

inline FBFloatVector
FBPitchToFreqAccurateRaw(FBFloatVector pitch)
{
  return 440.0f * xsimd::pow(FBFloatVector(2.0f), (pitch - 69.0f) / 12.0f);
}

inline FBFloatVector
FBPitchToFreqAccurate(FBFloatVector pitch, float sampleRate)
{ 
  return xsimd::min(FBFloatVector(sampleRate) * 0.5f, FBPitchToFreqAccurateRaw(pitch));
}

inline FBFloatVector
FBPitchToFreqFastAndInaccurate(FBFloatVector pitch)
{
  FBFloatVector freqLow;
  FBFloatVector freqHigh;
  alignas(sizeof(FBIntVector)) std::array<int, FBVectorIntCount> centLowArray;
  alignas(sizeof(FBIntVector)) std::array<int, FBVectorIntCount> centHighArray;
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> freqLowArray;
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> freqHighArray;

  FBFloatVector cent = xsimd::clip(pitch * 100.0f, FBFloatVector(0.0f), FBFloatVector(static_cast<float>(FBNoteCentCount)));
  FBFloatVector floor = xsimd::floor(cent);
  FBFloatVector mix = cent - floor;
  FBIntVector centLow0 = xsimd::batch_cast<int>(floor);
  FBIntVector centHigh0 = centLow0 + 1;
  centLow0.store_aligned(&centLowArray[0]);
  centHigh0.store_aligned(&centHighArray[0]);
  for (int i = 0; i < FBVectorFloatCount; i++)
  {
    freqLowArray[i] = FBPitchToFreqTable()[centLowArray[i]];
    freqHighArray[i] = FBPitchToFreqTable()[centHighArray[i]];
  }
  freqLow = FBFloatVector::load_aligned(&freqLowArray[0]);
  freqHigh = FBFloatVector::load_aligned(&freqHighArray[0]);
  return (1.0f - mix) * freqLow + mix * freqHigh;
}