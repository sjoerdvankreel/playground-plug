#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedFloatAudioBlock.hpp>
#include <playground_plug/modules/master/FFMasterProcessor.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class FFMasterExchangeState final
{
  bool active = {};
public:
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterExchangeState);
};

class alignas(sizeof(FBFloatVector)) FFMasterDSPState final
{
  friend class FFPlugProcessor;
  FFMasterProcessor processor = {};
public:
  FBFixedFloatAudioBlock input = {};
  FBFixedFloatAudioBlock output = {};
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterDSPState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterAccParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TGlobalAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterAccParamState);
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFMasterBlockParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  std::array<TGlobalBlock, 1> voices = {};
  std::array<TGlobalBlock, 1> hostSmoothTime = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterBlockParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFMasterParamState final
{
  friend class FFMasterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeMasterTopo();
  FFMasterAccParamState<TGlobalAcc> acc = {};
  FFMasterBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFMasterParamState);
};