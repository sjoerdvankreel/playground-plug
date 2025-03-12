#pragma once

#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/dsp/shared/FBPhase.hpp>
#include <playground_base/dsp/shared/FBTrackingPhase.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciVoiceState final
{
  int note = {};
  float key = {}; // TODO floating key
  FFOsciType type = {};
  int unisonCount = {};
  float unisonOffset = {};
  bool basicSinOn = {};
  bool basicSawOn = {};
  bool basicTriOn = {};
  bool basicSqrOn = {};
  FFOsciDSFMode dsfMode = {};
  int dsfDistance = {};
  int dsfOvertones = {};
  float dsfBandwidth = {};
};

class FFOsciProcessor final
{
  FBTrackingPhase _phase = {};
  FFOsciVoiceState _voiceState = {};
  std::array<FBPhase, FFOsciUnisonMaxCount> _phases = {};

  void ProcessBasic(
    FBModuleProcState& state, 
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock& audioOut);

  void ProcessDSF(
    FBModuleProcState& state,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock& audioOut);

  void ProcessType(
    FBModuleProcState& state,
    FBFixedFloatBlock const& phase,
    FBFixedFloatBlock const& freq,
    FBFixedFloatBlock const& incr,
    FBFixedFloatBlock& audioOut);

  void ProcessUnisonVoice(
    FBModuleProcState& state,
    FBFixedFloatBlock const& basePitch,
    FBFixedFloatBlock const& detunePlain,
    FBFixedFloatBlock& audioOut,
    int unisonVoice, float pos);

  void ProcessUnison(
    FBModuleProcState& state,
    FBFixedFloatBlock const& basePitch,
    FBFixedFloatAudioBlock& audioOut);

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState const& state);
};