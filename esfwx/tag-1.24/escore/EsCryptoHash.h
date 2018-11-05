#ifndef _es_crypto_hash_h_
#define _es_crypto_hash_h_

#ifdef ES_USE_CRYPTO

// Implementation of MD5 digest hash
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsCryptoHash, ESCORE_CLASS)

  /// Create FNV_1a 32bit hash from input sequence
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, fnv1a32, cr_EsVariant);

  /// Create FNV_1a 64bit hash from input sequence
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, fnv1a64, cr_EsVariant);

  /// Calculate MD5 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, md5, cr_EsVariant);

  /// Calculate SHA1 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha1, cr_EsVariant);

  /// Calculate SHA224 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha224, cr_EsVariant);

  /// Calculate SHA256 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha256, cr_EsVariant);

  /// Calculate SHA384 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha384, cr_EsVariant);

  /// Calculate SHA512 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha512, cr_EsVariant);

  /// Calculate SHA3_224 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha3_224, cr_EsVariant);

  /// Calculate SHA3_256 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha3_256, cr_EsVariant);

  /// Calculate SHA3_384 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha3_384, cr_EsVariant);

  /// Calculate SHA3_512 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, sha3_512, cr_EsVariant);

  /// Calculate RIPEMD_128 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, ripemd128, cr_EsVariant);

  /// Calculate RIPEMD_160 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, ripemd160, cr_EsVariant);

  /// Calculate RIPEMD_256 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, ripemd256, cr_EsVariant);

  /// Calculate RIPEMD_320 hash from single, or multiple sequences
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, ripemd320, cr_EsVariant);

ES_DECL_REFLECTED_SERVICES_END

#endif

#endif // _es_crypto_hash_h_
