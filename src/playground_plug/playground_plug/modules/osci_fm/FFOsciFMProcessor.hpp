#pragma once

#include <playground_plug/modules/osci_fm/FFOsciFMTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>

#include <array>

struct FBModuleProcState;

struct FFOsciFMVoiceState final
{
  std::array<FFOsciFMMode, FFOsciModSlotCount> mode = {};
};

class FFOsciFMProcessor final
{
  FFOsciFMVoiceState _voiceState = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFOsciFMProcessor);
  void Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};