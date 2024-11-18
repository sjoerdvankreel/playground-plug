#pragma once
#include <playground_plug/base/shared/FBUtilityMacros.hpp>

// internal block size
// we process in fixed blocks
// hope to have some autovectorizer gains from this!
// however it does introduce latency so keep it small
#define FF_BLOCK_SIZE 32

#define FF_OSCI_COUNT 2
#define FF_SHAPER_COUNT 2

#define FF_VENDOR_NAME "Sjoerd van Kreel"
#define FF_VENDOR_MAIL "sjoerdvankreel@gmail.com"
#define FF_VENDOR_URL "https://github.com/sjoerdvankreel"

#define FF_PLUGIN_NAME "Playground Plug"
#define FF_PLUGIN_PROCESSOR_ID "754068B351A04DB4813B58D562BDFC1F"
#define FF_PLUGIN_CONTROLLER_ID "959E6302402B461A8C9AA5A6737BCAAD"
#define FF_PLUGIN_VERSION FB_STRINGIFY(FF_PLUGIN_VERSION_MAJOR.FF_PLUGIN_VERSION_MINOR.FF_PLUGIN_VERSION_PATCH)