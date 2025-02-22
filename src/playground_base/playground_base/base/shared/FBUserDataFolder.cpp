#include <playground_base/base/shared/FBUserDataFolder.hpp>
#include <playground_base/base/topo/static/FBStaticTopoMeta.hpp>

#include <juce_core/juce_core.h>
#include <string>

using namespace juce;

std::filesystem::path
FBGetUserDataFolder()
{
#ifdef __linux__
  char const* home = std::getenv("XDG_CONFIG_HOME");
  if (home == nullptr) 
    home = "~/.config";
  return std::filesystem::path(home);
#else
  auto userData = File::getSpecialLocation(File::userApplicationDataDirectory);
  return std::filesystem::path(userData.getFullPathName().toStdString());
#endif
}

std::filesystem::path
FBGetUserPluginDataFolder(FBStaticTopoMeta const& meta)
{
  return FBGetUserDataFolder() / meta.vendor / meta.name / meta.id;
}