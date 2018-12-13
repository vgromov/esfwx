// arc4.cpp - written and placed in the public domain by Wei Dai

// The ARC4 algorithm was first revealed in an anonymous email to the
// cypherpunks mailing list. This file originally contained some
// code copied from this email. The code has since been rewritten in order
// to clarify the copyright status of this file. It should now be
// completely in the public domain.

#include "pch.h"
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include "arc4.h"

NAMESPACE_BEGIN(CryptoPP)
namespace Weak1 {

#if CRYPTOPP_DEBUG && !defined(CRYPTOPP_DOXYGEN_PROCESSING)
void ARC4_TestInstantiations()
{
  ARC4 x;
}
#endif

ARC4_Base::~ARC4_Base()
{
  m_x = m_y = 0;
}

void ARC4_Base::UncheckedSetKey(const CryptoPP::byte *key, unsigned int keyLen, const NameValuePairs &params)
{
  AssertValidKeyLength(keyLen);

  m_x = 1;
  m_y = 0;

  unsigned int i;
  for (i=0; i<256; i++)
    m_state[i] = CryptoPP::byte(i);

  unsigned int keyIndex = 0, stateIndex = 0;
  for (i=0; i<256; i++)
  {
    unsigned int a = m_state[i];
    stateIndex += key[keyIndex] + a;
    stateIndex &= 0xff;
    m_state[i] = m_state[stateIndex];
    m_state[stateIndex] = CryptoPP::byte(a);
    if (++keyIndex >= keyLen)
      keyIndex = 0;
  }

  int discardBytes = params.GetIntValueWithDefault("DiscardBytes", GetDefaultDiscardBytes());
  DiscardBytes(discardBytes);
}

template <class T>
static inline unsigned int MakeByte(T &x, T &y, CryptoPP::byte *s)
{
  unsigned int a = s[x];
  y = CryptoPP::byte((y+a) & 0xff);
  unsigned int b = s[y];
  s[x] = CryptoPP::byte(b);
  s[y] = CryptoPP::byte(a);
  x = CryptoPP::byte((x+1) & 0xff);
  return s[(a+b) & 0xff];
}

void ARC4_Base::GenerateBlock(CryptoPP::byte *output, size_t size)
{
  while (size--)
    *output++ = static_cast<CryptoPP::byte>(MakeByte(m_x, m_y, m_state));
}

void ARC4_Base::ProcessData(CryptoPP::byte *outString, const CryptoPP::byte *inString, size_t length)
{
  if (length == 0)
    return;

  CryptoPP::byte *const s = m_state;
  unsigned int x = m_x;
  unsigned int y = m_y;

  if (inString == outString)
  {
    do
    {
      *outString++ ^= MakeByte(x, y, s);
    } while (--length);
  }
  else
  {
    do
    {
      *outString++ = *inString++ ^ CryptoPP::byte(MakeByte(x, y, s));
    }
    while(--length);
  }

  m_x = CryptoPP::byte(x);
  m_y = CryptoPP::byte(y);
}

void ARC4_Base::DiscardBytes(size_t length)
{
  if (length == 0)
    return;

  CryptoPP::byte *const s = m_state;
  unsigned int x = m_x;
  unsigned int y = m_y;

  do
  {
    MakeByte(x, y, s);
  }
  while(--length);

  m_x = CryptoPP::byte(x);
  m_y = CryptoPP::byte(y);
}

}
NAMESPACE_END
