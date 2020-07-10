#ifndef _es_stream_xml_h_
#define _es_stream_xml_h_

/// XML stream object implementation
/// Not all stream flags are supported, namely, Encrypted and Compressed, are not.
///
class ESCORE_CLASS ES_INTF_IMPL_DERIVED(EsStreamXml, EsStream)

protected:
  EsStreamXml(ulong flags, ulong version, const EsString& src, const EsBaseIntfPtr& factory);

public:
  ES_DECL_REFLECTED_CLASS(EsStreamXml, EsStream)
  virtual ~EsStreamXml();

  static EsStreamIntf::Ptr create(ulong flags, const EsString& src = EsString::null(), const EsBaseIntfPtr& factory = nullptr);

  static EsStreamIntf::Ptr create(ulong flags, ulong version, const EsString& src = EsString::null(), const EsBaseIntfPtr& factory = nullptr);

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  // Reflected ctors
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, ulong, cr_EsString)
  {
    return create(p1, p2);
  }

  ES_DECL_REFLECTED_CLASS_CTOR3(EsBaseIntfPtr, ulong, ulong, cr_EsString)
  {
    return create(p1, p2, p3);
  }

  // Save xml contents to target file. if target is empty,
  // try to save to the default file, if any
  ES_DECL_REFLECTED_CONST_METHOD1(void, save, cr_EsString);

  // EsStreamIntf implementation
  ES_DECL_INTF_METHOD(EsString, asString)() const ES_OVERRIDE;

protected:
  /// internal services
  ///
  void init(const EsString& src, ulong version);

  // Save stream contents to assigned file
  bool internalFileSave(const EsString& fname) const;

  // Validate XML header
  void xmlHeaderCheck(const EsXmlDocument& xml) const;

  // Parse XML document into Stream Block Model
  void parse(const EsXmlDocument& xml);
  void nodeParse(const EsXmlNode& node);

  // Create XML document from Stream Block Model
  void generate(EsXmlDocument& xml) const;
  void blockToXml(EsXmlNode& node) const;

  // Exception short-cuts
  static void throwStreamIsMalformed(const EsString& expected, const EsString& got);
  static void throwStreamIsMalformedNoAttr(const EsString& attr);

protected:
  mutable EsString m_file;
};

#endif // _es_stream_xml_h_
