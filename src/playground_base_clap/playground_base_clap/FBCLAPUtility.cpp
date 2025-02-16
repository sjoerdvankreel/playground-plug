#include <playground_base_clap/FBCLAPUtility.hpp>
#include <playground_base/base/topo/static/FBStaticAudioParam.hpp>

#include <cmath>

double
FBNormalizedToCLAP(FBStaticAudioParam const& topo, float normalized)
{
  if (topo.ValueCount() == 0)
    return normalized;
  return std::round(normalized * (topo.ValueCount() - 1.0));
}

double
FBCLAPToNormalized(FBStaticAudioParam const& topo, double clap)
{
  if (topo.ValueCount() == 0)
    return clap;
  double normalized = clap / (topo.ValueCount() - 1.0);
  return std::clamp(normalized, 0.0, 1.0);
}