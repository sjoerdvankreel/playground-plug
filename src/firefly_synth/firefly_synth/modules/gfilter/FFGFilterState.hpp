#pragma once

#include <firefly_synth/modules/gfilter/FFGFilterProcessor.hpp>

#include <firefly_base/base/shared/FBSArray.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <array>
#include <memory>

struct FBStaticModule;

class alignas(FBSIMDAlign) FFGFilterDSPState final
{
  friend class FFPlugProcessor;
  std::unique_ptr<FFGFilterProcessor> processor = {};
public:
  FFGFilterDSPState();
  ~FFGFilterDSPState();
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFGFilterDSPState);
  FBSArray2<float, FBFixedBlockSamples, 2> input = {};
  FBSArray2<float, FBFixedBlockSamples, 2> output = {};
};

template <class TGlobalBlock>
class alignas(alignof(TGlobalBlock)) FFGFilterBlockParamState final
{
  friend class FFGFilterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();
  std::array<TGlobalBlock, 1> on = {};
  std::array<TGlobalBlock, 1> mode = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterBlockParamState);
};

template <class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGFilterAccParamState final
{
  friend class FFGFilterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();
  std::array<TGlobalAcc, 1> res = {};
  std::array<TGlobalAcc, 1> freq = {};
  std::array<TGlobalAcc, 1> gain = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterAccParamState);
};

template <class TGlobalBlock, class TGlobalAcc>
class alignas(alignof(TGlobalAcc)) FFGFilterParamState final
{
  friend class FFGFilterProcessor;
  friend std::unique_ptr<FBStaticModule> FFMakeGFilterTopo();
  FFGFilterAccParamState<TGlobalAcc> acc = {};
  FFGFilterBlockParamState<TGlobalBlock> block = {};
public:
  FB_NOCOPY_NOMOVE_DEFCTOR(FFGFilterParamState);
};