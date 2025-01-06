#pragma once

#include <playground_base/base/shared/FBLifetime.hpp>
#include <playground_base/base/shared/FBStringify.hpp>
#include <playground_base/gui/glue/FBHostGUIContext.hpp>

#include <public.sdk/source/vst/vsteditcontroller.h>
#include <memory>

using namespace Steinberg;
using namespace Steinberg::Vst;

struct FBStaticTopo;
struct FBRuntimeTopo;
class FBVST3GUIEditor;

class FBVST3EditController final:
public EditControllerEx1,
public IFBHostGUIContext
{
  FBVST3GUIEditor* _guiEditor = {};
  std::unique_ptr<FBRuntimeTopo> _topo;

public:
  FBVST3EditController(FBStaticTopo const& topo);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBVST3EditController);

  void EndParamChange(int index) override;
  void BeginParamChange(int index) override;
  float GetParamNormalized(int index) const override;
  void PerformParamEdit(int index, float normalized) override;

  IPlugView* PLUGIN_API createView(FIDString name) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setComponentState(IBStream* state) override;
  tresult PLUGIN_API setParamNormalized(ParamID tag, ParamValue value) override;
};