#pragma once

#include <firefly_base/base/shared/FBUtility.hpp>

#include <vector>
#include <utility>

struct FBRuntimeTopo;
class FBProcStateContainer;

class FBScalarStateContainer final
{
  std::vector<double*> _params;
  void* _rawState;
  void (*_freeRawState)(void*);

public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBScalarStateContainer);
  FBScalarStateContainer(FBRuntimeTopo const& topo);
  ~FBScalarStateContainer() { _freeRawState(_rawState); }

  void CopyFrom(FBProcStateContainer const& proc);
  void CopyFrom(FBScalarStateContainer const& scalar);
  std::vector<double*> const& Params() const { return _params; }

  void* Raw() { return _rawState; }
  void const* Raw() const { return _rawState; }
  template <class T> T* As() { return static_cast<T*>(_rawState); }
  template <class T> T const* As() const { return static_cast<T const*>(_rawState); }
};