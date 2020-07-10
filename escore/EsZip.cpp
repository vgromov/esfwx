#include "escorepch.h"
#pragma hdrstop

#ifdef ES_USE_ZLIB

#if ES_COMPILER_VENDOR != ES_COMPILER_VENDOR_BORLAND && defined(ES_USE_ECC_ZLIB)
# undef ES_USE_ECC_ZLIB
#endif

#include "EsZip.h"
#ifdef ES_USE_ECC_ZLIB
# include <System.ZLib.hpp>
#else
  extern "C" {
# include "zlib/zlib.h"

# include "zlib/adler32.c"
# undef DO1
# undef DO2
# undef DO4
# undef DO8
# undef DO16
# undef BASE
# undef NMAX

# include "zlib/compress.c"

# include "zlib/crc32.c"
# undef DO1
# undef DO8

# include "zlib/deflate.c"

# include "zlib/infback.c"
# undef LOAD
# undef RESTORE
# undef INITBITS
# undef PULL
# undef PULLBYTE
# undef NEEDBITS
# undef BITS
# undef DROPBITS
# undef BYTEBITS
# undef ROOM

# include "zlib/inffast.c"

# include "zlib/inflate.c"
# undef CRC2
# undef CRC4
# undef LOAD
# undef RESTORE
# undef INITBITS
# undef PULLBYTE
# undef NEEDBITS
# undef BITS
# undef DROPBITS
# undef BYTEBITS

# include "zlib/inftrees.c"
# include "zlib/trees.c"
# include "zlib/uncompr.c"
# include "zlib/zutil.c"
  }
# undef compress //< Do not mess with our member method
#endif

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// zip compressor
//
EsZipCompressor::EsZipCompressor(int level /*= compressionBest*/,
    int memLevel /*= memlevelDefault*/,
    Strategy strategy /*= strategyDefault*/) :
m_impl(0)
{
  m_impl = calloc(1, sizeof(z_stream));


  int result =
#ifdef ES_USE_ECC_ZLIB
    deflateInit2(
      *((z_stream*)m_impl),
      level,
      Z_DEFLATED,
      15,
      memLevel,
      strategy
    );
#else
    deflateInit2(
      (z_stream*)m_impl,
      level,
      Z_DEFLATED,
      15,
      memLevel,
      strategy
    );
#endif

  if( Z_OK != result )
    EsException::Throw(esT("Could not initialize zip compressor"));
}

EsZipCompressor::~EsZipCompressor()
{
  if(m_impl)
    deflateEnd(
#ifdef ES_USE_ECC_ZLIB
      *((z_stream*)m_impl)
#else
      (z_stream*)m_impl
#endif
      );
  free(m_impl);
  m_impl = 0;
}

void EsZipCompressor::chunkRead(EsBinBuffer::pointer chunkBuff)
{
  ES_ASSERT(m_impl);
  z_stream* zs = (z_stream*)m_impl;

  zs->avail_out = zipChunkSizeDef;
  zs->next_out = chunkBuff;
  int result = deflate(
#ifdef ES_USE_ECC_ZLIB
    *zs
#else
    zs
#endif
    , Z_FINISH);

  if(Z_STREAM_ERROR == result)
    EsException::Throw(esT("Error while compressing data"));
}

EsBinBuffer EsZipCompressor::compress(const EsBinBuffer& src)
{
  ES_ASSERT(m_impl);

  if(src.empty())
    return EsBinBuffer::null();

  EsBinBuffer result;
  result.reserve(src.size());

  z_stream* zs = (z_stream*)m_impl;
  deflateReset(
#ifdef ES_USE_ECC_ZLIB
    *zs
#else
    zs
#endif
    );

  zs->avail_in = static_cast<unsigned>(src.size());
  zs->next_in =
#ifdef ES_USE_ECC_ZLIB
    (System::Byte*)
#else
    (Bytef*)
#endif
    &src[0];

  // read compressed buffer in chunks
  EsBinBuffer::value_type chunk[zipChunkSizeDef];

  do
  {
    chunkRead(chunk);
    result.append(zipChunkSizeDef - zs->avail_out, chunk);

  } while( 0 == zs->avail_out );

  return result;
}

// decompressor
//
EsZipDecompressor::EsZipDecompressor() :
m_impl(0)
{
  m_impl = calloc(1, sizeof(z_stream));
  int result =
#ifdef ES_USE_ECC_ZLIB
    inflateInit(
      *((z_stream*)m_impl)
    );
#else
    inflateInit(
      (z_stream*)m_impl
    );
#endif

  if( Z_OK != result )
    EsException::Throw(esT("Could not initialize zip de-compressor"));
}

EsZipDecompressor::~EsZipDecompressor()
{
  if(m_impl)
#ifdef ES_USE_ECC_ZLIB
    inflateEnd(
      *((z_stream*)m_impl)
    );
#else
    inflateEnd(
      (z_stream*)m_impl
    );
#endif

  free(m_impl);
  m_impl = 0;
}

void EsZipDecompressor::chunkRead(EsBinBuffer::pointer chunkBuff)
{
  ES_ASSERT(m_impl);
  z_stream* zs = (z_stream*)m_impl;
  zs->avail_out = zipChunkSizeDef;
  zs->next_out = chunkBuff;

  int result = inflate(
#ifdef ES_USE_ECC_ZLIB
    *zs
#else
    zs
#endif
    , Z_NO_FLUSH);
  switch(result)
  {
  case Z_STREAM_ERROR:
    EsException::Throw(esT("The ZIP stream is inconsistent"));
    break;
  case Z_NEED_DICT:
    EsException::Throw(esT("A preset dictionary is needed"));
    break;
  case Z_DATA_ERROR:
    EsException::Throw(esT("ZIP input data is corrupted"));
    break;
  case Z_MEM_ERROR:
    EsException::Throw(esT("Not enough memory for decompression"));
    break;
  case Z_STREAM_END:
    // all input is decompressed
    break;
  }
}

EsBinBuffer EsZipDecompressor::decompress(const EsBinBuffer& src)
{
  ES_ASSERT(m_impl);

  if(src.empty())
    return EsBinBuffer::null();

  EsBinBuffer result;
  result.reserve(4*src.size());

  z_stream* zs = (z_stream*)m_impl;
  inflateReset(
#ifdef ES_USE_ECC_ZLIB
    *zs
#else
    zs
#endif
    );

  zs->avail_in = static_cast<unsigned>(src.size());
  zs->next_in =
#ifdef ES_USE_ECC_ZLIB
    (System::Byte*)
#else
    (Bytef*)
#endif
    &src[0];

  // read de-compressed buffer in chunks
  EsBinBuffer::value_type chunk[zipChunkSizeDef];

  do
  {
    chunkRead(chunk);
    result.append(zipChunkSizeDef - zs->avail_out, chunk);

  } while( 0 == zs->avail_out );

  return result;
}

#endif // #ifdef ES_USE_ZLIB

