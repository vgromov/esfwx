#ifndef _es_code_section_h_
#define _es_code_section_h_

// script code section interface implementation
class EsCodeSection
{
public:
	// misc consts
	enum {
		defCodeCnt = 256,
		defParamsCnt = 8,
		defInlineCnt = 8,
		defInlineCalls = 8,
		defDependencyCount = 8,
	};
	
	typedef boost::shared_ptr<EsCodeSection> Ptr;
	
protected:
	// accessible to EsScriptMachine friend
	EsCodeSection(const EsString& name, const EsString& nameSpace, const std::locale& loc);
	static EsCodeSection::Ptr create(const EsString& name, const EsString& nameSpace, const std::locale& loc)
	{
		return Ptr(new EsCodeSection(name, nameSpace, loc));
	}
	// special copy constructor. every templated src member gets cloned to non-template, but for the code container itself, which just gets referenced
	EsCodeSection(const EsCodeSection& src);
	
public:
	~EsCodeSection();

	// EsCodeSection api implementation
	//
	// get code section name
	const EsString& getName() const
	{
		return m_name;
	}
	// get code section namespace
	const EsString& getNamespace() const
	{
		return m_namespace;
	}
	// global check
	bool isGlobal() const;
	// empty check
	bool isEmpty() const
	{
		wxASSERT(m_code);
		return m_code->empty();
	}	
	// template check
	bool isTemplate() const 
	{	
		return m_template; 
	}
	// object method check
	bool isObjectMethod() const
	{
		return 0 != m_this;
	}
	// working with code
	//
	size_t addInstruction(EsInstructionOpcode opcode, const EsVariant& param = EsVariant::s_null);
	void modifyInstruction(size_t instrPos, EsInstructionOpcode opcode, const EsVariant& param = EsVariant::s_null);
	EsInstructions::const_iterator instrStart() const;
	size_t instrStartPos() const 
	{
		return 0;
	}
	EsInstructions::const_iterator instrEnd() const;
	size_t instrEndPos() const
	{
		return m_code->size();
	}
	// var && param declaration && lookup
	void declVar(const EsString& name);
	void declParam(const EsString& name);
	// access count of input parameters
	size_t getInputParamCnt() const
	{
		return m_params.size();
	}
	// indexed parameter access
	void setParam(size_t idx, EsAccessorIntf::Ptr val);
	// all-at-once parameters assignment
	void setParams(const EsVariant& params);
	// local vars and params symbols access
	EsSymTable& vars()
	{
		return m_vars;
	}
	// check if variable is currently declared in this code section
	bool findVariable(const EsString& name) const
	{
		return m_vars.findSymbol(name);
	}
	// access section variable by name (for non-template mode)
	EsAccessorIntf::Ptr getVariable(const EsString& name)
	{
		return m_vars.getSymbol(name, true);
	}
	// execution result access. 
	//
	void setResult(EsAccessorIntf::Ptr result)
	{
		m_return = result;
	}
	// NB! internal result register must be reset after this call is made
	EsAccessorIntf::Ptr popResult()
	{
		EsAccessorIntf::Ptr result = m_return;
		m_return.reset();
		return result;
	}
	// object instance access
	EsScriptObjectIntf* getThis() const
	{
		return m_this;
	}
	void setThis(EsScriptObjectIntf* This)
	{
		m_this = This;
	}
	EsCodeSection::Ptr clone() const
	{
		checkTemplateOperation();
		return Ptr(new EsCodeSection(*this));
	}
	void addThisFieldDependency(const EsString& fieldName)
	{
		checkTemplateOperation();
		m_thisFieldDependencies.push_back(fieldName);
	}
	const EsString::Array& thisFieldDependenciesGet() const
	{
		return m_thisFieldDependencies;
	}
	
protected:
	void checkTemplateOperation() const
	{
		if( !m_template )
			EsException::Throw(0, EsException::Generic, wxT("Operation is supported only by template code sections"));
	}
	
protected:
	// is this code section a template
	bool m_template;
	// code section locale
	const std::locale& m_loc;
	// code section name
	EsString m_name;
	// code section namespace
	EsString m_namespace;
  // variables local to this code section
  EsSymTable m_vars;
	// reference to the block input parameters, for indexed access
	EsString::Array m_params;
	// shared pointer to the code instructions
	EsInstructionsPtr m_code;
	// return value register
	EsAccessorIntf::Ptr m_return;
	// special "self" value, make sense only for object methods
	EsScriptObjectIntf* m_this;
	// if this code section is object code, the dependency list
	// may contain this object field names on which this code section
	// result is depending on
	EsString::Array m_thisFieldDependencies;
	
	friend class EsScriptMachine;
	friend class EsScriptObject;
};

// call stack vector
typedef std::vector<EsCodeSection::Ptr> EsCallStack;

#endif // _es_code_section_h_