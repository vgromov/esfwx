#ifndef _es_script_machine_h_
#define _es_script_machine_h_

// temporary value accessor. it's special because internally it's tightly coupled with EsScriptMachine
// which owns it through lifetime
//
class EsTmpAccessor : public EsAccessorIntf
{
protected:
	EsTmpAccessor(const EsVariant& val, EsScriptMachine& owner);
	
public:
	ES_INTF_MAP_BEGIN(EsTmpAccessor)
		ES_INTF_SUPPORTS(EsTmpAccessor, EsAccessorIntf)
	ES_INTF_MAP_END;
	
	// EsBaseIntf override
	//
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
	{ 
		return getClassName(); 
	}

	// interface impl
	ES_DECL_INTF_METHOD(EsVariant, get)() const;
	ES_DECL_INTF_METHOD(EsVariant&, acc)();
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val);

protected:
	// data members
	EsScriptMachine& m_owner;
	EsVariant m_val; // const value
	
	friend class EsScriptMachine;	
};

// script methods map
typedef std::map<MethodInfoKeyT, EsCodeSection::Ptr> EsMethodMap;
typedef boost::shared_ptr<EsMethodMap> EsMethodMapPtr;
typedef containers::structured_map<EsString, EsMethodMapPtr> EsCodeMap;

// script object interface definition
//
ES_DECL_INTF_BEGIN2( 0682FC7E, 05CE43FA, B30D7199, A5FBEBF0, EsScriptObjectIntf, EsReflectedClassIntf)
public:
	typedef boost::shared_ptr<EsBinBuffer> EsSharedBinBufferPtr;
	
protected:
	// internal usage only methods
	//
	// seal object instance
	ES_DECL_INTF_METHOD(void, seal)() = 0;
	// create instance of script object. 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, createObject)(EsSharedBinBufferPtr buff) const = 0;
	// internal-usage object cloning 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, internalClone)(EsSharedBinBufferPtr buff) const = 0;
	// set script object parent. 
	ES_DECL_INTF_METHOD(void, setParent)(EsScriptObjectIntf* parent) = 0;
	// get script object parent. 
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, getParent)() const = 0;
	// add attribute to the object metaclass or instance
	ES_DECL_INTF_METHOD(void, addAttribute)(const EsString& name, const EsVariant& val) = 0;
	// get attribute value accessor
	ES_DECL_INTF_METHOD(EsValAccessor::Ptr, internalGetAttribute)(const EsString& name) const = 0;
	// add field object to the script object
	ES_DECL_INTF_METHOD(void, addField)(const EsString& name, EsScriptObjectIntf::Ptr field) = 0;
	// access script object ancestor
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, getAncestor)() const = 0;
	// call ancestor's method implementation (as if our 'this' was cast to ancestor's 'this')
	ES_DECL_INTF_METHOD(EsVariant, callAncestorMethod)(const EsString& name, const EsVariant& params, const EsString& ancestorTypeName) = 0;
	// return only owned field count
	ES_DECL_INTF_METHOD(size_t, internalGetFieldCount)() const = 0;
	// return name of the field by its index in internal collection
	ES_DECL_INTF_METHOD(const EsString&, internalGetFieldName)(size_t idx) const = 0;
	// read-only access to object's flags
	ES_DECL_INTF_METHOD(DWORD, getFlags)() const = 0;
	// update object memory layout. fields and ancestor offsets and|or sizes get updated
	// as a result of this process
	ES_DECL_INTF_METHOD(void, internalUpdateLayout)(size_t offs) = 0;
	ES_DECL_INTF_METHOD(void, internalUpdateFieldsLayout)(size_t offs) = 0;
	// internal write access to the object's offset value
	ES_DECL_INTF_METHOD(void, internalSetOffset)(size_t offs) = 0;
	// add object value update subscriber
	ES_DECL_INTF_METHOD(void, internalAddUpdateSubscriber)(EsScriptObjectIntf* subscriber) = 0;
	// internal data update helpers
	ES_DECL_INTF_METHOD(void, internalOnDataChanged)(EsScriptObjectIntf* sender) = 0;
	// internal data layout invalidation
	ES_DECL_INTF_METHOD(void, internalInvalidateLayout)() = 0;
		
public:
	// return true if this object is root one (data buffer owning object, no parent), false otherwise (fields, etc.)
	ES_DECL_INTF_METHOD(bool, isRoot)() const = 0;
	// return true if object is final
	ES_DECL_INTF_METHOD(bool, isFinal)() const = 0;
	// return true if object is array
	ES_DECL_INTF_METHOD(bool, isArray)() const = 0;
	// return true if object is POD
	ES_DECL_INTF_METHOD(bool, isPOD)() const = 0;
	// return root object for this object
	ES_DECL_INTF_METHOD(const EsScriptObjectIntf*, getRootObject)() const = 0;
	ES_DECL_INTF_METHOD(EsScriptObjectIntf*, getRootObject)() = 0;	
	// return script object absolute offset in bytes from the start of data buffer
	ES_DECL_INTF_METHOD(size_t, getOffs)() const = 0;
	// return script object size in bytes
	ES_DECL_INTF_METHOD(size_t, getSize)() const = 0;
	// check attribute existence
	ES_DECL_INTF_METHOD(bool, attributeExists)(const EsString& name) const = 0;
	// get all assigned attribute names
	ES_DECL_INTF_METHOD(EsStringArray, getAttributeNames)() const = 0;
	// get attribute value
	ES_DECL_INTF_METHOD(EsVariant, getAttribute)(const EsString& name) const = 0;
	// find the script object method declared in script.
	// if method is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsCodeSection::Ptr, findScriptedMethod)(const MethodInfoKeyT& key) const = 0;
	// create new instance of EsScriptObject which is clone of the existing instance.
	// if called from metaclass, new (default values used) object instance is created.
	// may be called from the root objects (parent=NULL) only.
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, clone)() const = 0;
	// find named field in this object or one of its ancestors. 
	// if field is not found, the empty pointer is returned
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, findField)(const EsString& name) const = 0;	
	// return all field names
	ES_DECL_INTF_METHOD(EsStringArray, getFieldNames)() const = 0;
	// return field count
	ES_DECL_INTF_METHOD(ulong, getFieldCount)() const = 0;
	// return field object by name
	ES_DECL_INTF_METHOD(EsScriptObjectIntf::Ptr, getField)(const EsString& name) const = 0;
	// handle data change event notification from subscription publisher
	ES_DECL_INTF_METHOD(void, onDataChanged)(EsScriptObjectIntf* sender) = 0;	
	
	friend class EsScriptObject;
	friend class EsScriptArrayObject;
	friend class EsScriptMachine;
ES_DECL_INTF_END

// script object metaclass map
typedef containers::structured_map<EsString, EsScriptObjectIntf::Ptr> EsScriptMetaclassMap;

// ekosf script virtual machine
class EKOSF_SCRIPT_CLASS EsScriptMachine
{
public:
	// misc consts
	enum {
		defCallStackDepth = 256,
		defDataStackDepth = 64,
		defTmpPoolSize = 32,
		// variable type
		vtLocal = 0,
		vtGlobal = 1,
	};
	// data stack vector
	typedef std::vector<EsAccessorIntf::Ptr> EsDataStack;
	// unnamed constants map
	typedef std::map<EsVariant, EsAccessorIntf::Ptr> EsUnnamedMap;
	// global namespace name
	static const EsString::const_pointer s_global;

protected:
	// only accessible to script friend class
	EsScriptMachine();
	~EsScriptMachine();
	
public:
	// return true if script machine is in destruction stage
	bool isDestroying() const { return m_destroying; }
	// reset virtual machine to uninitialized state
	void reset();
	// rewind virtual machine to pre-start state
	void rewind();
	// execute entire script, beginning from startup code
	EsVariant exec();
	// call script non-object method given name and parameters
	EsVariant call(const EsString& name, const EsString& objectOrNamespace = s_global);
	EsVariant call(const EsString& name, const EsVariant& p0, const EsString& objectOrNamespace = s_global);	
	EsVariant call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsString& objectOrNamespace = s_global);
	EsVariant call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsString& objectOrNamespace = s_global);	
	EsVariant call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsString& objectOrNamespace = s_global);
	EsVariant call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsString& objectOrNamespace = s_global);
	EsVariant call(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsVariant& p5, const EsString& objectOrNamespace = s_global);
	EsVariant callFunction(const EsString& name, const EsVariant& params, const EsString& objectOrNamespace = s_global);
	EsVariant callExtFunction(const EsString& name, const EsVariant& params, const EsString& nameSpace);
	// misc
	//
	// return machine locale
	const std::locale& loc() const;
	// r-o access to constants
	const EsSymTable& consts() const;
	// compile time operations
	//
	// declare constant
	void declConst(const EsString& name, const EsVariant& val);
	// add new unnamed const and return its accessor. if such const already declared, return its accessor
	EsAccessorIntf::Ptr declUnnamed(const EsVariant& val);
	// specific standalone accessor, keeping empty constant value
	EsAccessorIntf::Ptr emptyVal() const;
	
	// code section AKA script method selection
	//
	// create new named code section AKA script method
	EsCodeSection::Ptr createMethod(const EsString& name, const EsString& namespaceOrObject = s_global);	
	// add method to the method table. NB! the method section properties,
	// i.e. name & input parameter count, must be fully defined at the moment 
	// of this call
	void addMethod(EsCodeSection::Ptr section);
	// return object or namespace methods
	EsMethodMapPtr getMethodTemplates(const EsString& namespaceOrObject = s_global, bool doThrow = true) const;
	// get section by key
	EsCodeSection::Ptr getMethodTemplate(const MethodInfoKeyT& key, const EsString& namespaceOrObject, bool doThrow = true) const;
	// get all sections keys
	MethodInfoKeysT getAllMethodsKeys(const EsString& namespaceOrObject = s_global) const;
	// return script method key given method code section ptr
	static MethodInfoKeyT constructMethodKey(EsCodeSection::Ptr code);
	// try to set current script method by key. 
	// if method is not found, an exception is thrown
	void setActiveMethodTemplate(const MethodInfoKeyT& key, const EsString& namespaceOrObject = s_global);
	// !!!semi-private method!!! just set current script code as-is. no checks performed. 
	void setActiveMethod(EsCodeSection::Ptr code);
	// set global startup section as current section
	void setActiveStartupTemplate();
	// return currently active code section
	EsCodeSection::Ptr getActiveMethod();
	// return global startup code section
	EsCodeSection::Ptr getStartup();

	// script metaclasses manipulation
	//
	// find script object metaclass by name, throw exception if metaclass is not found
	EsScriptObjectIntf::Ptr findMetaclass(const EsString& name) const;
	// create new script object metaclass by name, optionally using baseName metaclass as its ancestor, 
	// throw exception if baseName metaclass is not found, or metaclass 'name' already exists 
	EsScriptObjectIntf::Ptr declareMetaclass(const EsString& name, const EsString& baseName);
	// declare simple|array metaclass field
	EsScriptObjectIntf::Ptr declareMetaclassField(const EsString& metaclassName, const EsString& fieldType, const EsString& fieldName);
	EsScriptObjectIntf::Ptr declareMetaclassArrayField(const EsString& metaclassName, const EsString& fieldType, 
		const EsString& fieldName, EsCodeSection::Ptr sizeExpr);	
	// declare attribute for the given object
	void declareObjectAttribute(EsScriptObjectIntf::Ptr obj, const EsString& attrName, const EsVariant& attrVal);
	// create and return object instance by object name.
	EsReflectedClassIntf::Ptr createObject(const EsString& name, const EsVariant& params);
	
	// state machine stuff
	//
	// stack depth monitors
	size_t getCallStackDepth() const;	
	size_t getDataStackDepth() const;
	size_t getTmpPoolSize() const;

protected:
	// internal services	
	//
	static void throwMetaclassAlreadyExists(const EsString& name);
	static void throwMetaclassDoesNotExist(const EsString& name);
	// initialization stuff
	//
	// register base script POD (Plain Old Data) metaclasses
	void registerMetaclass(EsScriptObjectIntf::Ptr metaclass);
	void registerPodMetaclasses();	
	// execution stuff
	EsInstructionOpcode exec(EsInstructions::const_iterator beg, EsInstructions::const_iterator end);
	EsAccessorIntf::Ptr exec(EsCodeSection::Ptr cs, const EsVariant& params, EsScriptObjectIntf* This = 0);
	// indexed (offset from the top) access to the data stack
	EsAccessorIntf::Ptr getDataAtOffs(size_t offs) const;
	void setDataAtOffs(size_t offs, EsAccessorIntf::Ptr val);
	// call stack manipulation
	void pushCall(EsCodeSection::Ptr cs);
	EsCodeSection::Ptr popCall();
	// data stack manipulation
	void pushDataValue(EsAccessorIntf::Ptr acc);
	EsAccessorIntf::Ptr popDataValue();	
	// temporary values pool support
	EsAccessorIntf::Ptr createTemporaryVal(const EsVariant& tmp);
	// execution helpers
	//
	// load variable reference to the data stack
	void doLoadVarRef(const EsInstruction& instr);
	// construct indexed item accessor and push it onto data stack
	void doConstructItemAccessor();
	// construct object field accessor and push it onto data stack
	void doConstructFieldAccessor(const EsInstruction& instr);
	// access attribute value
	void doAccessAttributeValue(const EsInstruction& instr);
	// perform script function call
	void doCall(const EsInstruction& instr);
	// perform binary operation expression using specified opid
	void doBinOp(long opid);
	// perform unary operator expression, using specified opid
	void doUnaryOp(long opid);
	// perform assignment using specified opcode and opid
	void doAsnOp(const EsInstruction& instr);
	// perform incdec execution
	void doIncDec(const EsInstruction& instr);
	// do short-cirquit logic operation evaluation checking
	void doLogicCheck(EsInstructions::const_iterator& instr);	
	// perform jump instruction. return true if jump was handled
	bool doJump(EsInstructions::const_iterator& instr);
	// debug trace
	//
public:
	void traceInstruction(EsInstructions::const_iterator instr);
	void traceDataStack() const;
	static EsString traceVariant(const EsVariant& v);
	
protected:
	// locale used for compilation|execution
	const std::locale& m_loc;	
	// special empty constant value accessor
	EsConstAccessor m_empty;
	// constants
	EsSymTable m_consts;
	// unnamed constants (values, ref-ed by their string representation)
	EsUnnamedMap m_unnamed;
	// script code mapped by namespaces and objects
	EsCodeMap m_code;
	EsScriptMetaclassMap m_metaclassMap;
	// call stack
	EsCallStack m_calls;
	// data stack
	EsDataStack m_data;
	// currently active code && startup code
	EsCodeSection::Ptr m_startup;
	EsCodeSection::Ptr m_activeCode;
	// temporary values pool	
	EsDataStack m_tmpPool;
	// destruction flag. mainly used for final deletion of temporaries
	bool m_destroying;
	
	friend class EsScript;
	friend class EsTmpAccessor;
	friend class EsScriptObject;
	friend class EsScriptArrayObject;
};

#endif // _es_script_machine_h_
