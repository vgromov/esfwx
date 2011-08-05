#include "stdafx.h"
#pragma hdrstop

#ifndef USE_MEMLEAK_DETECTION
#	define BOOST_SPIRIT_USE_BOOST_ALLOCATOR_FOR_TREES
#endif
#include <boost/spirit/include/classic_symbols.hpp>
using namespace boost::spirit::classic;

#include "EsCodeSection.h"

// code section implementation
//
EsCodeSection::EsCodeSection( const EsString& name, const EsString& nameSpace, const std::locale& loc ) :
m_template(true),
m_loc(loc),
m_name(name),
m_namespace(nameSpace),
m_return(0),
m_this(0)
{
	wxLogDebug(wxT("Code section template '%s::%s' ctor called"), m_namespace.c_str(), m_name.c_str()); 
	m_code.reset( new EsInstructions );
	m_code->reserve(defCodeCnt);
	m_params.reserve(defParamsCnt);
	m_thisFieldDependencies.reserve(defDependencyCount);
}

// special copy constructor. every templated src member gets cloned to non-template, but for the code container itself, which just gets referenced
EsCodeSection::EsCodeSection(const EsCodeSection& src) :
m_template(false),
m_loc(src.m_loc),
m_name(src.m_name),
m_namespace(src.m_namespace),
m_vars(src.m_vars, src.m_loc),
m_params(src.m_params),
m_code(src.m_code),
m_return(0),
m_this(0),
m_thisFieldDependencies(src.m_thisFieldDependencies)
{
	wxLogDebug(wxT("Code section '%s::%s' copy ctor called"), m_namespace.c_str(), m_name.c_str()); 
}

EsCodeSection::~EsCodeSection()
{
	wxLogDebug(wxT("Code section '%s::%s' (taking %d parameters) dtor called"), 
		m_namespace.c_str(), m_name.c_str(), m_params.size()); 
}

// global check
bool EsCodeSection::isGlobal() const
{
	return EsString::cmpEqual == m_namespace.compare(EsScriptMachine::s_global);
}

size_t EsCodeSection::addInstruction(EsInstructionOpcode opcode, const EsVariant& param /*= EsVariant::s_null*/)
{
	checkTemplateOperation();
	EsInstruction instr(opcode, param);
	wxASSERT(m_code);
	m_code->push_back(instr);
	wxLogDebug(wxT("instruction '%s' added to '%s::%s'"), 
		EsInstruction::getOpcodeString(instr.opcode()), m_namespace.c_str(), m_name.c_str());
	
	return m_code->size()-1;
}

void EsCodeSection::modifyInstruction(size_t instrPos, EsInstructionOpcode opcode, const EsVariant& param /*= EsVariant::s_null*/)
{
	checkTemplateOperation();
	wxASSERT(m_code);
	wxASSERT(instrPos < m_code->size());
	EsInstruction& instr = m_code->at(instrPos);
	instr.m_opcode = opcode;
	instr.m_param = param;
	wxLogDebug(wxT("Instruction '%s' modified in '%s::%s' at %d"), 
		EsInstruction::getOpcodeString(opcode), m_namespace.c_str(), m_name.c_str(), instrPos);
}

EsInstructions::const_iterator EsCodeSection::instrStart() const
{
	wxASSERT(m_code);	
	return m_code->begin();
}

EsInstructions::const_iterator EsCodeSection::instrEnd() const
{
	wxASSERT(m_code);
	return m_code->end();
}

// add variable to code section, registering it in symtable
void EsCodeSection::declVar(const EsString& name)
{
	EsScript::checkNotReservedWord(name);
	EsScript::checkNotRegisteredObject(name);
	checkTemplateOperation();
	m_vars.addSymbol(name);
	wxLogDebug(wxT("'%s' declared in section '%s::%s'"), name.c_str(), m_namespace.c_str(), m_name.c_str());
}

// add input parameter to code section
void EsCodeSection::declParam(const EsString& name)
{
	declVar(name);
	m_params.push_back(name);
	wxLogDebug(wxT("param '%s' declared in section '%s::%s'"), name.c_str(), m_namespace.c_str(), m_name.c_str());
}

// indexed parameter access
void EsCodeSection::setParam(size_t idx, EsAccessorIntf::Ptr val)
{
	wxASSERT(idx < m_params.size());
	wxASSERT(val);
	m_vars.setSymbolVal(m_params[idx], val->get());
}

// all-at-once parameters assignment
void EsCodeSection::setParams(const EsVariant& params)
{
	if( params.getType() == EsVariant::VAR_VARIANT_COLLECTION )
	{
		wxASSERT(params.getCount() == m_params.size());
		for( int idx = 0; idx < params.getCount(); ++idx )
			m_vars.setSymbolVal(m_params[idx], params.getItem(idx));
	}
	else if( !params.isEmpty() )
	{
		wxASSERT(1 == m_params.size());
		m_vars.setSymbolVal(m_params[0], params);
	}
}
