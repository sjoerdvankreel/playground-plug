#include <playground_base/dsp/pipeline/shared/FBVoiceManager.hpp>

#include <limits>
#include <cassert>

void 
FBVoiceManager::ReturnOldest(FBNoteEvent const& event)
{
  assert(!event.on);
  
  int slot = -1;
  std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();  
  for (int i = 0; i < _voices.size(); i++)
    if (event.note.id != -1 && 
      event.note.id == _voices[i].event.note.id ||
      event.note.channel == _voices[i].event.note.channel &&
      event.note.key == _voices[i].event.note.channel)
      if (_num[i] < oldest)
      {
        slot = i;
        oldest = _num[i];
      }

  if (slot != -1)
    _voices[slot].active = false;
}

void 
FBVoiceManager::Lease(FBNoteEvent const& event)
{
  assert(event.on);

  int slot = -1;
  for (int i = 0; i < _voices.size() && slot == -1; i++)
    if (!_voices[i].active)
      slot = i;

  std::uint64_t oldest = std::numeric_limits<std::uint64_t>::max();
  if(slot == -1)
    for(int i = 0; i < _voices.size(); i++)
      if (_num[i] < oldest)
      {
        slot = i;
        oldest = _num[i];
      }

  assert(0 <= slot && slot < _voices.size());
  _num[slot] = _counter++;
  _voices[slot].event = event;
  _voices[slot].active = true;
  _voices[slot].initialOffset = event.pos;
}