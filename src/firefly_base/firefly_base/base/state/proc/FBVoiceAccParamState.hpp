#pragma once

#include <firefly_base/dsp/voice/FBVoiceManager.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/state/proc/FBAccParamState.hpp>

class alignas(FBSIMDAlign) FBVoiceAccParamState final
{
  friend class FBVoiceManager;
  friend class FBProcParamState;
  friend class FBSmoothingProcessor;

  float _value = {};
  std::array<FBAccParamState, FBMaxVoices> _voice = {};

  float Value() const { return _value; }
  void Value(float value) { _value = value; }
  std::array<FBAccParamState, FBMaxVoices>& Voice() { return _voice; }
  void Modulate(int slot, float offset) { _voice[slot].Modulate(offset); }
  void SmoothNext(int slot, int sample) { _voice[slot].SmoothNext(sample, _value); }

  void InitProcessing(float value);
  void SetSmoothingCoeffs(int sampleCount);
  void InitProcessing(int voice, float value) { _voice[voice].InitProcessing(value); }

public:
  FBVoiceAccParamState(float sampleRate);
  FB_NOCOPY_NOMOVE_DEFCTOR(FBVoiceAccParamState);
  std::array<FBAccParamState, FBMaxVoices> const& Voice() const { return _voice; }
};

inline void
FBVoiceAccParamState::InitProcessing(float value)
{
  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].InitProcessing(value);
}

inline void
FBVoiceAccParamState::SetSmoothingCoeffs(int sampleCount)
{
  for (int v = 0; v < FBMaxVoices; v++)
    _voice[v].SetSmoothingCoeffs(sampleCount);
}