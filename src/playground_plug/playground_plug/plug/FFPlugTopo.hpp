#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

inline int constexpr FFGLFOCount = 1;
inline int constexpr FFGFilterCount = 2;
inline int constexpr FFOsciCount = 2; // todo oscis - it must be a single module
inline int constexpr FFOsciGainCount = 2;

enum { FFModuleGLFO, FFModuleGFilter, FFModuleOsci, FFModuleMaster, FFModuleCount }; // todo enum class
