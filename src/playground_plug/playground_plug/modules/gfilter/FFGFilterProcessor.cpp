#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/gfilter/FFGFilterTopo.hpp>
#include <playground_plug/modules/gfilter/FFGFilterProcessor.hpp>

#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/dsp/pipeline/fixed/FBFixedDoubleBlock.hpp>

void
FFGFilterProcessor::Process(FBModuleProcState& state)
{
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.global.gFilter[state.moduleSlot];
  auto const& res = procParams.acc.res[0].Global();
  auto const& freq = procParams.acc.freq[0].Global();
  auto const& gain = procParams.acc.gain[0].Global();

  auto& output = procState->dsp.global.gFilter[state.moduleSlot].output;
  auto const& input = procState->dsp.global.gFilter[state.moduleSlot].input;
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::GFilter];
  bool on = topo.params[(int)FFGFilterParam::On].Boolean().NormalizedToPlainFast(procParams.block.on[0].Value());
  auto type = (FFGFilterType)topo.params[(int)FFGFilterParam::Type].List().NormalizedToPlainFast(procParams.block.type[0].Value());

  if (!on)
  {
    output.CopyFrom(input);
    return;
  }

  FBFixedDoubleBlock resBlock, gainBlock;
  resBlock.LoadCastFromFloatArray(res.CV());
  //freqBlock.LoadCastFromFloatArray(freq.CV());
  
  FBFixedDoubleBlock a;
  switch (type)
  {
  case FFGFilterType::BLL:
  case FFGFilterType::LSH:
  case FFGFilterType::HSH:
    gainBlock.LoadCastFromFloatArray(gain.CV());
    a.Transform([&](int v) {
      auto plainGain = topo.params[(int)FFGFilterParam::Gain].Linear().NormalizedToPlainFast(gainBlock[v]);
      return xsimd::pow(FBDoubleVector(10.0), plainGain / 40.0); });
    break;
  default:
    break;
  }

  FBFixedDoubleBlock g, k, freqPlain; // TODO
  topo.NormalizedToLog2Fast(FFGFilterParam::Freq, freq, freqPlain);
  k.Transform([&](int v) { return 2.0 - 2.0 * resBlock[v]; });
  g.Transform([&](int v) {
    return xsimd::tan(std::numbers::pi * freqPlain[v] / state.input->sampleRate); });

  switch (type)
  {
  case FFGFilterType::BLL: k.Transform([&](int v) { return k[v] / a[v]; }); break;
  case FFGFilterType::LSH: g.Transform([&](int v) { return g[v] / xsimd::sqrt(a[v]); }); break;
  case FFGFilterType::HSH: g.Transform([&](int v) { return g[v] * xsimd::sqrt(a[v]); }); break;
  default: break;
  }
  
  FBFixedDoubleBlock a1b, a2b, a3b;
  a1b.Transform([&](int v) { return 1.0 / (1.0 + g[v] * (g[v] + k[v])); });
  a2b.Transform([&](int v) { return g[v] * a1b[v]; });
  a3b.Transform([&](int v) { return g[v] * a2b[v]; });

  FBFixedDoubleArray a1a, a2a, a3a;
  a1b.StoreToDoubleArray(a1a);
  a2b.StoreToDoubleArray(a2a);
  a3b.StoreToDoubleArray(a3a);

  FBFixedDoubleBlock m0b, m1b, m2b;
  switch (type)
  {
  case FFGFilterType::LPF:
    m0b.Transform([&](int v) { return 0.0; });
    m1b.Transform([&](int v) { return 0.0; });
    m2b.Transform([&](int v) { return 1.0; });
    break;
  case FFGFilterType::BPF:
    m0b.Transform([&](int v) { return 0.0; });
    m1b.Transform([&](int v) { return 1.0; });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterType::HPF:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -k[v]; });
    m2b.Transform([&](int v) { return -1.0; });
    break;
  case FFGFilterType::BSF:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -k[v]; });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterType::PEQ:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -k[v]; });
    m2b.Transform([&](int v) { return -2.0; });
    break;
  case FFGFilterType::APF:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return -2.0 * k[v]; });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterType::BLL:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return k[v] * (a[v] * a[v] - 1.0); });
    m2b.Transform([&](int v) { return 0.0; });
    break;
  case FFGFilterType::LSH:
    m0b.Transform([&](int v) { return 1.0; });
    m1b.Transform([&](int v) { return k[v] * (a[v] - 1.0); });
    m2b.Transform([&](int v) { return a[v] * a[v] - 1.0; });
    break;
  case FFGFilterType::HSH:
    m0b.Transform([&](int v) { return a[v] * a[v]; });
    m1b.Transform([&](int v) { return k[v] * (1.0 - a[v]) * a[v]; });
    m2b.Transform([&](int v) { return 1.0 - a[v] * a[v]; });
    break;
  default:
    assert(false);
    break;
  }

  FBFixedDoubleArray m0a, m1a, m2a;
  m0b.StoreToDoubleArray(m0a);
  m1b.StoreToDoubleArray(m1a);
  m2b.StoreToDoubleArray(m2a);

  FBFixedDoubleAudioArray audioIn;
  FBFixedDoubleAudioArray audioOut = {};
  input.StoreCastToDoubleArray(audioIn);
  
  for (int s = 0; s < FBFixedBlockSamples; s++)
    for (int ch = 0; ch < 2; ch++)
    {
      double v0 = audioIn.data[ch].data[s];
      double v3 = v0 - _ic2eq[ch];
      double v1 = a1a.data[s] * _ic1eq[ch] + a2a.data[s] * v3;
      double v2 = _ic2eq[ch] + a2a.data[s] * _ic1eq[ch] + a3a.data[s] * v3;
      _ic1eq[ch] = 2 * v1 - _ic1eq[ch];
      _ic2eq[ch] = 2 * v2 - _ic2eq[ch];
      audioOut.data[ch].data[s] = m0a.data[s] * v0 + m1a.data[s] * v1 + m2a.data[s] * v2;
    }
  output.LoadCastFromDoubleArray(audioOut);  

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;
  auto& exchangeParams = exchangeToGUI->param.global.gFilter[state.moduleSlot];
  exchangeToGUI->global.gFilter[state.moduleSlot].active = true;
  exchangeParams.acc.res[0] = res.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.freq[0] = freq.CV().data[FBFixedBlockSamples - 1];
  exchangeParams.acc.gain[0] = gain.CV().data[FBFixedBlockSamples - 1];
}