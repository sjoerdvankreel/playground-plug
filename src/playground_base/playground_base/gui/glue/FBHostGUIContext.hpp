#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

class IFBHostGUIContext
{
public:
  FB_NOCOPY_MOVE_DEFCTOR(IFBHostGUIContext);
  virtual float GetParamNormalized(int index) const = 0;
  virtual void SetParamNormalized(int index, float normalized) = 0;
};