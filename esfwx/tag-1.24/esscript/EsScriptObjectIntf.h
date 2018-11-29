#ifndef _es_script_object_intf_h_
#define _es_script_object_intf_h_

// fwd decl
class EsScriptObjectIntf;

// script object internals traversers
// NB! "normal" traversers are OK for run-time usage, while
// "fast" ones meant for compile time only
//
class ESSCRIPT_CLASS EsScriptObjectTraverserBase
{
protected:
	EsScriptObjectTraverserBase(const EsScriptObjectIntf* obj, bool enumerateFields, bool allHierarchy) ES_NOTHROW;
	void onObject(const EsScriptObjectIntf* obj) ES_NOTHROW;
	void onEnumerateFields(const EsScriptObjectIntf* obj) ES_NOTHROW;

	// overridable services
	virtual bool objectProcess(const EsScriptObjectIntf* obj) ES_NOTHROW { return true; }
	virtual bool fieldProcess(const EsScriptObjectIntf* obj, const EsString& name) ES_NOTHROW { return true; }

protected:
	const EsScriptObjectIntf* m_obj;
	bool m_continue;
	bool m_enumerateFields;
	bool m_allHierarchy;
	
private:
	EsScriptObjectTraverserBase() ES_REMOVEDECL;
	EsScriptObjectTraverserBase(const EsScriptObjectTraverserBase&) ES_REMOVEDECL;
	EsScriptObjectTraverserBase& operator=(const EsScriptObjectTraverserBase&) ES_REMOVEDECL;
};

class ESSCRIPT_CLASS EsScriptObjectTopDownHierarchyTraverser : public EsScriptObjectTraverserBase
{
public:
	EsScriptObjectTopDownHierarchyTraverser(const EsScriptObjectIntf* obj, bool enumerateFields, bool allHierarchy) ES_NOTHROW;
	void traverse() ES_NOTHROW;
	
protected:
	void doEnterAncestor(const EsScriptObjectIntf* obj) ES_NOTHROW;
	
private:
	EsScriptObjectTopDownHierarchyTraverser() ES_REMOVEDECL;
	EsScriptObjectTopDownHierarchyTraverser(const EsScriptObjectTopDownHierarchyTraverser&) ES_REMOVEDECL;
	const EsScriptObjectTopDownHierarchyTraverser& operator=(const EsScriptObjectTopDownHierarchyTraverser&) ES_REMOVEDECL;
};

class ESSCRIPT_CLASS EsScriptObjectDownTopHierarchyTraverser : public EsScriptObjectTraverserBase
{
public:
	EsScriptObjectDownTopHierarchyTraverser(const EsScriptObjectIntf* obj, bool enumerateFields, bool allHierarchy) ES_NOTHROW;
	void traverse() ES_NOTHROW;

private:
	EsScriptObjectDownTopHierarchyTraverser() ES_REMOVEDECL;
	EsScriptObjectDownTopHierarchyTraverser(const EsScriptObjectDownTopHierarchyTraverser&) ES_REMOVEDECL;
	const EsScriptObjectDownTopHierarchyTraverser& operator=(const EsScriptObjectDownTopHierarchyTraverser&) ES_REMOVEDECL;
};

// special 'fast' finder helper class base - for use only
// during metaclass compilation|object cloning, perform object search
// from active branch-up only
class ESSCRIPT_CLASS EsScriptObjectFastFindBase
{
protected:
	EsScriptObjectFastFindBase(const EsScriptObjectIntf* obj) ES_NOTHROW;
	void find() ES_NOTHROW;
	void onObject(const EsScriptObjectIntf* obj) ES_NOTHROW;

	// overridable services
	virtual bool objectProcess() ES_NOTHROW { return true; }

protected:
	const EsScriptObjectIntf* m_startObj;
	const EsScriptObjectIntf* m_obj;
	bool m_continue;

private:
	EsScriptObjectFastFindBase() ES_REMOVEDECL;
	EsScriptObjectFastFindBase(const EsScriptObjectFastFindBase&) ES_REMOVEDECL;
	const EsScriptObjectFastFindBase& operator=(const EsScriptObjectFastFindBase&) ES_REMOVEDECL;
};

/// Script object interface definition
///
ES_DECL_INTF_BEGIN2( 0682FC7E, 05CE43FA, B30D7199, A5FBEBF0, EsScriptObjectIntf, EsBaseIntf)
public:
	// object container types
	typedef std::vector<EsScriptObjectIntf::Ptr> ObjectsVector;
	// weak interface references list
	typedef EsScriptObjectIntf* WeakPtr;
	typedef std::list<WeakPtr> WeakPtrList;

public:
	// internal usage only methods
	//
	// seal object instance
	ES_DECL_INTF_METHOD(void, seal)() ES_NOTHROW = 0;
	// create instance of script object. 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, objectCreate)(const EsScriptObjectDataBufferPtr& buff, bool splitCtx) const = 0;
	// internal-usage object cloning 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, internalClone)(EsScriptObjectIntf* parent, const EsScriptObjectDataBufferPtr& buff, bool splitCtx = false) const = 0;
  // Direct access object data buffer
  ES_DECL_INTF_METHOD(EsScriptObjectDataBuffer::Ptr, internalBuffAccess)() ES_NOTHROW = 0;
  // Direct access class attributes
	ES_DECL_INTF_METHOD(EsAttributesIntf::Ptr, classAttrsAccess)() const ES_NOTHROW = 0;
	// assign instance attributes to the instance-type object
	ES_DECL_INTF_METHOD(void, instAttrsAssign)(const EsAttributesIntf::Ptr& attrs) ES_NOTHROW = 0;
	// internal service meant for script compile time only
	// assign attributes to this metaclass object. use attributes owner name as field name.
	ES_DECL_INTF_METHOD(void, thisFieldAttributesAssign)(const EsAttributesIntf::Ptr& attrs) = 0;
	// set script object parent.
	ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent) = 0;
	// add attribute to the object metaclass or instance
	ES_DECL_INTF_METHOD(void, attributeAddInstOrMetaclass)(const EsString& name, const EsVariant& val, bool metaclass) = 0;
	ES_DECL_INTF_METHOD(void, addMetaclassAttribute)(const EsString& name, const EsVariant& val) = 0;
	ES_DECL_INTF_METHOD(void, addInstanceAttribute)(const EsString& name, const EsVariant& val) = 0;
	// add field object to the script object
	ES_DECL_INTF_METHOD(void, fieldAdd)(const EsString& name, const EsScriptObjectIntf::Ptr& field, 
		const EsAttributesIntf::Ptr& attrs, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()) = 0;
	// add conditional field object to the script object. conditionals are special unnamed fields to augment dynamic object layout
	ES_DECL_INTF_METHOD(void, fieldConditionalAdd)(const EsScriptObjectIntf::Ptr& field, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()) = 0;
	// return array of conditional fields declared for this object
	ES_DECL_INTF_METHOD(const	ObjectsVector&, conditionalFieldsGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(bool, thisHasFields)() const ES_NOTHROW = 0;
	// access script object ancestor
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, ancestorGet)() const ES_NOTHROW = 0;
	// call ancestor's method implementation (as if our 'this' was cast to ancestor's 'this')
	ES_DECL_INTF_METHOD(EsVariant, callAncestorMethod)(const EsString& name, const EsVariant& params, const EsString& ancestorTypeName) = 0;
	// return array of owned fields only
	ES_DECL_INTF_METHOD(const	EsStringIndexedMap&, thisFieldsMapGet)() const ES_NOTHROW = 0;
	// return name of the field by its index in internal collection
	ES_DECL_INTF_METHOD(const EsString&, thisFieldNameGet)(ulong idx) const = 0;	
	// access this field node by its index
	ES_DECL_INTF_METHOD(const EsVariant&, thisFieldGet)(ulong idx) const = 0;
	// read-only access to object's flags
	ES_DECL_INTF_METHOD(esU32, flagsGet)() const ES_NOTHROW = 0;
	// update object memory layout. fields and ancestor offsets and|or sizes get updated
	// as a result of this process
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(ulong offs) = 0;
	ES_DECL_INTF_METHOD(void, internalUpdateFieldsLayout)(ulong offs) = 0;
	// internal data update helper
	ES_DECL_INTF_METHOD(void, internalPublishDataChanged)(const EsScriptObjectIntf::WeakPtrList& subscribers) = 0;
	// handle data change event notification from subscription publisher
	ES_DECL_INTF_METHOD(void, publishDataChanged)(const EsScriptObjectIntf::WeakPtrList& subscribers) = 0;
	// internal write access to the object's offset value
	ES_DECL_INTF_METHOD(void, internalOffsetSet)(long offs) = 0;
	// add object value update subscriber
	ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) = 0;
	// internal data layout invalidation
	ES_DECL_INTF_METHOD(void, internalInvalidateLayout)(bool propagate) = 0;
	// access field expression, valid for fields with expression only, namely, for arrays and conditional fields
	ES_DECL_INTF_METHOD(EsScriptCodeSection::Ptr, thisFieldExprGet)() const ES_NOTHROW = 0;
	// access This scoped method table, for proxy objects only
	ES_DECL_INTF_METHOD(EsScriptMethodMapPtr, thisScriptedMethodsGet)() const ES_NOTHROW = 0;
	// internal binary buffer assignment helper
	ES_DECL_INTF_METHOD(bool, internalBinBufferSet)(EsBinBuffer::const_pointer& pos, EsBinBuffer::const_pointer end) = 0;
	// member variables manipulation
	ES_DECL_INTF_METHOD(void, variableDeclare)(const EsString& name, 
		const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()) = 0;
	// access _this_ only variables
	ES_DECL_INTF_METHOD(EsScriptSymbolTable::Ptr, thisVariablesGet)() const ES_NOTHROW = 0;
	// properties declaration and manipulation
	ES_DECL_INTF_METHOD(void, propertyDeclare)(const EsString& name, const EsAttributesIntf::Ptr& attrs,
		const EsString& readerName, const EsString& writerName) = 0;
	// this _only_ scope access
	ES_DECL_INTF_METHOD(EsStringArray, thisVariablesNamesGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsStringIndexedMap::Ptr, thisPropertiesMapGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsStringArray, thisScriptedPropertiesNamesGet)(bool persistentOnly) const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(bool, thisHasScriptedProperty)(const EsString& name) const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsScriptMachine*, machineGet)() ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(void, machineSet)(EsScriptMachine*) = 0;

	// generic usage methods
	//
	// return true if this object is template, i.e. metaclass, which is used to create instances of that object
	ES_DECL_INTF_METHOD(bool, isMetaclass)() const ES_NOTHROW = 0;
	// return true if this object is root one (data buffer owning object, no parent), false otherwise (fields, etc.)
	ES_DECL_INTF_METHOD(bool, isRoot)() const ES_NOTHROW = 0;
	// return true if object is final
	ES_DECL_INTF_METHOD(bool, isFinal)() const ES_NOTHROW = 0;
	// return true if object is array
	ES_DECL_INTF_METHOD(bool, isArray)() const ES_NOTHROW = 0;
	// return true if object is IF conditional
	ES_DECL_INTF_METHOD(bool, isIf)() const ES_NOTHROW = 0;
	// return true if object is IF-branch conditional
	ES_DECL_INTF_METHOD(bool, isIfBranch)() const ES_NOTHROW = 0;
	// return true if object is conditional - related (IF or IF-branch)
	ES_DECL_INTF_METHOD(bool, isConditional)() const ES_NOTHROW = 0;
	// return true if object is POD
	ES_DECL_INTF_METHOD(bool, isPOD)() const ES_NOTHROW = 0;
	// return true if object is invalid (not laid out)
	ES_DECL_INTF_METHOD(bool, isInvalid)() const ES_NOTHROW = 0;
	// Return true if this object and all its parents are invalid
	ES_DECL_INTF_METHOD(bool, isAllInvalid)() const ES_NOTHROW = 0;
	// return object item type name. has meaning for arrays only, 
	// other objects will return the same value, as typeNameGet does
	ES_DECL_INTF_METHOD(EsString, itemTypeNameGet)() const ES_NOTHROW = 0;
	// get script object's parent. 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, parentGet)() const ES_NOTHROW = 0;
	// return root object for this object
	ES_DECL_INTF_METHOD(const EsScriptObjectIntf*, getRootObject)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, getRootObject)() ES_NOTHROW = 0;
	// get execution scope 'this' for proxy objects and array's size expression
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, topNonProxyGet)() ES_NOTHROW = 0;
	// return script object absolute offset in bytes from the start of data buffer
	ES_DECL_INTF_METHOD(long, offsGet)() const ES_NOTHROW = 0;
	// return script object size in bytes
	ES_DECL_INTF_METHOD(ulong, sizeGet)() const ES_NOTHROW = 0;
	// get object as binary buffer
	ES_DECL_INTF_METHOD(EsBinBuffer, binBufferGet)() const ES_NOTHROW = 0;
	// (try to) set existing object from binary buffer
	ES_DECL_INTF_METHOD(void, binBufferSet)(const EsBinBuffer& buff) = 0;
	// for IF fields only. in instance mode empty pointer is returned
	// for inactive condition, in metaclass mode all conditions are accessible
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, conditionalBranchGet)(int condition) const = 0;
	// check attribute existence
	ES_DECL_INTF_METHOD(bool, hasAttribute)(const EsString& name) const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(bool, hasClassAttribute)(const EsString& name) const ES_NOTHROW = 0;
	// get all assigned attribute names
	ES_DECL_INTF_METHOD(EsStringArray, attributeNamesGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsStringArray, classAttributeNamesGet)() const ES_NOTHROW = 0;
	// get attribute value
	ES_DECL_INTF_METHOD(EsVariant, attributeGet)(const EsString& name) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, classAttributeGet)(const EsString& name) const = 0;
	// find the script object method declared in script.
	// if method is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsScriptCodeSection::Ptr, findScriptedMethod)(const EsMethodInfoKeyT& key, bool allHierarchy = true) const ES_NOTHROW = 0;
	// return object scripted method keys, with optional search restrictions
	ES_DECL_INTF_METHOD(EsMethodInfoKeysT, scriptedMethodKeysGet)(bool allHierarchy = true) const ES_NOTHROW = 0;
	// create new instance of EsScriptObject which is clone of the existing instance.
	// if called from metaclass, new (default values used) object instance is created.
	// may be called from the root objects (parent=NULL) only.
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, clone)() const = 0;
	// find named field in this object or one of its ancestors. 
	// if field is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, fieldFind)(const EsString& name) const ES_NOTHROW = 0;
  // check if field exists
  ES_DECL_INTF_METHOD(bool, hasField)(const EsString& name) const ES_NOTHROW = 0;
	// return all field names
	ES_DECL_INTF_METHOD(EsStringArray, fieldNamesGet)() const ES_NOTHROW = 0;
	// return the first field name. if object contain no fields, empty string is returned
	ES_DECL_INTF_METHOD(EsString, firstFieldNameGet)() const ES_NOTHROW = 0;
	// return field object name which is next to one specified. if specified name is the last one,
	// the empty string is returned
	ES_DECL_INTF_METHOD(EsString, nextFieldNameGet)(const EsString& name) const ES_NOTHROW = 0;
	// return field object by name
	ES_DECL_INTF_METHOD(EsBaseIntfPtr, fieldGet)(const EsString& name) const = 0;
	// return field attributes by field name
	ES_DECL_INTF_METHOD(EsBaseIntfPtr, fieldAttrsGet)(const EsString& name) const = 0;
	// return true if variable with specified name exists in object or its ancestors
	ES_DECL_INTF_METHOD(bool, hasVariable)(const EsString& name) const ES_NOTHROW = 0;
	// return all variable names
	ES_DECL_INTF_METHOD(EsStringArray, varNamesGet)() const ES_NOTHROW = 0;
	// find member var accessor by name
	ES_DECL_INTF_METHOD(EsScriptValAccessorIntf::Ptr, variableFind)(const EsString& name, bool doThrow) const = 0;
	// return member variable value
	ES_DECL_INTF_METHOD(const EsVariant&, variableGet)(const EsString& name) const = 0;
	// set member variable value
	ES_DECL_INTF_METHOD(void, variableSet)(const EsString& name, const EsVariant& val) = 0;
ES_DECL_INTF_END

#endif // _es_script_object_intf_h_
