#ifndef _es_script_array_object_h_
#define _es_script_array_object_h_

// array field object
class ES_INTF_IMPL_DERIVED(EsScriptArrayObject, EsScriptObject)

protected:
	// friends-only services
	EsScriptArrayObject(const EsScriptContext::Ptr& ctx, esU32 flags, const EsScriptObjectDataBufferPtr& buff,
		const EsScriptCodeSection::Ptr& sizeExpr, const EsScriptObjectIntf::Ptr& itemMetaclass);

	static EsScriptObjectIntf::Ptr createMetaclass(const EsScriptContext::Ptr& ctx, const EsScriptObjectIntf::Ptr& itemMetaclass);
	// base class overrides
	//
	ES_DECL_INTF_METHOD(EsString, itemTypeNameGet)() const ES_NOTHROW ES_OVERRIDE { return m_itemMetaclass->typeNameGet(); }
	ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent) ES_OVERRIDE;
	// create instance of script object from its metaclass instance
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const ES_OVERRIDE;
	ES_DECL_INTF_METHOD(bool, internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end) ES_OVERRIDE;
	// update object memory layout. fields and ancestor offsets and|or sizes get updated
	// as a result of this process
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(ulong offs) ES_OVERRIDE;
	// access field expression, valid for fields with expression only, namely, for arrays and conditional fields
	ES_DECL_INTF_METHOD(EsScriptCodeSection::Ptr, thisFieldExprGet)() const ES_NOTHROW ES_OVERRIDE { return m_expr; }

public:
	ES_DECL_REFLECTED_CLASS(EsScriptArrayObject, EsScriptObject)

	ES_DECL_INTF_METHOD(void, binBufferSet)(const EsBinBuffer& buff) ES_OVERRIDE;

	// Reflected services for indexed object
	//
	ES_DECL_REFLECTED_CONST_METHOD0(ulong, countGet);
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, itemGet, cr_EsVariant);
	ES_DECL_REFLECTED_METHOD2(void, itemSet, cr_EsVariant, cr_EsVariant);

protected:
	// internal service
	void initializeUpdateSubscription();
	ulong internalCountGet();
	EsScriptObjectIntf::Ptr internalGetItem(ulong idx);

protected:
	// data members
	//
  // array item metaclass
	EsScriptObjectIntf::Ptr m_itemMetaclass;
	// size expression
	EsScriptCodeSection::Ptr m_expr;
	// array itself
	ObjectsVector m_arr;
	// supscription initialized flag
	bool m_subscriptionInitialized;
	// internal flag for direct bin buffer assignment handling
	bool m_directBinBufferAssignment;

	friend class EsScriptMachine;
	friend class EsScriptObject;
};

#endif // _es_script_array_object_h_