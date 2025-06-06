#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>
#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <firefly_base/base/state/proc/FBProcParamState.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBHostGUIContext;
class FBScalarStateContainer;
class FBExchangeStateContainer;

class FBProcStateContainer final
{
  friend class FBVoiceManager;
  friend class FBHostProcessor;
  friend class FBSmoothingProcessor;

  void* _rawState;
  void (*_freeRawState)(void*);
  FBSpecialParams _special;

  int _smoothingDurationSamples = {};
  std::vector<FBProcParamState> _params = {};
  std::vector<FBProcParamState>& Params() { return _params; }

  void InitProcessing(int index, int voice, float value);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBProcStateContainer);
  FBProcStateContainer(FBRuntimeTopo const& topo);
  ~FBProcStateContainer() { _freeRawState(_rawState); }

  void SetSmoothingCoeffs(int sampleCount);
  void InitProcessing(int index, float value);
  void InitProcessing(FBHostGUIContext const* context);
  void InitProcessing(FBScalarStateContainer const& scalar);
  void InitProcessing(FBExchangeStateContainer const& exchange);

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  FBSpecialParams const& Special() const { return _special; }
  std::vector<FBProcParamState> const& Params() const { return _params; }
  template <class T> T* As() { return static_cast<T*>(Raw()); }
  template <class T> T const* As() const { return static_cast<T const*>(Raw()); }
};