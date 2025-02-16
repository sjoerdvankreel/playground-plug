#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBSpecialGUIParams.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;

class FBGUIStateContainer final
{
  friend class FBPlugGUIContext;

  std::vector<float*> _params;
  void* _rawState;
  void (*_freeRawState)(void*);
  FBSpecialGUIParams _special;

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGUIStateContainer);
  FBGUIStateContainer(FBRuntimeTopo const& topo);
  ~FBGUIStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBGUIStateContainer const& gui);
  std::vector<float*> const& Params() const { return _params; }
  FBSpecialGUIParams const& Special() const { return _special; }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  template <class T> T* As() { return static_cast<T*>(_rawState); }
  template <class T> T const* As() const { return static_cast<T const*>(_rawState); }
};