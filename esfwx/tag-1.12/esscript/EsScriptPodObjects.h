#ifndef _es_script_pod_objects_h_
#define _es_script_pod_objects_h_

// script Plain Old Data (POD) objects
//
#define ES_DECL_POD_OBJECT(DataType) \
class EsScript## DataType ##Object : public EsScriptObject {	\
protected: \
	EsScript## DataType ##Object(EsScriptMachine& machine, DWORD flags, EsSharedBinBufferPtr buff) : \
		EsScriptObject(wxT( #DataType ), EsScriptObjectIntf::Ptr(), EsMethodMapPtr(), machine, flags, buff) { m_size = sizeof(DataType); } \
	static EsScriptObjectIntf::Ptr createMetaclass(EsScriptMachine& machine) \
	{ return EsScriptObjectIntf::Ptr( new EsScript## DataType ##Object(machine, ofMetaclass|ofPOD, EsSharedBinBufferPtr()) ); } \
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, createObject)(EsSharedBinBufferPtr buff) const \
	{ EsScriptObjectIntf::Ptr result( new EsScript## DataType ##Object(m_machine, m_flags & ~ofMetaclass, buff) ); \
		wxASSERT(result); \
		wxLogDebug(wxT("New instance of '%s' object type created"), m_typeName.c_str()); \
		return result; } \
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(size_t offs) { \
		internalSetOffset(offs); } \
	virtual void set_value(const EsVariant& val) \
	{ DataType tmp = To_## DataType ##(val); memcpy(&m_data->at(m_offs), &tmp, m_size); onSetValueParentNotify(); } \
	virtual EsVariant get_value() const \
	{ DataType tmp; memcpy(&tmp, &m_data->at(m_offs), m_size); return From_## DataType ##(tmp); } \
	friend class EsScriptMachine; \
};

// declare POD script objects
ES_DECL_POD_OBJECT(UINT8)
ES_DECL_POD_OBJECT(UINT16)
ES_DECL_POD_OBJECT(UINT32)
ES_DECL_POD_OBJECT(UINT64)
ES_DECL_POD_OBJECT(INT8)
ES_DECL_POD_OBJECT(INT16)
ES_DECL_POD_OBJECT(INT32)
ES_DECL_POD_OBJECT(INT64)
ES_DECL_POD_OBJECT(DATETIME)
ES_DECL_POD_OBJECT(FLOAT)

#endif // _es_script_pod_objects_h_
