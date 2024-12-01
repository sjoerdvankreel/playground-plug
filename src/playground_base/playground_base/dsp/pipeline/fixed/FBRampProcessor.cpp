#include <playground_base/dsp/pipeline/fixed/FBRampProcessor.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedCVBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedOutputBlock.hpp>

void 
FBRampProcessor::ProcessRamping(
  FBFixedInputBlock const& input, FBFixedOutputBlock& output)
{
  auto& scalar = output.state.scalar;
  auto& proc = output.state.proc->param;
  for (int p = 0; p < proc.size(); p++)
  {
    proc[p]->pos = 0;
    proc[p]->rampedCV.Fill(0, proc[p]->rampedCV.Count(), *scalar->acc[p]);
  }

  for (int a = 0; a < input.acc.size(); a++)
  {
    auto const& event = input.acc[a];
    float currentVal = *scalar->acc[event.index];
    int currentPos = proc[event.index]->pos;
    int posRange = event.pos - currentPos;
    float valRange = event.normalized - currentVal;
    proc[event.index]->pos = event.pos;
    *scalar->acc[event.index] = event.normalized;

    for (int pos = 0; pos < posRange; pos++)
      proc[event.index]->rampedCV[currentPos + pos] =
      currentVal + pos / static_cast<float>(posRange) * valRange;
    if (a < input.acc.size() - 1 && input.acc[a + 1].index != event.index)
      for (int pos = event.pos; pos < input.audio.Count(); pos++)
        proc[event.index]->rampedCV[pos] = event.normalized;
  }
}