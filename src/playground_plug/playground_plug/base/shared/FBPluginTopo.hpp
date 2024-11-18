#pragma once

#include <map>
#include <array>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

int
FBMakeHash(std::string const& id);
std::string
FBMakeId(std::string id, int slot);
std::string
FBMakeName(std::string name, int slotCount, int slot);

inline double 
FBDiscreteToNormalized(int count, int index)
{
  return index / (count - 1.0);
}

inline int
FBNormalizedToDiscrete(int count, double normalized)
{
  return std::min(count - 1, (int)(normalized * count));
}

inline bool
FBNormalizedToBool(double normalized)
{
  return FBNormalizedToDiscrete(2, normalized) != 0;
}

// all this stuff is templated for the sole reason to allow 
// getting to the actual memory (FFProcessorMemory) without void* 's

template <class ProcessorMemory>
struct FBStaticParam
{
  int slotCount; // multi-slot params are useful for mod matrices
  int valueCount; // 0 for continuous normalized, > 1 for discrete (so vst stepCount + 1)

  std::string id;
  std::string name;
  std::string unit;

  // reason for template
  float* (*plugParamAddr)(
    int moduleSlot, int paramSlot, ProcessorMemory* memory);
  std::array<float, ProcessorMemory::BlockSize>* (*autoParamAddr)(
    int moduleSlot, int paramSlot, ProcessorMemory* memory);
};

template <class ProcessorMemory>
struct FBStaticModule
{
  int slotCount;
  std::string id;
  std::string name;
  std::vector<FBStaticParam<ProcessorMemory>> plugParams; // editable by UI only, backed by scalar
  std::vector<FBStaticParam<ProcessorMemory>> autoParams; // automatable/modulatable, backed by dense buffer
};

template <class ProcessorMemory>
struct FBStaticTopo
{
  std::vector<FBStaticModule<ProcessorMemory>> modules;
};

template <class ProcessorMemory>
struct FBRuntimeParam
{
  int tag; // VST3 / CLAP param tag
  int moduleSlot;
  int paramSlot;

  std::string id;
  std::string name;
  FBStaticParam<ProcessorMemory> staticTopo;

  FBRuntimeParam(
    FBStaticModule<ProcessorMemory> const& module, int moduleSlot,
    FBStaticParam<ProcessorMemory> const& param, int paramSlot);
};

template <class ProcessorMemory>
struct FBRuntimeModule
{
  std::string id;
  std::string name;
  std::vector<FBRuntimeParam<ProcessorMemory>> plugParams;
  std::vector<FBRuntimeParam<ProcessorMemory>> autoParams;

  FBRuntimeModule(
    FBStaticModule<ProcessorMemory> const& module, int slot);
};

template <class ProcessorMemory>
struct FBRuntimeTopo
{
  std::map<int, int> tagToPlugParam;
  std::map<int, int> tagToAutoParam;
  std::vector<FBRuntimeModule<ProcessorMemory>> modules;
  std::vector<FBRuntimeParam<ProcessorMemory>> plugParams;
  std::vector<FBRuntimeParam<ProcessorMemory>> autoParams;

  FBRuntimeTopo(FBStaticTopo<ProcessorMemory> const& topo);
};

template <class ProcessorMemory>
FBRuntimeParam<ProcessorMemory>::
FBRuntimeParam(
  FBStaticModule<ProcessorMemory> const& module, int moduleSlot_,
  FBStaticParam<ProcessorMemory> const& param, int paramSlot_)
{
  moduleSlot = moduleSlot_;
  paramSlot = paramSlot_;
  staticTopo = param;

  id = FBMakeId(module.id, moduleSlot);
  id += "-" + FBMakeId(param.id, paramSlot);
  name = FBMakeName(module.name, module.slotCount, moduleSlot);
  name += " " + FBMakeName(param.name, param.slotCount, paramSlot);
  tag = FBMakeHash(id);
}

template <class ProcessorMemory>
FBRuntimeModule<ProcessorMemory>::
FBRuntimeModule(
  FBStaticModule<ProcessorMemory> const& module, int slot)
{
  id = FBMakeId(module.id, slot);
  name = FBMakeName(module.name, module.slotCount, slot);
  for (int ppi = 0; ppi < module.plugParams.size(); ppi++)
    for (int pps = 0; pps < module.plugParams[ppi].slotCount; pps++)
      plugParams.push_back(FBRuntimeParam(module, slot, module.plugParams[ppi], pps));
  for (int api = 0; api < module.autoParams.size(); api++)
    for (int aps = 0; aps < module.autoParams[api].slotCount; aps++)
      autoParams.push_back(FBRuntimeParam(module, slot, module.autoParams[api], aps));
}

template <class ProcessorMemory>
FBRuntimeTopo<ProcessorMemory>::
FBRuntimeTopo(
  FBStaticTopo<ProcessorMemory> const& topo)
{
  for (int mi = 0; mi < topo.modules.size(); mi++)
    for (int ms = 0; ms < topo.modules[mi].slotCount; ms++)
      modules.push_back(FBRuntimeModule(topo.modules[mi], ms));
  for (int m = 0; m < modules.size(); m++)
    for (int pp = 0; pp < modules[m].plugParams.size(); pp++)
      plugParams.push_back(modules[m].plugParams[pp]);
  for (int m = 0; m < modules.size(); m++)
    for (int ap = 0; ap < modules[m].autoParams.size(); ap++)
      autoParams.push_back(modules[m].autoParams[ap]);
  for (int pp = 0; pp < plugParams.size(); pp++)
    tagToPlugParam[plugParams[pp].tag] = pp;
  for (int ap = 0; ap < autoParams.size(); ap++)
    tagToAutoParam[autoParams[ap].tag] = ap;
}