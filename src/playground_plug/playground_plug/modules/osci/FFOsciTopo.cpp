#include <playground_plug/shared/FFPlugTopo.hpp>
#include <playground_plug/shared/FFTopoDetail.hpp>
#include <playground_plug/modules/osci/FFOsciTopo.hpp>
#include <playground_base/base/topo/FBStaticModule.hpp>

std::unique_ptr<FBStaticModule>
FFMakeOsciTopo()
{
  auto result = std::make_unique<FBStaticModule>();
  result->voice = true;
  result->name = "Osc";
  result->slotCount = FFOsciCount;
  result->id = "{73BABDF5-AF1C-436D-B3AD-3481FD1AB5D6}";
  result->params.resize((int)FFOsciParam::Count);
  auto selectModule = [](auto& state) { return &state.voice.osci; };

  auto& on = result->params[(int)FFOsciParam::On];
  on.acc = false;
  on.name = "On";
  on.slotCount = 1;
  on.id = "{35FC56D5-F0CB-4C37-BCA2-A0323FA94DCF}";
  on.type = FBParamType::Boolean;
  auto selectOn = [](auto& module) { return &module.block.on; };
  on.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectOn);
  on.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectOn);

  auto& type = result->params[(int)FFOsciParam::Type];
  type.acc = false;
  type.defaultText = "Sine";
  type.name = "Type";
  type.slotCount = 1;
  type.id = "{43F55F08-7C81-44B8-9A95-CC897785D3DE}";
  type.type = FBParamType::List;
  type.list.items = {
    { "{2400822D-BFA9-4A43-91E8-2849756DE659}", "Sine" },
    { "{ECE0331E-DD96-446E-9CCA-5B89EE949EB4}", "Saw" },
    { "{E552CDF9-62A8-4EE2-84E2-8D7170D15919}", "Pulse" } };
  auto selectType = [](auto& module) { return &module.block.type; };
  type.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectType);
  type.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectType);

  std::vector<std::string> notes = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  auto& note = result->params[(int)FFOsciParam::Note];
  note.acc = false;
  note.defaultText = "C4";
  note.name = "Note";
  note.slotCount = 1;
  note.id = "{592BFC17-0E32-428F-B4B0-E0DF39514BF0}";
  note.type = FBParamType::Discrete;
  note.discrete.valueCount = 128;
  note.discrete.toText = [notes](int i) { return notes[i % 12] + std::to_string(i / 12 - 1); };
  auto selectNote = [](auto& module) { return &module.block.note; };
  note.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectNote);
  note.voiceBlockAddr = FFTopoDetailSelectProcAddr(selectModule, selectNote);

  auto& gain = result->params[(int)FFOsciParam::Gain];
  gain.acc = true;
  gain.defaultText = "100";
  gain.name = "Gain";
  gain.slotCount = FFOsciGainCount;
  gain.unit = "%";
  gain.id = "{211E04F8-2925-44BD-AA7C-9E8983F64AD5}";
  gain.type = FBParamType::Linear;
  gain.linear.displayMultiplier = 100.0f;
  auto selectGain = [](auto& module) { return &module.acc.gain; };
  gain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGain);
  gain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGain);

  auto& cent = result->params[(int)FFOsciParam::Cent];
  cent.acc = true;
  cent.defaultText = "0";
  cent.name = "Cent";
  cent.slotCount = 1;
  cent.unit = "Cent";
  cent.id = "{0115E347-874D-48E8-87BC-E63EC4B38DFF}";
  cent.type = FBParamType::Linear;
  cent.linear.min = -1.0f;
  cent.linear.max = 1.0f;
  cent.linear.displayMultiplier = 100.0f;
  auto selectCent = [](auto& module) { return &module.acc.cent; };
  cent.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectCent);
  cent.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectCent);

  auto& pw = result->params[(int)FFOsciParam::PW];
  pw.acc = true;
  pw.defaultText = "50";
  pw.name = "PW";
  pw.slotCount = 1;
  pw.unit = "%";
  pw.id = "{CDB18D21-6C2A-4352-93E1-FCF37EA7D35F}";
  pw.type = FBParamType::Linear;
  pw.linear.displayMultiplier = 100.0f;
  auto selectPW = [](auto& module) { return &module.acc.pw; };
  pw.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectPW);
  pw.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectPW);

  auto& gLFOToGain = result->params[(int)FFOsciParam::GLFOToGain];
  gLFOToGain.acc = true;
  gLFOToGain.name = "GLFO To Gain";
  gLFOToGain.slotCount = 1;
  gLFOToGain.unit = "%";
  gLFOToGain.id = "{5F4BE3D9-EA5F-49D9-B6C5-8FCD0C279B93}";
  gLFOToGain.type = FBParamType::Linear;
  gLFOToGain.linear.displayMultiplier = 100.0f;
  auto selectGLFOToGain = [](auto& module) { return &module.acc.gLFOToGain; };
  gLFOToGain.scalarAddr = FFTopoDetailSelectScalarAddr(selectModule, selectGLFOToGain);
  gLFOToGain.voiceAccAddr = FFTopoDetailSelectProcAddr(selectModule, selectGLFOToGain);
  return result;
}