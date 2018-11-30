#ifndef _es_code_section_h_
#define _es_code_section_h_

/// Forward declaration(s)
class EsScriptThreadContext;
class EsScriptCodeSection;

/// Try-catch frame class
class EsScriptTryCatchBlock
{
public:
	EsScriptTryCatchBlock(
    EsScriptCodeSection* cs = nullptr,
    ulong tryStart = 0,
    ulong tryEnd = 0,
    ulong catchStart = 0,
    ulong catchEnd = 0
  ) ES_NOTHROW;

	EsScriptTryCatchBlock(const EsScriptTryCatchBlock& other) ES_NOTHROW;

	const EsScriptTryCatchBlock& operator=(const EsScriptTryCatchBlock& other) ES_NOTHROW;

protected:
	void updateInstructionRange() ES_NOTHROW;

	EsScriptInstructionOpcode exec(EsScriptThreadContext& ctx, EsScriptInstructions::const_iterator& instr);
	EsScriptInstructionOpcode execCatch(EsScriptThreadContext& ctx, EsScriptInstructions::const_iterator& instr);

public:
  bool isOk() const ES_NOTHROW { return 0 != m_owner; }

  bool isEmpty() const ES_NOTHROW { return m_tryInstructionsStart == m_tryInstructionsEnd; }

  ulong tryStart() const ES_NOTHROW { return m_tryInstructionsStart; }

  ulong tryEnd() const ES_NOTHROW { return m_tryInstructionsEnd; }

  ulong catchStart() const ES_NOTHROW { return m_catchInstructionsStart; }

  ulong catchEnd() const ES_NOTHROW { return m_catchInstructionsEnd; }

protected:
	EsScriptCodeSection* m_owner;
	// try-catch instruction indexes (into owner code section's code)
  ulong m_tryInstructionsStart;
  ulong m_tryInstructionsEnd;
  ulong m_catchInstructionsStart;
  ulong m_catchInstructionsEnd;
	// try block selected (compile-time only)
	bool m_tryActive;

	friend class EsScriptCodeSection;
};
//---------------------------------------------------------------------------
typedef std::vector<EsScriptTryCatchBlock> EsScriptTryCatchBlocks;

/// Script code section interface implementation
///
class EsScriptCodeSection
{
public:
	// misc consts
	enum {
		defCodeCnt = 64,
		defParamsCnt = 8,
		defInlineCnt = 8,
		defInlineCalls = 8,
		defDependencyCount = 8,
	};

	typedef std::shared_ptr<EsScriptCodeSection> Ptr;

protected:
	EsScriptCodeSection(const EsString& name);
	EsScriptCodeSection(const EsString& name, EsScriptObjectIntf* metaclass);

	// accessible to EsScriptMachine friend
	static EsScriptCodeSection::Ptr create(const EsString& name, const EsString::Array& params);

	static EsScriptCodeSection::Ptr create(const EsString& name, const EsString::Array& params, EsScriptObjectIntf* metaclass);

	// special copy constructor. every templated src member gets cloned to non-template, just before execution
	EsScriptCodeSection(const EsScriptCodeSection& src, EsScriptObjectIntf* This, const EsVariant& params);

public:
	~EsScriptCodeSection() ES_NOTHROW;

  static const EsScriptCodeSection::Ptr& null();

	// EsScriptCodeSection api implementation
	//
	inline EsAttributesIntf::Ptr attributesAccess() const ES_NOTHROW
	{
		return m_attrs;
	}

	// get code section name
	inline const EsString& nameGet() const ES_NOTHROW
	{
		return m_name;
	}

	// empty check
	inline bool isEmpty() const ES_NOTHROW
	{
		ES_ASSERT(m_code);
		return m_code->empty();
	}

	// template check
	inline bool isTemplate() const ES_NOTHROW
	{
		return m_template;
	}

	// object method check
	inline bool isObjectMethod() const ES_NOTHROW
	{
		return 0 != m_metaclass;
	}

  // Return true if code section is startup one
  inline bool isStartup() const ES_NOTHROW
  {
    return m_params->empty() && m_name == EsScriptStdNames::startup();
  }

	// working with code
	//
	inline const EsScriptInstructions& codeAccess() const ES_NOTHROW { return *m_code.get(); }

  inline EsScriptInstruction& instructionAdd(EsScriptInstructionOpcode opcode, const EsScriptDebugInfoIntf::Ptr& debugInfo = EsScriptDebugInfoIntf::Ptr())
  {
    ulong dummy;
    return instructionAdd(
      dummy,
      opcode,
      debugInfo
    );
  }

  EsScriptInstruction& instructionAdd(ulong& pos,	EsScriptInstructionOpcode opcode,	const EsScriptDebugInfoIntf::Ptr& debugInfo);
  EsScriptInstruction& instructionModifyAt(ulong instrPos);

	inline EsScriptInstructions::const_iterator instructionStartGet() const ES_NOTHROW
  {
    ES_ASSERT(m_code);
    return m_code->begin();
  }

  inline EsScriptInstructions::const_iterator instructionEndGet() const ES_NOTHROW
  {
    ES_ASSERT(m_code);
    return m_code->end();
  }

	inline ulong instructionStartPosGet() const ES_NOTHROW
	{
		return 0;
	}

	inline ulong instructionEndPosGet() const ES_NOTHROW
	{
		return static_cast<ulong>(m_code->size());
	}

	// var && param declaration && lookup
	void variableDeclare(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg = nullptr);
	void variableDeclarePrivateReusable(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg = nullptr);
	void paramDeclare(const EsString& name, const EsScriptDebugInfoIntf::Ptr& dbg = nullptr);

	// access count of input parameters
	inline ulong inputParametersCntGet() const ES_NOTHROW
	{
		return static_cast<ulong>(m_params->size());
	}

	// indexed parameter access
	void parameterSet(ulong idx, const EsVariant& val);

	// all-at-once parameters assignment
	void parametersSet(const EsVariant& params);

	// parameters collection access
	inline const EsStringArray& paramNamesAccess() const ES_NOTHROW
	{
		return *m_params.get();
	}

	// local variablesAccess and params symbols access
	inline EsScriptSymbolTable& variablesAccess() ES_NOTHROW
	{
		return m_vars;
	}

	// check if variable is currently declared in this code section
	inline bool variableFind(const EsString& name) const ES_NOTHROW
	{
		return m_vars.symbolExists(name);
	}

	// access section variable by name (for non-template mode)
	inline EsScriptValAccessorIntf::Ptr variableGet(const EsString& name)
	{
		return m_vars.symbolGet(name, true);
	}

	/// Execution result access.
	///
	inline void resultSet(const EsScriptValAccessorIntf::Ptr& result) ES_NOTHROW
	{
		m_return = result;
	}

	// NB! internal result register must be reset after this call is made
	EsScriptValAccessorIntf::Ptr resultPop() ES_NOTHROW;

	/// Object metaclass and instance access
  ///
  EsScriptObjectIntf* thisGet() const ES_NOTHROW;

  EsScriptObjectIntf* metaclassGet() const ES_NOTHROW;

  inline EsScriptObjectIntf* immediateMetaclassGet() const ES_NOTHROW
  {
    return m_metaclass;
  }

	/// Create code section template's clone
	/// it's being called before code section execution
  ///
	EsScriptCodeSection::Ptr clone( EsScriptObjectIntf* This, const EsVariant& params ) const
	{
		checkTemplateOperation();
		return Ptr(new EsScriptCodeSection(*this, This, params));
	}

	void thisFieldDependencyAdd(const EsString& fieldName)
	{
		checkTemplateOperation();
		m_thisFieldDependencies->push_back(fieldName);
	}

	inline const EsString::Array& thisFieldDependenciesGet() const ES_NOTHROW
	{
		return *m_thisFieldDependencies.get();
	}

	void thisMemberVarDependencyAdd(const EsString& varName)
	{
		checkTemplateOperation();
		m_thisMemberVarDependencies->push_back(varName);
	}

	inline const EsString::Array& thisMemberVarDependenciesGet() const ES_NOTHROW
	{
		return *m_thisMemberVarDependencies.get();
	}

	// try-catch blocks manipulation
	//
	inline const EsScriptTryCatchBlocks& tryCatchBlocksAccess() const ES_NOTHROW { return m_tryCatchBlocks; }

	// create new try-catch block, return its index in try-catch blocks collection
	ulong tryCatchBlockCreate(ulong tryStart = 0, ulong tryEnd = 0, ulong catchStart = 0, ulong catchEnd = 0);

	// select and execute try-catch block at specified index
	EsScriptInstructionOpcode tryCatchBlockExecute(
    ulong tryCatchBlockIdx,
    EsScriptThreadContext& script,
		EsScriptInstructions::const_iterator& instr
  );

	// select try-block of the currently active try-catch block
	void tryBlockSelect();

	// select catch-block of the currently active try-catch block
	void catchBlockSelect();

	// deselect the currently active try-catch block
	void activeTryCatchBlockDeselect() ES_NOTHROW;

	// return true if the catch block is currently active
	bool isCatchBlockSelected() const;

#ifdef ES_DEBUG
	EsString asString() const;
#endif

protected:
	void checkTemplateOperation() const
	{
		if( !m_template )
			EsScriptException::Throw(esT("Operation is supported only by template code sections"));
	}

protected:
	// code section name
	EsString m_name;
	// metaclass owning this code section
	EsScriptObjectIntf* m_metaclass;
	// special "this" value, make sense only for object methods,
	// is valid during execution time only
	EsScriptObjectIntf* m_this;
	// if this code section is object code, the dependency list
	// may contain this object field names on which this code section
	// result is depending on
	EsString::ArrayPtr m_thisFieldDependencies;
	EsString::ArrayPtr m_thisMemberVarDependencies;
	// code section attributes (optional)
	EsAttributesIntf::Ptr m_attrs;
  // variables local to this code section
  EsScriptSymbolTable m_vars;
	// reference to the block input parameters, for indexed access
	EsString::ArrayPtr m_params;
	// shared pointer to the code instructions
	EsScriptInstructionsPtr m_code;
	// return value register
	EsScriptValAccessorIntf::Ptr m_return;
	// try-catch blocks && stuff
	EsScriptTryCatchBlocks m_tryCatchBlocks;
	std::vector<ulong> m_tryCatchStack;
	// is this code section a template
	bool m_template;

private:
	EsScriptCodeSection() ES_REMOVEDECL;
	EsScriptCodeSection(const EsScriptCodeSection&) ES_REMOVEDECL;
	const EsScriptCodeSection& operator=(const EsScriptCodeSection&) ES_REMOVEDECL;

	friend class EsScriptMachine;
	friend class EsScriptObject;
	friend class EsScriptArrayObject;
	friend class EsScriptIfObject;
	friend class EsScriptTryCatchBlock;
};
//---------------------------------------------------------------------------

/// Script methods map types
typedef std::map<EsMethodInfoKeyT, EsScriptCodeSection::Ptr> EsScriptMethodMap;
typedef std::shared_ptr<EsScriptMethodMap> EsScriptMethodMapPtr;
//---------------------------------------------------------------------------

#endif // _es_code_section_h_
