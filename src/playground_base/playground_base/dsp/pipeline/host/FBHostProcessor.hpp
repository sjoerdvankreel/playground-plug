#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBRuntimeTopo;
struct FBProcStatePtrs;
struct FBHostInputBlock;

class FBRampProcessor;
class FBHostAudioBlock;
class IFBPlugProcessor;
class FBSmoothProcessor;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor final
{
  FBPlugInputBlock _plugIn = {};
  FBFixedOutputBlock _fixedOut = {};
  std::unique_ptr<IFBPlugProcessor> _plug;

  std::unique_ptr<FBRampProcessor> _ramp;
  std::unique_ptr<FBSmoothProcessor> _smooth;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;

public:
  ~FBHostProcessor();
  FB_NOCOPY_MOVE_NODEFCTOR(FBHostProcessor);
  FBHostProcessor(
    std::unique_ptr<IFBPlugProcessor>&& plug,
    FBProcStatePtrs const* state, float sampleRate);

  void ProcessHost(FBHostInputBlock const& input, FBHostAudioBlock& output);
};