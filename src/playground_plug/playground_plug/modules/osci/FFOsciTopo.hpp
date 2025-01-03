#pragma once

#include <memory>

struct FBStaticModule;
std::unique_ptr<FBStaticModule> FFMakeOsciTopo();

enum class FFOsciType { Sine, Saw, Pulse, Count };
enum class FFOsciParam { On, Type, Note, Gain, Cent, PW, GLFOToGain, Count };