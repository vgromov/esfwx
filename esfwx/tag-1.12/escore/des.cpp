#include <stdafx.h>
#pragma hdrstop

#include "des.h"

const int KEY_SHIFT[DES::ROUNDS] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

const BYTE COMPRESSION_PERMUTATION[DES::SBUFFER_SIZE] =
{
	14,17,11,24, 1, 5, 3,28,15, 6,21,10,
	23,19,12, 4,26, 8,16, 7,27,20,13, 2,
	41,52,31,37,47,55,30,40,51,45,33,48,
	44,49,39,56,34,53,46,42,50,36,29,32
};

const BYTE EXPANSION_PERMUTATION[DES::SBUFFER_SIZE] =
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

const BYTE MODIFIED_SBOX_1[4*64] =
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

const BYTE MODIFIED_SBOX_2[4*64] =
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

const BYTE MODIFIED_SBOX_3[4*64] =
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

const BYTE MODIFIED_SBOX_4[4*64] =
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

const BYTE MODIFIED_SBOX_5[4*64] =
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

const BYTE MODIFIED_SBOX_6[4*64] =
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

const BYTE MODIFIED_SBOX_7[4*64] =
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

const BYTE MODIFIED_SBOX_8[4*64] =
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

const BYTE PBOX_PERMUTATION[32] =
{
	16, 7,20,21,29,12,28,17, 1,15,23,26, 5,18,31,10,
	2, 8,24,14,32,27, 3, 9,19,13,30, 6,22,11, 4,25
};

static void shift_half_key_left(BYTE *k)
{
	BYTE x = *k;
	BYTE *kmax = k + (DES::KEY_SIZE/2-1);
	do k[0] = k[1]; while (++k < kmax);
	*k = x;
}

/*-----------------------------------------------------------------------------
This function performs the required shifts and precomputes the 16 shifted
and compressed keys.
-----------------------------------------------------------------------------*/

void DES::initialize(const BYTE key[DES::KEY_SIZE])
{
	BYTE shifted_key[DES::KEY_SIZE];
	memcpy(shifted_key, key, DES::KEY_SIZE);
	for (int round = 0; round < DES::ROUNDS; round++)
	{
		shift_half_key_left(shifted_key);
		shift_half_key_left(shifted_key+DES::KEY_SIZE/2);
		if (KEY_SHIFT[round] == 2)
		{
			shift_half_key_left(shifted_key);
			shift_half_key_left(shifted_key+DES::KEY_SIZE/2);
		}
		for (int i = 0; i < DES::SBUFFER_SIZE; i++)
		{
			compressed_shifted_key[round][i] =
				shifted_key[COMPRESSION_PERMUTATION[i]-1];
		}
	}
}

/*-----------------------------------------------------------------------------
This function performs a DES encryption or decryption on a single 64-bit block
of data, using the specified 56-bit key. The data and key are packed one bit
per byte. Every element of data[] must be either 0 or 1. The results
will be highly anomalous if this is not the case.

The encrypted or decrypted data is returned to the data[] buffer.
-----------------------------------------------------------------------------*/

void DES::encrypt_decrypt(BYTE data[DES::DATA_SIZE],
													bool encrypt)
{
	BYTE sbuffer[DES::SBUFFER_SIZE];
	union pbuffer_tag
	{
		BYTE byte[DES::DATA_SIZE/2];
		DWORD dword[DES::DATA_SIZE/8];
	} pbuffer;
	for (int round = 0; round < DES::ROUNDS; round++)
	{
		/* XOR compressed key with expanded right half of data */
		{
			int i;
			for (i = 0; i < DES::SBUFFER_SIZE; i++)
			{
				sbuffer[i] = data[DES::DATA_SIZE/2 + EXPANSION_PERMUTATION[i]-1] ^
					compressed_shifted_key[encrypt ? round : 15-round][i];
			}
		}
		/* S-Box substitutions */
		{
#define index(n) ((sbuffer[(n)] << (5+2)) | (sbuffer[(n)+5] << (4+2)) | \
	(sbuffer[(n)+1] << (3+2)) | (sbuffer[(n)+2] << (2+2)) | (sbuffer[(n)+3] << (1+2)) | \
	(sbuffer[(n)+4] << (0+2)))
			pbuffer.dword[0] = * (DWORD *) (MODIFIED_SBOX_1 + index(0));
			pbuffer.dword[1] = * (DWORD *) (MODIFIED_SBOX_2 + index(6));
			pbuffer.dword[2] = * (DWORD *) (MODIFIED_SBOX_3 + index(12));
			pbuffer.dword[3] = * (DWORD *) (MODIFIED_SBOX_4 + index(18));
			pbuffer.dword[4] = * (DWORD *) (MODIFIED_SBOX_5 + index(24));
			pbuffer.dword[5] = * (DWORD *) (MODIFIED_SBOX_6 + index(30));
			pbuffer.dword[6] = * (DWORD *) (MODIFIED_SBOX_7 + index(36));
			pbuffer.dword[7] = * (DWORD *) (MODIFIED_SBOX_8 + index(42));
		}

		/* XOR and swap */
		if (round < 15)
		{
			int i;
			for (i = 0; i < DES::DATA_SIZE/2; i++)
			{
				BYTE x = data[DES::DATA_SIZE/2 + i];
				data[DES::DATA_SIZE/2 + i] = data[i] ^ pbuffer.byte[PBOX_PERMUTATION[i]-1];
				data[i] = x;
			}
		}
		else
		{
			int i;
			for (i = 0; i < DES::DATA_SIZE/2; i++)
				data[i] = data[i] ^ pbuffer.byte[PBOX_PERMUTATION[i]-1];
		}
	}
}

/*----------------------------------------------------------------------------
This function takes 64 bits (8 bytes) of a password, XOR's it into a buffer,
and DES encrypts the buffer contents with the specified key. It then returns a
pointer to the next character of the password.
----------------------------------------------------------------------------*/

const char *DES::xmix(const char *password,
											BYTE buffer[DES::DATA_SIZE], const BYTE key[DES::KEY_SIZE])
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

static BYTE MIXER1[DES::KEY_SIZE] =
{
	1,0,1,0,0,0,0,0,0,1,
	0,1,0,0,1,1,0,1,0,0,
	1,1,0,1,1,1,0,1,1,0,
	0,0,0,0,1,0,0,1,0,1,
	1,1,0,0,0,0,1,1,1,0,
	0,1,0,1,1,0
};
static BYTE MIXER2[DES::KEY_SIZE] =
{
	1,0,1,0,1,1,0,0,0,0,
	0,1,1,1,1,0,1,0,0,1,
	1,1,1,0,1,0,0,0,0,0,
	0,1,1,0,1,0,1,0,0,0,
	1,1,0,0,0,0,1,1,0,0,
	0,0,0,0,0,0
};
static BYTE MIXER3[DES::KEY_SIZE] =
{
	1,0,0,0,1,1,1,1,1,0,
	1,0,0,0,1,1,0,0,1,0,
	0,1,0,1,1,0,0,0,0,0,
	0,0,1,1,1,0,0,0,1,0,
	1,0,0,0,1,0,0,0,1,1,
	1,1,1,1,0,0
};
static BYTE MIXER4[DES::KEY_SIZE] =
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

void DES::setKey(const EsString& key)
{
	BYTE buffer[DES::DATA_SIZE];
	memset(buffer, 0, sizeof(buffer));
	wxString _key = key.c_str();
	wxCharBuffer asc = _key.ToAscii();
	const char* p = xmix(asc, buffer, MIXER1);
	p = xmix(p, buffer, MIXER2);
	p = xmix(p, buffer, MIXER3);
	xmix(p, buffer, MIXER4);
	initialize(buffer);
}

void DES::process(const EsBinBuffer& src, EsBinBuffer& dest, bool encode /*= true*/)
{
	EsBinBuffer in(src);
	//8 byte - align input buffer, padding it with 0
	int iMod = in.size() % 8;
	if( iMod )
		in.resize( in.size() + 8 - iMod, 0 );
	//prepare buff out
	dest.clear();
	//buff-out length is buff-in length + 8 byte - aligned
	dest.reserve( in.size() );

	//encode input password & compare
	EsBinBuffer::value_type block[DES::DATA_SIZE];

	size_t inIdx = 0;
	while( inIdx < in.size() )
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

		for(blockIdx = 0; blockIdx < DES::DATA_SIZE; blockIdx += 8)
		{
			EsBinBuffer::value_type item =
				block[blockIdx]		<< 7 |
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
}