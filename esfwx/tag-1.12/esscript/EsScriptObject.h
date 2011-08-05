#ifndef _es_script_object_h_
#define _es_script_object_h_

// base class for script objects hierarchy
//
class EKOSF_SCRIPT_CLASS EsScriptObject : public EsScriptObjectIntf
{
protected:
	// internal type defs
	//
	// for reflected methods & property defs
	typedef EsScriptObject ThisClass;
	
	// object bit flags
	enum {
		// object attribute flags
		ofMetaclass				= 0x00000001,	// this object's instance is used as metaclass, so it does not, and may not contain any data
		ofFinal						= 0x00000002, // object type is final and may not be extended or contain fields
		ofPOD							= 0x00000004,	// object is of POD (plain old data) type, its size if known and fixed (unless it's of array type)
		ofArray						= 0x00000008, // object is of array type
		// action flags
		ofNeedUpdateLayout= 0x00010000, // object memory layout must be updated
	};
	
	// weak interface references list
	typedef std::list<EsScriptObjectIntf*> EsScriptObjectIntfWeakPtrList;

protected:
	// constructor for metaclass usage only
	EsScriptObject(const EsString& typeName, EsScriptObjectIntf::Ptr ancestor, 
		const EsMethodMapPtr& methods, EsScriptMachine& machine, DWORD flags,
		EsSharedBinBufferPtr buff);

public:
	ES_INTF_MAP_BEGIN( EsScriptObject )
		ES_INTF_SUPPORTS( EsScriptObject, EsReflectedClassIntf )
		ES_INTF_SUPPORTS( EsScriptObject, EsScriptObjectIntf )
	ES_INTF_MAP_END
	{
		// destroy implementation
		delete this;
	}	
	
	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
	{ 
		return m_typeName; 
	}
	
	// EsReflectedClassIntf interface implementation. EsScriptObject has its own specifics
	// due to its dual nature - one part is C++ -coded reflection, other is script-coded
	//
	static const ClassInfo& getClassInfoStatic(); 
	ES_DECL_INTF_METHOD(const ClassInfo&, getClassInfo)() const
	{
		return getClassInfoStatic();
	}
	ES_DECL_INTF_METHOD(bool, isKindOf)(const EsReflectedClassIntf* other) const;
	ES_DECL_INTF_METHOD(bool, isIndexed)() const;
	ES_DECL_INTF_METHOD(EsString::Array, getPropertyNames)() const;
	ES_DECL_INTF_METHOD(EsString::Array, getPersistentPropertyNames)() const;	
	ES_DECL_INTF_METHOD(bool, hasProperty)(const EsString& name) const;
	ES_DECL_INTF_METHOD(EsVariant, getProperty)(const EsString& name) const;
	ES_DECL_INTF_METHOD(void, setProperty)(const EsString& name, const EsVariant& val);
	ES_DECL_INTF_METHOD(EsString, getConfigString)() const;
	ES_DECL_INTF_METHOD(void, setConfigString)(const EsString& str, bool discardUnknownProperties = true);
	ES_DECL_INTF_METHOD(bool, hasMethod)(const MethodInfoKeyT& key) const;
	ES_DECL_INTF_METHOD(EsVariant, callMethod)(const EsString& name, const EsVariant& params);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5);
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6);
	ES_DECL_INTF_METHOD(EsVariant, callClassMethod)(const EsString& name, const EsVariant& params) const;
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name) const { return getClassInfo().classCall(name); }
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1) const {	return getClassInfo().classCall(name, param1); }
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2) const { return getClassInfo().classCall(name, param1, param2); }
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const {	return getClassInfo().classCall(name, param1, param2, param3); }
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const { return getClassInfo().classCall(name, param1, param2, param3, param4); }
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const { return getClassInfo().classCall(name, param1, param2, param3, param4, param5); }
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const { return getClassInfo().classCall(name, param1, param2, param3, param4, param5, param6); }
	
	// EsScriptObjectIntf implementation
	//
	// seal object instance
	ES_DECL_INTF_METHOD(void, seal)() { m_flags |= ofFinal; }
	// create instance of script object from its metaclass instance
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, createObject)(EsSharedBinBufferPtr buff) const;
	// internal object cloning
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, internalClone)(EsSharedBinBufferPtr buff) const;
	// add attribute to the object metaclass or instance
	ES_DECL_INTF_METHOD(void, addAttribute)(const EsString& name, const EsVariant& val);
	// get attribute value accessor
	ES_DECL_INTF_METHOD(EsValAccessor::Ptr, internalGetAttribute)(const EsString& name) const;
	// add field object to the script object
	ES_DECL_INTF_METHOD(void, addField)(const EsString& name, EsScriptObjectIntf::Ptr field);
	// set script object parent. 
	ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent) { m_parent = parent; }
	// get script object parent. 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, getParent)() const { return m_parent; } 
	// access script object ancestor
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, getAncestor)() const { return m_ancestor; }
	// call ancestor's method implementation (as if our 'this' was cast to ancestor's 'this')
	ES_DECL_INTF_METHOD(EsVariant, callAncestorMethod)(const EsString& name, const EsVariant& params, const EsString& ancestorTypeName);
	// return only owned field count
	ES_DECL_INTF_METHOD(size_t, internalGetFieldCount)() const { return m_fields.size(); }
	// return name of the field by its index in internal collection
	ES_DECL_INTF_METHOD(const EsString&, internalGetFieldName)(size_t idx) const;
	// read-only access to object's flags
	ES_DECL_INTF_METHOD(DWORD, getFlags)() const { return m_flags; }
	// update object memory layout. fields and ancestor offsets and|or sizes get updated
	// as a result of this process
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(size_t offs);
	ES_DECL_INTF_METHOD(void, internalUpdateFieldsLayout)(size_t offs);
	// internal write access to the object's offset value
	ES_DECL_INTF_METHOD(void, internalSetOffset)(size_t offs);
	// add object value update subscriber
	ES_DECL_INTF_METHOD(void, internalAddUpdateSubscriber)(EsScriptObjectIntf* subscriber);
	// internal data update helpers
	ES_DECL_INTF_METHOD(void, internalOnDataChanged)(EsScriptObjectIntf* sender);
	// internal layout invalidation
	ES_DECL_INTF_METHOD(void, internalInvalidateLayout)();
	
	// return true if this object is root one (data buffer owning object, no parent), false otherwise (fields, etc.)
	ES_DECL_INTF_METHOD(bool, isRoot)() const { return m_parent == 0; }
	// return true if object is final
	ES_DECL_INTF_METHOD(bool, isFinal)() const { return ofFinal == (m_flags & ofFinal); }
	// return true if object is POD
	ES_DECL_INTF_METHOD(bool, isPOD)() const { return ofPOD == (m_flags & ofPOD); }
	// return true if object is array
	ES_DECL_INTF_METHOD(bool, isArray)() const { return ofArray == (m_flags & ofArray); }
	// return root object for this object
	ES_DECL_INTF_METHOD(const EsScriptObjectIntf*, getRootObject)() const;
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, getRootObject)();
	// return script object absolute offset in bytes from the start of data buffer
	ES_DECL_INTF_METHOD(size_t, getOffs)() const { return m_offs; }
	// return script object size in bytes
	ES_DECL_INTF_METHOD(size_t, getSize)() const { return m_size; }
	// finds the script object method declared in script.
	// if method is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsCodeSection::Ptr, findScriptedMethod)(const MethodInfoKeyT& key) const;
	// create new instance of EsScriptObject which is clone of the existing instance.
	// if called from metaclass, new (default values used) object instance is created.
	// may be called from the root objects (parent=NULL) only.
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, clone)() const;	
	// check attribute existence
	ES_DECL_INTF_METHOD(bool, attributeExists)(const EsString& name) const;
	// get all assigned attribute names
	ES_DECL_INTF_METHOD(EsStringArray, getAttributeNames)() const;
	// get attribute value
	ES_DECL_INTF_METHOD(EsVariant, getAttribute)(const EsString& name) const;	
	// find named field in this object or one of its ancestors. 
	// if field is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, findField)(const EsString& name) const;	
	// return all field names
	ES_DECL_INTF_METHOD(EsStringArray, getFieldNames)() const;
	// return field count
	ES_DECL_INTF_METHOD(ulong, getFieldCount)() const;
	// return field object by name. if field is not found, an exception is thrown
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, getField)(const EsString& name) const;
	// handle data change event notification from subscription publisher
	ES_DECL_INTF_METHOD(void, onDataChanged)(EsScriptObjectIntf* sender);
	
	// reflected properties
	//
	// script object standard value property
	ES_DECL_PROPERTY(value, EsVariant);
	// reflected methods
	//
	// standard null setter
	ES_DECL_REFLECTED_INTF_METHOD0(void, setToNull);
	// standard asString getter
	ES_DECL_REFLECTED_INTF_METHOD0(EsString, asString);
	
protected:
	// internal services
	EsCodeSection::Ptr findThisScriptedMethod(const MethodInfoKeyT& key) const;
	void onSetValueParentNotify();
	
protected:
	// field && attribute container types
	typedef std::vector<EsScriptObjectIntf::Ptr> EsObjectsVector;
	typedef std::vector<EsAccessorIntf::Ptr> EsAttributesVector;
	typedef boost::bimap<size_t, EsString> EsIndexedNamesMap;

protected:
	// data members
	//
	// script machine in context of which this object exists
	EsScriptMachine& m_machine;
	// script object methods map
	EsMethodMapPtr m_methods;
	// ancestor object pointer. ancestor is the object, the current one is derived from
	EsScriptObjectIntf::Ptr m_ancestor;
	// parent object pointer. parent is the object which holds this object as field
	EsScriptObjectIntf* m_parent;
	// list of weak references to update subscribers
	EsScriptObjectIntfWeakPtrList m_updateSubscribers;
	
	// script object type name. is defined on script compile time, apart from class name,
	// which is always a C++ reflected class name.
	//
	EsString m_typeName;
	// script object flags
	DWORD m_flags;
	// script object data buffer
	EsSharedBinBufferPtr m_data;
	// attribute container, for both indexed and named access
	EsAttributesVector m_attrs;
	EsIndexedNamesMap m_attrsMap;
	// field containers, for both indexed and named access
	EsObjectsVector m_fields;
	EsIndexedNamesMap m_fieldsMap;
	// object size and absolute offset, in bytes
	size_t m_size;
	size_t m_offs;

	friend class EsScriptMachine;
};

#endif // _es_script_object_h_
