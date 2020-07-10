#include "escorepch.h"
#pragma hdrstop

#include "EsCryptoAes.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_USE_CRYPTO

# ifdef ES_USE_CRYPTO_DEBUG_TRACE
#   define ES_CRYPTOAES_DEBUG_TRACE ES_DEBUG_TRACE
# else
#   define ES_CRYPTOAES_DEBUG_TRACE(...) ((void)0)
# endif
//---------------------------------------------------------------------------

#include "crypto/aes.h"
#include "crypto/rdtables.cpp"
#include "crypto/rijndael.cpp"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsCryptoAes, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoAes, encodeReflected, encode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoAes, decodeReflected, decode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

EsCryptoAes::EsCryptoAes(const EsBinBuffer& key)
{
  keySet( key );
}
//---------------------------------------------------------------------------

void EsCryptoAes::keySet(const EsBinBuffer& key)
{
  if( key.empty() )
    EsException::Throw(
      esT("AES Codec key must not be empty")
    );

  if( key.size() < CryptoPP::AES::MIN_KEYLENGTH )
    EsException::Throw(
      esT("AES Codec key is too short, expected length is at least %d bytes"),
      CryptoPP::AES::MIN_KEYLENGTH
    );

  m_key = key;

  // Find next larger appropriate keylength in range CryptoPP::AES::MIN_KEYLENGTH .. CryptoPP::AES::MAX_KEYLENGTH
  size_t sze = key.size();

  sze = CryptoPP::AES::MIN_KEYLENGTH + (
    (sze - CryptoPP::AES::MIN_KEYLENGTH) / 8 +
    (((sze - CryptoPP::AES::MIN_KEYLENGTH) % 8) ?
      1 :
      0
    )
  ) * 8;

  if( sze > CryptoPP::AES::MAX_KEYLENGTH )
    sze = CryptoPP::AES::MAX_KEYLENGTH;

  ES_CRYPTOAES_DEBUG_TRACE(
    esT("AES codec key length %d was selected"),
    sze
  );

  m_keybuf.resize(
    sze,
    0
  );

  memcpy(
    m_keybuf.data(),
    m_key.data(),
    std::min(
      m_keybuf.size(),
      m_key.size()
    )
  );
}
//---------------------------------------------------------------------------

void EsCryptoAes::encode(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  if( src.empty() )
  {
    dest = EsBinBuffer::null();
    return;
  }

  ulong sze = static_cast<ulong>(
    src.size() + sizeof(ulong)
  );

  sze = (
    (sze / CryptoPP::AESEncryption::BLOCKSIZE)
    + (
      (sze % CryptoPP::AESEncryption::BLOCKSIZE) ?
        1 :
        0
      )
    ) * CryptoPP::AESEncryption::BLOCKSIZE;

  dest.resize(
    sze,
    0
  );

  EsBinBuffer::pointer pos = dest.data();
  sze = static_cast<ulong>(src.size()); //< Prepend with original length
  memcpy(
    pos,
    &sze,
    sizeof(ulong)
  );
  pos += sizeof(ulong);

  memcpy(
    pos,
    src.data(),
    src.size()
  );

  pos = dest.data();
  EsBinBuffer::const_pointer end = pos+dest.size();

  CryptoPP::AESEncryption enc(
    m_keybuf.data(),
    m_keybuf.size()
  );

  while( pos < end )
  {
    enc.ProcessBlock(
      pos
    );
    pos += CryptoPP::AESEncryption::BLOCKSIZE;
  }
}
//---------------------------------------------------------------------------

void EsCryptoAes::decode(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  if( src.empty() )
  {
    dest = EsBinBuffer::null();
    return;
  }

  if( src.size() % CryptoPP::AESDecryption::BLOCKSIZE )
    EsException::Throw(
      esT("Invalid AES decoder input data size. Data must be multiple of %d"),
      CryptoPP::AESDecryption::BLOCKSIZE
    );

  dest.resize(
    src.size()
  );

  EsBinBuffer::const_pointer pos = src.data();
  EsBinBuffer::const_pointer end = pos+src.size();
  EsBinBuffer::pointer opos = dest.data();

  CryptoPP::AESDecryption dec(
    m_keybuf.data(),
    m_keybuf.size()
  );

  while( pos < end )
  {
    dec.ProcessBlock(
      pos,
      opos
    );
    pos += CryptoPP::AESDecryption::BLOCKSIZE;
    opos += CryptoPP::AESDecryption::BLOCKSIZE;
  }

  // Extract decrypted length from dest
  ulong sze = 0;
  memcpy(
    &sze,
    dest.data(),
    sizeof(ulong)
  );

  if( sze > src.size() )  //< Not a strict decoding validity check, but will do...
    EsException::Throw(
      esT("Invalid decoded AES data length %d"),
      sze
    );

  // Reposition data to the beginning of the dest
  std::rotate(
    dest.begin(),
    dest.begin()+sizeof(ulong),
    dest.end()
  );

  // Correct dest length
  dest.resize(sze);
}
//---------------------------------------------------------------------------

void EsCryptoAes::process(EsBinBuffer& dest, const EsBinBuffer& src, bool doEncode /*= true*/) const
{
  if( doEncode )
    encode(dest, src);
  else
    decode(dest, src);
}
//---------------------------------------------------------------------------

EsVariant EsCryptoAes::encodeReflected(cr_EsVariant src, cr_EsVariant key)
{
  EsCryptoAes aes( key.asBinBuffer() );

  EsBinBuffer dest;
  aes.encode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

EsVariant EsCryptoAes::decodeReflected(cr_EsVariant src, cr_EsVariant key)
{
  EsCryptoAes aes( key.asBinBuffer() );

  EsBinBuffer dest;
  aes.decode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

#endif // ES_USE_CRYPTO


