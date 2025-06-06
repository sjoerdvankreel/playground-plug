#pragma once

#include <firefly_synth/modules/env/FFEnvTopo.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/dsp/shared/FBBasicLPFilter.hpp>

#include <array>

struct FFScalarState;
struct FBStaticTopo;
struct FBModuleProcState;
class FBGraphRenderState;

class FFEnvProcessor final
{
  bool _sync = {};
  FFEnvType _type = {};
  int _loopStart = {};
  int _loopLength = {};
  int _releasePoint = {};

  bool _released = false;
  bool _finished = false;
  int _smoothSamples = 0;
  int _lengthSamples = 0;
  int _positionSamples = 0;
  int _lengthSamplesUpToRelease = 0;
  float _lastOverall = 0.0f;
  float _lastBeforeRelease = 0.0f;

  int _smoothPosition = {};
  FBBasicLPFilter _smoother = {};
  std::array<int, (int)FFEnvStageCount> _stageSamples = {};
  std::array<int, (int)FFEnvStageCount> _stagePositions = {};
  std::array<int, (int)FFEnvStageCount> _lengthSamplesUpToStage = {};

public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFEnvProcessor);
  int Process(FBModuleProcState& state);
  void BeginVoice(FBModuleProcState& state);
};