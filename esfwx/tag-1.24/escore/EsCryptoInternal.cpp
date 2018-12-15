#include "escorepch.h"
#pragma hdrstop

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_USE_CRYPTO

#include "crypto/misc.cpp"
#include "crypto/fips140.cpp"
#include "crypto/cryptlib.cpp"
#include "crypto/filters.cpp"
#include "crypto/cpu.cpp"
#include "crypto/sse-simd.cpp"
#include "crypto/osrng.cpp"
#include "crypto/rng.cpp"

#endif // #ifdef ES_USE_CRYPTO
