#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/static/FBModuleAddrs.hpp>
#include <playground_base/base/topo/static/FBStaticParam.hpp>
#include <playground_base/base/topo/static/FBStaticGUIParam.hpp>

#include <array>
#include <string>
#include <vector>
#include <functional>

struct FBModuleGraphComponentData;

typedef std::function<void(FBModuleGraphComponentData* graphData)>
FBModuleGraphRenderer;

struct FBStaticModule final
{
  bool voice = false;
  int slotCount = {};
  std::string id = {};
  std::string name = {};
  FBModuleGraphRenderer renderer = {};
  FBModuleAddrSelectors addrSelectors = {};
  std::vector<FBStaticParam> params = {};
  std::vector<FBStaticGUIParam> guiParams = {};
  FB_EXPLICIT_COPY_MOVE_DEFCTOR(FBStaticModule);
};