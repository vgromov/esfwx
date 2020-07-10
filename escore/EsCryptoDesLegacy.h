#ifndef _es_crypto_des_legacy_h_
#define _es_crypto_des_legacy_h_

#if defined(ES_USE_CRYPTO_LEGACY)

// Legacy implementation of des codec
class ESCORE_CLASS EsCryptoDesLegacy
{
public:
  enum {
    KEY_SIZE      =  56,
    DATA_SIZE     =  64,
    SBUFFER_SIZE  =  48,
    ROUNDS        =  16
  };

public:
  EsCryptoDesLegacy(const EsString& key) { setKey(key); }
  void setKey(const EsString& key);
  void process(EsBinBuffer& dest, const EsBinBuffer& src, bool encode = true);

private:
  void initialize(const esU8*) ES_NOTHROW;

  const char* xmix(const char*, esU8*, const esU8*) ES_NOTHROW;
  void encrypt_decrypt(esU8*, bool) ES_NOTHROW;

private:
  esU8 m_compressed_shifted_key[EsCryptoDesLegacy::ROUNDS][EsCryptoDesLegacy::SBUFFER_SIZE];

private:
  EsCryptoDesLegacy() ES_REMOVEDECL;
  EsCryptoDesLegacy(const EsCryptoDesLegacy&) ES_REMOVEDECL;
  EsCryptoDesLegacy& operator=(const EsCryptoDesLegacy&) ES_REMOVEDECL;
};

#endif // ES_USE_CRYPTO

#endif // _es_crypto_des_legacy_h_
