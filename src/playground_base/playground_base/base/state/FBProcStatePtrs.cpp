#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStateContainer.hpp>

void
FBProcStatePtrs::CopyFrom(FBScalarStateContainer const& scalar)
{
  for (int p = 0; p < Params().size(); p++)
    Params()[p].Value(*scalar.Params()[p]);
}

void
FBProcStatePtrs::SetSmoothingCoeffs(float sampleRate, float durationSecs)
{
  for (int p = 0; p < Params().size(); p++)
    Params()[p].SetSmoothingCoeffs(sampleRate, durationSecs);
}

void
FBProcStatePtrs::InitDefaults(FBRuntimeTopo const& topo)
{
  for (int p = 0; p < Params().size(); p++)
    Params()[p].Value(topo.params[p].static_.DefaultNormalizedByText());
}