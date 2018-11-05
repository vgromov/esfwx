#include "escorepch.h"
#pragma hdrstop

#include "EsCryptoInternal.h"
#include "EsCryptoDes.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_USE_CRYPTO

//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsCryptoDes, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoDes, encodeReflected, encode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoDes, encodeReflected, encode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoDes, encodeReflected, encode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoDes, decodeReflected, decode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoDes, decodeReflected, decode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsCryptoDes, decodeReflected, decode, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END

//---------------------------------------------------------------------------

EsCryptoDes::EsCryptoDes(const EsBinBuffer& key, const EsBinBuffer& key1 /*= EsBinBuffer::null()*/, const EsBinBuffer& key2 /*= EsBinBuffer::null()*/) :
m_keybuf(
  CryptoPP::DES::KEYLENGTH * 3
)
{
  keySet(key);
  keySet1(key1);
  keySet2(key2);
}
//---------------------------------------------------------------------------

void EsCryptoDes::keybufInit(EsBinBuffer& keybuf, EsBinBuffer& keyvar, const EsBinBuffer& key, int keyIdx)
{
  keyvar = key;
  if( key.empty() )
    return;

  if( key.size() < CryptoPP::DES::KEYLENGTH )
    EsException::Throw(
      esT("DES Codec key is too short, expected length is at least %d bytes"),
      CryptoPP::DES::KEYLENGTH
    );

  int offs = CryptoPP::DES::KEYLENGTH*keyIdx;
  memcpy(
    keybuf.data()+offs,
    key.data(),
    std::min(
      keybuf.size(),
      key.size()
    )
  );

  if( !CryptoPP::DES::CheckKeyParityBits(
      &keybuf[offs]
    )
  )
    CryptoPP::DES::CorrectKeyParityBits(
      &keybuf[offs]
    );
}
//---------------------------------------------------------------------------

void EsCryptoDes::keySet(const EsBinBuffer& key)
{
  if( key.empty() )
    EsException::Throw(
      esT("DES Codec key must not be empty")
    );

  keybufInit(
    m_keybuf,
    m_key,
    key,
    0
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::keySet1(const EsBinBuffer& key)
{
  keybufInit(
    m_keybuf,
    m_key1,
    key,
    1
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::keySet2(const EsBinBuffer& key)
{
  keybufInit(
    m_keybuf,
    m_key2,
    key,
    2
  );
}
//---------------------------------------------------------------------------

template < typename EncObjT >
static void internal_encode(EncObjT& enc, EsBinBuffer& dest, const EsBinBuffer& src)
{
  if( src.empty() )
  {
    dest = EsBinBuffer::null();
    return;
  }

  ulong sze = src.size() + sizeof(ulong);
  sze = (
    (sze / EncObjT::BLOCKSIZE)
    + (
      (sze % EncObjT::BLOCKSIZE) ?
        1 :
        0
      )
    ) * EncObjT::BLOCKSIZE;

  dest.resize(
    sze,
    0
  );

  EsBinBuffer::pointer pos = dest.data();
  sze = src.size(); //< Prepend with original length
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
  while( pos < end )
  {
    enc.ProcessBlock(
      pos
    );
    pos += EncObjT::BLOCKSIZE;
  }
}
//---------------------------------------------------------------------------

void EsCryptoDes::encode1(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  CryptoPP::DESEncryption enc(
    m_keybuf.data(),
    CryptoPP::DES::KEYLENGTH
  );

  internal_encode<CryptoPP::DESEncryption>(
    enc,
    dest,
    src
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::encode2(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  CryptoPP::DES_EDE2_Encryption enc(
    m_keybuf.data(),
    CryptoPP::DES::KEYLENGTH*2
  );

  internal_encode<CryptoPP::DES_EDE2_Encryption>(
    enc,
    dest,
    src
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::encode3(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  CryptoPP::DES_EDE3_Encryption enc(
    m_keybuf.data(),
    CryptoPP::DES::KEYLENGTH*3
  );

  internal_encode<CryptoPP::DES_EDE3_Encryption>(
    enc,
    dest,
    src
  );
}
//---------------------------------------------------------------------------

template < typename DecObjT >
static void internal_decode(DecObjT& dec, EsBinBuffer& dest, const EsBinBuffer& src)
{
  if( src.empty() )
  {
    dest = EsBinBuffer::null();
    return;
  }

  if( src.size() % DecObjT::BLOCKSIZE )
    EsException::Throw(
      esT("Invalid DES decoder input data size. Data must be multiple of %d"),
      DecObjT::BLOCKSIZE
    );

  dest.resize(
    src.size()
  );

  EsBinBuffer::const_pointer pos = src.data();
  EsBinBuffer::const_pointer end = pos+src.size();
  EsBinBuffer::pointer opos = dest.data();

  while( pos < end )
  {
    dec.ProcessBlock(
      pos,
      opos
    );
    pos += DecObjT::BLOCKSIZE;
    opos += DecObjT::BLOCKSIZE;
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
      esT("Invalid decoded DES data length %d"),
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

void EsCryptoDes::decode1(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  CryptoPP::DESDecryption dec(
    m_keybuf.data(),
    CryptoPP::DES::KEYLENGTH
  );

  internal_decode<CryptoPP::DESDecryption>(
    dec,
    dest,
    src
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::decode2(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  CryptoPP::DES_EDE2_Decryption dec(
    m_keybuf.data(),
    CryptoPP::DES::KEYLENGTH*2
  );

  internal_decode<CryptoPP::DES_EDE2_Decryption>(
    dec,
    dest,
    src
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::decode3(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  CryptoPP::DES_EDE3_Decryption dec(
    m_keybuf.data(),
    CryptoPP::DES::KEYLENGTH*3
  );

  internal_decode<CryptoPP::DES_EDE3_Decryption>(
    dec,
    dest,
    src
  );
}
//---------------------------------------------------------------------------

void EsCryptoDes::encode(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  if( !m_key1.empty() && !m_key2.empty() )
  {
    ES_DEBUG_TRACE(
      esT("DES EDE3 encoder selected for processing")
    );

    encode3(
      dest,
      src
    );
  }
  else if( !m_key1.empty() )
  {
    ES_DEBUG_TRACE(
      esT("DES EDE2 encoder selected for processing")
    );

    encode2(
      dest,
      src
    );
  }
  else
  {
    ES_DEBUG_TRACE(
      esT("DES encoder selected for processing")
    );

    encode1(
      dest,
      src
    );
  }
}
//---------------------------------------------------------------------------

void EsCryptoDes::decode(EsBinBuffer& dest, const EsBinBuffer& src) const
{
  if( !m_key1.empty() && !m_key2.empty() )
  {
    ES_DEBUG_TRACE(
      esT("DES EDE3 decoder selected for processing")
    );

    decode3(
      dest,
      src
    );
  }
  else if( !m_key1.empty() )
  {
    ES_DEBUG_TRACE(
      esT("DES EDE2 decoder selected for processing")
    );

    decode2(
      dest,
      src
    );
  }
  else
  {
    ES_DEBUG_TRACE(
      esT("DES decoder selected for processing")
    );

    decode1(
      dest,
      src
    );
  }
}
//---------------------------------------------------------------------------

void EsCryptoDes::process(EsBinBuffer& dest, const EsBinBuffer& src, bool doEncode /*= true*/) const
{
  if( doEncode )
    encode(
      dest,
      src
    );
  else
    decode(
      dest,
      src
    );
}
//---------------------------------------------------------------------------

EsVariant EsCryptoDes::encodeReflected(cr_EsVariant src, cr_EsVariant key)
{
  EsCryptoDes codec(
    key.asBinBuffer()
  );

  EsBinBuffer dest;
  codec.encode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

EsVariant EsCryptoDes::encodeReflected(cr_EsVariant src, cr_EsVariant key1, cr_EsVariant key2)
{
  EsCryptoDes codec(
    key1.asBinBuffer(),
    key2.asBinBuffer()
  );

  EsBinBuffer dest;
  codec.encode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

EsVariant EsCryptoDes::encodeReflected(cr_EsVariant src, cr_EsVariant key1, cr_EsVariant key2, cr_EsVariant key3)
{
  EsCryptoDes codec(
    key1.asBinBuffer(),
    key2.asBinBuffer(),
    key3.asBinBuffer()
  );

  EsBinBuffer dest;
  codec.encode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

EsVariant EsCryptoDes::decodeReflected(cr_EsVariant src, cr_EsVariant key)
{
  EsCryptoDes codec(
    key.asBinBuffer()
  );

  EsBinBuffer dest;
  codec.decode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

EsVariant EsCryptoDes::decodeReflected(cr_EsVariant src, cr_EsVariant key1, cr_EsVariant key2)
{
  EsCryptoDes codec(
    key1.asBinBuffer(),
    key2.asBinBuffer()
  );

  EsBinBuffer dest;
  codec.decode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

EsVariant EsCryptoDes::decodeReflected(cr_EsVariant src, cr_EsVariant key1, cr_EsVariant key2, cr_EsVariant key3)
{
  EsCryptoDes codec(
    key1.asBinBuffer(),
    key2.asBinBuffer(),
    key3.asBinBuffer()
  );

  EsBinBuffer dest;
  codec.decode(
    dest,
    src.asBinBuffer()
  );

  return dest;
}
//---------------------------------------------------------------------------

#endif // ES_USE_CRYPTO

