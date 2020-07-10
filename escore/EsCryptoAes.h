#ifndef _es_crypto_aes_h_
#define _es_crypto_aes_h_

#ifdef ES_USE_CRYPTO

// Implementation of AES codec
class ESCORE_CLASS EsCryptoAes
{
public:
  EsCryptoAes(const EsBinBuffer& key);

  void keySet(const EsBinBuffer& key);
  const EsBinBuffer& keyGet() const ES_NOTHROW { return m_key; }

  void encode(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void decode(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void process(EsBinBuffer& dest, const EsBinBuffer& src, bool doEncode = true) const;

  ES_USE_AS_EXISTING_REFLECTED_CLASS_SERVICE

  /// Reflected class services
  ///
  
  /// encode|decode sequence
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, encodeReflected, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, decodeReflected, cr_EsVariant, cr_EsVariant);

private:
  EsBinBuffer m_key;
  EsBinBuffer m_keybuf;

private:
  EsCryptoAes() ES_REMOVEDECL;
  EsCryptoAes(const EsCryptoAes&) ES_REMOVEDECL;
  EsCryptoAes& operator=(const EsCryptoAes&) ES_REMOVEDECL;
};

#endif

#endif // _es_crypto_aes_h_
