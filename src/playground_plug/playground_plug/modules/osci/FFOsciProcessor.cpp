#include <playground_plug/plug/FFPlugTopo.hpp>
#include <playground_plug/plug/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/base/topo/FBStaticTopo.hpp>
#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

static FBFloatVector
GenerateSin(FBFloatVector phase)
{
  return xsimd::sin(phase * FBTwoPi);
}

static FBFloatVector
GenerateSaw(FBFloatVector phase, FBFloatVector incr)
{ 
  // TODO fast paul saw
  // https://www.kvraudio.com/forum/viewtopic.php?t=375517
  // y = phase * 2 - 1
  // if (phase < inc) y -= b = phase / inc, (2.0f - b) * b - 1.0f
  // else if (phase >= 1.0f - inc) y -= b = (phase - 1.0f) / inc, (b + 2.0f) * b + 1.0f
  FBFloatVector one = 1.0f;
  FBFloatVector zero = 0.0f;
  FBFloatVector blepLo = phase / incr;
  FBFloatVector blepHi = (phase - 1.0f) / incr;
  FBFloatVector result = phase * 2.0f - 1.0f;
  auto loMask = xsimd::lt(phase, incr);
  auto hiMask = xsimd::ge(phase, 1.0f - incr);
  hiMask = xsimd::bitwise_andnot(hiMask, loMask);
  FBFloatVector loMul = xsimd::select(loMask, one, zero);
  FBFloatVector hiMul = xsimd::select(hiMask, one, zero);
  result -= loMul * ((2.0f - blepLo) * blepLo - 1.0f);
  result -= hiMul * ((blepHi + 2.0f) * blepHi + 1.0f);
  return result;
}

static FBFloatVector
GeneratePulse(FBFloatVector phase, FBFloatVector incr, FBFloatVector pw)
{
  // TODO PWM is no good
  FBFloatVector minPW = 0.05f;
  FBFloatVector realPW = (minPW + (1.0f - minPW * pw)) * 0.5f;
  FBFloatVector phase2 = phase + realPW;
  phase2 -= xsimd::floor(phase2);
  return (GenerateSaw(phase, incr) - GenerateSaw(phase2, incr)) * 0.5f;
}

void
FFOsciProcessor::Process(FFModuleProcState const& state, int voice)
{
  float key = static_cast<float>(state.voice->event.note.key);
  auto const& gLFO = state.proc->dsp.global.gLFO[0].output;
  auto& output = state.proc->dsp.voice[voice].osci[state.moduleSlot].output;

  auto const& topo = state.topo->modules[(int)FFModuleType::Osci];
  auto const& params = state.proc->param.voice.osci[state.moduleSlot];
  bool on = topo.params[(int)FFOsciParam::On].NormalizedToBool(params.block.on[0].Voice()[voice]);
  int type = topo.params[(int)FFOsciParam::Type].NormalizedToDiscrete(params.block.type[0].Voice()[voice]);
  int note = topo.params[(int)FFOsciParam::Note].NormalizedToDiscrete(params.block.note[0].Voice()[voice]);

  if (!on)
  {
    output.Clear();
    return;
  }

  FBFixedFloatBlock incr;
  FBFixedFloatBlock mono;
  FBFixedFloatBlock phase;
  incr.Transform([&](int v) {
    auto cent = params.acc.cent[0].Voice()[voice].CV(v);
    auto centPlain = topo.params[(int)FFOsciParam::Cent].NormalizedToPlainLinear(cent);
    auto pitch = key + note - 60.0f + centPlain;
    auto freq = FBPitchToFreq(pitch, state.sampleRate);
    return freq / state.sampleRate; });
  phase.Transform([&](int v) { return _phase.Next(incr[v]); });

  switch ((FFOsciType)type)
  {
  case FFOsciType::Sine: mono.Transform([&](int v) {
    return GenerateSin(phase[v]); }); break;
  case FFOsciType::Saw: mono.Transform([&](int v) {
    return GenerateSaw(phase[v], incr[v]); }); break;
  case FFOsciType::Pulse: mono.Transform([&](int v) {
    auto pw = params.acc.pw[0].Voice()[voice].CV(v);
    return GeneratePulse(phase[v], incr[v], pw); }); break;
  default: assert(false); break;
  }
  mono.Transform([&](int v) {
    auto gain = params.acc.gain[0].Voice()[voice].CV(v);
    auto gLFOToGain = params.acc.gLFOToGain[0].Voice()[voice].CV(v);
    return gain * ((1.0f - gLFOToGain) * mono[v] + mono[v] * gLFOToGain * gLFO[v]); });
  output.Transform([&](int ch, int v) {
    return mono[v];
  });
}