#pragma once

#include <firefly_base/base/topo/static/FBStaticTopo.hpp>
#include <memory>

#if !defined(FF_PLUG_VERSION_MAJOR) || !defined(FF_PLUG_VERSION_MINOR) || !defined(FF_PLUG_VERSION_PATCH)
#error
#endif

#define FF_PLUG_VERSION FB_STRINGIFY(FF_PLUG_VERSION_MAJOR.FF_PLUG_VERSION_MINOR.FF_PLUG_VERSION_PATCH)

struct FBStaticTopo;
struct FBStaticTopoMeta;

FBStaticTopoMeta FFPlugMeta(FBPlugFormat format);
std::unique_ptr<FBStaticTopo> FFMakeTopo(FBPlugFormat format);
enum class FFModuleType { Output, GUISettings, Master, GMix, Osci, OsciMod, VEffect, GEffect, VMix, Env, Count };

inline int constexpr FFEnvCount = 8;
inline int constexpr FFOsciCount = 4;
inline int constexpr FFEffectCount = 4;
inline constexpr char FFVendorName[] = "Sjoerd van Kreel";
inline constexpr char FFVendorMail[] = "sjoerdvankreel@gmail.com";
inline constexpr char FFVendorURL[] = "https://github.com/sjoerdvankreel";
inline constexpr char FFPlugName[] = "Firefly Synth 2";
inline constexpr char FFPlugUniqueId[] = "754068B351A04DB4813B58D562BDFC1F";
inline constexpr char FFPlugControllerId[] = "959E6302402B461A8C9AA5A6737BCAAD";