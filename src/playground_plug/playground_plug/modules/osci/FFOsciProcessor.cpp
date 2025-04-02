#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_plug/modules/osci/FFOsciProcessor.hpp>

#include <playground_base/dsp/shared/FBDSPUtility.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>
#include <playground_base/dsp/pipeline/glue/FBPlugInputBlock.hpp>
#include <playground_base/base/topo/runtime/FBRuntimeTopo.hpp>
#include <playground_base/base/state/proc/FBModuleProcState.hpp>

static inline int
OsciModStartSlot(int osciSlot)
{
  switch (osciSlot)
  {
  case 0: return -1;
  case 1: return 0;
  case 2: return 1;
  case 3: return 3;
  default: assert(false); return -1;
  }
}

static inline float
GenerateSin(float phase)
{
  return std::sin(phase * FBTwoPi);
}

static inline float
GenerateSaw(float phase, float incr)
{
  float one = 1.0f;
  float zero = 0.0f;
  float blepLo = phase / incr;
  float blepHi = (phase - 1.0f) / incr;
  float result = phase * 2.0f - 1.0f;
  float loMul = phase < incr ? 1.0f : 0.0f;
  float hiMul = phase >= 1.0f - incr ? 1.0f : 0.0f;
  hiMul *= (1.0f - loMul);
  result -= loMul * ((2.0f - blepLo) * blepLo - 1.0f);
  result -= hiMul * ((blepHi + 2.0f) * blepHi + 1.0f);
  return result;
}

static inline float
GenerateSqr(float phase, float incr, float pw)
{
  float minPW = 0.05f;
  float realPW = (minPW + (1.0f - minPW) * pw) * 0.5f;
  float phase2 = phase + realPW;
  phase2 -= std::floor(phase2);
  return (GenerateSaw(phase, incr) - GenerateSaw(phase2, incr)) * 0.5f;
}

static inline float
GenerateBLAMP(float phase, float incr)
{
  float y = 0.0f;
  float one = 1.0f;
  float zero = 0.0f;
  float phaseLtIncrMul = phase < incr ? one : zero;
  float phaseBetween0And2IncrMul = phase >= 0.0f && phase < 2.0f * incr ? one : zero;
  float x = phase / incr;
  float u = 2.0f - x;
  float u2 = u * u;
  u *= u2 * u2;
  y -= phaseBetween0And2IncrMul * u;
  float v = 1.0f - x;
  float v2 = v * v;
  v *= v2 * v2;
  y += 4.0f * phaseBetween0And2IncrMul * phaseLtIncrMul * v;
  return y * incr / 15.0f;
}

static inline float
GenerateTri(float phase, float incr)
{
  float v = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
  v += GenerateBLAMP(phase, incr);
  v += GenerateBLAMP(1.0f - phase, incr);
  phase += 0.5f;
  phase -= std::floor(phase);
  v -= GenerateBLAMP(phase, incr);
  v -= GenerateBLAMP(1.0f - phase, incr);
  return v;
}

static inline float
GenerateDSF(
  float phase, float freq, float decay,
  float distFreq, float overtones)
{
  float const decayRange = 0.99f;
  float const scaleFactor = 0.975f;

  float n = overtones;
  float w = decay * decayRange;
  float wPowNp1 = std::pow(w, n + 1.0f);
  float u = 2.0f * FBPi * phase;
  float v = 2.0f * FBPi * distFreq * phase / freq;
  float a = w * std::sin(u + n * v) - std::sin(u + (n + 1.0f) * v);
  float x = (w * std::sin(v - u) + std::sin(u)) + wPowNp1 * a;
  float y = 1.0f + w * w - 2.0f * w * std::cos(v);
  float scale = (1.0f - wPowNp1) / (1.0f - w);
  return x * scaleFactor / (y * scale);
}

static inline float
GenerateDSFOvertones(
  float phase, float freq, float decay,
  float distFreq, float maxOvertones, int overtones_)
{
  float overtones = static_cast<float>(overtones_);
  overtones = std::min(overtones, std::floor(maxOvertones));
  return GenerateDSF(phase, freq, decay, distFreq, overtones);
}

static inline float
GenerateDSFBandwidth(
  float phase, float freq, float decay,
  float distFreq, float maxOvertones, float bandwidth)
{
  float overtones = 1.0f + std::floor(bandwidth * (maxOvertones - 1.0f));
  overtones = std::min(overtones, std::floor(maxOvertones));
  return GenerateDSF(phase, freq, decay, distFreq, overtones);
}

void
FFOsciProcessor::BeginVoice(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& params = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  _voiceState.key = static_cast<float>(state.voice->event.note.key);
  _voiceState.note = topo.NormalizedToNoteFast(FFOsciParam::Note, params.block.note[0].Voice()[voice]);
  _voiceState.type = topo.NormalizedToListFast<FFOsciType>(FFOsciParam::Type, params.block.type[0].Voice()[voice]);
  _voiceState.basicSinOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSinOn, params.block.basicSinOn[0].Voice()[voice]);
  _voiceState.basicSawOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSawOn, params.block.basicSawOn[0].Voice()[voice]);
  _voiceState.basicTriOn = topo.NormalizedToBoolFast(FFOsciParam::BasicTriOn, params.block.basicTriOn[0].Voice()[voice]);
  _voiceState.basicSqrOn = topo.NormalizedToBoolFast(FFOsciParam::BasicSqrOn, params.block.basicSqrOn[0].Voice()[voice]);
  _voiceState.dsfMode = topo.NormalizedToListFast<FFOsciDSFMode>(FFOsciParam::DSFMode, params.block.dsfMode[0].Voice()[voice]);
  _voiceState.dsfDistance = topo.NormalizedToDiscreteFast(FFOsciParam::DSFDistance, params.block.dsfDistance[0].Voice()[voice]);
  _voiceState.dsfOvertones = topo.NormalizedToDiscreteFast(FFOsciParam::DSFOvertones, params.block.dsfOvertones[0].Voice()[voice]);
  _voiceState.dsfBandwidthPlain = topo.NormalizedToIdentityFast(FFOsciParam::DSFBandwidth, params.block.dsfBandwidth[0].Voice()[voice]);
  _voiceState.unisonCount = topo.NormalizedToDiscreteFast(FFOsciParam::UnisonCount, params.block.unisonCount[0].Voice()[voice]);
  _voiceState.unisonDetuneHQ = topo.NormalizedToBoolFast(FFOsciParam::UnisonDetuneHQ, params.block.unisonDetuneHQ[0].Voice()[voice]);
  _voiceState.unisonOffsetPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffset, params.block.unisonOffset[0].Voice()[voice]);
  _voiceState.unisonOffsetRandomPlain = topo.NormalizedToIdentityFast(FFOsciParam::UnisonOffsetRandom, params.block.unisonOffsetRandom[0].Voice()[voice]);

  _prng = {};
  _phase = {};
  for (int i = 0; i < _voiceState.unisonCount; i++)
  {
    float offsetRandom = _voiceState.unisonOffsetRandomPlain;
    float unisonPhase = i * _voiceState.unisonOffsetPlain / _voiceState.unisonCount;
    _phases[i] = FFOsciPhase(((1.0f - offsetRandom) + offsetRandom * _prng.Next()) * unisonPhase);
  }

  int modStartSlot = OsciModStartSlot(state.moduleSlot);
  auto const& modParams = procState->param.voice.osciMod[0];
  auto const& modTopo = state.topo->static_.modules[(int)FFModuleType::OsciMod];
  for (int modSlot = modStartSlot; modSlot < modStartSlot + state.moduleSlot; modSlot++)
  {
    int srcOsciSlot = modSlot - modStartSlot;
    auto const& srcOsciParams = procState->param.voice.osci[srcOsciSlot];
    _voiceState.modSourceOn[srcOsciSlot] = modTopo.NormalizedToBoolFast(FFOsciModParam::On, modParams.block.on[modSlot].Voice()[voice]);
    _voiceState.modSourceUnisonCount[srcOsciSlot] = topo.NormalizedToDiscreteFast(FFOsciParam::UnisonCount, srcOsciParams.block.unisonCount[0].Voice()[voice]);
  }

  auto& prevUnisonOutput = procState->dsp.voice[voice].osci[state.moduleSlot].prevUnisonOutput;
  for (int i = 0; i < _voiceState.unisonCount; i++)
    prevUnisonOutput[i].Fill(0.0f);
}

// TODO is this worth it?
// TODO should rewrite the whole thing ?
void
FFOsciProcessor::ProcessBasicUnisonVoiceFast(
  FBFixedFloatArray& unisonAudioOut,
  FBFixedFloatArray const& phasePlusFm,
  FBFixedFloatArray const& incr,
  FBFixedFloatArray const& sinGainPlain,
  FBFixedFloatArray const& sawGainPlain,
  FBFixedFloatArray const& triGainPlain,
  FBFixedFloatArray const& sqrGainPlain,
  FBFixedFloatArray const& sqrPWPlain)
{
  unisonAudioOut.Fill(0.0f);
  if (_voiceState.basicSinOn)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      unisonAudioOut[s] += GenerateSin(phasePlusFm[s]) * sinGainPlain[s];
  if (_voiceState.basicSawOn)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      unisonAudioOut[s] += GenerateSaw(phasePlusFm[s], incr[s]) * sawGainPlain[s];
  if (_voiceState.basicTriOn)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      unisonAudioOut[s] += GenerateTri(phasePlusFm[s], incr[s]) * triGainPlain[s];
  if (_voiceState.basicSqrOn)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      unisonAudioOut[s] += GenerateSqr(phasePlusFm[s], incr[s], sqrPWPlain[s]) * sqrGainPlain[s];
}

void
FFOsciProcessor::ProcessBasicUnisonVoiceSlow(
  float& unisonAudioOut,
  float const phasePlusFm,
  float const incr,
  float const sinGainPlain,
  float const sawGainPlain,
  float const triGainPlain,
  float const sqrGainPlain,
  float const sqrPWPlain)
{
  unisonAudioOut = 0.0f;
  if (_voiceState.basicSinOn)
    unisonAudioOut += GenerateSin(phasePlusFm) * sinGainPlain;
  if (_voiceState.basicSawOn)
    unisonAudioOut += GenerateSaw(phasePlusFm, incr) * sawGainPlain;
  if (_voiceState.basicTriOn)
    unisonAudioOut += GenerateTri(phasePlusFm, incr) * triGainPlain;
  if (_voiceState.basicSqrOn)
    unisonAudioOut += GenerateSqr(phasePlusFm, incr, sqrPWPlain) * sqrGainPlain;
}

// https://www.verklagekasper.de/synths/dsfsynthesis/dsfsynthesis.html
void
FFOsciProcessor::ProcessDSFUnisonVoiceFast(
  float sampleRate,
  FBFixedFloatArray& unisonAudioOut,
  FBFixedFloatArray const& phasePlusFm,
  FBFixedFloatArray const& freq,
  FBFixedFloatArray const& decayPlain)
{
  FBFixedFloatArray distFreq;
  FBFixedFloatArray maxOvertones;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    distFreq[s] = static_cast<float>(_voiceState.dsfDistance) * freq[s];
    maxOvertones[s] = (sampleRate * 0.5f - freq[s]) / distFreq[s];
  }

  if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
    for (int s = 0; s < FBFixedBlockSamples; s++)
      unisonAudioOut[s] = GenerateDSFOvertones(
        phasePlusFm[s], freq[s], decayPlain[s], distFreq[s], maxOvertones[s], _voiceState.dsfOvertones);
  else
    for (int s = 0; s < FBFixedBlockSamples; s++)
      unisonAudioOut[s] = GenerateDSFBandwidth(
        phasePlusFm[s], freq[s], decayPlain[s], distFreq[s], maxOvertones[s], _voiceState.dsfBandwidthPlain);
}

void
FFOsciProcessor::ProcessDSFUnisonVoiceSlow(
  float sampleRate,
  float& unisonAudioOut,
  float const phasePlusFm,
  float const freq,
  float const decayPlain)
{
  float distFreq;
  float maxOvertones;
  distFreq = static_cast<float>(_voiceState.dsfDistance) * freq;
  maxOvertones = (sampleRate * 0.5f - freq) / distFreq;

  if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
    unisonAudioOut = GenerateDSFOvertones(phasePlusFm, freq, decayPlain, distFreq, maxOvertones, _voiceState.dsfOvertones);
  else
    unisonAudioOut = GenerateDSFBandwidth(phasePlusFm, freq, decayPlain, distFreq, maxOvertones, _voiceState.dsfBandwidthPlain);
}

void
FFOsciProcessor::ProcessTypeUnisonVoiceFast(
  float sampleRate,
  FBFixedFloatArray& unisonAudioOut,
  FBFixedFloatArray const& phasePlusFm,
  FBFixedFloatArray const& freq,
  FBFixedFloatArray const& incr,
  FBFixedFloatArray const& basicSinGainPlain,
  FBFixedFloatArray const& basicSawGainPlain,
  FBFixedFloatArray const& basicTriGainPlain,
  FBFixedFloatArray const& basicSqrGainPlain,
  FBFixedFloatArray const& basicSqrPWPlain,
  FBFixedFloatArray const& dsfDecayPlain)
{
  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasicUnisonVoiceFast(unisonAudioOut, phasePlusFm, incr,
      basicSinGainPlain, basicSawGainPlain,
      basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain);
  else if (_voiceState.type == FFOsciType::DSF)
    ProcessDSFUnisonVoiceFast(sampleRate, unisonAudioOut, phasePlusFm, freq, dsfDecayPlain);
  else
    assert(false);
}

void
FFOsciProcessor::ProcessTypeUnisonVoiceSlow(
  float sampleRate,
  float& unisonAudioOut,
  float const phasePlusFm,
  float const freq,
  float const incr,
  float const basicSinGainPlain,
  float const basicSawGainPlain,
  float const basicTriGainPlain,
  float const basicSqrGainPlain,
  float const basicSqrPWPlain,
  float const dsfDecayPlain)
{
  if (_voiceState.type == FFOsciType::Basic)
    ProcessBasicUnisonVoiceSlow(unisonAudioOut, phasePlusFm, incr,
      basicSinGainPlain, basicSawGainPlain,
      basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain);
  else if (_voiceState.type == FFOsciType::DSF)
    ProcessDSFUnisonVoiceSlow(sampleRate, unisonAudioOut, phasePlusFm, freq, dsfDecayPlain);
  else
    assert(false);
}

void
FFOsciProcessor::ProcessUnisonVoice(
  FBModuleProcState const& state,
  int unisonVoice,
  FBFixedFloatArray& unisonAudioOut,
  FBFixedFloatArray const& unisonPos,
  FBFixedFloatArray const& basePitch,
  FBFixedFloatArray const& detunePlain,
  FBFixedFloatArray const& basicSinGainPlain,
  FBFixedFloatArray const& basicSawGainPlain,
  FBFixedFloatArray const& basicTriGainPlain,
  FBFixedFloatArray const& basicSqrGainPlain,
  FBFixedFloatArray const& basicSqrPWPlain,
  FBFixedFloatArray const& dsfDecayPlain)
{
  FBFixedFloatArray incr;
  FBFixedFloatArray freq;
  FBFixedFloatArray pitch;
  FBFixedFloatArray fmModulator;

  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();

#if 0
  // TODO self-fm and delay
  fmModulator.Fill(0.0f);
  for (int src = 0; src < state.moduleSlot; src++)
    if (_voiceState.fmSourceMode[src] != FFOsciFMMode::Off && _voiceState.modSourceUnisonCount[src] > unisonVoice)
    {
      float sourceScale;
      float sourceOffset;
      switch (_voiceState.fmSourceMode[src])
      {
      case FFOsciFMMode::Up:
        sourceScale = 0.5f;
        sourceOffset = 0.5f;
        break;
      case FFOsciFMMode::Down:
        sourceScale = 0.5f;
        sourceOffset = -0.5f;
        break;
      case FFOsciFMMode::ThroughZero:
        sourceScale = 1.0f;
        sourceOffset = 0.0f;
        break;
      default:
        assert(false);
        break;
      }

      int slot = FFOsciModSourceAndTargetToSlot().at({ src, state.moduleSlot });
      FBFixedFloatArray const& index = procState->dsp.voice[voice].osciFM.outputIndex[slot];

      int d;
      FBFixedFloatArray baseModulatorBlock;
      auto const& baseModulatorCurrentBlock = procState->dsp.voice[voice].osci[src].unisonOutput[unisonVoice];
      auto const& baseModulatorPrevBlock = procState->dsp.voice[voice].osci[src].prevUnisonOutput[unisonVoice];
      for (d = 0; d < _voiceState.fmSourceDelay[src]; d++)
        baseModulatorBlock[d] = baseModulatorPrevBlock[FBFixedBlockSamples - _voiceState.fmSourceDelay[src] + d];
      for (; d < FBFixedBlockSamples; d++)
        baseModulatorBlock[d] = baseModulatorCurrentBlock[d - _voiceState.fmSourceDelay[src]];
      for (int s = 0; s < FBFixedBlockSamples; s++)
        fmModulator[s] = fmModulator[s] + (baseModulatorBlock[s] * sourceScale + sourceOffset) * index[s];
    }

#endif // todo

  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    pitch[s] = basePitch[s] + unisonPos[s] * detunePlain[s];
    if (_voiceState.unisonDetuneHQ || _voiceState.unisonCount == 1)
      freq[s] = FBPitchToFreqAccurate(pitch[s], sampleRate);
    else
      //freq[v] = FBPitchToFreqFastAndInaccurate(pitch[v]);
      freq[s] = FBPitchToFreqAccurate(pitch[s], sampleRate); // TODO just drop it
    incr[s] = freq[s] / sampleRate;
  }

  // TODO
#if 0
  if (_voiceState.fmSourceMode[state.moduleSlot] == FFOsciFMMode::Off || _voiceState.modSourceUnisonCount[state.moduleSlot] <= unisonVoice)
  {
    FBFixedFloatArray phasePlusFM;
    for (int s = 0; s < FBFixedBlockSamples; s++)
      phasePlusFM[s] = _phases[unisonVoice].Next(incr[s], fmModulator[s]);
    ProcessTypeUnisonVoiceFast(
      sampleRate, unisonAudioOut, phasePlusFM, freq, incr,
      basicSinGainPlain, basicSawGainPlain, basicTriGainPlain,
      basicSqrGainPlain, basicSqrPWPlain, dsfDecayPlain);
  }
  else
  {
#if 0 // TODO
    float sourceScale;
    float sourceOffset;
    switch (_voiceState.fmSourceMode[state.moduleSlot])
    {
    case FFOsciFMMode::Up:
      sourceScale = 0.5f;
      sourceOffset = 0.5f;
      break;
    case FFOsciFMMode::Down:
      sourceScale = 0.5f;
      sourceOffset = -0.5f;
      break;
    case FFOsciFMMode::ThroughZero:
      sourceScale = 1.0f;
      sourceOffset = 0.0f;
      break;
    default:
      assert(false);
      break;
    }

    FBFixedFloatArray selfFMIndexArray;
    int slot = FFOsciModSourceAndTargetToSlot().at({ state.moduleSlot, state.moduleSlot });
    auto const& selfFMIndexBlock = procState->dsp.voice[voice].osciFM.outputIndex[slot];

    FBFixedFloatArray selfFMModulatorArray;
    auto const& selfFMModulatorPrevBlock = procState->dsp.voice[voice].osci[state.moduleSlot].prevUnisonOutput[unisonVoice];
    selfFMModulatorPrevBlock.StoreToFloatArray(selfFMModulatorArray);

    int selfFMModulatorWritePos = 0;
    int selfFMModulatorReadPos = FBFixedBlockSamples - 1 - _voiceState.fmSourceDelay[state.moduleSlot];
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      // todo code up a proper ringbuffer
      float selfFMModulatorValue = selfFMModulatorArray.data[selfFMModulatorReadPos] * sourceScale + sourceOffset;
      phasePlusFMArray.data[s] = _phases[unisonVoice].Next(incrArray.data[s], fmModulatorArray.data[s] + selfFMModulatorValue * selfFMIndexArray.data[s]);
      ProcessTypeUnisonVoiceSlow(
        sampleRate, unisonAudioOutArray.data[s], phasePlusFMArray.data[s], freqArray.data[s], incrArray.data[s],
        basicSinGainPlainArray.data[s], basicSawGainPlainArray.data[s], basicTriGainPlainArray.data[s],
        basicSqrGainPlainArray.data[s], basicSqrPWPlainArray.data[s], dsfDecayPlainArray.data[s]);
      selfFMModulatorArray.data[selfFMModulatorWritePos++] = unisonAudioOutArray.data[s];
      selfFMModulatorReadPos = (selfFMModulatorReadPos + 1) % FBFixedBlockSamples;
    }

    unisonAudioOut.LoadFromFloatArray(unisonAudioOutArray);
#endif
  }
#endif

#if 0

  for (int src = 0; src <= state.moduleSlot; src++)
    if (_voiceState.amSourceOn[src] && _voiceState.modSourceUnisonCount[src] > unisonVoice)
    {
      int slot = FFOsciModSourceAndTargetToSlot().at({ src, state.moduleSlot });
      auto const& mix = procState->dsp.voice[voice].osciAM.outputMix[slot];
      auto const& ring = procState->dsp.voice[voice].osciAM.outputRing[slot];
      auto const& rmModulator = procState->dsp.voice[voice].osci[src].unisonOutput[unisonVoice];
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        float amModulator = rmModulator[s] * 0.5f + 0.5f;
        float amModulated = unisonAudioOut[s] * amModulator;
        float rmModulated = unisonAudioOut[s] * rmModulator[s];
        float modulated = (1.0f - ring[s]) * amModulated + ring[s] * rmModulated;
        unisonAudioOut[s] = (1.0f - mix[s]) * unisonAudioOut[s] + mix[s] * modulated;
      }
    }
#endif
}

// TODO drop this
void
FFOsciProcessor::ShiftPrevUnisonOutputForAllOscis(
  FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  for (int o = 0; o < FFOsciCount; o++)
  {
    auto& unisonOutput = procState->dsp.voice[voice].osci[o].unisonOutput;
    auto& prevUnisonOutput = procState->dsp.voice[voice].osci[o].prevUnisonOutput;
    for (int uv = 0; uv < _voiceState.unisonCount; uv++)
      unisonOutput[uv].CopyTo(prevUnisonOutput[uv]);
  }
}

void
FFOsciProcessor::ProcessUnison(
  FBModuleProcState& state,
  FBFixedFloatAudioArray& audioOut,
  std::array<FBFixedFloatArray, FFOsciUnisonMaxCount>& unisonAudioOut,
  FBFixedFloatArray const& basePitch)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];

  FBFixedFloatArray dsfDecayPlain;
  FBFixedFloatArray basicSqrPWPlain;
  FBFixedFloatArray basicSinGainPlain;
  FBFixedFloatArray basicSawGainPlain;
  FBFixedFloatArray basicTriGainPlain;
  FBFixedFloatArray basicSqrGainPlain;

  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  auto const& detuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& spreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];

  if (_voiceState.type == FFOsciType::Basic)
  {
    if (_voiceState.basicSinOn)
      topo.NormalizedToLinearFast(FFOsciParam::BasicSinGain, basicSinGainNorm, basicSinGainPlain);
    if (_voiceState.basicSawOn)
      topo.NormalizedToLinearFast(FFOsciParam::BasicSawGain, basicSawGainNorm, basicSawGainPlain);
    if (_voiceState.basicTriOn)
      topo.NormalizedToLinearFast(FFOsciParam::BasicTriGain, basicTriGainNorm, basicTriGainPlain);
    if (_voiceState.basicSqrOn)
    {
      topo.NormalizedToIdentityFast(FFOsciParam::BasicSqrPW, basicSqrPWNorm, basicSqrPWPlain);
      topo.NormalizedToLinearFast(FFOsciParam::BasicSqrGain, basicSqrGainNorm, basicSqrGainPlain);
    }
  }
  else if (_voiceState.type == FFOsciType::DSF)
    topo.NormalizedToIdentityFast(FFOsciParam::DSFDecay, dsfDecayNorm, dsfDecayPlain);
  else
    assert(false);

  FBFixedFloatArray unisonPos;
  FBFixedFloatArray detunePlain;
  if (_voiceState.unisonCount == 1)
  {
    unisonPos.Fill(0.0f);
    detunePlain.Fill(0.0f);
    ProcessUnisonVoice(
      state, 0, unisonAudioOut[0], unisonPos,
      basePitch, detunePlain, basicSinGainPlain, basicSawGainPlain,
      basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain, dsfDecayPlain);
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      audioOut[0][s] = unisonAudioOut[0][s];
      audioOut[1][s] = unisonAudioOut[0][s];
    }
  }
  else
  {
    audioOut.Fill(0.0f);
    FBFixedFloatArray panning;
    FBFixedFloatArray spreadPlain;
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonDetune, detuneNorm, detunePlain);
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonSpread, spreadNorm, spreadPlain);

    for (int i = 0; i < _voiceState.unisonCount; i++)
    {
      unisonPos.Fill(i / (_voiceState.unisonCount - 1.0f) - 0.5f);
      ProcessUnisonVoice(
        state, i, unisonAudioOut[i], unisonPos,
        basePitch, detunePlain, basicSinGainPlain, basicSawGainPlain,
        basicTriGainPlain, basicSqrGainPlain, basicSqrPWPlain, dsfDecayPlain);
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        panning[s] = 0.5f + unisonPos[s] * spreadPlain[s];
        audioOut[0][s] += (1.0f - panning[s]) * unisonAudioOut[i][s];
        audioOut[1][s] += panning[s] * unisonAudioOut[i][s];
      }
    }
  }

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.unisonDetune[0][voice] = detuneNorm.Last();
  exchangeParams.acc.unisonSpread[0][voice] = spreadNorm.Last();
  exchangeParams.acc.dsfDecay[0][voice] = dsfDecayNorm.Last();
  exchangeParams.acc.basicSqrPW[0][voice] = basicSqrPWNorm.Last();
  exchangeParams.acc.basicSinGain[0][voice] = basicSinGainNorm.Last();
  exchangeParams.acc.basicSawGain[0][voice] = basicSawGainNorm.Last();
  exchangeParams.acc.basicTriGain[0][voice] = basicTriGainNorm.Last();
  exchangeParams.acc.basicSqrGain[0][voice] = basicSqrGainNorm.Last();
}

int
FFOsciProcessor::Process(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  float sampleRate = state.input->sampleRate;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;
  auto& unisonOutput = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutput;

  output.Fill(0.0f);
  for (int u = 0; u < _voiceState.unisonCount; u++)
    unisonOutput[u].Fill(0.0f);
  if (_voiceState.type == FFOsciType::Off)
  {
    if (state.moduleSlot == FFOsciCount - 1)
      ShiftPrevUnisonOutputForAllOscis(state);
    return 0;
  }

  auto const& dsfDecayNorm = procParams.acc.dsfDecay[0].Voice()[voice];
  auto const& detuneNorm = procParams.acc.unisonDetune[0].Voice()[voice];
  auto const& spreadNorm = procParams.acc.unisonSpread[0].Voice()[voice];
  auto const& basicSqrPWNorm = procParams.acc.basicSqrPW[0].Voice()[voice];
  auto const& basicSqrGainNorm = procParams.acc.basicSqrGain[0].Voice()[voice];
  auto const& basicSinGainNorm = procParams.acc.basicSinGain[0].Voice()[voice];
  auto const& basicSawGainNorm = procParams.acc.basicSawGain[0].Voice()[voice];
  auto const& basicTriGainNorm = procParams.acc.basicTriGain[0].Voice()[voice];
  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();

  float centNorm;
  FBFixedFloatArray basePitch, baseFreq, baseIncr, basePhase; // todo bph
  float notePitch = _voiceState.key + _voiceState.note - 60.0f;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    centNorm = procParams.acc.cent[0].Voice()[voice].CV()[s];
    float centPlain = topo.NormalizedToLinearFast(FFOsciParam::Cent, centNorm);
    basePitch[s] = notePitch + centPlain;
    baseFreq[s] = FBPitchToFreqAccurate(basePitch[s], state.input->sampleRate);
    baseIncr[s] = baseFreq[s] / state.input->sampleRate;
    basePhase[s] = _phase.Next(baseIncr[s]);
  }

  FBFixedFloatArray detunePlain;
  FBFixedFloatArray spreadPlain;
  std::array<float, FFOsciUnisonMaxCount> unisonPos;
  if (_voiceState.unisonCount == 1)
  {
    unisonPos[0] = 0.5f;
    detunePlain.Fill(0.0f);
    spreadPlain.Fill(0.0f);
  }
  else
  {
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonDetune, detuneNorm, detunePlain);
    topo.NormalizedToIdentityFast(FFOsciParam::UnisonSpread, spreadNorm, spreadPlain);
    for(int u = 0; u < _voiceState.unisonCount; u++)
      unisonPos[u] = u / (_voiceState.unisonCount - 1.0f) - 0.5f;
  }

  for (int u = 0; u < _voiceState.unisonCount; u++)
  {
    FBFixedFloatArray uniFreq;
    FBFixedFloatArray uniIncr;
    FBFixedFloatArray uniPhase;
    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float uniPitch = basePitch[s] + unisonPos[u] * detunePlain[s];
      uniFreq[s] = FBPitchToFreqAccurate(uniPitch, sampleRate);
      uniIncr[s] = uniFreq[s] / sampleRate;
      uniPhase[s] = _phases[u].Next(uniIncr[s], 0.0f); // TODO FM
    }

    if (_voiceState.type == FFOsciType::Basic)
    {
      if (_voiceState.basicSinOn)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutput[u][s] += GenerateSin(uniPhase[s]) * topo.NormalizedToLinearFast(FFOsciParam::BasicSinGain, basicSinGainNorm.CV()[s]);
      if (_voiceState.basicSawOn)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutput[u][s] += GenerateSaw(uniPhase[s], uniIncr[s]) * topo.NormalizedToLinearFast(FFOsciParam::BasicSawGain, basicSawGainNorm.CV()[s]);
      if (_voiceState.basicTriOn)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutput[u][s] += GenerateTri(uniPhase[s], uniIncr[s]) * topo.NormalizedToLinearFast(FFOsciParam::BasicTriGain, basicTriGainNorm.CV()[s]);
      if (_voiceState.basicSqrOn)
        for (int s = 0; s < FBFixedBlockSamples; s++)
        {
          float pwPlain = topo.NormalizedToIdentityFast(FFOsciParam::BasicSqrPW, basicSqrPWNorm.CV()[s]);
          unisonOutput[u][s] += GenerateSqr(uniPhase[s], uniIncr[s], pwPlain) * topo.NormalizedToLinearFast(FFOsciParam::BasicSqrGain, basicSqrGainNorm.CV()[s]);
        }
    } else if (_voiceState.type == FFOsciType::DSF)
    {
      FBFixedFloatArray dsfDistFreq;
      FBFixedFloatArray dsfDecayPlain;
      FBFixedFloatArray dsfMaxOvertones;
      for (int s = 0; s < FBFixedBlockSamples; s++)
      {
        dsfDistFreq[s] = static_cast<float>(_voiceState.dsfDistance) * baseFreq[s];
        dsfMaxOvertones[s] = (sampleRate * 0.5f - baseFreq[s]) / dsfDistFreq[s];
        dsfDecayPlain[s] = topo.NormalizedToIdentityFast(FFOsciParam::DSFDecay, dsfDecayNorm.CV()[s]);
      }
      if (_voiceState.dsfMode == FFOsciDSFMode::Overtones)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutput[u][s] = GenerateDSFOvertones(basePhase[s], baseFreq[s], dsfDecayPlain[s], dsfDistFreq[s], dsfMaxOvertones[s], _voiceState.dsfOvertones);
      else if (_voiceState.dsfMode == FFOsciDSFMode::Bandwidth)
        for (int s = 0; s < FBFixedBlockSamples; s++)
          unisonOutput[u][s] = GenerateDSFBandwidth(basePhase[s], baseFreq[s], dsfDecayPlain[s], dsfDistFreq[s], dsfMaxOvertones[s], _voiceState.dsfBandwidthPlain);
    }

    for (int src = 0; src < state.moduleSlot; src++)
      if (_voiceState.modSourceOn[src] && _voiceState.modSourceUnisonCount[src] > u)
      {
        int modSlot = OsciModStartSlot(state.moduleSlot) + src;
        auto const& am = procState->dsp.voice[voice].osciMod.outputAM[modSlot];
        auto const& rm = procState->dsp.voice[voice].osciMod.outputRM[modSlot];
        auto const& rmModulator = procState->dsp.voice[voice].osci[src].unisonOutput[u];
        for (int s = 0; s < FBFixedBlockSamples; s++)
        {
          float amModulator = rmModulator[s] * 0.5f + 0.5f;
          float amModulated = unisonOutput[u][s] * amModulator;
          float rmModulated = unisonOutput[u][s] * rmModulator[s];
          float modulated = (1.0f - rm[s]) * amModulated + rm[s] * rmModulated;
          unisonOutput[u][s] = (1.0f - am[s]) * unisonOutput[u][s] + am[s] * modulated;
        }
      }

    for (int s = 0; s < FBFixedBlockSamples; s++)
    {
      float uniPanning = 0.5f + unisonPos[u] * spreadPlain[s];
      output[0][s] += (1.0f - uniPanning) * unisonOutput[u][s];
      output[1][s] += uniPanning * unisonOutput[u][s];
    }
  }

  FBFixedFloatArray gainPlain;
  FBFixedFloatArray gLFOToGainPlain;
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& gLFOToGainNorm = procParams.acc.gLFOToGain[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::Gain, gainNorm, gainPlain);
  topo.NormalizedToIdentityFast(FFOsciParam::GLFOToGain, gLFOToGainNorm, gLFOToGainPlain);

  // TODO this might prove difficult, lets see how it fares with the matrices
  FBFixedFloatArray gLFO;
  auto* exchangeFromGUI = state.ExchangeFromGUIAs<FFExchangeState>();
  if (exchangeFromGUI != nullptr)
    gLFO.Fill(exchangeFromGUI->global.gLFO[0].lastOutput);
  else
    procState->dsp.global.gLFO[0].output.CopyTo(gLFO);

  FBFixedFloatArray gainWithGLFOBlock;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    gainWithGLFOBlock[s] = ((1.0f - gLFOToGainPlain[s]) + gLFOToGainPlain[s] * gLFO[s]) * gainPlain[s];
    output[0][s] *= gainWithGLFOBlock[s];
    output[1][s] *= gainWithGLFOBlock[s];
  }

  if (state.moduleSlot == FFOsciCount - 1)
    ShiftPrevUnisonOutputForAllOscis(state);

// TODO output all stuff

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreq.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.cent[0][voice] = centNorm;
  exchangeParams.acc.gain[0][voice] = gainWithGLFOBlock.Last();
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGainNorm.Last();
  return FBFixedBlockSamples;
}

int
FFOsciProcessor::Process2(FBModuleProcState& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcAs<FFProcState>();
  auto const& procParams = procState->param.voice.osci[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Osci];
  auto& output = procState->dsp.voice[voice].osci[state.moduleSlot].output;
  auto& unisonOutput = procState->dsp.voice[voice].osci[state.moduleSlot].unisonOutput;

  if (_voiceState.type == FFOsciType::Off)
  {
    if (state.moduleSlot == FFOsciCount - 1)
      ShiftPrevUnisonOutputForAllOscis(state);
    output.Fill(0.0f);
    return 0;
  }

  int prevPositionSamplesUpToFirstCycle = _phase.PositionSamplesUpToFirstCycle();

  FBFixedFloatArray centPlain;
  auto const& centNorm = procParams.acc.cent[0].Voice()[voice];
  topo.NormalizedToLinearFast(FFOsciParam::Cent, centNorm, centPlain);

  FBFixedFloatArray baseFreq;
  FBFixedFloatArray baseIncr;
  FBFixedFloatArray basePitch;
  float notePitch = _voiceState.key + _voiceState.note - 60.0f;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    basePitch[s] = notePitch + centPlain[s];
    baseFreq[s] = FBPitchToFreqAccurate(basePitch[s], state.input->sampleRate);
    baseIncr[s] = baseFreq[s] / state.input->sampleRate;
    _phase.Next(baseIncr[s]);
  }

  ProcessUnison(state, output, unisonOutput, basePitch);

  FBFixedFloatArray gainPlain;
  FBFixedFloatArray gLFOToGainPlain;
  auto const& gainNorm = procParams.acc.gain[0].Voice()[voice];
  auto const& gLFOToGainNorm = procParams.acc.gLFOToGain[0].Voice()[voice];
  topo.NormalizedToIdentityFast(FFOsciParam::Gain, gainNorm, gainPlain);
  topo.NormalizedToIdentityFast(FFOsciParam::GLFOToGain, gLFOToGainNorm, gLFOToGainPlain);

  // TODO this might prove difficult, lets see how it fares with the matrices
  FBFixedFloatArray gLFO;
  auto* exchangeFromGUI = state.ExchangeFromGUIAs<FFExchangeState>();
  if (exchangeFromGUI != nullptr)
    gLFO.Fill(exchangeFromGUI->global.gLFO[0].lastOutput);
  else
    procState->dsp.global.gLFO[0].output.CopyTo(gLFO);

  FBFixedFloatArray gainWithGLFOBlock;
  for (int s = 0; s < FBFixedBlockSamples; s++)
  {
    gainWithGLFOBlock[s] = ((1.0f - gLFOToGainPlain[s]) + gLFOToGainPlain[s] * gLFO[s]) * gainPlain[s];
    output[0][s] *= gainWithGLFOBlock[s];
    output[1][s] *= gainWithGLFOBlock[s];
  }

  if (state.moduleSlot == FFOsciCount - 1)
    ShiftPrevUnisonOutputForAllOscis(state);

  auto* exchangeToGUI = state.ExchangeToGUIAs<FFExchangeState>();
  if (exchangeToGUI == nullptr)
    return _phase.PositionSamplesUpToFirstCycle() - prevPositionSamplesUpToFirstCycle;

  auto& exchangeDSP = exchangeToGUI->voice[voice].osci[state.moduleSlot];
  exchangeDSP.active = true;
  exchangeDSP.lengthSamples = FBFreqToSamples(baseFreq.Last(), state.input->sampleRate);
  exchangeDSP.positionSamples = _phase.PositionSamplesCurrentCycle() % exchangeDSP.lengthSamples;

  auto& exchangeParams = exchangeToGUI->param.voice.osci[state.moduleSlot];
  exchangeParams.acc.cent[0][voice] = centNorm.Last();
  exchangeParams.acc.gain[0][voice] = gainWithGLFOBlock.Last();
  exchangeParams.acc.gLFOToGain[0][voice] = gLFOToGainNorm.Last();
  return FBFixedBlockSamples;
}