#ifndef _es_crypto_des_h_
#define _es_crypto_des_h_

#ifdef ES_USE_CRYPTO

// Implementation of DES codec
class ESCORE_CLASS EsCryptoDes
{
public:
  EsCryptoDes(const EsBinBuffer& key, const EsBinBuffer& key1 = EsBinBuffer::null(), const EsBinBuffer& key2 = EsBinBuffer::null());

  void keySet(const EsBinBuffer& key);
  const EsBinBuffer& keyGet() const ES_NOTHROW { return m_key; }

  void keySet1(const EsBinBuffer& key);
  const EsBinBuffer& keyGet1() const ES_NOTHROW { return m_key1; }

  void keySet2(const EsBinBuffer& key);
  const EsBinBuffer& keyGet2() const ES_NOTHROW { return m_key2; }

  void encode(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void decode(EsBinBuffer& dest, const EsBinBuffer& src) const;

  void process(EsBinBuffer& dest, const EsBinBuffer& src, bool doEncode = true) const;

  ES_USE_AS_EXISTING_REFLECTED_CLASS_SERVICE

  // Reflected class services
  //
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, encodeReflected, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD3(EsVariant, encodeReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD4(EsVariant, encodeReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, decodeReflected, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD3(EsVariant, decodeReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD4(EsVariant, decodeReflected, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

private:
  static void keybufInit(EsBinBuffer& keybuf, EsBinBuffer& keyvar, const EsBinBuffer& key, int keyIdx);

  void encode1(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void encode2(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void encode3(EsBinBuffer& dest, const EsBinBuffer& src) const;

  void decode1(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void decode2(EsBinBuffer& dest, const EsBinBuffer& src) const;
  void decode3(EsBinBuffer& dest, const EsBinBuffer& src) const;

private:
  EsBinBuffer m_key;
  EsBinBuffer m_key1;
  EsBinBuffer m_key2;
  EsBinBuffer m_keybuf;

private:
  EsCryptoDes() ES_REMOVEDECL;
  EsCryptoDes(const EsCryptoDes&) ES_REMOVEDECL;
  EsCryptoDes& operator=(const EsCryptoDes&) ES_REMOVEDECL;
};

#endif // ES_USE_CRYPTO

#endif // _es_crypto_des_h_
