#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBProcParamState.hpp>
#include <playground_base/dsp/pipeline/fixed/FBSmoothProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBSmoothProcessor::ProcessSmoothing(
  FBFixedOutputBlock& output)
{
  auto& acc = output.state->acc;
  for (int p = 0; p < output.state->isBlock.size(); p++)
    if(!output.state->isBlock[p])
      for (int s = 0; s < FBFixedAudioBlock::Count(); s++)
        acc[p]->smoothedCV[s] = acc[p]->smooth.Next(acc[p]->rampedCV[s]);
}