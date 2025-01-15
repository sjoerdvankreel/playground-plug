#pragma once

#include <string>

struct FBPlugVersion final
{
  int major = 0;
  int minor = 0;
  int patch = 0;
};

struct FBStaticTopoMeta final
{
  std::string id;
  std::string name;
  std::string vendor;
  FBPlugVersion version;
};