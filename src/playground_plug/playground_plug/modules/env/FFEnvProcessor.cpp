#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFPlugState.hpp>
#include <playground_plug/pipeline/FFModuleProcState.hpp>
#include <playground_plug/modules/env/FFEnvTopo.hpp>
#include <playground_plug/modules/env/FFEnvProcessor.hpp>

#include <playground_base/base/topo/FBRuntimeTopo.hpp>
#include <playground_base/dsp/pipeline/shared/FBVoiceInfo.hpp>

#include <cmath>

void
FFEnvProcessor::BeginVoice(FFModuleProcState const& state)
{
  _finished = false;
  _stagePositions.fill(0);
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  _voiceState.holdSamples = (int)std::round(topo.params[(int)FFEnvParam::HoldTime].linear.NormalizedToPlain(
    params.block.holdTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.decaySamples = (int)std::round(topo.params[(int)FFEnvParam::DecayTime].linear.NormalizedToPlain(
    params.block.decayTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.delaySamples = (int)std::round(topo.params[(int)FFEnvParam::DelayTime].linear.NormalizedToPlain(
    params.block.delayTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.attackSamples = (int)std::round(topo.params[(int)FFEnvParam::AttackTime].linear.NormalizedToPlain(
    params.block.attackTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.releaseSamples = (int)std::round(topo.params[(int)FFEnvParam::ReleaseTime].linear.NormalizedToPlain(
    params.block.releaseTime[0].Voice()[voice]) * state.sampleRate);
  _voiceState.on = topo.params[(int)FFEnvParam::On].boolean.NormalizedToPlain(params.block.on[0].Voice()[voice]);
  _voiceState.type = (FFEnvType)topo.params[(int)FFEnvParam::Type].list.NormalizedToPlain(params.block.type[0].Voice()[voice]);
}

bool 
FFEnvProcessor::Process(FFModuleProcState const& state)
{
  int voice = state.voice->slot;
  auto* procState = state.ProcState<FFProcState>();
  auto const& topo = state.topo->static_.modules[(int)FFModuleType::Env];
  auto const& params = procState->param.voice.env[state.moduleSlot];
  auto& output = procState->dsp.voice[voice].env[state.moduleSlot].output;

  if (!_voiceState.on || _finished)
  {
    output.Clear();
    return true;
  }

  int s = 0;
  FBFixedFloatArray scratch = {};
  FBFixedFloatBlock sustainBlock = {};
  FBFixedFloatArray sustainScratch = {};
  sustainBlock.Transform([&](int v) { return params.acc.sustainLevel[0].Voice()[voice].CV(v); });
  sustainBlock.StoreToFloatArray(sustainScratch);

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Delay] < _voiceState.delaySamples; 
    s++, _stagePositions[(int)FFEnvStage::Delay]++)
    scratch.data[s] = 0.0f;

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Attack] < _voiceState.attackSamples; 
    s++, _stagePositions[(int)FFEnvStage::Attack]++)
    scratch.data[s] = _stagePositions[(int)FFEnvStage::Attack] / (float)_voiceState.attackSamples;

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Hold] < _voiceState.holdSamples; 
    s++, _stagePositions[(int)FFEnvStage::Hold]++)
    scratch.data[s] = 1.0f;

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Decay] < _voiceState.decaySamples; 
    s++, _stagePositions[(int)FFEnvStage::Decay]++)
    scratch.data[s] = sustainScratch.data[s] + (1.0f - sustainScratch.data[s]) * 
      (1.0f - _stagePositions[(int)FFEnvStage::Decay] / (float)_voiceState.decaySamples);

  for (; s < FBFixedBlockSamples && _stagePositions[(int)FFEnvStage::Release] < _voiceState.releaseSamples;
    s++, _stagePositions[(int)FFEnvStage::Release]++)
    scratch.data[s] = sustainScratch.data[s] * 
      (1.0f - _stagePositions[(int)FFEnvStage::Release] / (float)_voiceState.releaseSamples);

  for (; s < FBFixedBlockSamples; s++)
  {
    _finished = true;
    scratch.data[s] = 0.0f;
  }
  output.LoadFromFloatArray(scratch);
  return _finished;
}