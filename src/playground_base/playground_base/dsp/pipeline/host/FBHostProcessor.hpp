#pragma once

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/dsp/pipeline/plug/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <memory>

struct FBHostInputBlock;
struct FBHostOutputBlock;

class IFBPlugProcessor;
class IFBHostProcessContext;

class FBVoiceManager;
class FBHostAudioBlock;
class FBSmoothingProcessor;
class FBProcStateContainer;
class FBHostBufferProcessor;
class FBFixedBufferProcessor;

class FBHostProcessor final
{
  FBStaticTopo _topo;
  float _sampleRate;
  FBPlugInputBlock _plugIn = {};
  FBFixedOutputBlock _fixedOut = {};
  FBProcStateContainer* _state = {};
  IFBHostProcessContext* _hostContext = {};

  std::unique_ptr<IFBPlugProcessor> _plug;
  std::unique_ptr<FBVoiceManager> _voiceManager;
  std::unique_ptr<FBHostBufferProcessor> _hostBuffer;
  std::unique_ptr<FBFixedBufferProcessor> _fixedBuffer;
  std::unique_ptr<FBSmoothingProcessor> _smoothing;

public:
  ~FBHostProcessor();  
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBHostProcessor);

  FBHostProcessor(
    IFBHostProcessContext* hostContext,
    FBStaticTopo const& topo,
    std::unique_ptr<IFBPlugProcessor>&& plug,
    FBProcStateContainer* state, float sampleRate);

  void ProcessAllVoices();
  void ProcessVoice(int slot);
  void ProcessHost(FBHostInputBlock const& input, FBHostOutputBlock& output);
};