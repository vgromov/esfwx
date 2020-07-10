#ifndef _es_script_thread_context_h_
#define _es_script_thread_context_h_

#ifndef ES_USE_VECTOR_FOR_STACK
# define ES_USE_VECTOR_FOR_STACK 1
#else
# include <deque>
#endif

/// Forward decls
///
class EsScriptCodeSectionScope;
class EsScriptMachine;

/// Script threading context
///
class EsScriptThreadContext
{
public:
  typedef std::shared_ptr<EsScriptThreadContext> Ptr;

protected:
  EsScriptThreadContext(EsScriptMachine& owner, EsThreadId threadId) ES_NOTHROW;

public:
  /// Return ID of the thread this context is created for
  inline EsThreadId threadId() const ES_NOTHROW { return m_threadId; }

  /// Return current value of execution flag
  inline bool isExecuting() const ES_NOTHROW { return m_executing; }

  /// Debug trace | code dumping helpers
  ///
  static EsString instructionAsString(const EsScriptInstruction& instr);
  void traceInstruction(EsScriptInstructions::const_iterator instr);

  /// Return debug information (if any) for instruction currently being executed
  EsScriptDebugInfoIntf::Ptr currentDebugInfoGet() const ES_NOTHROW;

  /// Call external method|method in specific namespace
  static void callExtMethod(
    EsVariant& result,
    const EsString& name,
    const EsVariant& params,
    const EsString& nameSpace,
    const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()
  );

  /// Variant service call
  static void variantServiceCall(
    EsVariant& result,
    const EsString& name,
    const EsVariant::Array& params,
    const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr()
  );

protected:
  /// Code section AKA script method selection
  ///

  /// Return currently active code section
  EsScriptCodeSection::Ptr currentMethodGet() ES_NOTHROW;
  const EsScriptCodeSection::Ptr& currentMethodGet() const ES_NOTHROW;

  /// Currently active execution scope access
  inline EsScriptCodeSectionScope& codeScopeGet()
  {
    ES_ASSERT(m_csScope);
    return *m_csScope;
  }

  inline const EsScriptCodeSectionScope& codeScopeGet() const
  {
    ES_ASSERT(m_csScope);
    return *m_csScope;
  }

  /// Try to set current script method by key.
  /// if method is not found, an exception is thrown
  ///
  void methodSetCurrent(const EsMethodInfoKeyT& key);

  /// !!!Semi-private method!!! just set current script code as-is. no checks performed.
  void methodSetCurrent(const EsScriptCodeSection::Ptr& code);

  /// Set global startup section as current section
  void startupMethodSetCurrent(const EsScriptCodeSection::Ptr& section = EsScriptCodeSection::Ptr());

  /// Track instruction, currently being executed
  void currentInstructionTrack(EsScriptInstructions::const_iterator instr);

  /// Execution helpers
  ///

  /// Execute entire script, beginning from startup code
  EsVariant exec();

  /// Generic call handlers
  ///

  /// Call global function by its method key
  EsVariant callGlobalMethod(const EsMethodInfoKeyT& key, const EsVariant& params);

  /// Call global function by its name and parameters
  EsVariant callGlobalMethod(const EsString& name, const EsVariant& params);

  /// Execution services
  ///
  inline EsScriptValAccessorIntf::Ptr exec(
    const EsScriptCodeSection::Ptr& cs,
    const EsVariant& params,
    EsScriptEvalMode evalMode,
    EsScriptObjectIntf* This
  )
  {
    return exec(
      cs.get(),
      params,
      evalMode,
      This
    );
  }

  EsScriptValAccessorIntf::Ptr exec(
    const EsScriptCodeSection* pcs,
    const EsVariant& params,
    EsScriptEvalMode evalMode,
    EsScriptObjectIntf* This
  );

  EsScriptInstructionOpcode exec(
    EsScriptInstructions::const_iterator beg,
    EsScriptInstructions::const_iterator end,
    EsScriptInstructions::const_iterator& instr
  );

  /// Execution helpers
  ///

  /// Load variable reference to the data stack
  void doLoadVarRef(const EsScriptInstruction& instr);

  /// Load immediate value, specified in instruction, to the data stack
  void doLoadImmediateVal(const EsScriptInstruction& instr);

  /// Construct indexed item accessor and push it onto data stack
  void doConstructItemAccessor();

  /// Construct auto indexed item accessor and push it onto data stack
  void doConstructAutoItemAccessor();

  /// Construct object field accessor and push it onto data stack
  void doConstructMemberAccessor(const EsScriptInstruction& instr);

  /// Construct object property accessor and push it onto data stack
  void doConstructPropAccessor(const EsScriptInstruction& instr);

  /// Access attribute value
  void doAccessAttributeValue(const EsScriptInstruction& instr);

  /// Perform script function call
  void doCall(const EsScriptInstruction& instr);

  /// Perform binary operation expression using specified opid
  void doBinOp(long opid);

  /// Perform unary operator expression, using specified opid
  void doUnaryOp(long opid);

  /// Perform assignment using specified opcode and opid
  void doAsnOp(const EsScriptInstruction& instr);

  /// Perform incdec execution
  void doIncDec(const EsScriptInstruction& instr);

  /// Do short-cirquit logic operation evaluation checking
  void doLogicCheck(EsScriptInstructions::const_iterator& instr);

  /// Perform jump instruction. return true if jump was handled
  bool doJump(EsScriptInstructions::const_iterator& instr);

  /// Issue script exception
  void doThrow(const EsScriptInstruction& instr);

  /// Create auto item accessor at the top of the stack
  void doAutoItemLoad();

  /// Load top data accessor isOk result on the data stack
  void doIsOkLoad();

  /// Internal object method call
  void doObjectMethodCall(
    const EsReflectedClassIntf::Ptr& obj,
    const EsString& name,
    const EsVariant::Array& params,
    EsVariant& result
  );

  /// Internal external method call with current debug info attachment
  void doCallExtMethod(
    const EsString& name,
    const EsVariant::Array& params,
    const EsString& nameSpace,
    EsVariant& result
  );

  /// Call global scripted method with current debug info attachment
  void doCallGlobalMethod(
    const EsString& name,
    const EsVariant::Array& params,
    EsVariant& result
  );

  // Call variant service with current debug info attachment
  void doVariantServiceCall(
    const EsString& name,
    const EsVariant::Array& params,
    EsVariant& result
  );

  /// Object creation helper
  ///

  /// Create with parameters, and return object instance by object name.
  EsReflectedClassIntf::Ptr objectCreateWithParameters(const EsString& name, const EsVariant& params);

public:
  // Semi-private services.
  static void binOpEval(long opid, const EsVariant& _1, const EsVariant& _2, EsVariant& result);
  static void unaryOpEval(long opid, EsVariant& val);

protected:
  EsScriptMachine& m_owner;
  EsThreadId m_threadId;

  /// Currently active code
  EsScriptCodeSection::Ptr m_activeCode;

  /// Currently executed instruction
  const EsScriptInstruction* m_curInstr;

  /// Currently active execution scope
  EsScriptCodeSectionScope* m_csScope;

  /// Execution flag
  volatile bool m_executing;

  friend class EsScriptMachine;
  friend class EsScriptObjectCtrExecutor;
  friend class EsScriptTryCatchBlock;
  friend class EsScriptOptbl;
  friend class EsScriptCodeSectionScope;
};
//---------------------------------------------------------------------------

/// Code section automatic scope helper class
///
class EsScriptCodeSectionScope
{
private:
#if 1 == ES_USE_VECTOR_FOR_STACK
  enum { depthDefault = 8 };
#endif

protected:
  EsScriptCodeSectionScope( EsScriptThreadContext& This, const EsScriptCodeSection::Ptr& code );

public:
  ~EsScriptCodeSectionScope();

  bool hasParentScope() const ES_NOTHROW { return nullptr == m_old; }

  EsScriptCodeSection::Ptr codeGet() const ES_NOTHROW { return m_code; }
  const EsScriptCodeSectionScope* parentScopeGet() const ES_NOTHROW { return m_old; }
  const EsScriptCodeSectionScope* topScopeGet() const ES_NOTHROW;

  /// Scope data stack manipulation
  ///
  inline ulong stackSizeGet() const ES_NOTHROW { return static_cast<ulong>(m_stack.size()); }
  inline bool stackIsEmpty() const ES_NOTHROW { return m_stack.empty(); }

  inline void stackPush(const EsScriptValAccessorIntf::Ptr& acc) ES_NOTHROW { m_stack.push(acc); }
  inline EsScriptValAccessorIntf::Ptr stackPop() { return m_stack.pop(); }

  /// Take current stack snapshot
  void stackSnapshotPush();

  /// Restore current stack from the snapshot
  void stackSnapshotPop();

  /// Return current call stack trace, top (most recent callers) to bottom (first callers)
  EsString::Array stackTraceGet(bool includeParams, bool includeLocals, bool includeDataStack) const ES_NOTHROW;

  /// Return current data stack trace top to bottom
  EsString::Array dataStackTraceGet() const ES_NOTHROW;

protected:
  EsString scopeTraceGet(bool includeParams, bool includeLocals, bool includeDataStack) const ES_NOTHROW;

protected:
  EsScriptThreadContext& m_this;
  EsScriptCodeSection::Ptr m_code;
  EsScriptCodeSectionScope* m_old;
  EsScriptDataStack m_stack;

#if 1 == ES_USE_VECTOR_FOR_STACK
  std::vector< EsScriptDataStack::Ptr > m_snapshots;
#else
  std::deque< EsScriptDataStack::Ptr > m_snapshots;
#endif

private:
  // prohibited functionality
  ES_NO_DEFAULT_CTR( EsScriptCodeSectionScope );
  ES_NON_COPYABLE( EsScriptCodeSectionScope );

  friend class EsScriptThreadContext;
};
//---------------------------------------------------------------------------

#endif // _es_script_thread_context_h_
