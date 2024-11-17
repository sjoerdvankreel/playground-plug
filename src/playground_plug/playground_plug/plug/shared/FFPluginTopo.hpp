#pragma once

#include <playground_plug/base/shared/FBPluginTopo.hpp>
#include <playground_plug/base/shared/FBPluginBlock.hpp>
#include <playground_plug/plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/plug/dsp/FFShaperProcessor.hpp>
#include <playground_plug/plug/shared/FFPluginConfig.hpp>

#include <array>

enum { FFModuleOsci, FFModuleShaper, FFModuleCount };
enum { FFShaperPlugParamOn, FFShaperPlugParamCount };
enum { FFShaperAutoParamGain, FFShaperAutoParamCount };
enum { FFOsciPlugParamOn, FFOsciPlugParamCount };
enum { FFOsciAutoParamGain, FFOsciAutoParamPitch, FFOsciAutoParamCount };

FBStaticTopo
FFMakeTopo();

struct FFPluginProcessors
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFPluginProcessors);

  std::array<FFOsciProcessor, FF_OSCI_COUNT> osci;
  std::array<FFShaperProcessor, FF_SHAPER_COUNT> shaper;
};

template <class T>
struct FFParamMemory
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFParamMemory);

  std::array<std::array<T, FFOsciAutoParamCount>, FF_OSCI_COUNT> osciAuto;
  std::array<std::array<T, FFShaperAutoParamCount>, FF_SHAPER_COUNT> shaperAuto;
  std::array<std::array<float, FFOsciPlugParamCount>, FF_OSCI_COUNT> osciPlug;
  std::array<std::array<float, FFShaperPlugParamCount>, FF_SHAPER_COUNT> shaperPlug;
};

typedef FBMonoBlock<FF_BLOCK_SIZE> FFMonoBlock;
typedef FBStereoBlock<FF_BLOCK_SIZE> FFStereoBlock;
typedef FFParamMemory<float> FFControllerParamMemory;
typedef FFParamMemory<FFMonoBlock> FFProcessorParamMemory;

struct FFProcessorBlock
{
  FB_NOCOPY_NOMOVE_DEFAULT_CTOR(FFProcessorBlock);

  float sampleRate;
  FFProcessorParamMemory paramMemory;
  std::array<FFStereoBlock, FF_OSCI_COUNT> osciOutput;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperInput;
  std::array<FFStereoBlock, FF_SHAPER_COUNT> shaperOutput;
};