#include "escorepch.h"
#pragma hdrstop

#include "EsCryptoInternal.h"
#include "EsCryptoHash.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_USE_CRYPTO

//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsCryptoHash, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, fnv1a32, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, fnv1a64, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, md5, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha1, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha224, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha256, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha384, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha512, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha3_224, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha3_256, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha3_384, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, sha3_512, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, ripemd128, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, ripemd160, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, ripemd256, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCryptoHash, ripemd320, EsBinBuffer_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

template <typename HashT>
static EsBinBuffer hashCalculate(const EsVariant& src)
{
  if( src.isEmpty() )
    return EsBinBuffer::null();

  EsBinBuffer digest(
    HashT::DIGESTSIZE
  );

  HashT hash;
  if( src.isCollection() )
  {
    const EsVariant::Array& va = src.asVariantCollection();

#ifdef ES_MODERN_CPP

    for(auto const& v : va)
    {
#else

    size_t cnt = va.size();
    for(size_t idx = 0; idx < cnt; ++idx)
    {
      const EsVariant& v = va[idx];
#endif

      const EsBinBuffer& bb = v.asBinBuffer();

      hash.Update(
        bb.data(),
        bb.size()
      );
    }
  }
  else
  {
    const EsBinBuffer& bb = src.asBinBuffer();

    hash.Update(
      bb.data(),
      bb.size()
    );
  }

  hash.Final(
    digest.data()
  );

  return digest;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

template <typename FnvT, typename RetT>
static RetT fnvCalculate(const EsVariant& src)
{
  FnvT hash;

  if( src.isEmpty() ||
      ((src.isString() || src.isIndexed()) && 0 == src.countGet())
  )
    EsException::Throw(
      esT("Could not use an empty input in EsHashFNV_1a generator")
    );

  if( src.isCollection() )
  {
    const EsVariant::Array& va = src.asVariantCollection();

#ifdef ES_MODERN_CPP
    for(auto const& v : va)
    {
#else

    size_t cnt = va.size();
    for(size_t idx = 0; idx < cnt; ++idx)
    {
      const EsVariant& v = va[idx];

#endif

      if( v.isString() )
        hash.update(
          v.asString()
        );
      else
        hash.update(
          v.asBinBuffer()
        );
    }
  }
  else
  {
    if( src.isString() )
      hash.update(
        src.asString()
      );
    else
      hash.update(
        src.asBinBuffer()
      );
  }

  return static_cast<RetT>(hash.get());
}
//---------------------------------------------------------------------------

EsVariant EsCryptoHash::fnv1a32(const EsVariant& src)
{
  return fnvCalculate<EsHashFNV_1a_32, ulong>(src);
}
//---------------------------------------------------------------------------

EsVariant EsCryptoHash::fnv1a64(const EsVariant& src)
{
  return fnvCalculate<EsHashFNV_1a_64, ullong>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::md5(const EsVariant& src)
{
  return hashCalculate<CryptoPP::Weak::MD5>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha1(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA1>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha224(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA224>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha256(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA256>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha384(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA384>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha512(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA512>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha3_224(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA3_224>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha3_256(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA3_256>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha3_384(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA3_384>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::sha3_512(const EsVariant& src)
{
  return hashCalculate<CryptoPP::SHA3_512>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::ripemd128(const EsVariant& src)
{
  return hashCalculate<CryptoPP::RIPEMD128>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::ripemd160(const EsVariant& src)
{
  return hashCalculate<CryptoPP::RIPEMD160>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::ripemd256(const EsVariant& src)
{
  return hashCalculate<CryptoPP::RIPEMD256>(src);
}
//---------------------------------------------------------------------------

EsBinBuffer EsCryptoHash::ripemd320(const EsVariant& src)
{
  return hashCalculate<CryptoPP::RIPEMD320>(src);
}
//---------------------------------------------------------------------------

#endif // ES_USE_CRYPTO


