#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>

#include <array>

template <class T, int N, int A>
class alignas(A) FBStaticArray
{
  std::array<T, N> _data = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBStaticArray);

  T& operator[](int i) { return _data[i]; }
  T const& operator[](int i) const { return _data[i]; }
  std::array<T, N>& Data() { return _data; }
  std::array<T, N> const& Data() const { return _data; }

  void Fill(T val) { _data.fill(val); }
  T const& Last() { return _data[N - 1]; }
  void Add(FBStaticArray const& rhs) { for (int s = 0; s < N; s++) _data[s] += rhs[s]; }
  void Mul(FBStaticArray const& rhs) { for (int s = 0; s < N; s++) _data[s] *= rhs[s]; }
  void CopyTo(FBStaticArray& rhs) const { for (int s = 0; s < N; s++) rhs[s] = _data[s]; }
};

template <class T>
using FBFixedArray = FBStaticArray<T, FBFixedBlockSamples, FBFixedBlockAlign>;

template <class T>
struct alignas(FBFixedBlockAlign) FBFixedAudioArray
{ 
  std::array<FBFixedArray<T>, 2> _data = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FBFixedAudioArray);
  FBFixedArray<T>& operator[](int c) { return _data[c]; }
  FBFixedArray<T> const& operator[](int c) const { return _data[c]; }

  void Fill(T val) { for(int c = 0; c < 2; c++) _data[c].Data().fill(val); }
  void Mul(FBFixedArray<T> const& rhs) { for (int c = 0; c < 2; c++) _data[c].Mul(rhs); }
  void Add(FBFixedAudioArray<T> const& rhs) { for (int c = 0; c < 2; c++) _data[c].Add(rhs[c]); }
  void CopyTo(FBFixedAudioArray<T>& rhs) const { for (int c = 0; c < 2; c++) _data[c].CopyTo(rhs[c]); }
};

typedef FBFixedArray<float> FBFixedFloatArray;
typedef FBFixedArray<double> FBFixedDoubleArray;
typedef FBFixedAudioArray<float> FBFixedFloatAudioArray;
typedef FBFixedAudioArray<double> FBFixedDoubleAudioArray;

inline void 
FBFixedFloatToDoubleArray(FBFixedFloatArray const& floats, FBFixedDoubleArray& doubles)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    doubles[s] = floats[s];
}

inline void
FBFixedDoubleToFloatArray(FBFixedDoubleArray const& doubles, FBFixedFloatArray& floats)
{
  for (int s = 0; s < FBFixedBlockSamples; s++)
    floats[s] = static_cast<float>(doubles[s]);
}

inline void
FBFixedFloatAudioToDoubleArray(FBFixedFloatAudioArray const& floats, FBFixedDoubleAudioArray& doubles)
{
  for (int c = 0; c < 2; c++)
    FBFixedFloatToDoubleArray(floats[c], doubles[c]);
}

inline void
FBFixedDoubleAudioToFloatArray(FBFixedDoubleAudioArray const& doubles, FBFixedFloatAudioArray& floats)
{
  for (int c = 0; c < 2; c++)
    FBFixedDoubleToFloatArray(doubles[c], floats[c]);
}