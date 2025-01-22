#include <playground_base_vst3/FBVST3Utility.hpp>
#include <playground_base_vst3/FBVST3Parameter.hpp>
#include <playground_base_vst3/FBVST3GUIEditor.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>

#include <playground_base/base/shared/FBLogging.hpp>
#include <playground_base/base/state/FBGUIState.hpp>
#include <playground_base/base/topo/FBRuntimeTopo.hpp>

#include <base/source/fstring.h>
#include <utility>
#include <algorithm>

using namespace juce;

static UnitInfo
MakeUnitInfo(FBRuntimeModule const& module, int id)
{
  UnitInfo result;
  result.id = id;
  result.parentUnitId = kRootUnitId;
  result.programListId = kNoProgramListId;
  FBVST3CopyToString128(module.name, result.name);
  return result;
}

static ParameterInfo
MakeParamInfo(FBRuntimeParam const& param, int unitId)
{
  ParameterInfo result;
  result.id = param.tag;
  result.unitId = unitId;
  result.stepCount = std::max(0, param.static_.ValueCount() - 1);
  result.defaultNormalizedValue = param.static_.DefaultNormalizedByText();

  FBVST3CopyToString128(param.longName, result.title);
  FBVST3CopyToString128(param.shortName, result.shortTitle);
  FBVST3CopyToString128(param.static_.unit, result.units);

  result.flags = ParameterInfo::kNoFlags;
  if (FBParamTypeIsStepped(param.static_.type))
  {
    result.flags |= ParameterInfo::kIsHidden;
    result.flags |= ParameterInfo::kIsReadOnly;
    if (param.static_.type == FBParamType::List)
      result.flags |= ParameterInfo::kIsList;
  }
  else
    result.flags = ParameterInfo::kCanAutomate;
  return result;
}

FBVST3EditController::
~FBVST3EditController()
{
  FB_LOG_ENTRY_EXIT();
}

FBVST3EditController::
FBVST3EditController(FBStaticTopo const& topo) :
_topo(std::make_unique<FBRuntimeTopo>(topo)),
_guiState(std::make_unique<FBGUIState>())
{
  FB_LOG_ENTRY_EXIT();
}

void
FBVST3EditController::ResetView()
{
  FB_LOG_ENTRY_EXIT();
  _guiEditor = nullptr;
}

void 
FBVST3EditController::EndParamChange(int index)
{
  endEdit(_topo->params[index].tag);
}

void 
FBVST3EditController::BeginParamChange(int index)
{
  beginEdit(_topo->params[index].tag);
}

float
FBVST3EditController::GetParamNormalized(int index) const
{
  return parameters.getParameterByIndex(index)->getNormalized();
}

std::unique_ptr<PopupMenu>
FBVST3EditController::MakeParamContextMenu(int index)
{
  if (!_guiEditor)
    return {};
  return _guiEditor->MakeParamContextMenu(componentHandler, index);
}

void
FBVST3EditController::PerformParamEdit(int index, float normalized)
{
  parameters.getParameterByIndex(index)->setNormalized(normalized);
  performEdit(_topo->params[index].tag, normalized);
}

tresult PLUGIN_API 
FBVST3EditController::setParamNormalized(ParamID tag, ParamValue value)
{
  if (_guiEditor != nullptr)
    _guiEditor->SetParamNormalized(_topo->paramTagToIndex[tag], (float)value);
  return EditControllerEx1::setParamNormalized(tag, value);
}

void
FBVST3EditController::ParamContextMenuClicked(int paramIndex, int itemTag)
{
  if (_guiEditor)
    _guiEditor->ParamContextMenuClicked(componentHandler, paramIndex, itemTag);
}

IPlugView* PLUGIN_API
FBVST3EditController::createView(FIDString name)
{
  FB_LOG_ENTRY_EXIT();
  if (ConstString(name) != ViewType::kEditor) return nullptr;
  if(_guiEditor == nullptr)
    _guiEditor = new FBVST3GUIEditor(_topo->static_.gui.factory, _topo.get(), this, _guiState.get());
  return _guiEditor;
}

tresult PLUGIN_API
FBVST3EditController::getState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  std::string json = _topo->SaveGUIStateToString(*_guiState);
  if (!FBVST3SaveIBStream(state, json))
    return kResultFalse;
  return kResultOk;
}

tresult PLUGIN_API 
FBVST3EditController::setState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  std::string json;
  if (!FBVST3LoadIBStream(state, json))
    return kResultFalse;
  if (!_topo->LoadGUIStateFromStringWithDryRun(json, *_guiState))
    return kResultFalse;
  return kResultTrue;
}

tresult PLUGIN_API
FBVST3EditController::setComponentState(IBStream* state)
{
  FB_LOG_ENTRY_EXIT();
  std::string json;
  if (!FBVST3LoadIBStream(state, json))
    return kResultFalse;
  FBScalarStateContainer edit(*_topo);
  if (!_topo->LoadEditStateFromString(json, edit))
    return kResultFalse;
  for (int i = 0; i < edit.Params().size(); i++)
  {
    float normalized = *edit.Params()[i];
    parameters.getParameterByIndex(i)->setNormalized(normalized);
    if (_guiEditor != nullptr)
      _guiEditor->SetParamNormalized(i, normalized);
  }
  return kResultOk;
}

tresult PLUGIN_API
FBVST3EditController::initialize(FUnknown* context)
{
  FB_LOG_ENTRY_EXIT();
  if (EditController::initialize(context) != kResultTrue)
    return kResultFalse;

  int unitId = 1;
  for (int m = 0; m < _topo->modules.size(); m++)
  {
    addUnit(new Unit(MakeUnitInfo(_topo->modules[m], unitId)));
    for (int p = 0; p < _topo->modules[m].params.size(); p++)
    {
      auto const& topo = _topo->modules[m].params[p];
      auto info = MakeParamInfo(topo, unitId);
      parameters.addParameter(new FBVST3Parameter(topo.static_, info));
    }
    unitId++;
  }
  return kResultTrue;
}