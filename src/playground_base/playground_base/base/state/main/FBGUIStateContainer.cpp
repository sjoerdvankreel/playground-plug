#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/main/FBGUIStateContainer.hpp>

#include <set>
#include <cassert>

FBGUIStateContainer::
FBGUIStateContainer(FBRuntimeTopo const& topo):
_params(),
_rawState(topo.static_.state.allocRawGUIState()),
_freeRawState(topo.static_.state.freeRawGUIState),
_special(topo.static_.state.specialGUISelector(topo.static_, _rawState))
{
  for (int p = 0; p < topo.gui.params.size(); p++)
    _params.push_back(
      topo.gui.params[p].static_.addrSelector(
        topo.gui.params[p].topoIndices.module.slot,
        topo.gui.params[p].topoIndices.param.slot, _rawState));
  for (int p = 0; p < _params.size(); p++)
    *_params[p] = topo.gui.params[p].static_.DefaultNormalizedByText();
#ifndef NDEBUG
  std::set<double*> uniquePtrs(_params.begin(), _params.end());
  assert(uniquePtrs.size() == _params.size());
#endif
}

void 
FBGUIStateContainer::CopyFrom(FBGUIStateContainer const& gui)
{
  for (int p = 0; p < Params().size(); p++)
    *Params()[p] = *gui.Params()[p];
}