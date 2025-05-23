#pragma once

#include <playground_base/base/shared/FBSArray.hpp>
#include <playground_base/base/shared/FBUtility.hpp>
#include <playground_base/dsp/host/FBHostBlock.hpp>

#include <memory>

struct FBPlugInputBlock;
struct FBModuleProcState;

class FBPlugGUI;
class FBVoiceManager;
class FBProcStateContainer;
class FBExchangeStateContainer;

class FBGraphRenderState final
{
  friend class FBModuleGraphDisplayComponent;

  std::vector<FBNoteEvent> _notes = {};
  FBSArray2<float, FBFixedBlockSamples, 2> _audio = {};

  FBPlugGUI const* const _plugGUI;
  std::unique_ptr<FBPlugInputBlock> _input;
  std::unique_ptr<FBModuleProcState> _moduleState;
  std::unique_ptr<FBProcStateContainer> _procState;
  std::unique_ptr<FBVoiceManager> _primaryVoiceManager;
  std::unique_ptr<FBVoiceManager> _exchangeVoiceManager;

public:
  ~FBGraphRenderState();
  FBGraphRenderState(FBPlugGUI const* plugGUI);
  FB_NOCOPY_NOMOVE_NODEFCTOR(FBGraphRenderState);

  void PrepareForRenderExchange();
  void PrepareForRenderPrimaryVoice();
  void PrepareForRenderExchangeVoice(int voice);
  void PrepareForRenderPrimary(float sampleRate, float bpm);

  FBPlugGUI const* PlugGUI() const;
  FBModuleProcState* ModuleProcState();
  FBModuleProcState const* ModuleProcState() const;
  FBExchangeStateContainer const* ExchangeContainer() const;
  bool GlobalModuleExchangeStateEqualsPrimary(int moduleIndex, int moduleSlot) const;
  bool VoiceModuleExchangeStateEqualsPrimary(int voice, int moduleIndex, int moduleSlot) const;

  bool GUIParamBool(FBParamTopoIndices const& indices) const;
  float GUIParamLinear(FBParamTopoIndices const& indices) const;
  int GUIParamLinearTimeSamples(FBParamTopoIndices const& indices, float sampleRate) const;
  int GUIParamBarsSamples(FBParamTopoIndices const& indices, float sampleRate, float bpm) const;

  bool AudioParamBool(FBParamTopoIndices const& indices) const;
  int AudioParamDiscrete(FBParamTopoIndices const& indices) const;
  float AudioParamLinear(FBParamTopoIndices const& indices) const;
  int AudioParamLinearFreqSamples(FBParamTopoIndices const& indices, float sampleRate) const;
  int AudioParamLinearTimeSamples(FBParamTopoIndices const& indices, float sampleRate) const;
  int AudioParamBarsSamples(FBParamTopoIndices const& indices, float sampleRate, float bpm) const;
};