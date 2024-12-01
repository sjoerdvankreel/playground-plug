#include <playground_base_vst3/FBVST3EditController.hpp>
#include <playground_base/base/plug/FBPlugTopo.hpp>

#include <utility>
#include <algorithm>

static void
CopyToString128(std::string const& in, String128& out)
{
  memset(out, 0, sizeof(out));
  for (int i = 0; i < 127 && i < in.size(); i++)
    out[i] = in[i];
}

static UnitInfo
MakeUnitInfo(FBRuntimeModule const& module, int id)
{
  UnitInfo result;
  result.id = id;
  result.parentUnitId = kRootUnitId;
  result.programListId = kNoProgramListId;
  CopyToString128(module.name, result.name);
  return result;
}

static ParameterInfo
MakeParamInfo(FBRuntimeParam const& param, int unitId, bool automate)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = unitId;
  result.defaultNormalizedValue = 0.0; // TODO
  result.stepCount = std::max(0, param.static_.valueCount - 1);

  CopyToString128(param.longName, result.title);
  CopyToString128(param.shortName, result.shortTitle);
  CopyToString128(param.static_.unit, result.units);

  // TODO once we drop generic editor
  if (automate)
    result.flags = ParameterInfo::kCanAutomate;
  else
    result.flags = ParameterInfo::kCanAutomate;
  return result;
}

FBVST3EditController::
FBVST3EditController(FBStaticTopo const& topo) :
_topo(std::make_unique<FBRuntimeTopo>(topo)) {}

tresult PLUGIN_API
FBVST3EditController::initialize(FUnknown* context)
{
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1;
  for (int m = 0; m < _topo->modules.size(); m++)
  {
    addUnit(new Unit(MakeUnitInfo(_topo->modules[m], unitId)));
    for (int a = 0; a < _topo->modules[m].acc.size(); a++)
    {
      auto info = MakeParamInfo(_topo->modules[m].acc[a], unitId, true);
      parameters.addParameter(new Parameter(info));
    }
    for (int b = 0; b < _topo->modules[m].block.size(); b++)
    {
      auto info = MakeParamInfo(_topo->modules[m].block[b], unitId, false);
      parameters.addParameter(new Parameter(info));
    }
    unitId++;
  }
  return kResultTrue;
}