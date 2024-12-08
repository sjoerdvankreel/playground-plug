#pragma once

#include <playground_plug/shared/FFPlugConfig.hpp>
#include <playground_plug/dsp/FFOsciProcessor.hpp>
#include <playground_plug/dsp/FFGLFOProcessor.hpp>
#include <playground_plug/dsp/FFShaperProcessor.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>
#include <playground_base/base/state/FBGlobalBlockParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedAudioBlock.hpp>

#include <array>

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGLFOBlockParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TGlobalBlock, 1> on = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOAccParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TGlobalAcc, 1> rate = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGLFOParamState final
{
  friend class FFGLFOProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFGLFOAccParamState<TGlobalAcc> acc = {};
  FFGLFOBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFOParamState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFOsciBlockParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> type = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciAccParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceAcc, 1> pitch = {};
  std::array<TVoiceAcc, 1> glfoToGain = {};
  std::array<TVoiceAcc, FF_OSCI_GAIN_COUNT> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFOsciParamState final
{
  friend class FFOsciProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFOsciAccParamState<TVoiceAcc> acc = {};
  FFOsciBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciParamState);
};

template <class TVoiceBlock>
class alignas(alignof(TVoiceBlock)) FFShaperBlockParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceBlock, 1> on = {};
  std::array<TVoiceBlock, 1> clip = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperBlockParamState);
};

template <class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFShaperAccParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  std::array<TVoiceAcc, 1> gain = {};
  std::array<TVoiceAcc, 1> glfoToGain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperAccParamState);
};

template <class TVoiceBlock, class TVoiceAcc>
class alignas(alignof(TVoiceAcc)) FFShaperParamState final
{
  friend class FFShaperProcessor;
  friend std::unique_ptr<FBStaticTopo> FFMakeTopo();
  FFShaperAccParamState<TVoiceAcc> acc = {};
  FFShaperBlockParamState<TVoiceBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
struct alignas(alignof(TGlobalAcc)) FFGlobalParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalParamState);
  std::array<FFGLFOParamState<TGlobalBlock, TGlobalAcc>, FF_GLFO_COUNT> glfo = {};
};

template <class TVoiceBlock, class TVoiceAcc>
struct alignas(alignof(TVoiceAcc)) FFVoiceParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceParamState);
  std::array<FFOsciParamState<TVoiceBlock, TVoiceAcc>, FF_OSCI_COUNT> osci = {};
  std::array<FFShaperParamState<TVoiceBlock, TVoiceAcc>, FF_SHAPER_COUNT> shaper = {};
};

struct FFScalarParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarParamState);
  FFVoiceParamState<float, float> voice = {};
  FFGlobalParamState<float, float> global = {};
};

struct FFScalarState final
{
  FFScalarParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFScalarState);
};

struct alignas(FBSIMDVectorAlign) FFProcParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  FFVoiceParamState<FBVoiceBlockParamState, FBVoiceAccParamState> voice = {};
  FFGlobalParamState<FBGlobalBlockParamState, FBGlobalAccParamState> global = {};
};

class alignas(FBSIMDVectorAlign) FFGLFODSPState final
{
  friend class FFPlugProcessor;
  FFGLFOProcessor processor = {};
public:
  FBFixedCVBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGLFODSPState);
};

class alignas(FBSIMDVectorAlign) FFOsciDSPState final
{
  friend class FFPlugProcessor;
  FFOsciProcessor processor = {};
public:
  FBFixedAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciDSPState);
};

class alignas(FBSIMDVectorAlign) FFShaperDSPState final
{
  friend class FFPlugProcessor;
  FFShaperProcessor processor = {};
public:
  FBFixedAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFShaperDSPState);
};

struct alignas(FBSIMDVectorAlign) FFGlobalDSPState final
{
  std::array<FFGLFODSPState, FF_GLFO_COUNT> glfo = {};
};

struct alignas(FBSIMDVectorAlign) FFVoiceDSPState final
{
  FBFixedAudioBlock output = {};
  std::array<FFOsciDSPState, FF_OSCI_COUNT> osci = {};
  std::array<FFShaperDSPState, FF_SHAPER_COUNT> shaper = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceDSPState);
};

struct alignas(FBSIMDVectorAlign) FFProcDSPState final
{
  FFGlobalDSPState global = {};
  std::array<FFVoiceDSPState, FB_MAX_VOICES> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
};

struct alignas(FBSIMDVectorAlign) FFProcState final
{
  FFProcDSPState dsp = {};
  FFProcParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};