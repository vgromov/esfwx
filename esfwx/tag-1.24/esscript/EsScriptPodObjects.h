#ifndef _es_script_pod_objects_h_
#define _es_script_pod_objects_h_

/// Script Plain Old Data (POD) objects, AKA built-in types
///

/// Special POD value moniker template class, to augment
/// POD value operations, and value io debug
///
template <typename DataT, typename OwnerT>
class EsScriptPodValMoniker
{
public:
	EsScriptPodValMoniker(OwnerT& owner) :
	m_owner(owner)
	{}

	void set_value(const DataT& val)
	{
		memcpy(&m_owner.m_data->at(m_owner.m_offs), &val, m_owner.m_size);
		m_owner.onSetValueParentNotify();
	}

	DataT get_value() const
	{
		DataT tmp;
		memcpy(&tmp, &m_owner.m_data->at(m_owner.m_offs), m_owner.m_size);
		return tmp;
	}

	bool internalBinBufferSet(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end)
	{
		if( pos+m_owner.m_size > end )
			return false;

		DataT val;
		memcpy(&val, pos, m_owner.m_size);
		set_value( val );
		pos += m_owner.m_size;

		return true;
	}

protected:
	OwnerT& m_owner;
};

// "friend declaration requires elaborate type" work-around
#if defined(__clang__) || defined(ES_COMPILER_VENDOR_GNUC)
# define FRIEND_DATA_MONIKER_T friend DataMonikerT
#else
# define FRIEND_DATA_MONIKER_T friend class DataMonikerT
#endif

/// Macros-ed POD data classes definitions
///
#define ES_DECL_POD_OBJECT_START(DataType) \
class ES_INTF_IMPL_DERIVED( ES_CONCAT(EsScript_, DataType), EsScriptObject) \
protected: \
	typedef EsScriptPodValMoniker< DataType, ES_CONCAT(EsScript_, DataType) > DataMonikerT; \
	FRIEND_DATA_MONIKER_T; \
	ES_CONCAT(EsScript_, DataType) (const EsScriptContext::Ptr& ctx, esU32 flags, const EsScriptObjectDataBufferPtr& buff, \
		const EsAttributesIntf::Ptr& classAttrs) : \
	EsScriptObject(ES_STRINGIZE(DataType), EsScriptObjectIntf::Ptr(), EsScriptMethodMapPtr(), ctx, flags, buff, \
		classAttrs ), \
	m_moniker(*this) \
	{ m_size = sizeof(DataType); } \
	static EsScriptObjectIntf::Ptr createMetaclass(const EsScriptContext::Ptr& ctx) \
	{ return EsScriptObjectIntf::Ptr( new ES_CONCAT(EsScript_, DataType) (ctx, ofMetaclass|ofPOD, EsScriptObjectDataBufferPtr(), \
			EsAttributesIntf::Ptr()) ); } \
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const ES_OVERRIDE \
	{ EsScriptObjectIntf::Ptr result( new ES_CONCAT(EsScript_, DataType) (m_ctx, m_flags & ~ofMetaclass, buff, \
			m_attrsClass ) ); \
		ES_ASSERT(result); \
		ESSCRIPT_OBJECT_TRACE2(esT("New instance of '%s' object type created"), m_typeName.c_str()) \
		return result; } \
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(size_t offs) ES_OVERRIDE { \
		internalOffsetSet(offs); } \
	ES_DECL_INTF_METHOD(bool, internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end) ES_OVERRIDE; \
	virtual void validate(const EsVariant& val) const ES_OVERRIDE; \
public: \
	ES_DECL_REFLECTED_CLASS( ES_CONCAT(EsScript_, DataType), EsScriptObject ) \
	ES_DECL_PROPERTY(value, EsVariant) \
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString); \
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, add, cr_EsVariant); \
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, subtract, cr_EsVariant); \
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, multiply, cr_EsVariant); \
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, divide, cr_EsVariant); \
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, compare, cr_EsVariant); \
	friend class EsScriptMachine;

#define ES_DECL_POD_OBJECT_END \
protected: \
	DataMonikerT m_moniker;	};

#define ES_DECL_POD_OBJECT(DataType) \
	ES_DECL_POD_OBJECT_START(DataType) \
	ES_DECL_POD_OBJECT_END

// declare POD script objects
ES_DECL_POD_OBJECT(esU8)
ES_DECL_POD_OBJECT(esU16)
ES_DECL_POD_OBJECT(esU32)
ES_DECL_POD_OBJECT(esU64)
ES_DECL_POD_OBJECT(esI8)
ES_DECL_POD_OBJECT(esI16)
ES_DECL_POD_OBJECT(esI32)
ES_DECL_POD_OBJECT(esI64)
ES_DECL_POD_OBJECT(esF)
ES_DECL_POD_OBJECT(esD)

// use specialized POD declaration for esDT, as it implements custom serialization
ES_DECL_POD_OBJECT_START(esDT)
	ES_DECL_REFLECTED_METHOD1(void, streamWrite, cr_EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD1(void, streamRead, cr_EsBaseIntfPtr);
ES_DECL_POD_OBJECT_END

#endif // _es_script_pod_objects_h_
