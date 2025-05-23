#pragma once

#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

struct FBPlugInputBlock;

class FFVoiceProcessor final
{
public:
  bool Process(FBModuleProcState state);
  void BeginVoice(FBModuleProcState state);
  FB_NOCOPY_NOMOVE_DEFCTOR(FFVoiceProcessor);
};