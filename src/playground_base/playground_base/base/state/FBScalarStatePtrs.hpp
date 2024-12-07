#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <vector>
#include <utility>

class FBProcStatePtrs;

class FBScalarStatePtrs final
{
  std::vector<float*> _params;

public:
  FB_NOCOPY_MOVE_NODEFCTOR(FBScalarStatePtrs);
  void CopyFrom(FBProcStatePtrs const& proc);
  void CopyFrom(FBScalarStatePtrs const& scalar);

  std::vector<float*> const& Params() const { return _params; }
  FBScalarStatePtrs(std::vector<float*>&& params) : _params(std::move(params)) {}
};