#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeEnvTopo();

enum class FFEnvMode { Linear, Exp };
enum class FFEnvType { Sustain, Follow, Release };

enum class FFEnvParam { 
  On, Type, Sustain, Sync, Mode, Smooth,
  DelayTime, AttackTime, AttackSlope, HoldTime, 
  DecayTime, DecaySlope, ReleaseTime, ReleaseSlope, Count };