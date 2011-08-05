#ifndef _es_script_array_object_h_
#define _es_script_array_object_h_

// array field object
class EsScriptArrayObject : public EsScriptObject
{
protected:
	// friends-only services
	EsScriptArrayObject(EsScriptMachine& machine, DWORD flags, EsSharedBinBufferPtr buff, 
		EsCodeSection::Ptr sizeExpr, EsScriptObjectIntf::Ptr itemMetaclass);
	static EsScriptObjectIntf::Ptr createMetaclass(EsScriptMachine& machine, EsCodeSection::Ptr sizeExpr, EsScriptObjectIntf::Ptr itemMetaclass);
	// base class overrides
	//
	ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent);
	// create instance of script object from its metaclass instance
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, createObject)(EsSharedBinBufferPtr buff) const;
	// update object memory layout. fields and ancestor offsets and|or sizes get updated
	// as a result of this process
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(size_t offs);	
	
public:
	ES_DECL_REFLECTED_CLASS(EsScriptArrayObject)

	// reflected services for indexed object
	//
	ES_DECL_REFLECTED_CONST_METHOD0(int, getCount);
	ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, getItem, cr_EsVariant);
	ES_DECL_REFLECTED_METHOD2(void, setItem, cr_EsVariant, cr_EsVariant);

protected:
	// internal service
	void initializeUpdateSubscription();
	size_t internalCountGet();
	EsScriptObjectIntf::Ptr internalGetItem(size_t idx);

protected:
	// data members
	//
	// array item metaclass
	EsScriptObjectIntf::Ptr m_itemMetaclass;
	// size expression. either owned, or external, if array is used as stand-alone object
	EsCodeSection::Ptr m_sizeExpr;
	// array itself
	EsObjectsVector m_arr;
	
	friend class EsScriptMachine;
	friend class EsScriptObject;
};

#endif // _es_script_array_object_h_