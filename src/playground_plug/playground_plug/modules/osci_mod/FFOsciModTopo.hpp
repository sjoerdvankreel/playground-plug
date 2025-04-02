#pragma once

#include <playground_plug/shared/FFPlugTopo.hpp>
#include <memory>
#include <utility>

inline int constexpr FFOsciModSlotCount = (FFOsciCount * (FFOsciCount - 1)) / 2;

struct FBStaticModule;
enum class FFOsciModParam { On, AM, RM, TZ, FM, Count };
std::unique_ptr<FBStaticModule> FFMakeOsciModTopo();