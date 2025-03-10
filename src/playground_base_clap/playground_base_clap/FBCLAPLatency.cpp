#include <playground_base_clap/FBCLAPPlugin.hpp>
#include <playground_base/dsp/shared/FBDSPConfig.hpp>
#include <playground_base/dsp/pipeline/glue/FBHostProcessor.hpp>

uint32_t
FBCLAPPlugin::latencyGet() const noexcept
{
  return FBFixedBlockSamples;
}

bool 
FBCLAPPlugin::implementsLatency() const noexcept
{
  return true; 
}