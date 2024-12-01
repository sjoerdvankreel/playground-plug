#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_base/base/plug/FBPlugState.hpp>
#include <playground_base/dsp/shared/FBOnePoleFilter.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>

#include <array>
#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };
enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciAccGain, FFOsciAccPitch, FFOsciAccCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciBlockCount };
enum { FFShaperAccGain, FFShaperAccCount };
enum { FFShaperBlockOn, FFShaperBlockClip, FFShaperBlockCount };

struct FFOsciBlockState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockState);
  std::array<std::array<float, 1>, FF_OSCI_COUNT> on = {};
  std::array<std::array<float, 1>, FF_OSCI_COUNT> type = {};
};

struct FFShaperBlockState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockState);
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> on = {};
  std::array<std::array<float, 1>, FF_SHAPER_COUNT> clip = {};
};

template <class T>
struct alignas(alignof(T)) FFShaperAccState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccState);
  std::array<std::array<T, 1>, FF_SHAPER_COUNT> gain = {};
};

template <class T>
struct alignas(alignof(T)) FFOsciAccState
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccState);
  std::array<std::array<T, 1>, FF_OSCI_COUNT> pitch = {};
  std::array<std::array<T, FF_OSCI_GAIN_COUNT>, FF_OSCI_COUNT> gain = {};
};

template <class T>
struct alignas(alignof(T)) FFAccState
{
  FFOsciAccState<T> osci = {};
  FFShaperAccState<T> shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFAccState);
};

struct FFBlockState
{
  FFOsciBlockState osci = {};
  FFShaperBlockState shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFBlockState);
};

struct FFScalarState:
public FBScalarStateAddrs
{
  FFBlockState block = {};
  FFAccState<float> acc = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct alignas(alignof(FBFixedCVBlock)) FFProcState:
public FFScalarState,
public FBProcStateAddrs
{
  FFAccState<int> pos = {};
  FFAccState<FBFixedCVBlock> cv = {};
  FFAccState<FBOnePoleFilter> smooth = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};