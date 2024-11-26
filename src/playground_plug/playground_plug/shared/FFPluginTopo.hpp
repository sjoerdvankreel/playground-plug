#pragma once

#include <playground_plug/shared/FFPluginConfig.hpp>
#include <playground_base/shared/FBPlugTopo.hpp>
#include <playground_base/shared/FBSignalBlock.hpp>
#include <playground_base/shared/FBSharedUtility.hpp>

#include <array>

FBStaticTopo
FFMakeTopo();

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };

enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciAccParamGain, FFOsciAccParamPitch, FFOsciAccParamCount };
enum { FFOsciBlockParamOn, FFOsciBlockParamType, FFOsciBlockParamCount };

enum { FFShaperAccParamGain, FFShaperAccParamCount };
enum { FFShaperBlockParamOn, FFShaperBlockParamClip, FFShaperBlockParamCount };

template <class T>
struct alignas(alignof(T)) FFOsciAccParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamMemory);
  std::array<std::array<T, FF_OSCI_GAIN_COUNT>, FF_OSCI_COUNT> gain;
  std::array<std::array<T, FB_SINGLE_SLOT_COUNT>, FF_OSCI_COUNT> pitch;
};

struct FFOsciBlockParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamMemory);
  std::array<std::array<float, FB_SINGLE_SLOT_COUNT>, FF_OSCI_COUNT> on;
  std::array<std::array<float, FB_SINGLE_SLOT_COUNT>, FF_OSCI_COUNT> type;
};

template <class T>
struct alignas(alignof(T)) FFShaperAccParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccParamMemory);
  std::array<std::array<T, FB_SINGLE_SLOT_COUNT>, FF_SHAPER_COUNT> gain;
};

struct FFShaperBlockParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockParamMemory);
  std::array<std::array<float, FB_SINGLE_SLOT_COUNT>, FF_SHAPER_COUNT> on;
  std::array<std::array<float, FB_SINGLE_SLOT_COUNT>, FF_SHAPER_COUNT> clip;
};

template <class T>
struct alignas(alignof(T)) FFAccParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFAccParamMemory);
  FFOsciAccParamMemory<T> osci;
  FFShaperAccParamMemory<T> shaper;
};

struct FFBlockParamMemory
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFBlockParamMemory);
  FFOsciBlockParamMemory osci;
  FFShaperBlockParamMemory shaper;
};

struct FFScalarParamMemory:
public FBScalarParamMemoryBase
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarParamMemory);
  FFBlockParamMemory block;
  FFAccParamMemory<float> acc;
};

struct alignas(alignof(FBFixedCVBlock)) FFProcessorParamMemory:
public FFScalarParamMemory,
public FBProcessorParamMemoryBase
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcessorParamMemory);
  FFAccParamMemory<FBFixedCVBlock> dense;
  FFAccParamMemory<int> pos;
};

#if 0 // todo search if 0

#pragma once

#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginConfig.hpp>

#include <array>

typedef FBMonoBlock<FF_BLOCK_SIZE> FFMonoBlock;
typedef FBStereoBlock<FF_BLOCK_SIZE> FFStereoBlock;

struct FFPluginProcessors
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginProcessors);
  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

struct FFScalarParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFScalarParamMemory);
  std::array<std::array<float, FFOsciPlugParamCount>, FF_OSCI_COUNT> osciPlug;
  std::array<std::array<float, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciAuto;
  std::array<std::array<float, FFShaperPlugParamCount>, FF_SHAPER_COUNT> shaperPlug;
  std::array<std::array<float, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaperAuto;
};

struct FFDenseParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFDenseParamMemory);
  std::array<std::array<int, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciEvent;
  std::array<std::array<int, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaperEvent;
  std::array<std::array<FFMonoBlock, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciDense;
  std::array<std::array<FFMonoBlock, FFShaperAutoParamCount>, FF_OSCI_COUNT> shaperDense;
};

struct FFProcessorMemory
{
  static constexpr int BlockSize = FF_BLOCK_SIZE;
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFProcessorMemory);

  FFStereoBlock masterOut;
  FFDenseParamMemory denseParam;
  FFScalarParamMemory scalarParam;

  std::array<FFStereoBlock, FF_OSCI_COUNT> osciOut;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperIn;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperOut;
};

typedef FBStaticTopo<FFProcessorMemory> FFStaticTopo;
typedef FBStaticParam<FFProcessorMemory> FFStaticParam;
typedef FBStaticModule<FFProcessorMemory> FFStaticModule;

typedef FBRuntimeTopo<FFProcessorMemory> FFRuntimeTopo;
typedef FBRuntimeParam<FFProcessorMemory> FFRuntimeParam;
typedef FBRuntimeModule<FFProcessorMemory> FFRuntimeModule;

FFStaticTopo
FFMakeTopo();


#endif