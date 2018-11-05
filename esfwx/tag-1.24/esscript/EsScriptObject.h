#ifndef _es_script_object_h_
#define _es_script_object_h_

/// Base class for script objects hierarchy
///
class ESSCRIPT_CLASS ES_INTF_IMPL2(EsScriptObject, EsScriptObjectIntf, EsReflectedClassIntf)

public:
	// object bit flags
	enum {
		// object attribute flags
		ofMetaclass				= 0x00000001,	// this object's instance is used as metaclass, so it does not, and may not contain any data
		ofFinal						= 0x00000002, // object type is final and may not be extended or contain fields
		ofPOD							= 0x00000004,	// object is of POD (plain old data) type, its size if known and fixed (unless it's of array type)
		ofArray						= 0x00000008, // object is of array type
		ofIf							= 0x00000010, // object is of IF conditional type
		ofIfBranch				= 0x00000020, // object is of IF conditional branch type
		// action flags
		ofNeedUpdateLayout= 0x00010000, // object memory layout must be updated
	};

protected:
	// internal type defs
	//
	typedef EsScriptObject ThisClassT;
	
protected:
	// constructor for metaclass usage only
	EsScriptObject(
    const EsString& typeName,
		const EsScriptObjectIntf::Ptr& ancestor,
		const EsScriptMethodMapPtr& methods,
		const EsScriptContext::Ptr& ctx,
		esU32 flags,
		const EsScriptObjectDataBufferPtr& buff,
		const EsAttributesIntf::Ptr& attrsClass
  );

public:
	virtual ~EsScriptObject();

	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE
	{ 
		return m_typeName; 
	}
	ES_DECL_INTF_METHOD(void, destroy)() ES_NOTHROW ES_OVERRIDE;
	
	// EsReflectedClassIntf interface implementation. EsScriptObject has its own specifics
	// due to its dual nature - one part is C++ -coded reflection, other is script-coded
	//
	static const EsClassInfo& classInfoGetStatic() ES_NOTHROW;
	ES_DECL_INTF_METHOD(const EsClassInfo&, classInfoGet)() const ES_NOTHROW ES_OVERRIDE
	{
		return classInfoGetStatic();
	}
	ES_DECL_INTF_METHOD(EsMetaclassIntf::Ptr, metaGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(bool, isKindOf)(const EsReflectedClassIntf::Ptr& other) const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, isKindOf, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(bool, isKindOf)(const EsClassInfo& other) const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD1(void, copyFrom, cr_EsBaseIntfPtr) ES_OVERRIDE;
 	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsBaseIntfPtr, asWeakReference) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isIndexed) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isScripted) ES_NOTHROW ES_OVERRIDE { return true; }
	ES_DECL_INTF_METHOD(void, attributesAssign)(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW ES_OVERRIDE { m_attrsClass = attrs; }
	ES_DECL_INTF_METHOD(void, attributeAdd)(const EsString& name, const EsVariant& val) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasAttribute, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasClassAttribute, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsString::Array, attributeNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsString::Array, classAttributeNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, attributeGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, classAttributeGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsString::Array, propertyNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsString::Array, persistentPropertyNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasProperty, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, propertyCanRead, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, propertyCanWrite, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, propertyGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD2(void, propertySet, cr_EsString, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, propertyAttributeNamesGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, propertyHasAttribute, cr_EsString, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, propertyAttributeGet, cr_EsString, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, propertyLabelGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, propertyDescriptionGet, cr_EsString) ES_OVERRIDE;
	ES_DECL_INTF_METHOD(bool, hasMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(bool, hasClassMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, callMethod, cr_EsString, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD1(EsVariant, call, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, call, cr_EsString, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD3(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD4(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD5(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD6(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD7(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, callClassMethod, cr_EsString, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, classCall, cr_EsString) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, classCall, cr_EsString, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD3(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD4(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD5(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD6(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD7(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

protected:	
	// EsScriptObjectIntf implementation
	//
	// seal object instance
	ES_DECL_INTF_METHOD(void, seal)() ES_NOTHROW ES_OVERRIDE;
	// create instance of script object from its metaclass instance
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const ES_OVERRIDE;
	// internal object cloning
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, internalClone)(EsScriptObjectIntf* parent, const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const ES_OVERRIDE;
  // Direct access object data buffer
  ES_DECL_INTF_METHOD(EsScriptObjectDataBuffer::Ptr, internalBuffAccess)() ES_NOTHROW ES_OVERRIDE { return m_data; }
  // Direct access to the class attributes
	ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, classAttrsAccess)() const ES_NOTHROW ES_OVERRIDE;
	// assign instance attributes to the instance-type object
	ES_DECL_INTF_METHOD(void, instAttrsAssign)(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW ES_OVERRIDE;
	// internal service meant for script compile time only
	// assign attributes to this metaclass object. use attributes owner name as field name.
	ES_DECL_INTF_METHOD(void, thisFieldAttributesAssign)(const EsAttributesIntf::Ptr& attrs) ES_OVERRIDE;
	// add attribute to the object metaclass or instance
	ES_DECL_INTF_METHOD(void, attributeAddInstOrMetaclass)(const EsString& name, const EsVariant& val, bool metaclass) ES_OVERRIDE;
	ES_DECL_INTF_METHOD(void, addMetaclassAttribute)(const EsString& name, const EsVariant& val) ES_OVERRIDE { attributeAddInstOrMetaclass(name, val, true); }
	ES_DECL_INTF_METHOD(void, addInstanceAttribute)(const EsString& name, const EsVariant& val) ES_OVERRIDE { attributeAddInstOrMetaclass(name, val, false); }
	// add field object to the script object
	ES_DECL_INTF_METHOD(void, fieldAdd)(const EsString& name, const EsScriptObjectIntf::Ptr& field,
		const EsAttributesIntf::Ptr& attrs,	const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()) ES_OVERRIDE;
	// add conditional field object to the script object. conditionals are special unnamed fields to augment dynamic object layout
	ES_DECL_INTF_METHOD(void, fieldConditionalAdd)(const EsScriptObjectIntf::Ptr& field, 
		const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()) ES_OVERRIDE;
	// return array of conditional fields declared for this object
	ES_DECL_INTF_METHOD(const	ObjectsVector&, conditionalFieldsGet)() const ES_NOTHROW ES_OVERRIDE { return m_conditionals; }
	// set script object parent. 
	ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent) ES_OVERRIDE;
	// get script object parent. 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, parentGet)() const ES_NOTHROW ES_OVERRIDE { return m_parent; }
	// access script object ancestor
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, ancestorGet)() const ES_NOTHROW ES_OVERRIDE { return m_ancestor; }
	// call ancestor's method implementation (as if our 'this' was cast to ancestor's 'this')
	ES_DECL_INTF_METHOD(EsVariant, callAncestorMethod)(const EsString& name, const EsVariant& params, const EsString& ancestorTypeName) ES_OVERRIDE;
	// return only owned field count
	ES_DECL_INTF_METHOD(bool, thisHasFields)() const ES_NOTHROW ES_OVERRIDE { return !m_fieldsMap.isEmpty(); }
	// return name of the field by its index in internal collection
	ES_DECL_INTF_METHOD(const EsString&, thisFieldNameGet)(size_t idx) const ES_OVERRIDE { return m_fieldsMap.nameGet(idx); }
	// return field node object by its index in fields collection
	ES_DECL_INTF_METHOD(const EsVariant&, thisFieldGet)(size_t idx) const ES_OVERRIDE { return m_fieldsMap.valueGet(idx); }
	// return owned fields only
	ES_DECL_INTF_METHOD(const	EsStringIndexedMap&, thisFieldsMapGet)() const ES_NOTHROW ES_OVERRIDE { return m_fieldsMap; }
	// read-only access to object's flags
	ES_DECL_INTF_METHOD(esU32, flagsGet)() const ES_NOTHROW ES_OVERRIDE { return m_flags; }
	// update object memory layout. fields and ancestor offsets and|or sizes get updated
	// as a result of this process
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(size_t offs) ES_OVERRIDE;
	ES_DECL_INTF_METHOD(void, internalUpdateFieldsLayout)(size_t offs) ES_OVERRIDE;
	// internal write access to the object's offset value
	ES_DECL_INTF_METHOD(void, internalOffsetSet)(long offs) ES_OVERRIDE;
	// add object value update subscriber
	ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) ES_OVERRIDE;
	// internal data update helper
	ES_DECL_INTF_METHOD(void, internalPublishDataChanged)(const EsScriptObjectIntf::WeakPtrList& subscribers) ES_OVERRIDE;
	// handle data change event notification from subscription publisher
	ES_DECL_INTF_METHOD(void, publishDataChanged)(const EsScriptObjectIntf::WeakPtrList& subscribers) ES_OVERRIDE;
	// internal layout invalidation
	ES_DECL_INTF_METHOD(void, internalInvalidateLayout)(bool propagate) ES_OVERRIDE;
	// access field expression, valid for fields with expressions only, namely, for arrays and conditional fields
	ES_DECL_INTF_METHOD(EsScriptCodeSection::Ptr, thisFieldExprGet)() const ES_NOTHROW ES_OVERRIDE;
	// get execution scope this for proxy objects and array's size expression
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, topNonProxyGet)() ES_NOTHROW ES_OVERRIDE;
	// access This scoped method table, for proxy objects only
	ES_DECL_INTF_METHOD(EsScriptMethodMapPtr, thisScriptedMethodsGet)() const ES_NOTHROW ES_OVERRIDE;
	// return object scripted method keys, with optional search restrictions
	ES_DECL_INTF_METHOD(EsMethodInfoKeysT, scriptedMethodKeysGet)(bool allHierarchy = true) const ES_NOTHROW ES_OVERRIDE;
	// internal binary buffer assignment helper
	ES_DECL_INTF_METHOD(bool, internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end) ES_OVERRIDE;
	// member variables manipulation
	ES_DECL_INTF_METHOD(void, variableDeclare)(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()) ES_OVERRIDE;
	// access _this_ only variables
	ES_DECL_INTF_METHOD(EsScriptSymbolTable::Ptr, thisVariablesGet)() const ES_NOTHROW ES_OVERRIDE { return m_memberVars; }
	// properties declaration and manipulation
	ES_DECL_INTF_METHOD(void, propertyDeclare)(const EsString& name, const EsAttributesIntf::Ptr& attrs,
		const EsString& readerName, const EsString& writerName) ES_OVERRIDE;
	ES_DECL_INTF_METHOD(bool, thisHasScriptedProperty)(const EsString& name) const ES_NOTHROW ES_OVERRIDE;
	// this _only_ scope access
	ES_DECL_INTF_METHOD(EsStringArray, thisVariablesNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsStringArray, thisScriptedPropertiesNamesGet)(bool persistentOnly) const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsStringIndexedMap::Ptr, thisPropertiesMapGet)() const ES_NOTHROW  ES_OVERRIDE { return m_propsMap; }
	ES_DECL_INTF_METHOD(EsScriptMachine*, machineGet)() ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(void, machineSet)(EsScriptMachine* other) ES_OVERRIDE;

public:	
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isMetaclass) ES_NOTHROW ES_OVERRIDE { return ofMetaclass == (m_flags & ofMetaclass); }
	// return true if this object is root one (data buffer owning object, no parent), false otherwise (fields, etc.)
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isRoot) ES_NOTHROW ES_OVERRIDE { return m_parent == 0; }
	// return true if object is final
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isFinal) ES_NOTHROW ES_OVERRIDE { return ofFinal == (m_flags & ofFinal); }
	// return true if object is POD
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isPOD) ES_NOTHROW ES_OVERRIDE { return ofPOD == (m_flags & ofPOD); }
	// return true if object is array
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isArray) ES_NOTHROW ES_OVERRIDE { return ofArray == (m_flags & ofArray); }
	// return true if object is IF conditional
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isIf) ES_NOTHROW ES_OVERRIDE { return ofIf == (m_flags & ofIf); }
	// return true if object is IF-branch conditional
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isIfBranch) ES_NOTHROW ES_OVERRIDE { return ofIfBranch == (m_flags & ofIfBranch); }
	// return true if object is conditional - related (IF or IF-branch)
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isConditional) ES_NOTHROW ES_OVERRIDE { return 0 != (m_flags & (ofIf|ofIfBranch)); }
	// return true if object is invalid (not laid out)
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isInvalid) ES_NOTHROW ES_OVERRIDE { return ofNeedUpdateLayout == (m_flags & ofNeedUpdateLayout); }
	// Return true if this object and all its parents are invalid
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isAllInvalid) ES_NOTHROW ES_OVERRIDE;
	// return object item type name. has meaning for arrays only,
	// other objects will return the same value, as typeNameGet does
	ES_DECL_INTF_METHOD(EsString, itemTypeNameGet)() const ES_NOTHROW ES_OVERRIDE { return typeNameGet(); }
	// return root object for this object
	ES_DECL_INTF_METHOD(const EsScriptObjectIntf*, getRootObject)() const ES_NOTHROW ES_OVERRIDE;
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, getRootObject)() ES_NOTHROW ES_OVERRIDE;
	// return script object absolute offset in bytes from the start of data buffer
	ES_DECL_INTF_METHOD(long, offsGet)() const ES_NOTHROW ES_OVERRIDE { return m_offs; }
	// return script object size in bytes
	ES_DECL_INTF_METHOD(size_t, sizeGet)() const ES_NOTHROW ES_OVERRIDE;
	// get object as binary buffer
	ES_DECL_INTF_METHOD(EsBinBuffer, binBufferGet)() const ES_NOTHROW ES_OVERRIDE;
	// (try to) set existing object from binary buffer
	ES_DECL_INTF_METHOD(void, binBufferSet)(const EsBinBuffer& buff) ES_OVERRIDE;
	// for IF fields only. in instance mode empty pointer is returned
	// for inactive condition, in metaclass mode all conditions are accessible
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, conditionalBranchGet)(int condition) const ES_OVERRIDE;
	// finds the script object method declared in script.
	// if method is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsScriptCodeSection::Ptr, findScriptedMethod)(const EsMethodInfoKeyT& key, bool allHierarchy = true) const ES_NOTHROW ES_OVERRIDE;
	// create new instance of EsScriptObject which is clone of the existing instance.
	// if called from metaclass, new (default values used) object instance is created.
	// may be called from the root objects (parent=NULL) only.
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, clone)() const ES_OVERRIDE;
	// find named field in this object or one of its ancestors. 
	// if field is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, fieldFind)(const EsString& name) const ES_NOTHROW ES_OVERRIDE;
	// check if field exists
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasField, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	// return all field names
	ES_DECL_INTF_METHOD(EsStringArray, fieldNamesGet)() const ES_NOTHROW ES_OVERRIDE;
	// return the first field name. if object contain no fields, empty string is returned
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, firstFieldNameGet) ES_NOTHROW ES_OVERRIDE;
	// return field object name which is next to one specified. if specified name is the last one,
	// the empty string is returned
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, nextFieldNameGet, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	// return field object by name. if field is not found, an exception is thrown
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsBaseIntfPtr, fieldGet, cr_EsString) ES_OVERRIDE;
	// return field attributes by field name
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsBaseIntfPtr, fieldAttrsGet, cr_EsString) ES_OVERRIDE;
	// return true if variable with specified name exists in object or its ancestors
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasVariable, cr_EsString) ES_NOTHROW ES_OVERRIDE;
	// return all variable names
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsStringArray, varNamesGet) ES_NOTHROW ES_OVERRIDE;
	// find member var accessor by name
	ES_DECL_INTF_METHOD(EsScriptValAccessorIntf::Ptr, variableFind)(const EsString& name, bool doThrow) const ES_OVERRIDE;
	// return member variable value
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(cr_EsVariant, variableGet, cr_EsString) ES_OVERRIDE;
	// set member variable value
  ES_DECL_REFLECTED_INTF_METHOD2(void, variableSet, cr_EsString, cr_EsVariant) ES_OVERRIDE;

	// Reflected properties
	//
	// Type name
	ES_DECL_PROPERTY_RO(type, EsString)
	// Size and offset r-o properties
	ES_DECL_PROPERTY_RO(size, ulong)
	ES_DECL_PROPERTY_RO(offset, ulong)
	// Object internals access
	ES_DECL_PROPERTY_RO(attributeNames, EsStringArray)
  ES_DECL_PROPERTY_RO(classAttributeNames, EsStringArray)
	ES_DECL_PROPERTY_RO(fieldNames, EsStringArray)
	ES_DECL_PROPERTY_RO(memberVarNames, EsStringArray)
	ES_DECL_PROPERTY_RO(propertyNames, EsStringArray)
	ES_DECL_PROPERTY_RO(persistentPropertyNames, EsStringArray)
  ES_DECL_PROPERTY_RO(weakReference, EsVariant)
	// Object buffer manipulation
	ES_DECL_PROPERTY(buffer, EsBinBuffer)
  // Return script host, to which this object's context belongs to
  ES_DECL_PROPERTY_RO(__scriptHost, EsVariant)

	// Reflected services
	//
	ES_DECL_REFLECTED_CONST_METHOD2(bool, hasMethod, cr_EsString, ulong) ES_NOTHROW;
	ES_DECL_REFLECTED_CONST_METHOD2(bool, hasClassMethod, cr_EsString, ulong) ES_NOTHROW;
	ES_DECL_REFLECTED_CONST_METHOD0(EsBaseIntfPtr, reflectedClone);

	// Link to specified scripted object's execution context
	ES_DECL_REFLECTED_METHOD1(void, useContextOf, cr_EsBaseIntfPtr);

	// Field node crackers
	static EsScriptObjectIntf::Ptr fnodeFieldGet(const EsVariant& fnode);
	static EsAttributesIntf::Ptr fnodeAttrsGet(const EsVariant& fnode);

private:
  EsScriptObject() ES_REMOVEDECL;
  EsScriptObject(const EsScriptObject&) ES_REMOVEDECL;
  EsScriptObject& operator=(const EsScriptObject&) ES_REMOVEDECL;

protected:
	// internal services
	//
	// internal fnode finder service
	EsVariant fnodeFind(const EsString& name) const ES_NOTHROW;
	// notify parent object on value change
	void onSetValueParentNotify();
	// value validator (meningful for POD objects only)
	virtual void validate(const EsVariant& val) const;
	// variable copy by value helper service
	EsVariant varCopy(const EsVariant& src) const;
	// misc exceptions
	void throwNoAttributeTable() const;

protected:
	// data members
	//
	// script machine in context of which this object exists
	EsScriptContext::Ptr m_ctx;
	// script object methods map
	EsScriptMethodMapPtr m_methods;
	// ancestor object pointer. ancestor is the object, the current one is derived from
	EsScriptObjectIntf::Ptr m_ancestor;
	// parent object pointer. parent is the object which holds this object as field
	EsScriptObjectIntf* m_parent;
	// list of weak references to update subscribers
	EsScriptObjectIntf::WeakPtrList m_updateSubscribers;

	// script object data buffer
	EsScriptObjectDataBufferPtr m_data;
	// attribute containers
	//
	// class attributes - shared among all instances of this class
	EsAttributesIntf::Ptr m_attrsClass;
	// Reference to the object's instance attributes. Has meaning only for fields
	EsAttributesIntf::Ptr m_attrsInst;

	/// Field container. fields are kept along with their 'instance' attributes as pairs:
	/// Field metaclass|object is at idx=0, instance attributes at idx=1
	EsStringIndexedMap m_fieldsMap;
	ObjectsVector m_conditionals;
	// member variables
	EsScriptSymbolTable::Ptr m_memberVars;
	// properties container
	EsStringIndexedMap::Ptr m_propsMap;

	// script object type name. is defined on script compile time, apart from class name,
	// which is always a C++ reflected class name.
	//
	EsString m_typeName;

	// Script object flags
	esU32 m_flags;

	// object size and absolute offset, in bytes
	size_t m_size;
	long m_offs;
	
	friend class EsScriptMachine;
	friend class EsScriptMetaclass;
};
//---------------------------------------------------------------------------

/// "Fast" traversers specific helpers
///
class ESSCRIPT_CLASS EsScriptObjectFieldFastFinder : public EsScriptObjectFastFindBase
{
public:
	EsScriptObjectFieldFastFinder(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW;
	const EsVariant& resultGet() ES_NOTHROW;
	bool found() ES_NOTHROW;

protected:
	// overridable services
	virtual bool objectProcess() ES_NOTHROW;

protected:
	bool m_found;
	EsString m_name;
	EsVariant m_result;

private:
	EsScriptObjectFieldFastFinder() ES_REMOVEDECL;
	EsScriptObjectFieldFastFinder(const EsScriptObjectFieldFastFinder&) ES_REMOVEDECL;
	const EsScriptObjectFieldFastFinder& operator=(const EsScriptObjectFieldFastFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

class ESSCRIPT_CLASS EsScriptObjectVarFastFinder : public EsScriptObjectFastFindBase
{
public:
	EsScriptObjectVarFastFinder(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW;
	EsScriptValAccessorIntf::Ptr resultGet() ES_NOTHROW;
	bool found() ES_NOTHROW;

protected:
	// overridable services
	virtual bool objectProcess() ES_NOTHROW;

protected:
	EsString m_name;
	bool m_found;
	EsScriptValAccessorIntf::Ptr m_result;

private:
	EsScriptObjectVarFastFinder() ES_REMOVEDECL;
	EsScriptObjectVarFastFinder(const EsScriptObjectVarFastFinder&) ES_REMOVEDECL;
	const EsScriptObjectVarFastFinder& operator=(const EsScriptObjectVarFastFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

class ESSCRIPT_CLASS EsScriptObjectMethodFastFinder : public EsScriptObjectFastFindBase
{
public:
	EsScriptObjectMethodFastFinder(const EsScriptObjectIntf* obj, const EsMethodInfoKeyT& key) ES_NOTHROW;
	EsScriptCodeSection::Ptr resultGet() ES_NOTHROW;

protected:
	// overridable services
	virtual bool objectProcess() ES_NOTHROW;

protected:
	EsMethodInfoKeyT m_key;
	EsScriptCodeSection::Ptr m_result;

private:
	EsScriptObjectMethodFastFinder() ES_REMOVEDECL;
	EsScriptObjectMethodFastFinder(const EsScriptObjectMethodFastFinder&) ES_REMOVEDECL;
	const EsScriptObjectMethodFastFinder& operator=(const EsScriptObjectMethodFastFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

class ESSCRIPT_CLASS EsScriptObjectPropFastFinder : public EsScriptObjectFastFindBase
{
public:
	EsScriptObjectPropFastFinder(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW;
	bool found() ES_NOTHROW;

protected:
	// overridable services
	virtual bool objectProcess() ES_NOTHROW;

protected:
	EsString m_name;
	bool m_result;

private:
	EsScriptObjectPropFastFinder() ES_REMOVEDECL;
	EsScriptObjectPropFastFinder(const EsScriptObjectPropFastFinder&) ES_REMOVEDECL;
	const EsScriptObjectPropFastFinder& operator=(const EsScriptObjectPropFastFinder&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

#ifdef ESSCRIPT_OBJECT_USE_TRACE
#	define ESSCRIPT_OBJECT_TRACE1(a)										ES_DEBUG_TRACE(a);
#	define ESSCRIPT_OBJECT_TRACE2(a, b)									ES_DEBUG_TRACE((a), (b));
#	define ESSCRIPT_OBJECT_TRACE3(a, b, c)							ES_DEBUG_TRACE((a), (b), (c));
#	define ESSCRIPT_OBJECT_TRACE4(a, b, c, d)						ES_DEBUG_TRACE((a), (b), (c), (d));
#	define ESSCRIPT_OBJECT_TRACE5(a, b, c, d, e)				ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#	define ESSCRIPT_OBJECT_TRACE6(a, b, c, d, e, f)			ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#	define ESSCRIPT_OBJECT_TRACE7(a, b, c, d, e, f, j)	ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f), (j));
#else
#	define ESSCRIPT_OBJECT_TRACE1(a)
#	define ESSCRIPT_OBJECT_TRACE2(a, b)
#	define ESSCRIPT_OBJECT_TRACE3(a, b, c)
#	define ESSCRIPT_OBJECT_TRACE4(a, b, c, d)
#	define ESSCRIPT_OBJECT_TRACE5(a, b, c, d, e)
#	define ESSCRIPT_OBJECT_TRACE6(a, b, c, d, e, f)
#	define ESSCRIPT_OBJECT_TRACE7(a, b, c, d, e, f, j)
#endif

#endif // _es_script_object_h_
