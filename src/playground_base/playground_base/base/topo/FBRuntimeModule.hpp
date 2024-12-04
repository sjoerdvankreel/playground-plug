#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/topo/FBRuntimeParam.hpp>

#include <string>
#include <vector>

struct FBStaticModule;

struct FBRuntimeModule final
{
  std::string const name;
  std::vector<FBRuntimeParam> const params;

  FB_EXPLICIT_COPY_MOVE_NODEFCTOR(FBRuntimeModule);
  FBRuntimeModule(FBStaticModule const& module, int slot);
};