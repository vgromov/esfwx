#ifndef _es_stream_binary_h_
#define _es_stream_binary_h_

/// Binary stream object implementation
///
class ESCORE_CLASS ES_INTF_IMPL_DERIVED(EsStreamBinary, EsStream)

protected:
	EsStreamBinary(ulong flags, ulong version, const EsVariant& src, const EsBaseIntfPtr& factory);

public:
	ES_DECL_REFLECTED_CLASS(EsStreamBinary, EsStream)
	virtual ~EsStreamBinary();

	static EsStreamIntf::Ptr create(ulong flags, ulong version);

	static EsStreamIntf::Ptr create(ulong flags, const EsVariant& src,
		const EsBaseIntfPtr& factory = EsBaseIntfPtr());

	static EsStreamIntf::Ptr create(ulong flags, ulong version,
    const EsVariant& src, const EsBaseIntfPtr& factory = EsBaseIntfPtr());

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

	// Reflected ctors
	ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, ulong, cr_EsVariant)
	{
		return create(p1, p2);
	}

	ES_DECL_REFLECTED_CLASS_CTOR3(EsBaseIntfPtr, ulong, ulong, cr_EsVariant)
	{
		return create(p1, p2, p3);
	}

	// Save binary contents to target file. if target is empty,
	// try to save to the default file, if any.
	ES_DECL_REFLECTED_CONST_METHOD1(void, save, cr_EsString);

	// EsStreamIntf specific implementation
	//
	ES_DECL_INTF_METHOD(EsString, asString)() const ES_OVERRIDE;

  /// Reflected properties
  ///
  ES_DECL_PROPERTY_STD(key, EsString)

protected:
	/// Internal services
	///
	void init(const EsVariant& src, ulong version);
  bool internalFileSave(const EsString& fname) const;

	static void mainHeaderCheck(const EsBinBuffer& bb);

  void parse(const EsBinBuffer& bb);
  void blockParse(const EsBinBuffer& data, ulong& pos, ulong end);

	void mainHeaderPrepend(EsBinBuffer& bb) const;

  void generate(EsBinBuffer& data) const;
  void blockToBuffer(EsBinBuffer& data, ulong& pos) const;

protected:
	mutable EsString m_file;
};

#endif // _es_stream_binary_h_

