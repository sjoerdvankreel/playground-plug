#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

#include <memory>
#include <functional>

class FBPlugGUI;
class IFBHostGUIContext;

struct FBRuntimeTopo;

typedef std::function<std::unique_ptr<FBPlugGUI>(
  FBRuntimeTopo const* topo, IFBHostGUIContext* context)>
FBPlugGUIFactory;

struct FBStaticGUITopo final
{
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticGUITopo);
  int plugWidth = {};
  float minUserScale = {};
  float maxUserScale = {};
  int aspectRatioWidth = {};
  int aspectRatioHeight = {};
  FBPlugGUIFactory factory = {};
};