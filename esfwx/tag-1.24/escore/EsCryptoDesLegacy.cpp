#include "escorepch.h"
#pragma hdrstop

#include "EsCryptoDesLegacy.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if defined(ES_USE_CRYPTO_LEGACY)
#include <limits>

const int KEY_SHIFT[EsCryptoDesLegacy::ROUNDS] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

const esU8 COMPRESSION_PERMUTATION[EsCryptoDesLegacy::SBUFFER_SIZE] =
{
  14,17,11,24, 1, 5, 3,28,15, 6,21,10,
  23,19,12, 4,26, 8,16, 7,27,20,13, 2,
  41,52,31,37,47,55,30,40,51,45,33,48,
  44,49,39,56,34,53,46,42,50,36,29,32
};

const esU8 EXPANSION_PERMUTATION[EsCryptoDesLegacy::SBUFFER_SIZE] =
{
  32, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 9,
  8, 9,10,11,12,13,12,13,14,15,16,17,
  16,17,18,19,20,21,20,21,22,23,24,25,
  24,25,26,27,28,29,28,29,30,31,32, 1
};

/*
S-Boxes:

14  4 13  1  2 15 11  8  3 10  6 12  5  9  0  7 
0 15  7  4 14  2 13  1 10  6 12 11  9  5  3  8 
4  1 14  8 13  6  2 11 15 12  9  7  3 10  5  0 
15 12  8  2  4  9  1  7  5 11  3 14 10  0  6 13

15  1  8 14  6 11  3  4  9  7  2 13 12  0  5 10 
3 13  4  7 15  2  8 14 12  0  1 10  6  9 11  5 
0 14  7 11 10  4 13  1  5  8 12  6  9  3  2 15 
13  8 10  1  3 15  4  2 11  6  7 12  0  5 14  9 

10  0  9 14  6  3 15  5  1 13 12  7 11  4  2  8 
13  7  0  9  3  4  6 10  2  8  5 14 12 11 15  1 
13  6  4  9  8 15  3  0 11  1  2 12  5 10 14  7 
1 10 13  0  6  9  8  7  4 15 14  3 11  5  2 12 

7 13 14  3  0  6  9 10  1  2  8  5 11 12  4 15
13  8 11  5  6 15  0  3  4  7  2 12  1 10 14  9
10  6  9  0 12 11  7 13 15  1  3 14  5  2  8  4
3 15  0  6 10  1 13  8  9  4  5 11 12  7  2 14

2 12  4  1  7 10 11  6  8  5  3 15 13  0 14  9
14 11  2 12  4  7 13  1  5  0 15 10  3  9  8  6
4  2  1 11 10 13  7  8 15  9 12  5  6  3  0 14
11  8 12  7  1 14  2 13  6 15  0  9 10  4  5  3

12  1 10 15  9  2  6  8  0 13  3  4 14  7  5 11
10 15  4  2  7 12  9  5  6  1 13 14  0 11  3  8
9 14 15  5  2  8 12  3  7  0  4 10  1 13 11  6
4  3  2 12  9  5 15 10 11 14  1  7  6  0  8 13

4 11  2 14 15  0  8 13  3 12  9  7  5 10  6  1
13  0 11  7  4  9  1 10 14  3  5 12  2 15  8  6
1  4 11 13 12  3  7 14 10 15  6  8  0  5  9  2
6 11 13  8  1  4 10  7  9  5  0 15 14  2  3 12

13  2  8  4  6 15 11  1 10  9  3 14  5  0 12  7
1 15 13  8 10  3  7  4 12  5  6 11  0 14  9  2 
7 11  4  1  9 12 14  2  0  6 10 13 15  3  5  8 
2  1 14  7  4 10  8 13 15 12  9  0  3  5  6 11 

*/

// The following are the above S-boxes, packed one bit per byte:

const esU8 MODIFIED_SBOX_1[4*64] =
{
  1,1,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,0,1,0,1,1,1,1,1,0,1,1,1,0,0,0,
  0,0,1,1,1,0,1,0,0,1,1,0,1,1,0,0,0,1,0,1,1,0,0,1,0,0,0,0,0,1,1,1,
  0,0,0,0,1,1,1,1,0,1,1,1,0,1,0,0,1,1,1,0,0,0,1,0,1,1,0,1,0,0,0,1,
  1,0,1,0,0,1,1,0,1,1,0,0,1,0,1,1,1,0,0,1,0,1,0,1,0,0,1,1,1,0,0,0,
  0,1,0,0,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,1,0,1,1,0,0,0,1,0,1,0,1,1,
  1,1,1,1,1,1,0,0,1,0,0,1,0,1,1,1,0,0,1,1,1,0,1,0,0,1,0,1,0,0,0,0,
  1,1,1,1,1,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,1,0,1,1,1,
  0,1,0,1,1,0,1,1,0,0,1,1,1,1,1,0,1,0,1,0,0,0,0,0,0,1,1,0,1,1,0,1
};

const esU8 MODIFIED_SBOX_2[4*64] =
{
  1,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,1,1,0,0,1,1,0,1,0,0,
  1,0,0,1,0,1,1,1,0,0,1,0,1,1,0,1,1,1,0,0,0,0,0,0,0,1,0,1,1,0,1,0,
  0,0,1,1,1,1,0,1,0,1,0,0,0,1,1,1,1,1,1,1,0,0,1,0,1,0,0,0,1,1,1,0,
  1,1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,0,1,1,0,1,0,1,
  0,0,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,0,1,0,0,1,0,0,1,1,0,1,0,0,0,1,
  0,1,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,0,0,1,0,0,1,1,0,0,1,0,1,1,1,1,
  1,1,0,1,1,0,0,0,1,0,1,0,0,0,0,1,0,0,1,1,1,1,1,1,0,1,0,0,0,0,1,0,
  1,0,1,1,0,1,1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,1,1,1,1,0,1,0,0,1
};

const esU8 MODIFIED_SBOX_3[4*64] =
{
  1,0,1,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,1,0,0,0,1,1,1,1,1,1,0,1,0,1,
  0,0,0,1,1,1,0,1,1,1,0,0,0,1,1,1,1,0,1,1,0,1,0,0,0,0,1,0,1,0,0,0,
  1,1,0,1,0,1,1,1,0,0,0,0,1,0,0,1,0,0,1,1,0,1,0,0,0,1,1,0,1,0,1,0,
  0,0,1,0,1,0,0,0,0,1,0,1,1,1,1,0,1,1,0,0,1,0,1,1,1,1,1,1,0,0,0,1,
  1,1,0,1,0,1,1,0,0,1,0,0,1,0,0,1,1,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0,
  1,0,1,1,0,0,0,1,0,0,1,0,1,1,0,0,0,1,0,1,1,0,1,0,1,1,1,0,0,1,1,1,
  0,0,0,1,1,0,1,0,1,1,0,1,0,0,0,0,0,1,1,0,1,0,0,1,1,0,0,0,0,1,1,1,
  0,1,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,0,1,1,0,1,0,1,0,0,1,0,1,1,0,0
};

const esU8 MODIFIED_SBOX_4[4*64] =
{
  0,1,1,1,1,1,0,1,1,1,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,0,0,1,1,0,1,0,
  0,0,0,1,0,0,1,0,1,0,0,0,0,1,0,1,1,0,1,1,1,1,0,0,0,1,0,0,1,1,1,1,
  1,1,0,1,1,0,0,0,1,0,1,1,0,1,0,1,0,1,1,0,1,1,1,1,0,0,0,0,0,0,1,1,
  0,1,0,0,0,1,1,1,0,0,1,0,1,1,0,0,0,0,0,1,1,0,1,0,1,1,1,0,1,0,0,1,
  1,0,1,0,0,1,1,0,1,0,0,1,0,0,0,0,1,1,0,0,1,0,1,1,0,1,1,1,1,1,0,1,
  1,1,1,1,0,0,0,1,0,0,1,1,1,1,1,0,0,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,
  0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,0,1,0,1,0,0,0,0,1,1,1,0,1,1,0,0,0,
  1,0,0,1,0,1,0,0,0,1,0,1,1,0,1,1,1,1,0,0,0,1,1,1,0,0,1,0,1,1,1,0
};

const esU8 MODIFIED_SBOX_5[4*64] =
{
  0,0,1,0,1,1,0,0,0,1,0,0,0,0,0,1,0,1,1,1,1,0,1,0,1,0,1,1,0,1,1,0,
  1,0,0,0,0,1,0,1,0,0,1,1,1,1,1,1,1,1,0,1,0,0,0,0,1,1,1,0,1,0,0,1,
  1,1,1,0,1,0,1,1,0,0,1,0,1,1,0,0,0,1,0,0,0,1,1,1,1,1,0,1,0,0,0,1,
  0,1,0,1,0,0,0,0,1,1,1,1,1,0,1,0,0,0,1,1,1,0,0,1,1,0,0,0,0,1,1,0,
  0,1,0,0,0,0,1,0,0,0,0,1,1,0,1,1,1,0,1,0,1,1,0,1,0,1,1,1,1,0,0,0,
  1,1,1,1,1,0,0,1,1,1,0,0,0,1,0,1,0,1,1,0,0,0,1,1,0,0,0,0,1,1,1,0,
  1,0,1,1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,1,0,1,1,0,1,
  0,1,1,0,1,1,1,1,0,0,0,0,1,0,0,1,1,0,1,0,0,1,0,0,0,1,0,1,0,0,1,1
};

const esU8 MODIFIED_SBOX_6[4*64] =
{
  1,1,0,0,0,0,0,1,1,0,1,0,1,1,1,1,1,0,0,1,0,0,1,0,0,1,1,0,1,0,0,0,
  0,0,0,0,1,1,0,1,0,0,1,1,0,1,0,0,1,1,1,0,0,1,1,1,0,1,0,1,1,0,1,1,
  1,0,1,0,1,1,1,1,0,1,0,0,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,1,0,1,0,1,
  0,1,1,0,0,0,0,1,1,1,0,1,1,1,1,0,0,0,0,0,1,0,1,1,0,0,1,1,1,0,0,0,
  1,0,0,1,1,1,1,0,1,1,1,1,0,1,0,1,0,0,1,0,1,0,0,0,1,1,0,0,0,0,1,1,
  0,1,1,1,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,1,1,0,1,1,0,1,1,0,1,1,0,
  0,1,0,0,0,0,1,1,0,0,1,0,1,1,0,0,1,0,0,1,0,1,0,1,1,1,1,1,1,0,1,0,
  1,0,1,1,1,1,1,0,0,0,0,1,0,1,1,1,0,1,1,0,0,0,0,0,1,0,0,0,1,1,0,1
};

const esU8 MODIFIED_SBOX_7[4*64] =
{
  0,1,0,0,1,0,1,1,0,0,1,0,1,1,1,0,1,1,1,1,0,0,0,0,1,0,0,0,1,1,0,1,
  0,0,1,1,1,1,0,0,1,0,0,1,0,1,1,1,0,1,0,1,1,0,1,0,0,1,1,0,0,0,0,1,
  1,1,0,1,0,0,0,0,1,0,1,1,0,1,1,1,0,1,0,0,1,0,0,1,0,0,0,1,1,0,1,0,
  1,1,1,0,0,0,1,1,0,1,0,1,1,1,0,0,0,0,1,0,1,1,1,1,1,0,0,0,0,1,1,0,
  0,0,0,1,0,1,0,0,1,0,1,1,1,1,0,1,1,1,0,0,0,0,1,1,0,1,1,1,1,1,1,0,
  1,0,1,0,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0,0,0,1,0,1,1,0,0,1,0,0,1,0,
  0,1,1,0,1,0,1,1,1,1,0,1,1,0,0,0,0,0,0,1,0,1,0,0,1,0,1,0,0,1,1,1,
  1,0,0,1,0,1,0,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,1,1,1,1,0,0
};

const esU8 MODIFIED_SBOX_8[4*64] =
{
  1,1,0,1,0,0,1,0,1,0,0,0,0,1,0,0,0,1,1,0,1,1,1,1,1,0,1,1,0,0,0,1,
  1,0,1,0,1,0,0,1,0,0,1,1,1,1,1,0,0,1,0,1,0,0,0,0,1,1,0,0,0,1,1,1,
  0,0,0,1,1,1,1,1,1,1,0,1,1,0,0,0,1,0,1,0,0,0,1,1,0,1,1,1,0,1,0,0,
  1,1,0,0,0,1,0,1,0,1,1,0,1,0,1,1,0,0,0,0,1,1,1,0,1,0,0,1,0,0,1,0,
  0,1,1,1,1,0,1,1,0,1,0,0,0,0,0,1,1,0,0,1,1,1,0,0,1,1,1,0,0,0,1,0,
  0,0,0,0,0,1,1,0,1,0,1,0,1,1,0,1,1,1,1,1,0,0,1,1,0,1,0,1,1,0,0,0,
  0,0,1,0,0,0,0,1,1,1,1,0,0,1,1,1,0,1,0,0,1,0,1,0,1,0,0,0,1,1,0,1,
  1,1,1,1,1,1,0,0,1,0,0,1,0,0,0,0,0,0,1,1,0,1,0,1,0,1,1,0,1,0,1,1
};

const esU8 PBOX_PERMUTATION[32] =
{
  16, 7,20,21,29,12,28,17, 1,15,23,26, 5,18,31,10,
  2, 8,24,14,32,27, 3, 9,19,13,30, 6,22,11, 4,25
};

static void shift_half_key_left(esU8 *k) ES_NOTHROW
{
  esU8 x = *k;
  esU8 *kmax = k + (EsCryptoDesLegacy::KEY_SIZE/2-1);
  do k[0] = k[1]; while (++k < kmax);
  *k = x;
}

/*-----------------------------------------------------------------------------
This function performs the required shifts and precomputes the 16 shifted
and compressed keys.
-----------------------------------------------------------------------------*/

void EsCryptoDesLegacy::initialize(const esU8 key[EsCryptoDesLegacy::KEY_SIZE]) ES_NOTHROW
{
  esU8 shifted_key[EsCryptoDesLegacy::KEY_SIZE];
  memcpy(shifted_key, key, EsCryptoDesLegacy::KEY_SIZE);
  for (int round = 0; round < EsCryptoDesLegacy::ROUNDS; round++)
  {
    shift_half_key_left(shifted_key);
    shift_half_key_left(shifted_key+EsCryptoDesLegacy::KEY_SIZE/2);
    if (KEY_SHIFT[round] == 2)
    {
      shift_half_key_left(shifted_key);
      shift_half_key_left(shifted_key+EsCryptoDesLegacy::KEY_SIZE/2);
    }
    for (int i = 0; i < EsCryptoDesLegacy::SBUFFER_SIZE; i++)
    {
      m_compressed_shifted_key[round][i] =
        shifted_key[COMPRESSION_PERMUTATION[i]-1];
    }
  }
}

/*-----------------------------------------------------------------------------
This function performs a EsCryptoDesLegacy encryption or decryption on a single 64-bit block
of data, using the specified 56-bit key. The data and key are packed one bit
per byte. Every element of data[] must be either 0 or 1. The results
will be highly anomalous if this is not the case.

The encrypted or decrypted data is returned to the data[] buffer.
-----------------------------------------------------------------------------*/

void EsCryptoDesLegacy::encrypt_decrypt(esU8 data[EsCryptoDesLegacy::DATA_SIZE], bool encrypt) ES_NOTHROW
{
  esU8 sbuffer[EsCryptoDesLegacy::SBUFFER_SIZE];
  union pbuffer_tag
  {
    esU8 byte[EsCryptoDesLegacy::DATA_SIZE/2];
    esU32 dword[EsCryptoDesLegacy::DATA_SIZE/8];
  } pbuffer;

  for (int round = 0; round < EsCryptoDesLegacy::ROUNDS; round++)
  {
    /* XOR compressed key with expanded right half of data */
    {
      int i;
      for (i = 0; i < EsCryptoDesLegacy::SBUFFER_SIZE; i++)
      {
        sbuffer[i] = data[EsCryptoDesLegacy::DATA_SIZE/2 + EXPANSION_PERMUTATION[i]-1] ^
          m_compressed_shifted_key[encrypt ? round : 15-round][i];
      }
    }
    /* S-Box substitutions */
    {
#define index(n) ((sbuffer[(n)] << (5+2)) | (sbuffer[(n)+5] << (4+2)) | \
  (sbuffer[(n)+1] << (3+2)) | (sbuffer[(n)+2] << (2+2)) | (sbuffer[(n)+3] << (1+2)) | \
  (sbuffer[(n)+4] << (0+2)))
      pbuffer.dword[0] = * (esU32 *) (MODIFIED_SBOX_1 + index(0));
      pbuffer.dword[1] = * (esU32 *) (MODIFIED_SBOX_2 + index(6));
      pbuffer.dword[2] = * (esU32 *) (MODIFIED_SBOX_3 + index(12));
      pbuffer.dword[3] = * (esU32 *) (MODIFIED_SBOX_4 + index(18));
      pbuffer.dword[4] = * (esU32 *) (MODIFIED_SBOX_5 + index(24));
      pbuffer.dword[5] = * (esU32 *) (MODIFIED_SBOX_6 + index(30));
      pbuffer.dword[6] = * (esU32 *) (MODIFIED_SBOX_7 + index(36));
      pbuffer.dword[7] = * (esU32 *) (MODIFIED_SBOX_8 + index(42));
    }

    /* XOR and swap */
    if (round < 15)
    {
      int i;
      for (i = 0; i < EsCryptoDesLegacy::DATA_SIZE/2; i++)
      {
        esU8 x = data[EsCryptoDesLegacy::DATA_SIZE/2 + i];
        data[EsCryptoDesLegacy::DATA_SIZE/2 + i] = data[i] ^ pbuffer.byte[PBOX_PERMUTATION[i]-1];
        data[i] = x;
      }
    }
    else
    {
      int i;
      for (i = 0; i < EsCryptoDesLegacy::DATA_SIZE/2; i++)
        data[i] = data[i] ^ pbuffer.byte[PBOX_PERMUTATION[i]-1];
    }
  }
}

/*----------------------------------------------------------------------------
This function takes 64 bits (8 bytes) of a password, XOR's it into a buffer,
and EsCryptoDesLegacy encrypts the buffer contents with the specified key. It then returns a
pointer to the next character of the password.
----------------------------------------------------------------------------*/

const char* EsCryptoDesLegacy::xmix(const char *password, esU8 buffer[EsCryptoDesLegacy::DATA_SIZE], const esU8 key[EsCryptoDesLegacy::KEY_SIZE]) ES_NOTHROW
{
  int i;
  for (i = 0; i < 64; i+=8)
  {
    int c = *password;
    if (c != 0) password++;
    buffer[i] ^= c >> 7 & 1;
    buffer[i+1] ^= c >> 6 & 1;
    buffer[i+2] ^= c >> 5 & 1;
    buffer[i+3] ^= c >> 4 & 1;
    buffer[i+4] ^= c >> 3 & 1;
    buffer[i+5] ^= c >> 2 & 1;
    buffer[i+6] ^= c >> 1 & 1;
    buffer[i+7] ^= c & 1;
  }
  initialize(key);
  encrypt_decrypt(buffer, true);
  return password;
}

static const esU8 MIXER1[EsCryptoDesLegacy::KEY_SIZE] =
{
  1,0,1,0,0,0,0,0,0,1,
  0,1,0,0,1,1,0,1,0,0,
  1,1,0,1,1,1,0,1,1,0,
  0,0,0,0,1,0,0,1,0,1,
  1,1,0,0,0,0,1,1,1,0,
  0,1,0,1,1,0
};
static const esU8 MIXER2[EsCryptoDesLegacy::KEY_SIZE] =
{
  1,0,1,0,1,1,0,0,0,0,
  0,1,1,1,1,0,1,0,0,1,
  1,1,1,0,1,0,0,0,0,0,
  0,1,1,0,1,0,1,0,0,0,
  1,1,0,0,0,0,1,1,0,0,
  0,0,0,0,0,0
};
static const esU8 MIXER3[EsCryptoDesLegacy::KEY_SIZE] =
{
  1,0,0,0,1,1,1,1,1,0,
  1,0,0,0,1,1,0,0,1,0,
  0,1,0,1,1,0,0,0,0,0,
  0,0,1,1,1,0,0,0,1,0,
  1,0,0,0,1,0,0,0,1,1,
  1,1,1,1,0,0
};
static const esU8 MIXER4[EsCryptoDesLegacy::KEY_SIZE] =
{
  0,1,1,0,1,0,0,0,0,1,
  0,1,0,0,0,1,1,0,0,1,
  0,1,0,1,1,1,0,0,0,1,
  0,0,1,1,1,0,1,1,0,1,
  1,1,1,1,1,0,0,0,1,0,
  0,1,1,1,0,0
};

/*----------------------------------------------------------------------------
This function hashes a password and turns it into a key. Then it initializes
the key.
----------------------------------------------------------------------------*/

void EsCryptoDesLegacy::setKey(const EsString& key)
{
  esU8 buffer[EsCryptoDesLegacy::DATA_SIZE];
  memset(buffer, 0, sizeof(buffer));
  const EsByteString& bs = EsString::toUtf8(key);

  if( bs.size() < 32 )
    EsException::Throw( esT("Codec key is too short") );

  const char* p = xmix(bs.c_str(), buffer, MIXER1);
  p = xmix(p, buffer, MIXER2);
  p = xmix(p, buffer, MIXER3);
  xmix(p, buffer, MIXER4);

  initialize(buffer);
}
//---------------------------------------------------------------------------

void EsCryptoDesLegacy::process(EsBinBuffer& dest, const EsBinBuffer& src, bool encode /*= true*/)
{
  // prepend -in with the original block length on encoding
  // otherwise, we have no information on original block length
  // codec will always align block length to nearest multiple of 8
  EsBinBuffer tmp;
  esU32 orgLen = 0;
  if(encode)
  {
    orgLen = static_cast<esU32>(src.size());
    // limit maximum processing block with maximal number of bytes esU32 may address
    ES_ASSERT(orgLen <= static_cast<esU32>(0xFFFFFFFF));
    tmp.assign(
      reinterpret_cast<EsBinBuffer::const_pointer>(&orgLen),
      reinterpret_cast<EsBinBuffer::const_pointer>(&orgLen) + sizeof(orgLen)
    );
  }
  EsBinBuffer in = tmp;
  in += src;

  //8 byte - align input buffer, padding it with 0
  size_t sze = in.size();
  size_t iMod = sze % 8;
  if( iMod )
    in.resize( sze + 8 - iMod, 0 );
  sze = in.size();

  //prepare buff out
  dest.clear();
  //buff-out length is buff-in length + 8 byte - aligned
  dest.reserve( sze );

  //encode input password & compare
  EsBinBuffer::value_type block[EsCryptoDesLegacy::DATA_SIZE];

  size_t inIdx = 0;
  while( inIdx < sze )
  {
    //encrypt by 8 byte blocks
    size_t blockIdx = 0;
    size_t byteCnt = 8;
    while( byteCnt )
    {
      EsBinBuffer::value_type item = in[inIdx++];
      block[blockIdx++] = item >> 7 & 1;
      block[blockIdx++] = item >> 6 & 1;
      block[blockIdx++] = item >> 5 & 1;
      block[blockIdx++] = item >> 4 & 1;
      block[blockIdx++] = item >> 3 & 1;
      block[blockIdx++] = item >> 2 & 1;
      block[blockIdx++] = item >> 1 & 1;
      block[blockIdx++] = item & 1;

      --byteCnt;
    }

    encrypt_decrypt( block, encode );

    for(blockIdx = 0; blockIdx < EsCryptoDesLegacy::DATA_SIZE; blockIdx += 8)
    {
      EsBinBuffer::value_type item =
        block[blockIdx]    << 7 |
        block[blockIdx+1] << 6 |
        block[blockIdx+2] << 5 |
        block[blockIdx+3] << 4 |
        block[blockIdx+4] << 3 |
        block[blockIdx+5] << 2 |
        block[blockIdx+6] << 1 |
        block[blockIdx+7];

      dest.push_back(item);
    }
  }

  if( !encode && !dest.empty() )
  {
    ES_ASSERT(dest.size() >= sizeof(esU32));

    // extract length from decoded block and
    // trim output to its original length
    memcpy(
      reinterpret_cast<EsBinBuffer::pointer>(&orgLen),
      dest.data(),
      sizeof(orgLen)
    );

    if( orgLen < dest.size() )
    {
      std::rotate(dest.begin(), dest.begin()+sizeof(orgLen), dest.end());
      dest.resize(orgLen);
    }
    else
      EsException::Throw(esT("Invalid decoded block length encountered"));
  }
}
//---------------------------------------------------------------------------

#endif // ES_USE_CRYPTO_LEGACY

