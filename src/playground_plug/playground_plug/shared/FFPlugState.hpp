#pragma once

#include <playground_plug/pipeline/FFVoiceProcessor.hpp>
#include <playground_plug/modules/env/FFEnvState.hpp>
#include <playground_plug/modules/glfo/FFGLFOState.hpp>
#include <playground_plug/modules/osci/FFOsciState.hpp>
#include <playground_plug/modules/master/FFMasterState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterState.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/base/state/FBVoiceBlockParamState.hpp>
#include <playground_base/base/state/FBGlobalBlockParamState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <array>

struct FFGlobalExchangeState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalExchangeState);
};

struct FFVoiceExchangeState final
{
  std::array<FFEnvExchangeState, FFEnvCount> env = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceExchangeState);
};

struct alignas(sizeof(FBFloatVector)) FFGlobalDSPState final
{
  FFMasterDSPState master = {};
  std::array<FFGLFODSPState, FFGLFOCount> gLFO = {};
  std::array<FFGFilterDSPState, FFGFilterCount> gFilter = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalDSPState);
};

struct alignas(sizeof(FBFloatVector)) FFVoiceDSPState final
{
  FFVoiceProcessor processor = {};
  FBFixedFloatAudioBlock output = {};
  std::array<FFEnvDSPState, FFEnvCount> env = {};
  std::array<FFOsciDSPState, FFOsciCount> osci = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceDSPState);
};

struct alignas(sizeof(FBFloatVector)) FFProcDSPState final
{
  FFGlobalDSPState global = {};
  std::array<FFVoiceDSPState, FBMaxVoices> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcDSPState);
}; 

template <class TGlobalBlock, class TGlobalAcc>
struct alignas(alignof(TGlobalAcc)) FFGlobalParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGlobalParamState);
  std::array<FFMasterParamState<TGlobalBlock, TGlobalAcc>, 1> master = {};
  std::array<FFGLFOParamState<TGlobalBlock, TGlobalAcc>, FFGLFOCount> gLFO = {};
  std::array<FFGFilterParamState<TGlobalBlock, TGlobalAcc>, FFGFilterCount> gFilter = {};
};

template <class TVoiceBlock, class TVoiceAcc>
struct alignas(alignof(TVoiceAcc)) FFVoiceParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceParamState);
  std::array<FFEnvParamState<TVoiceBlock, TVoiceAcc>, FFEnvCount> env = {};
  std::array<FFOsciParamState<TVoiceBlock, TVoiceAcc>, FFOsciCount> osci = {};
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

struct FFExchangeParamState final
{
  typedef std::array<float, FBMaxVoices> ScalarPerVoice;
  FB_NOCOPY_NOMOVE_DEFCTOR(FFExchangeParamState);
  FFGlobalParamState<float, float> global = {};
  FFVoiceParamState<ScalarPerVoice, ScalarPerVoice> voice = {};
};

struct FFExchangeState final
{
  FFExchangeParamState param = {};
  FFGlobalExchangeState global = {};
  std::array<FBVoiceInfo, FBMaxVoices> voiceState = {};
  std::array<FFVoiceExchangeState, FBMaxVoices> voice = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFExchangeState);
};

struct alignas(sizeof(FBFloatVector)) FFProcParamState final
{
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcParamState);
  FFVoiceParamState<FBVoiceBlockParamState, FBVoiceAccParamState> voice = {};
  FFGlobalParamState<FBGlobalBlockParamState, FBGlobalAccParamState> global = {};
};

struct alignas(sizeof(FBFloatVector)) FFProcState final
{
  FFProcDSPState dsp = {};
  FFProcParamState param = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFProcState);
};