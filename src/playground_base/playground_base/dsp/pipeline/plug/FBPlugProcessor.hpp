#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>

struct FBPlugInputBlock;
class FBFixedAudioBlock;

class IFBPlugProcessor
{
public:
  virtual ~IFBPlugProcessor() {}
  FB_NOCOPY_MOVE_DEFCTOR(IFBPlugProcessor);

  virtual void ProcessPreVoice(FBPlugInputBlock const& input) = 0;
  virtual void ProcessVoice(FBPlugInputBlock const& input, int voice) = 0;
  virtual void ProcessPostVoice(FBPlugInputBlock const& input, FBFixedAudioBlock& output) = 0;
};