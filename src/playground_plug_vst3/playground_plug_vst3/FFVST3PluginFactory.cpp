#include <playground_plug/dsp/FFPlugProcessor.hpp>
#include <playground_plug/shared/FFPlugConfig.hpp>

#include <playground_base_vst3/FBVST3AudioEffect.hpp>
#include <playground_base_vst3/FBVST3EditController.hpp>

#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivsteditcontroller.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace Steinberg;
using namespace Steinberg::Vst;

class FFVST3AudioEffect:
public FBVST3AudioEffect
{
public:
  FB_NOCOPY_NOMOVE_NODEFCTOR(FFVST3AudioEffect);
  FFVST3AudioEffect(FBStaticTopo const& topo, FUID const& controllerId):
  FBVST3AudioEffect(std::move(topo), controllerId) {}

protected:
  std::unique_ptr<IFBPlugProcessor>
  MakePlugProcessor(FBRuntimeTopo const& topo, float sampleRate) const override
  { return std::make_unique<FFPlugProcessor>(topo, sampleRate); }
};

static FUID
TextToFUID(char const* text)
{
  FUID result;
  result.fromString(text);
  return result;
}

static FUnknown*
ControllerFactory(void*) 
{
  auto topo = FFMakeTopo();
  auto result = new FBVST3EditController(*topo);
  return static_cast<IEditController*>(result);
}

static FUnknown*
ComponentFactory(void*)
{
  auto topo = FFMakeTopo();
  auto controllerFuid = TextToFUID(FF_PLUG_CONTROLLER_ID);
  auto result = new FFVST3AudioEffect(*topo, controllerFuid);
  return static_cast<IAudioProcessor*>(result);
}

BEGIN_FACTORY_DEF(FF_VENDOR_NAME, FF_VENDOR_URL, FF_VENDOR_MAIL)
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FF_PLUG_PROCESSOR_ID)),
      PClassInfo::kManyInstances, kVstAudioEffectClass, 
      FF_PLUG_NAME, kDistributable, PlugType::kInstrument,
      FF_PLUG_VERSION, kVstVersionString, ComponentFactory);
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(TextToFUID(FF_PLUG_CONTROLLER_ID)),
      PClassInfo::kManyInstances, kVstComponentControllerClass, 
      FF_PLUG_NAME, 0, "",
      FF_PLUG_VERSION, kVstVersionString, ControllerFactory)
END_FACTORY