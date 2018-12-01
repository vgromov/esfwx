#ifndef _es_zip_h_
#define _es_zip_h_

#ifdef ES_USE_ZLIB

// zip compression-deompression utility classes
//
enum {
  // default compression|decompression chunk size
  zipChunkSizeDef = 128*1024
};

class ESCORE_CLASS EsZipCompressor
{
public:
  enum {
    // compression levels shortcuts
    compressionNone      = 0,
    compressionFastest  = 1,
    compressionBest      = 9,
    compressionDefault  = -1,
    // memory levels
    memlevelSmallest    = 1,
    memlevelMax          = 9,
    memlevelDefault      = 8,
  };
  // compression strategies
  enum Strategy {
    strategyFiltered    = 1,
    strategyHuffmanOnly  = 2,
    strategyRle          = 3,
    strategyFixed        = 4,
    strategyDefault      = 0
  };

public:
  EsZipCompressor(int level = compressionBest,
    int memLevel = memlevelDefault, 
    Strategy strategy = strategyDefault);
  ~EsZipCompressor();

  EsBinBuffer compress(const EsBinBuffer& src);

private:
  // internal services
  void chunkRead(EsBinBuffer::pointer chunkBuff);

private:
  // prohibited functionality
  EsZipCompressor(const EsZipCompressor&);
  EsZipCompressor& operator=(const EsZipCompressor&);

private:
  void* m_impl;
};

class ESCORE_CLASS EsZipDecompressor
{
public:

public:
  EsZipDecompressor();
  ~EsZipDecompressor();

  EsBinBuffer decompress(const EsBinBuffer& src);

private:
  // internal services
  void chunkRead(EsBinBuffer::pointer chunkBuff);

private:
  // prohibited functionality
  EsZipDecompressor(const EsZipDecompressor&);
  EsZipDecompressor& operator=(const EsZipDecompressor&);

private:
  void* m_impl;
};

#endif // #ifdef ES_USE_ZLIB

#endif // _es_zip_h_
