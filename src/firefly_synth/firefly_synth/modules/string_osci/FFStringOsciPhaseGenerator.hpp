#pragma once

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/dsp/shared/FBDSPUtility.hpp>

#include <xsimd/xsimd.hpp>
#include <cmath>

class FFStringOsciPhaseGenerator final
{
  float _x = 0.0f;
public:
  float Next(float incr);
  FFStringOsciPhaseGenerator() = default;
  explicit FFStringOsciPhaseGenerator(float x) : _x(x) {}
};

inline float
FFStringOsciPhaseGenerator::Next(float incr)
{
  float y = _x;
  _x += incr;
  if (_x >= 1.0f)
    _x = 0.0f;
  return y;
}