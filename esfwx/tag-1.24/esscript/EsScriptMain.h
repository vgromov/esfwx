#ifndef _es_script_main_h_
#define _es_script_main_h_

ES_DECL_INTF_BEGIN2(D558F74B, 4D8A4b21, A441FB9E, D5F74C32, EsScriptIntf, EsBaseIntf)

  /// Include paths manipulation
  ES_DECL_INTF_METHOD(void, includePathAdd)(const EsString& path, bool doThrow = true) = 0;
  ES_DECL_INTF_METHOD(void, includePathsClear)() = 0;
  ES_DECL_INTF_METHOD(void, includePathsSet)(const EsStringArray& paths, bool doThrow = true) = 0;
  ES_DECL_INTF_METHOD(const EsStringArray&, includePathsGet)() const = 0;

  /// Link paths manipulation
  ES_DECL_INTF_METHOD(void, linkPathAdd)(const EsString& path, bool doThrow = true) = 0;
  ES_DECL_INTF_METHOD(void, linkPathsClear)() = 0;
  ES_DECL_INTF_METHOD(void, linkPathsSet)(const EsStringArray& paths, bool doThrow = true) = 0;
  ES_DECL_INTF_METHOD(const EsStringArray&, linkPathsGet)() const = 0;

  /// Compilation status check
  ES_DECL_INTF_METHOD(bool, isCompiled)() const = 0;

  /// Run string script compilation
  ES_DECL_INTF_METHOD(bool, compileText)(const EsString& src) = 0;
  ES_DECL_INTF_METHOD(bool, compileFile)(const EsString& file) = 0;
  ES_DECL_INTF_METHOD(bool, compileText)(const EsString& src, const EsBreakIntf::Ptr& brk) = 0;
  ES_DECL_INTF_METHOD(bool, compileFile)(const EsString& file, const EsBreakIntf::Ptr& brk) = 0;

  /// Access script virtual machine
  ES_DECL_INTF_METHOD(EsScriptMachine&, vm)() = 0;
  ES_DECL_INTF_METHOD(const EsScriptMachine&, vm)() const = 0;

  /// Progress monitor access
  ES_DECL_INTF_METHOD(EsProgressMonitorIntf::Ptr, progressMonitorGet)() const = 0;
  ES_DECL_INTF_METHOD(void, progressMonitorSet)(const EsProgressMonitorIntf::Ptr& monitor) = 0;

  /// Virtual machine proxies
  ///

  /// Signal we want to abort execution
  ES_DECL_INTF_METHOD(void, execAbort)() = 0;

  /// Rewind script machine
  ES_DECL_INTF_METHOD(void, rewind)() = 0;

  /// Startup method execution
  ES_DECL_INTF_METHOD(EsVariant, exec)() = 0;

  /// Script methods variadic execution
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsString) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsString, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsString, cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;
  ES_DECL_INTF_METHOD(EsVariant, exec)(cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) = 0;

  /// Script constant access
  ES_DECL_INTF_METHOD(EsVariant, constantGet)(cr_EsString) const = 0;

  /// Script variables access
  ES_DECL_INTF_METHOD(EsVariant, variableGet)(cr_EsString) const = 0;
  ES_DECL_INTF_METHOD(void, variableSet)(cr_EsString, cr_EsVariant) = 0;

  /// Shortcuts for creating objects with different count of parameters passed to ctor
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name, const EsVariant& p0) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name, const EsVariant& p0, const EsVariant& p1) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, objectCreate)(const EsString& name, const EsVariant& p0, const EsVariant& p1, const EsVariant& p2, const EsVariant& p3, const EsVariant& p4, const EsVariant& p5) = 0;

  /// Return translatables aggregated during compilation
  ES_DECL_INTF_METHOD(EsStringArray, translatablesGet)() const = 0;
  
  /// Return true if script has source modules information for debug purposes
  ES_DECL_INTF_METHOD(bool, hasSourceModulesInfo)() const = 0;

  /// Get executable source lines info from sourceModulesInfo comntainer by absolute source file name, if any.
  /// Return an empty variant, if no source file info exist, otherwise, return collection of executable line ranges,
  /// to which .has operator is applicable
  /// 
  ES_DECL_INTF_METHOD(EsVariant, sourceModuleExecutableLinesGet)(const EsString& srcPath) const = 0;

  /// Get source code hash value, if any.
  /// Return a null value, if no source file info exist, otherwise, return an appropriate hash value.
  /// 
  ES_DECL_INTF_METHOD(EsVariant, sourceModuleHashGet)(const EsString& srcPath) const = 0;

  /// Return full paths of source modules used in comopilation
  ES_DECL_INTF_METHOD(EsString::Array, sourceModulesPathsGet)() const = 0;

  /// Return true if script debug information contains source module with specified path, false otherwise
  ES_DECL_INTF_METHOD(bool, hasSourceModule)(const EsString& srcPath) const = 0;
    
  /// Compiled binary access
  ///

  /// Return compiled binary script, optionally retaining|discarding debug information, if any
  ES_DECL_INTF_METHOD(EsBinBuffer, compiledBinaryGet)(bool retainDebug = true) const = 0;

  /// Assign compiled binary script, optionally retaining|discarding debug information, if any
  ES_DECL_INTF_METHOD(void, compiledBinarySet)(const EsBinBuffer& buff, bool retainDebug = true) = 0;

  /// Load copiled binary script from file.
  ES_DECL_INTF_METHOD(EsBinBuffer, compiledBinaryLoad)(const EsString& fileName, bool retainDebug = true, const EsString& k = EsString::null()) = 0;

  /// Reset script contents
  ES_DECL_INTF_METHOD(void, reset)() = 0;

  /// Marshalling
  ES_DECL_INTF_METHOD(void, marshal)(const EsVariant& data) const = 0;

#if  defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
  /// Scriptlet creation && manipulation
  ///
  ES_DECL_INTF_METHOD(EsScriptletIntf::Ptr, scriptletCreate)(const EsString& name, const EsString& src, const EsString& args = EsString::null()) = 0;
  ES_DECL_INTF_METHOD(EsScriptletIntf::Ptr, scriptletGet)(const EsString& name, size_t paramCount) const = 0;
  ES_DECL_INTF_METHOD(EsScriptletIntfPtrArray, scriptletsLoad)(const EsString& file, const EsString& key = EsString::null()) = 0;
  ES_DECL_INTF_METHOD(EsScriptletIntfPtrArray, scriptletsGet)() const = 0;
  ES_DECL_INTF_METHOD(void, scriptletsReset)() = 0;
#endif

ES_DECL_INTF_END

/// Script event categories
#define ES_EVTC_SCRIPT esT("esscript")

/// Ekosfera script class implementation
///
class ESSCRIPT_CLASS ES_INTF_IMPL2(EsScript, EsScriptIntf, EsReflectedClassIntf)

public:
  // script event ids
  enum {
    evtError,
    evtCompilerInfo,
  };

public:
  ES_DECL_REFLECTED_CLASS_BASE_CUSTOM(EsScript)
  ES_DECL_ATTR_HANDLING_STD

  virtual ~EsScript();
  static EsScriptIntf::Ptr create(const EsScriptIntf::Ptr& master = EsScriptIntf::Ptr());
  
  // EsScriptIntf implementation
  //

  // include paths manipulation
  ES_DECL_REFLECTED_INTF_METHOD2(void, includePathAdd, cr_EsString, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, includePathsClear) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, includePathsSet)(const EsStringArray& paths, bool doThrow = true) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsStringArray&, includePathsGet)() const ES_OVERRIDE { return m_includes; }

  // link paths manipulation
  ES_DECL_REFLECTED_INTF_METHOD2(void, linkPathAdd, cr_EsString, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, linkPathsClear) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, linkPathsSet)(const EsStringArray& paths, bool doThrow = true) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsStringArray&, linkPathsGet)() const ES_OVERRIDE { return m_linkPaths; }

  // compilation status check
  ES_DECL_INTF_METHOD(bool, isCompiled)() const ES_OVERRIDE { return m_compiled; }

  // run string script compilation
  ES_DECL_REFLECTED_INTF_METHOD1(bool, compileText, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(bool, compileFile, cr_EsString) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(bool, compileText)(cr_EsString txt, const EsBreakIntf::Ptr& brk) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(bool, compileFile)(cr_EsString file, const EsBreakIntf::Ptr& brk) ES_OVERRIDE;

  // access script virtual machine
  ES_DECL_INTF_METHOD(EsScriptMachine&, vm)() ES_OVERRIDE;
  ES_DECL_INTF_METHOD(const EsScriptMachine&, vm)() const ES_OVERRIDE;

  /// Progress monitor access
  ES_DECL_INTF_METHOD(EsProgressMonitorIntf::Ptr, progressMonitorGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, progressMonitorSet)(const EsProgressMonitorIntf::Ptr& monitor) ES_OVERRIDE;

  /// Custom call handling, with delegation to VirtualMachine
  ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, callClassMethod, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, callMethod, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(EsVariant, call, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, call, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD4(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD5(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD6(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD7(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, classCall, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, classCall, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD3(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD4(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD5(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD6(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD7(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

  /// Reflected virtual machine proxies
  ///

  /// Signal we want to abort execution
  ES_DECL_REFLECTED_INTF_METHOD0(void, execAbort) ES_OVERRIDE;

  /// Rewind script machine
  ES_DECL_REFLECTED_INTF_METHOD0(void, rewind) ES_OVERRIDE;

  /// Startup method execution
  ES_DECL_REFLECTED_INTF_METHOD0(EsVariant, exec) ES_OVERRIDE;

  /// Script methods execution
  ES_DECL_REFLECTED_INTF_METHOD1(EsVariant, exec, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, exec, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(EsVariant, exec, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD4(EsVariant, exec, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD5(EsVariant, exec, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD6(EsVariant, exec, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

  /// Script constant access
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, constantGet, cr_EsString) ES_OVERRIDE;

  /// Script variables access
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, variableGet, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(void, variableSet, cr_EsString, cr_EsVariant) ES_OVERRIDE;

  /// Shortcuts for creating objects with different count of parameters passed to ctor
  ES_DECL_REFLECTED_INTF_METHOD1(EsBaseIntfPtr, objectCreate, cr_EsString) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsBaseIntfPtr, objectCreate, cr_EsString, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(EsBaseIntfPtr, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD4(EsBaseIntfPtr, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD5(EsBaseIntfPtr, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD6(EsBaseIntfPtr, objectCreate, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;
  virtual EsBaseIntfPtr objectCreate(cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

  /// Optional source module information access
  ES_DECL_INTF_METHOD(bool, hasSourceModulesInfo)() const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsVariant, sourceModuleExecutableLinesGet)(const EsString& srcPath) const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsVariant, sourceModuleHashGet)(const EsString& srcPath) const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsString::Array, sourceModulesPathsGet)() const ES_NOTHROW ES_OVERRIDE;
  ES_DECL_INTF_METHOD(bool, hasSourceModule)(const EsString& srcPath) const ES_NOTHROW ES_OVERRIDE;
  
  /// Return translatables aggregated during compilation
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsStringArray, translatablesGet) ES_OVERRIDE;

  /// Compiled binary access
  ///
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsBinBuffer, compiledBinaryGet, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(void, compiledBinarySet, cr_EsBinBuffer, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD3(EsBinBuffer, compiledBinaryLoad, cr_EsString, bool, cr_EsString) ES_OVERRIDE;

  // reset script contents
  ES_DECL_INTF_METHOD(void, reset)() ES_OVERRIDE;

  // Marshalling
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(void, marshal, cr_EsVariant) ES_OVERRIDE;

#if  defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
  // named scriptlet creation|manipulation
  //
  ES_DECL_INTF_METHOD(EsScriptletIntf::Ptr, scriptletCreate)(const EsString& name, const EsString& src, const EsString& args = EsString::null()) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsScriptletIntf::Ptr, scriptletGet)(const EsString& name, size_t paramCount) const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsScriptletIntfPtrArray, scriptletsGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsScriptletIntfPtrArray, scriptletsLoad)(const EsString& file, const EsString& key = EsString::null()) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, scriptletsReset)() ES_OVERRIDE;
#endif

  /// Reflected services
  ///

  /// Constructors
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsBaseIntfPtr);

#if  defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
  // reflected scriptlet creation (aliased to scriptletCreate)
  ES_DECL_REFLECTED_METHOD3(EsBaseIntfPtr, scriptletCreateReflected, cr_EsString, cr_EsString, cr_EsString);
#endif

  /// Micro expression evaluators
  ///

  /// Evaluate enum value or lable by enum_name$$value_name|enum_name$$value_name$$label expression.
  /// Throw an exception if either enum or symbol is not found
  ///
  ES_DECL_REFLECTED_METHOD1(EsVariant, enumEval, cr_EsVariant);

  /// Reflected properties
  ///
  ES_DECL_PROPERTY_RO(compiled, bool)
  ES_DECL_PROPERTY_RO(meta, EsBaseIntfPtr)
  ES_DECL_PROPERTY(includePaths, EsStringArray)
  ES_DECL_PROPERTY(linkPaths, EsStringArray)
  ES_DECL_PROPERTY(master, EsBaseIntfPtr)
  ES_DECL_PROPERTY(progressMonitor, EsBaseIntfPtr)
  ES_DECL_PROPERTY_RO(translatables, EsStringArray)

  // Static services
  //
  static EsString absoluteFileNameGet(const EsString& fileName, const EsStringArray& searchPaths,
    bool searchSubfolders = false, const EsString& curFile = EsString::null());

  // Identifier checking services
  //
  // Make sure identifier is not a reserved word
  static void checkNotReservedWord(const EsString& ident, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());
  // Make sure identifier is not a registered reflected class name
  static void checkNotRegisteredObject(const EsString& ident, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());
  // Script reserved words
  static const EsStringIndexedMap& reservedWordsGet();

protected:
  static void pathAdd(const EsString& path, EsStringArray& collection, const EsString& subject, bool doThrow = true);

private:
  EsScript(const EsScriptIntf::Ptr& master);

  // prohibited functionality
  EsScript() ES_REMOVEDECL;
  EsScript(const EsScript&) ES_REMOVEDECL;
  const EsScript& operator=(const EsScript&) ES_REMOVEDECL;

protected:
  // data members
  //
  // script code - virtual machine executor
  EsScriptMachine m_machine;
  // master script context (optional weak reference)
  EsScriptIntf::Ptr m_master;
#if  defined(ES_USE_SCRIPTLETS) && 1 == ES_USE_SCRIPTLETS
  // scriptlets loaded in this script context
  EsStringIndexedMap m_scriptlets;
#endif
  // script search directories
  EsStringArray m_includes;
  EsStringArray m_linkPaths;
  // script compiled flag
  bool m_compiled;
};

#endif // _es_script_main_h_
