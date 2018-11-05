#include "escorepch.h"
#pragma hdrstop

#include "EsCryptoInternal.h"

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
#include "crypto/des.cpp"
#include "crypto/dessp.cpp"
#include "crypto/cpu.cpp"
#include "crypto/rijndael.cpp"
#include "crypto/rdtables.cpp"
#include "crypto/iterhash.cpp"
#include "crypto/sha3.cpp"
#include "crypto/ripemd.cpp"
#include "crypto/md5.cpp"
#include "crypto/algparam.cpp"
#include "crypto/integer.cpp"
#include "crypto/queue.cpp"
#include "crypto/mqueue.cpp"
#include "crypto/nbtheory.cpp"
#include "crypto/asn.cpp"
#include "crypto/pubkey.cpp"
// Place last due to potential MACRO undefies and their propagation
#ifdef H
# undef H
#endif
#ifdef G
# undef G
#endif
#ifdef F
# undef F
#endif
#include "crypto/sha.cpp"

#endif // #ifdef ES_USE_CRYPTO
