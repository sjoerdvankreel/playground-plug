#include <firefly_base_vst3/FBVST3Utility.hpp>
#include <firefly_base/base/shared/FBUtility.hpp>

#include <public.sdk/source/vst/utility/stringconvert.h>
#include <cstring>

void
FBVST3CopyToString128(std::string const& in, TChar* out)
{
  memset(out, 0, 128 * sizeof(TChar));
  bool ok = Vst::StringConvert::convert(in, out, 127);
  FB_ASSERT(ok);
}

void
FBVST3CopyFromString128(TChar const* in, std::string& out)
{
  out = Vst::StringConvert::convert(in);
  out = out.substr(0, 127);
}

bool
FBVST3LoadIBStream(IBStream* stream, std::string& state)
{
  state = {};
  int32 read = 0;
  char buffer[1024];
  tresult result = kResultFalse;
  while ((result = stream->read(buffer, sizeof(buffer), &read)) == kResultTrue && read > 0)
    state.append(buffer, read);
  return result == kResultTrue;
}

bool
FBVST3SaveIBStream(IBStream* stream, std::string const& state)
{
  int32 written = 0;
  int32 numWritten = 0;
  tresult result = kResultFalse;
  char* data = const_cast<char*>(state.data());
  while ((result = stream->write(data + written, static_cast<int32>(state.size() - written), &numWritten)) == kResultTrue && numWritten > 0)
  {
    written += numWritten;
    numWritten = 0;
  }
  return result == kResultTrue;
}