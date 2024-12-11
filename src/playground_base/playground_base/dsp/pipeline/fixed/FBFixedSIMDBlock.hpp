#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

inline int constexpr FBFixedSIMDBlockFloatVectorCount = 
FBFixedBlockSize / FBSIMDVectorFloatCount;

class alignas(alignof(FBSIMDFloatVector)) FBFixedSIMDBlock
{
  static inline int constexpr VectorCount = FBFixedSIMDBlockFloatVectorCount;
  std::array<FBSIMDFloatVector, VectorCount> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedSIMDBlock);

  float& operator[](int index);
  float operator[](int index) const;
  
  void FB_SIMD_CALL SetToZero();
  void FB_SIMD_CALL SetToSineOfTwoPi();
  void FB_SIMD_CALL SetToUnipolarSineOfTwoPi();
  void FB_SIMD_CALL CopyFrom(FBFixedSIMDBlock const& rhs);
  void FB_SIMD_CALL MultiplyByOneMinus(FBFixedSIMDBlock const& rhs);
  void FB_SIMD_CALL FMA1(FBFixedSIMDBlock const& b, FBFixedSIMDBlock const& c);
  void FB_SIMD_CALL FMA2(FBFixedSIMDBlock const& b, FBFixedSIMDBlock const& c, FBFixedSIMDBlock const& d);

  FBFixedSIMDBlock& FB_SIMD_CALL operator+=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FB_SIMD_CALL operator-=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FB_SIMD_CALL operator*=(FBFixedSIMDBlock const& rhs);
  FBFixedSIMDBlock& FB_SIMD_CALL operator/=(FBFixedSIMDBlock const& rhs);
};

inline float& 
FBFixedSIMDBlock::operator[](int index)
{
  return _store[index / FBSIMDVectorFloatCount][index % FBSIMDVectorFloatCount];
}

inline float 
FBFixedSIMDBlock::operator[](int index) const
{
  return _store[index / FBSIMDVectorFloatCount][index % FBSIMDVectorFloatCount];
}

inline void
FBFixedSIMDBlock::SetToZero()
{
  for (int v = 0; v < VectorCount; v++)
    _store[v].SetToZero();
}

inline void
FBFixedSIMDBlock::SetToSineOfTwoPi()
{
  for (int v = 0; v < VectorCount; v++)
    _store[v].SetToSineOfTwoPi();
}

inline void
FBFixedSIMDBlock::SetToUnipolarSineOfTwoPi()
{
  for (int v = 0; v < VectorCount; v++)
    _store[v].SetToUnipolarSineOfTwoPi();
}

inline void
FBFixedSIMDBlock::CopyFrom(FBFixedSIMDBlock const& rhs)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v] = rhs._store[v];
}

inline void
FBFixedSIMDBlock::MultiplyByOneMinus(FBFixedSIMDBlock const& rhs)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v].MultiplyByOneMinus(rhs._store[v]);
}

inline void
FBFixedSIMDBlock::FMA1(FBFixedSIMDBlock const& b, FBFixedSIMDBlock const& c)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v].FMA1(b._store[v], c._store[v]);
}

inline void
FBFixedSIMDBlock::FMA2(FBFixedSIMDBlock const& b, FBFixedSIMDBlock const& c, FBFixedSIMDBlock const& d)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v].FMA2(b._store[v], c._store[v], d._store[v]);
}

inline FBFixedSIMDBlock& 
FBFixedSIMDBlock::operator+=(FBFixedSIMDBlock const& rhs)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v] += rhs._store[v];
  return *this;
}

inline FBFixedSIMDBlock& 
FBFixedSIMDBlock::operator-=(FBFixedSIMDBlock const& rhs)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v] -= rhs._store[v];
  return *this;
}

inline FBFixedSIMDBlock&
FBFixedSIMDBlock::operator*=(FBFixedSIMDBlock const& rhs)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v] *= rhs._store[v];
  return *this;
}

inline FBFixedSIMDBlock&
FBFixedSIMDBlock::operator/=(FBFixedSIMDBlock const& rhs)
{
  for (int v = 0; v < VectorCount; v++)
    _store[v] /= rhs._store[v];
  return *this;
}