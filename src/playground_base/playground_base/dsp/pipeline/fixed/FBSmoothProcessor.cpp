#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBAccParamState.hpp>
#include <playground_base/base/state/FBVoiceAccParamState.hpp>
#include <playground_base/base/state/FBGlobalAccParamState.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

#include <algorithm>

FBSmoothProcessor::
FBSmoothProcessor(FBVoiceManager* voiceManager) :
_voiceManager(voiceManager) {}

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  auto& myAcc = _accBySampleThenParam;
  auto& thatAcc = input.accByParamThenSample;
  myAcc.clear();
  myAcc.insert(myAcc.begin(), thatAcc.begin(), thatAcc.end());
  auto compare = [](auto const& l, auto const& r) {
    if (l.pos < r.pos) return true;
    if (l.pos > r.pos) return false;
    return l.index < r.index; };
  std::sort(myAcc.begin(), myAcc.end(), compare);

  // todo deal with nondestructive and pervoice
  int eventIndex = 0;
  auto& params = output.state->Params();
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    for (int eventIndex = 0; 
      eventIndex < myAcc.size() && myAcc[eventIndex].pos == s; 
      eventIndex++)
    {
      auto const& event = myAcc[eventIndex];
      if (!params[event.index].IsVoice())
      {
        auto& global = params[event.index].GlobalAcc();
        global.Value(event.normalized);
        global.Modulate(event.normalized);
      }
      else
      {
        auto& voice = params[event.index].VoiceAcc();
        voice.Value(event.normalized);
        for (int v = 0; v < FBMaxVoices; v++)
          if (_voiceManager->Voices()[v].active)
            voice.Modulate(v, event.normalized);
      }
    }

    for (int p = 0; p < params.size(); p++)
      if(params[p].IsAcc())
        if (!params[p].IsVoice())
          params[p].GlobalAcc().Global().SmoothNext(s);
        else
          for(int v = 0; v < FBMaxVoices; v++)
            if (_voiceManager->Voices()[v].active)
              params[p].VoiceAcc().Voice()[v].SmoothNext(s);
  }
}