#include "stdafx.h"
#pragma hdrstop

#include "EsSymtable.h"

// special empty value
const EsAccessorIntf::Ptr EsSymTable::s_empty;

EsSymTable::EsSymTable(bool checkTemplate /*= true*/) :
m_checkTemplate(checkTemplate),
m_template(true)
{
}

EsSymTable::EsSymTable(const EsSymTable& src, const std::locale& loc) :
m_checkTemplate(src.m_checkTemplate),
m_template(false)
{
	wxASSERT(src.isTemplate());
	for( Map::const_iterator cit = src.m_syms.begin(); cit != src.m_syms.end(); ++cit )
		m_syms[(*cit).first] = EsValAccessor::create(EsVariant::s_null, loc);
}

// symbol container implementation
void EsSymTable::reset()
{
	m_syms.clear();
}

bool EsSymTable::findSymbol(const EsString& name) const
{
	return m_syms.end() != m_syms.find(name);
}

void EsSymTable::addSymbol(const EsString& name)
{
	addSymbol(name, s_empty);
}

void EsSymTable::addSymbol(const EsString& name, EsAccessorIntf::Ptr acc)
{
	checkTemplateOperation();
	if( findSymbol(name) )
		EsException::Throw(0, EsException::Generic, wxT("Symbol '%s' is already declared"), name.c_str());
	
	m_syms[name] = acc;
}
	
EsAccessorIntf::Ptr EsSymTable::getSymbol(const EsString& name, bool doThrow)
{
	checkNonTemplateOperation();
	Map::iterator it = m_syms.find(name);
	if( it != m_syms.end() )
		return (*it).second;
	else if( doThrow )
		EsException::Throw(0, EsException::Generic, wxT("Symbol '%s' is not found"), name.c_str());	
	
	return s_empty;
}

void EsSymTable::setSymbolVal(const EsString& name, const EsVariant& val)
{
	EsAccessorIntf::Ptr sym = getSymbol(name, true);
	if( !sym )
		EsException::Throw(0, EsException::Generic, wxT("Symbol '%s' was not initialized"), name.c_str());

	sym->set(val);
}

// internal services
void EsSymTable::checkTemplateOperation() const
{
	if( m_checkTemplate && !m_template )
		EsException::Throw(0, EsException::Generic, wxT("Operation only allowed on symbol table template") );
}

void EsSymTable::checkNonTemplateOperation() const
{
	if( m_checkTemplate && m_template )
		EsException::Throw(0, EsException::Generic, wxT("Operation only allowed on non template symbol table") );
}
