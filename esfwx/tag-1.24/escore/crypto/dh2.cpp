// dh2.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "dh2.h"

NAMESPACE_BEGIN(CryptoPP)

#if CRYPTOPP_DEBUG && !defined(CRYPTOPP_DOXYGEN_PROCESSING)
void DH2_TestInstantiations()
{
  DH2 dh(*(SimpleKeyAgreementDomain*)NULL);
}
#endif

bool DH2::Agree(CryptoPP::byte *agreedValue,
    const CryptoPP::byte *staticSecretKey, const CryptoPP::byte *ephemeralSecretKey,
    const CryptoPP::byte *staticOtherPublicKey, const CryptoPP::byte *ephemeralOtherPublicKey,
    bool validateStaticOtherPublicKey) const
{
  return d1.Agree(agreedValue, staticSecretKey, staticOtherPublicKey, validateStaticOtherPublicKey)
    && d2.Agree(agreedValue+d1.AgreedValueLength(), ephemeralSecretKey, ephemeralOtherPublicKey, true);
}

NAMESPACE_END
