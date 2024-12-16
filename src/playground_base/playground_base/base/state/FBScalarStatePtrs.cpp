#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/base/state/FBProcStatePtrs.hpp>
#include <playground_base/base/state/FBScalarStatePtrs.hpp>

void
FBScalarStatePtrs::CopyFrom(FBProcStatePtrs const& proc) const
{
  for (int p = 0; p < Params().size(); p++)
  {
    auto val = proc.Params()[p].Value();
    *Params()[p] = val;
    auto val2 = *Params()[p];
    (void)val2;
  }
}

void 
FBScalarStatePtrs::CopyFrom(FBScalarStatePtrs const& scalar) const
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *scalar.Params()[p];
}

void
FBScalarStatePtrs::InitDefaults(FBRuntimeTopo const& topo) const
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = topo.params[p].static_.DefaultNormalizedByText();
}