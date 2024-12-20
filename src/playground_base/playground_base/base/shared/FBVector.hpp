#pragma once

#include <numbers>

// todo neon
#if FB_USE_SSE
#if FB_USE_AVX
#error
#endif
#include <immintrin.h>

typedef __m128 FBVectorStore;
inline int constexpr FBVectorBitCount = 128;

#define FBVectorCall __vectorcall
#define FBVectorFloatAddr(x) (x.m128_f32)

#define FBVectorFloatAdd _mm_add_ps
#define FBVectorFloatSub _mm_sub_ps
#define FBVectorFloatMul _mm_mul_ps
#define FBVectorFloatDiv _mm_div_ps
#define FBVectorFloatSin _mm_sin_ps
#define FBVectorFloatSet1 _mm_set1_ps
#define FBVectorFloatCmpLt _mm_cmplt_ps
#define FBVectorFloatCmpGt _mm_cmpgt_ps
#define FBVectorFloatCmpLe _mm_cmple_ps
#define FBVectorFloatCmpGe _mm_cmpge_ps
#define FBVectorFloatCmpEq _mm_cmpeq_ps
#define FBVectorFloatCmpNeq _mm_cmpneq_ps
#define FBVectorFloatBlend  _mm_blendv_ps
#define FBVectorFloatSetZero _mm_setzero_ps

#elif FB_USE_AVX
#include <immintrin.h>

typedef __m256 FBVectorStore;
inline int constexpr FBVectorBitCount = 256;

#define FBVectorCall __vectorcall
#define FBVectorFloatAddr(x) (x.m256_f32)

#define FBVectorFloatAdd _mm256_add_ps
#define FBVectorFloatSub _mm256_sub_ps
#define FBVectorFloatMul _mm256_mul_ps
#define FBVectorFloatDiv _mm256_div_ps
#define FBVectorFloatSin _mm256_sin_ps
#define FBVectorFloatSet1 _mm256_set1_ps
#define FBVectorFloatCmpLt _mm256_cmplt_ps
#define FBVectorFloatCmpGt _mm256_cmpgt_ps
#define FBVectorFloatCmpLe _mm256_cmple_ps
#define FBVectorFloatCmpGe _mm256_cmpge_ps
#define FBVectorFloatCmpEq _mm256_cmpeq_ps
#define FBVectorFloatCmpNeq _mm256_cmpneq_ps
#define FBVectorFloatBlend  _mm256_blendv_ps
#define FBVectorFloatSetZero _mm256_setzero_ps

#else
#error
#endif

inline int constexpr FBVectorByteCount = FBVectorBitCount / 8;
inline int constexpr FBVectorFloatCount = FBVectorByteCount / sizeof(float);

class alignas(FBVectorByteCount) FBFloatVector
{
  FBVectorStore _store;
  FBFloatVector(FBVectorStore store) : _store(store) {}

public:
  FBFloatVector() : _store(FBVectorFloatSetZero()) {}
  FBFloatVector(float x) : _store(FBVectorFloatSet1(x)) {}

  friend FBFloatVector FBVectorCall operator+(float l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator-(float l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator*(float l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator/(float l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator+(FBFloatVector l, float r);
  friend FBFloatVector FBVectorCall operator-(FBFloatVector l, float r);
  friend FBFloatVector FBVectorCall operator*(FBFloatVector l, float r);
  friend FBFloatVector FBVectorCall operator/(FBFloatVector l, float r);
  friend FBFloatVector FBVectorCall operator+(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator-(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator*(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall operator/(FBFloatVector l, FBFloatVector r);

  friend FBFloatVector FBVectorCall FBFloatVectorCmpLt(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall FBFloatVectorCmpGt(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall FBFloatVectorCmpLe(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall FBFloatVectorCmpGe(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall FBFloatVectorCmpEq(FBFloatVector l, FBFloatVector r);
  friend FBFloatVector FBVectorCall FBFloatVectorCmpNeq(FBFloatVector l, FBFloatVector r);

  friend FBFloatVector FBVectorCall FBFloatVectorBlend(FBFloatVector l, FBFloatVector r, FBFloatVector mask);

  FBFloatVector& FBVectorCall operator+=(float rhs) { return *this = *this + rhs; }
  FBFloatVector& FBVectorCall operator-=(float rhs) { return *this = *this - rhs; }
  FBFloatVector& FBVectorCall operator*=(float rhs) { return *this = *this * rhs; }
  FBFloatVector& FBVectorCall operator/=(float rhs) { return *this = *this / rhs; }
  FBFloatVector& FBVectorCall operator+=(FBFloatVector rhs) { return *this = *this + rhs; }
  FBFloatVector& FBVectorCall operator-=(FBFloatVector rhs) { return *this = *this - rhs; }
  FBFloatVector& FBVectorCall operator*=(FBFloatVector rhs) { return *this = *this * rhs; }
  FBFloatVector& FBVectorCall operator/=(FBFloatVector rhs) { return *this = *this / rhs; }

  void FBVectorCall Clear() { *this = 0.0f; }
  float& operator[](int index) { return FBVectorFloatAddr(_store)[index]; }
  float operator[](int index) const { return FBVectorFloatAddr(_store)[index]; }

  FBFloatVector FBVectorCall Sin() { return FBVectorFloatSin(_store); }
  FBFloatVector FBVectorCall Unipolar() { return (*this + 1.0f) * 0.5f; }
  FBFloatVector& FBVectorCall operator=(FBFloatVector rhs) { _store = rhs._store; return *this; }
  FBFloatVector& FBVectorCall operator=(float rhs) { _store = FBVectorFloatSet1(rhs); return *this; }
};

inline FBFloatVector FBVectorCall
operator+(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatAdd(l._store, r._store); }
inline FBFloatVector FBVectorCall
operator-(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatSub(l._store, r._store); }
inline FBFloatVector FBVectorCall
operator*(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatMul(l._store, r._store); }
inline FBFloatVector FBVectorCall
operator/(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatDiv(l._store, r._store); }

inline FBFloatVector FBVectorCall
operator+(float l, FBFloatVector r)
{ return FBVectorFloatAdd(FBVectorFloatSet1(l), r._store); }
inline FBFloatVector FBVectorCall
operator-(float l, FBFloatVector r)
{ return FBVectorFloatSub(FBVectorFloatSet1(l), r._store); }
inline FBFloatVector FBVectorCall
operator*(float l, FBFloatVector r)
{ return FBVectorFloatMul(FBVectorFloatSet1(l), r._store); }
inline FBFloatVector FBVectorCall
operator/(float l, FBFloatVector r)
{ return FBVectorFloatDiv(FBVectorFloatSet1(l), r._store); }

inline FBFloatVector FBVectorCall
operator+(FBFloatVector l, float r)
{ return FBVectorFloatAdd(l._store, FBVectorFloatSet1(r)); }
inline FBFloatVector FBVectorCall
operator-(FBFloatVector l, float r)
{ return FBVectorFloatSub(l._store, FBVectorFloatSet1(r)); }
inline FBFloatVector FBVectorCall
operator*(FBFloatVector l, float r)
{ return FBVectorFloatMul(l._store, FBVectorFloatSet1(r)); }
inline FBFloatVector FBVectorCall
operator/(FBFloatVector l, float r)
{ return FBVectorFloatDiv(l._store, FBVectorFloatSet1(r)); }

inline FBFloatVector FBVectorCall 
FBFloatVectorCmpLt(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatCmpLt(l._store, r._store); }
inline FBFloatVector FBVectorCall 
FBFloatVectorCmpGt(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatCmpGt(l._store, r._store); }
inline FBFloatVector FBVectorCall 
FBFloatVectorCmpLe(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatCmpLe(l._store, r._store); }
inline FBFloatVector FBVectorCall 
FBFloatVectorCmpGe(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatCmpGe(l._store, r._store); }
inline FBFloatVector FBVectorCall 
FBFloatVectorCmpEq(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatCmpEq(l._store, r._store); }
inline FBFloatVector FBVectorCall 
FBFloatVectorCmpNeq(FBFloatVector l, FBFloatVector r)
{ return FBVectorFloatCmpNeq(l._store, r._store); }

inline FBFloatVector FBVectorCall 
FBFloatVectorBlend(FBFloatVector l, FBFloatVector r, FBFloatVector mask)
{ return FBVectorFloatBlend(l._store, r._store, mask._store); }