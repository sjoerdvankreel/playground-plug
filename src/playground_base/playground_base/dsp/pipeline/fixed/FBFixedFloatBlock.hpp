#pragma once

#include <playground_base/base/shared/FBVector.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

typedef std::array<double, FBFixedBlockSamples> FBFixedDoubleArray;
inline int constexpr FBFixedFloatVectors = FBFixedBlockSamples / FBVectorFloatCount;

class alignas(sizeof(FBFloatVector)) FBFixedFloatBlock
{
  std::array<FBFloatVector, FBFixedFloatVectors> _store = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedFloatBlock);

  template <class Op>
  void Transform(Op op);
  void Clear();
  void Add(FBFixedFloatBlock const& rhs);

  void LoadUnaligned(float const* vals);
  void StoreUnaligned(float* vals) const;
  void LoadAligned(int v, float const* vals);
  void StoreAligned(int v, float* vals) const;
  void StoreToDouble(FBFixedDoubleArray& array) const;
  FBFloatVector& operator[](int index) { return _store[index]; }
  FBFloatVector const& operator[](int index) const { return _store[index]; } 
};

template <class Op>
void
FBFixedFloatBlock::Transform(Op op)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    (*this)[v] = op(v);
}

inline void
FBFixedFloatBlock::Clear()
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] = 0.0f;
}

inline void
FBFixedFloatBlock::Add(FBFixedFloatBlock const& rhs)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] += rhs._store[v];
}

inline void
FBFixedFloatBlock::StoreAligned(int v, float* vals) const
{
  _store[v].store_aligned(vals);
}

inline void
FBFixedFloatBlock::LoadAligned(int v, float const* vals)
{
  _store[v] = FBFloatVector::load_unaligned(vals);
}

inline void
FBFixedFloatBlock::StoreUnaligned(float* vals) const
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v].store_unaligned(vals + v * FBVectorFloatCount);
}

inline void
FBFixedFloatBlock::LoadUnaligned(float const* vals)
{
  for (int v = 0; v < FBFixedFloatVectors; v++)
    _store[v] = FBFloatVector::load_unaligned(vals + v * FBVectorFloatCount);
}

inline void
FBFixedFloatBlock::StoreToDouble(FBFixedDoubleArray& array) const
{
  alignas(sizeof(FBFloatVector)) std::array<float, FBVectorFloatCount> floats;
  for (int v = 0; v < FBFixedFloatVectors; v++)
  {
    StoreAligned(v, floats.data());
    for (int i = 0; i < FBVectorFloatCount; i++)
      array[v * FBVectorFloatCount + i] = floats[i];
  }
}