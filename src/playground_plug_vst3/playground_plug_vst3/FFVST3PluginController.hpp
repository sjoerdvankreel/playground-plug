#pragma once
#include <public.sdk/source/vst/vsteditcontroller.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3PluginController:
public EditControllerEx1
{
public:
  tresult PLUGIN_API initialize(FUnknown* context) override;
};