#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciPhase.hpp>
#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>
#include <playground_base/dsp/shared/FBParkMillerPRNG.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciVoiceState final
{
  int note = {};
  float key = {}; // TODO floating key
  FFOsciType type = {};

  bool basicSinOn = {};
  bool basicSawOn = {};
  bool basicTriOn = {};
  bool basicSqrOn = {};

  int dsfDistance = {};
  int dsfOvertones = {};
  FFOsciDSFMode dsfMode = {};
  float dsfBandwidthPlain = {};

  int unisonCount = {};
  bool unisonDetuneHQ = {};
  float unisonOffsetPlain = {};
  float unisonOffsetRandomPlain = {};

  std::array<bool, FFOsciCount> amSourceOn = {};
  std::array<int, FFOsciCount> fmSourceDelay = {}; // TODO not in samples
  std::array<FFOsciFMMode, FFOsciCount> fmSourceMode = {};
  std::array<bool, FFOsciCount> fmSourceThroughZero = {};
  std::array<int, FFOsciCount> modSourceUnisonCount = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FBParkMillerPRNG _prng = {};
  FFOsciVoiceState _voiceState = {};
  std::array<FFOsciPhase, FFOsciUnisonMaxCount> _phases = {};

  void ProcessBasicUnisonVoiceFast(
    FBFixedFloatBlock& unisonAudioOut,
    FBFixedFloatBlock const& phasePlusFm,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock const& sinGainPlain,
    FBFixedFloatBlock const& sawGainPlain,
    FBFixedFloatBlock const& triGainPlain,
    FBFixedFloatBlock const& sqrGainPlain,
    FBFixedFloatBlock const& sqrPWPlain);

  void ProcessBasicUnisonVoiceSlow(
    float& unisonAudioOut,
    float const phasePlusFm,
    float const incr,
    float const sinGainPlain,
    float const sawGainPlain,
    float const triGainPlain,
    float const sqrGainPlain,
    float const sqrPWPlain);

  void ProcessDSFUnisonVoiceFast(
    float sampleRate,
    FBFixedFloatBlock& unisonAudioOut,
    FBFixedFloatBlock const& phasePlusFm,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock const& decayPlain);

  void ProcessDSFUnisonVoiceSlow(
    float sampleRate,
    float& unisonAudioOut,
    float const phasePlusFm,
    float const freq,
    float const decayPlain);

  void ProcessTypeUnisonVoiceFast(
    float sampleRate,
    FBFixedFloatBlock& unisonAudioOut,
    FBFixedFloatBlock const& phasePlusFm,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock const& basicSinGainPlain,
    FBFixedFloatBlock const& basicSawGainPlain,
    FBFixedFloatBlock const& basicTriGainPlain,
    FBFixedFloatBlock const& basicSqrGainPlain,
    FBFixedFloatBlock const& basicSqrPWPlain,
    FBFixedFloatBlock const& dsfDecayPlain);

  void ProcessTypeUnisonVoiceSlow(
    float sampleRate,
    float& unisonAudioOut,
    float const phasePlusFm,
    float const freq,
    float const incr,
    float const basicSinGainPlain,
    float const basicSawGainPlain,
    float const basicTriGainPlain,
    float const basicSqrGainPlain,
    float const basicSqrPWPlain,
    float const dsfDecayPlain);

  void ProcessUnisonVoice(
    FBModuleProcState const& state,
    int unisonVoice, 
    FBFixedFloatBlock& unisonAudioOut,
    FBFixedFloatBlock const& unisonPos,
    FBFixedFloatBlock const& basePitch,
    FBFixedFloatBlock const& detunePlain,
    FBFixedFloatBlock const& basicSinGainPlain,
    FBFixedFloatBlock const& basicSawGainPlain,
    FBFixedFloatBlock const& basicTriGainPlain,
    FBFixedFloatBlock const& basicSqrGainPlain,
    FBFixedFloatBlock const& basicSqrPWPlain,
    FBFixedFloatBlock const& dsfDecayPlain);

  void ProcessUnison(
    FBModuleProcState& state,
    FBFixedFloatAudioBlock& audioOut,
    std::array<FBFixedFloatBlock, FFOsciUnisonMaxCount>& unisonAudioOut,
    FBFixedFloatBlock const& basePitch);

  void ShiftPrevUnisonOutputForAllOscis(
    FBModuleProcState& state);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};