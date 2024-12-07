#pragma once

#include <memory>

struct FBStaticTopo;
std::unique_ptr<FBStaticTopo> FFMakeTopo();

enum { FFModuleGLFO, FFModuleOsci, FFModuleShaper, FFModuleCount };
enum { FFGLFOBlockOn, FFGLFOAccRate, FFGLFOParamCount };
enum { FFOsciTypeSine, FFOsciTypeSaw, FFOsciTypeCount };
enum { FFOsciBlockOn, FFOsciBlockType, FFOsciAccGain, FFOsciAccPitch, FFOsciAccGLFOToGain, FFOsciParamCount };
enum { FFShaperBlockOn, FFShaperBlockClip, FFShaperAccGain, FFShaperAccGLFOToGain, FFShaperParamCount };