#ifndef _es_crypto_internal_h_
#define _es_crypto_internal_h_

#ifdef ES_USE_CRYPTO

#ifndef ENOTSUP
# define ENOTSUP 0x5F
#endif

#include "crypto/cryptlib.h"
#include "crypto/filters.h"
#include "crypto/aes.h"
#include "crypto/des.h"

#include "crypto/sha.h"
#include "crypto/sha3.h"
#include "crypto/ripemd.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "crypto/md5.h"

#endif // #ifdef ES_USE_CRYPTO

#endif // _es_crypto_internal_h_
