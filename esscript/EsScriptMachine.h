#ifndef _es_script_machine_h_
#define _es_script_machine_h_

/// Forward declarations
class EsScriptDebug;

/// Script binary links container
typedef std::map<EsString, EsDynamicLibrary::Ptr> EsScriptBinaryLinks;
//---------------------------------------------------------------------------

/// Script machine context class. Used for instant script object marshalling
/// from one script machine to another, without any code execution.
/// The only restriction for that marshalling, machines must contain identical script code.
///
class ESSCRIPT_CLASS EsScriptContext
{
public:
  typedef std::shared_ptr<EsScriptContext> Ptr;

protected:
  EsScriptContext(EsScriptMachine* machine) ES_NOTHROW;

public:
  static EsScriptContext::Ptr create(EsScriptMachine* machine) ES_NOTHROW;
  ~EsScriptContext();

  EsScriptMachine* vm() ES_NOTHROW { ES_ASSERT(m_machine); return m_machine; }
  const EsScriptMachine* vm() const ES_NOTHROW { ES_ASSERT(m_machine); return m_machine; }

protected:
  void switchTo( EsScriptMachine* other ) ES_NOTHROW;
  void linkTo(const EsScriptContext::Ptr other) ES_NOTHROW;
  void unlink() ES_NOTHROW;

private:
  void linkAdd(EsScriptContext* link) ES_NOTHROW;
  void linkDelete(EsScriptContext* link) ES_NOTHROW;

protected:
  EsScriptMachine* m_machine;
  EsScriptContext* m_masterCtx;
  std::set<EsScriptContext*> m_links;

  ES_NO_DEFAULT_CTR(EsScriptContext);
  ES_NON_COPYABLE(EsScriptContext);

  friend class EsScriptMachine;
  friend class EsScriptObject;
};
//---------------------------------------------------------------------------

/// ES Script virtual machine
///
class ESSCRIPT_CLASS EsScriptMachine
{
public:
  // compound field helper struct
  struct CompoundFieldCreationResult
  {
    EsScriptObjectIntf::Ptr m_fieldMetaclass;
    EsScriptCodeSection::Ptr m_fieldExpr;
  };

private:
  /// Special low-priority thread for periodic cleaning-up a dead thread contexts
  ///
  class GarbageCollector : public EsThread
  {
  public:
    GarbageCollector(EsScriptMachine& owner);

  protected:
    virtual long worker();

  protected:
    EsScriptMachine& m_owner;
  };

protected:
  // Only accessible to script friend class
  EsScriptMachine(const EsScript& owner);
  ~EsScriptMachine();

public:
  /// Return owner reference
  const EsScript& ownerGet() const ES_NOTHROW { return m_owner; }

  /// Return true if script machine is in destruction stage
  bool isDestroying() const ES_NOTHROW { return m_destroying; }

  /// Reset virtual machine to uninitialized state
  void reset();

  /// Source File (modules of compilation) services
  ///
  bool hasSourceModulesInfo() const ES_NOTHROW { return m_filesInfo && !m_filesInfo->isEmpty(); }
  EsVariant sourceModuleExecutableLinesGet(const EsString& srcPath) const ES_NOTHROW;
  EsVariant sourceModuleHashGet(const EsString& srcPath) const ES_NOTHROW;
  EsString::Array sourceModulesPathsGet() const ES_NOTHROW;
  bool hasSourceModule(const EsString& srcPath) const ES_NOTHROW;

  /// Execution services
  ///

  /// Rewind script machine execution, optionally strting GC thread (default-yes)
  void rewind(bool startGC = true);

  /// Execute startup code, and all, that is included in it
  EsVariant exec();

  /// Execute named method with no parameters
  EsVariant call(cr_EsString name);

  /// Execute named method with one parameter
  EsVariant call(cr_EsString name, cr_EsVariant p0);

  /// Execute named method with two parameters
  EsVariant call(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1);

  /// Execute named method with three parameters
  EsVariant call(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2);

  /// Execute named method with four parameters
  EsVariant call(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3);

  /// Execute named method with five parameters
  EsVariant call(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4);

  /// Execute named method with six parameters
  EsVariant call(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5);

  /// Execute named method with seven parameters
  EsVariant call(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5, cr_EsVariant p6);

  /// Object creation services
  ///

  /// Create an object by its name using constructor with no parameters
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name);

  /// Create an object by its name using constructor with one parameter
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name, cr_EsVariant p0);

  /// Create an object by its name using constructor with two parameters
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1);

  /// Create an object by its name using constructor with three parameters
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2);

  /// Create an object by its name using constructor with four parameters
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3);

  /// Create an object by its name using constructor with five parameters
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4);

  /// Create an object by its name using constructor with six parameters
  EsReflectedClassIntf::Ptr objectCreate(cr_EsString name, cr_EsVariant p0, cr_EsVariant p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5);

  /// Call global function by its name with specified parameters, and return result
  EsVariant callGlobalMethod(const EsString& name, const EsVariant& params);

  /// Miscellaneous services
  ///

  /// Script version
  EsString versionGet() const;

  /// Return machine locale
  const std::locale& loc() const ES_NOTHROW { return m_loc; }

  /// Linked binaries manipulation
  void linkAdd(const EsString& link);

  /// RO access to constants, system, and global variables
  const EsScriptSymbolTable& externs() const ES_NOTHROW { return m_externs; }
  const EsScriptSymbolTable& consts() const ES_NOTHROW { return m_consts; }
  const EsScriptSymbolTable& sysVars() const ES_NOTHROW { return m_sysVars; }
  const EsScriptSymbolTable& vars() const ES_NOTHROW;

  /// Constant access
  EsVariant constantGet(const EsString& name) const { return m_consts.symbolGet(name, true)->get(); }

  /// Variable access
  EsVariant sysVariableGet(const EsString& name) const { return m_sysVars.symbolGet(name, true)->get(); }
  EsVariant variableGet(const EsString& name) const { return vars().symbolGet(name, true)->get(); }
  void variableSet(const EsString& name, const EsVariant& val) { const_cast<EsScriptSymbolTable&>(vars()).symbolValSet(name, val); }

  /// Check if given symbol ia declared as an external
  bool isExternal(const EsString& name) const ES_NOTHROW { return m_externs.symbolExists(name); }

  /// Check if given symbol name is an extenal enum
  bool isExternalEnum(const EsString& name) const;

  /// Check if given symbol name is an extenal metaclass
  bool isExternalMetaclass(const EsString& name) const;

  /// Return global methods map
  const EsScriptMethodMap& globalMethodsMapGet() const ES_NOTHROW { return *m_globalMethods.get(); }

  /// Return global startup code section, which always exists in machine
  EsScriptCodeSection::Ptr startupMethodGet() ES_NOTHROW { return m_startup; }

  /// Check if global code section with specified key is known (declared)
  bool hasGlobalMethod(const EsMethodInfoKeyT& key) const ES_NOTHROW;

  /// Get global code section by method key
  EsScriptCodeSection::Ptr globalMethodGet(const EsMethodInfoKeyT& key, bool doThrow = true) const;

  /// Compile time operations
  ///

  /// Access special null constant
  static EsScriptValAccessorIntf::Ptr nullcGet() ES_NOTHROW;

  /// Declare an external symbol. External symbol got added to special symbol table for
  /// later resolution on run-time.
  ///
  void externEnumDeclare(const EsString& name);
  void externMetaclassDeclare(const EsString& name);

  /// Declare constant
  EsScriptValAccessorIntf::Ptr constDeclare(const EsString& name, const EsVariant& val, bool builtIn = false);

  /// Add new unnamed const and return its internal name. if const with such value is already declared,
  /// return existing name for its access
  ///
  EsString unnamedDeclare(const EsVariant& val);

  /// Script enumerations manipulation
  ///

  EsStringArray enumNamesGet() const ES_NOTHROW { return m_enumsMap.namesGet(); }

  /// Find enumeration object by name, throw exception if enum is not found && doThrow is set
  EsEnumerationIntf::Ptr enumFind(const EsString& name, bool doThrow = false) const;

  /// Create new enumeration object by name,
  /// throw exception if enumeration with 'name' already exists
  ///
  EsEnumerationIntf::Ptr enumDeclare(const EsString& name);

  /// Declare enumeration attribute
  void enumAttributeDeclare(const EsEnumerationIntf::Ptr& enu, const EsString& name, const EsVariant& val);

  /// Declare enumeration item. throw exception if item with such name already exists
  void enumItemDeclare(const EsEnumerationIntf::Ptr& enu, const EsString& symbol, const EsVariant& val, const EsString& label = EsString::null());

  /// Return enumeration value
  EsVariant enumValueGet(const EsString& name, const EsString& sym) const;

  /// Return enumeration value label
  EsVariant enumValueLabelGet(const EsString& name, const EsString& sym) const;

  /// Script metaclasses manipulation
  ///

  /// Return all declared metaclasses names
  EsStringArray metaclassNamesGet() const ES_NOTHROW { return m_metaclassesMap.namesGet(); }

  /// Find script object metaclass by name, throw exception if metaclass is not found && doThrow is set
  EsScriptObjectIntf::Ptr metaclassFind(const EsString& name, bool doThrow = false) const;

  /// Create new script object metaclass by name, optionally using baseName metaclass as its ancestor,
  /// throw exception if baseName metaclass is not found, or metaclass 'name' already exists
  ///
  EsScriptObjectIntf::Ptr metaclassDeclare(const EsString& name, const EsString& baseName);

  /// Declare simple|array metaclass field
  EsScriptObjectIntf::Ptr metaclassFieldDeclare(const EsString& metaclassName,
    const EsString& fieldType, const EsString& fieldName, const EsAttributesIntf::Ptr& attrs = EsAttributesIntf::Ptr());
  EsScriptObjectIntf::Ptr metaclassFieldDeclare(const EsScriptObjectIntf::Ptr& metaclass,
    const EsString& fieldType, const EsString& fieldName, const EsAttributesIntf::Ptr& attrs = EsAttributesIntf::Ptr());
  CompoundFieldCreationResult metaclassArrayFieldDeclare(const EsString& metaclassName,
    const EsString& fieldType, const EsString& fieldName, const EsAttributesIntf::Ptr& attrs = EsAttributesIntf::Ptr());
  CompoundFieldCreationResult metaclassArrayFieldDeclare(const EsScriptObjectIntf::Ptr& metaclass,
    const EsString& fieldType, const EsString& fieldName, const EsAttributesIntf::Ptr& attrs = EsAttributesIntf::Ptr());
  CompoundFieldCreationResult metaclassIfFieldDeclare(const EsString& metaclassName);
  CompoundFieldCreationResult metaclassIfFieldDeclare(const EsScriptObjectIntf::Ptr& metaclass);

  /// Declare attribute for the given object|object instance
  void objectAttributeDeclare(const EsScriptObjectIntf::Ptr& obj, const EsString& attrName, const EsVariant& attrVal);

  /// Declare metaclass member variable
  void metaclassMemberVarDeclare(const EsScriptObjectIntf::Ptr& obj, const EsString& attrName,
    const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());

  /// Declare new named code section AKA script method
  EsScriptCodeSection::Ptr methodDeclare(const EsString& name, const EsString::Array& params, const EsScriptObjectIntf::Ptr& metaclass);
  EsScriptCodeSection::Ptr globalMethodDeclare(const EsString& name, const EsString::Array& params);

  /// Declare attribute for the code section
  void functionAttributeDeclare(const EsScriptCodeSection::Ptr& function, const EsString& attrName, const EsVariant& attrVal);

  /// Special reflected classes, owned by machine
  ///

  /// Meta-information bridge class singleton instance access
  inline EsMetaclassIntf::Ptr metaGet() const ES_NOTHROW
  {
    return m_meta.asWeakReference();
  }

  /// Scripting host services proxy class singleton instance access
  inline EsReflectedClassIntf::Ptr hostGet() ES_NOTHROW
  {
    return m_host.asWeakReference();
  }

  /// Execution control
  inline void execAbort() ES_NOTHROW { m_abort = true; }
  inline bool isAborting() const ES_NOTHROW { return m_abort; }

  /// Return debug information (if any) for instruction currently being executed in active thread context
  EsScriptDebugInfoIntf::Ptr currentDebugInfoGet() const ES_NOTHROW;

  /// Trace instruction in debugger, if it is assigned
  void debuggerInstructionTrace(EsScriptInstructions::const_iterator instr);

  /// Variant container trace helper
  static EsString traceVariant(const EsVariant& v);

  /// Run non-executing thread contexts cleanup once
  void threadCtxCleanup();

  /// Stop GC thread. It will be automatically re-started on next rewind call
  void gcStop();

  /// Return current unique translatables as string array
  EsString::Array translatablesGet() const;

  /// Append unique translatable to collection
  void translatableAdd(const EsString& strVal);

protected:
  /// Internal services
  ///

  void rewindInternal(bool startGC);

  /// Thread Execution delegates
  ///

  /// Call global function by its method key
  EsVariant callGlobalMethod(const EsMethodInfoKeyT& key, const EsVariant& params);

  /// Execute specific code section inside an appropriate thread context
  EsScriptValAccessorIntf::Ptr exec(
    const EsScriptCodeSection::Ptr& cs,
    const EsVariant& params,
    EsScriptEvalMode evalMode,
    EsScriptObjectIntf* This
  );

  /// Create object by object name with parameters, in current thread execution context, and return object instance.
  EsReflectedClassIntf::Ptr objectCreateWithParameters(
    const EsString& name,
    const EsVariant& params
  );

  /// Marshalling between different Script Contexts
  ///

  /// Unregister and cleanup linked binaries
  void binLinksClear();

  /// Return true if machine may perform object marshalling
  bool canMarshal() const ES_NOTHROW;

  /// Internal service for optionally marshalling scripted objects to the master script context
  void marshalObject(const EsScriptObjectIntf::Ptr& obj) const;
  void marshalData(const EsVariant& data) const;

  /// Sys initialization
  void sysConstsInit();
  void sysVarsInit();

  /// Misc method lookup && checking services
  static void checkMethodIsNotInMap(const EsString& mapName, const EsScriptMethodMapPtr& map,
    const EsMethodInfoKeyT& key, const EsScriptDebugInfoIntf::Ptr& dbg);

  static void addMethodToMap(const EsString& mapName, EsScriptMethodMapPtr& map,
    const EsMethodInfoKeyT& key, const EsScriptCodeSection::Ptr& method,
    const EsScriptDebugInfoIntf::Ptr& dbg);

  static void deleteMethodFromMap(const EsString& mapName, EsScriptMethodMapPtr& map,
    const EsMethodInfoKeyT& key);

  /// Initialization services
  ///

  /// Return script method key given method code section ptr
  static EsMethodInfoKeyT methodKeyCreate(const EsScriptCodeSection::Ptr& code);

  /// Register base script POD (Plain Old Data) metaclasses
  void registerMetaclass(const EsScriptObjectIntf::Ptr& metaclass);
  void registerPodMetaclasses();

  /// Set temporary pointer to script debugger
  void debuggerSet(EsScriptDebug* dbg) ES_NOTHROW;

  /// Thread contexts manipulation
  ///

  /// Find thread context for thread ID. If not found, nullptr is returned,
  /// otherwise, return pointer to existing EsScriptThreadContext instance.
  ///
  EsScriptThreadContext::Ptr threadCtxFind(EsThreadId threadId) const;

  /// Remove thread context by thread ID. If it was cached,
  /// nullify cache as well.
  ///
  void threadCtxRemove(EsThreadId threadId);

  /// Return thread context for thread ID, create new one, if one does not exist,
  /// or find and get an existing one, making it cached.
  ///
  EsScriptThreadContext::Ptr threadCtxGet(EsThreadId threadId);

private:
  /// Non-interlocked context finder version, for internal use only
  EsScriptThreadContext::Ptr threadCtxFindInternal(EsThreadId threadId) const;

  /// Reset thread contexts
  void threadCtxsReset();

protected:
  mutable EsCriticalSection m_cs;
  EsCriticalSection m_csDbg;

  /// Owner of this virtual machine
  const EsScript& m_owner;

  /// Locale used for compilation|execution
  const std::locale& m_loc;

  // This script context
  EsScriptContext::Ptr m_ctxScriptThis;

  /// Garbage collector thread
  GarbageCollector m_gc;

  /// Attached script debugger
  EsScriptDebug* m_dbg;

  // Used source file references
  EsAssocContainerIntf::Ptr m_filesInfo;

  /// Linked binaries
  EsScriptBinaryLinks m_links;

  /// External symbols
  EsScriptSymbolTable m_externs;

  /// Constants
  EsScriptSymbolTable m_consts;

  /// System variables
  EsScriptSymbolTable m_sysVars;

  /// Startup code
  EsScriptCodeSection::Ptr m_startup;

  /// Global code
  EsScriptMethodMapPtr m_globalMethods;

  /// Metaclasses (as payload to the collection items)
  EsStringIndexedMap m_metaclassesMap;

  /// Enumerations (as payload to the collection items)
  EsStringIndexedMap m_enumsMap;

  /// Translatable strings aggregator. Used during compilation process
  /// to allocate unique translatable strings (marked by I) from source files.
  ///
  EsStringIndexedMap m_translatables;

  /// Metaclass proxy service object
  EsScriptMetaclass m_meta;

  /// Script host proxy service object
  EsScriptHost m_host;

  /// Thread contexts map
  std::map<EsThreadId, EsScriptThreadContext::Ptr> m_ctxs;
  /// Current thread context. Kind of cached thread context
  mutable EsScriptThreadContext::Ptr m_ctxCur;

  /// Execution and abortion flags
  volatile bool m_abort;

  /// Destruction flag. mainly used for final deletion of temporaries
  bool m_destroying;

  // direct reference to the null const accessor
  static EsScriptValAccessorIntf::Ptr s_nullc;

  ES_NON_COPYABLE(EsScriptMachine);

  friend class EsScript;
  friend class EsScriptHost;
  friend class EsScriptlet;
  friend class EsScriptTmpValAccessor;
  friend class EsScriptObject;
  friend class EsScriptArrayObject;
  friend class EsScriptIfObject;
  friend class EsScriptCompiledBinaryWriter;
  friend class EsScriptCompiledBinaryReader;
  friend class EsScriptDebug;
};

#endif // _es_script_machine_h_
