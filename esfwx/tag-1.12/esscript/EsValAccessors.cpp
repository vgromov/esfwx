#include "stdafx.h"
#pragma hdrstop

#include "EsValAccessors.h"

// constants
//
EsConstAccessor::EsConstAccessor(const EsVariant& val) :
m_val(val)
{
}

EsConstAccessor::~EsConstAccessor()
{}

// interface impl
ES_IMPL_INTF_METHOD(EsVariant, EsConstAccessor::get)() const
{
	return m_val;
}

ES_IMPL_INTF_METHOD(EsVariant&, EsConstAccessor::acc)()
{	
	EsException::Throw(0, EsException::Generic, wxT("Constant may not be accessed in a writeable way"));
	return m_val;
}

ES_IMPL_INTF_METHOD(void, EsConstAccessor::set)(const EsVariant& val)
{
	EsException::Throw(0, EsException::Generic, wxT("Cannot set value of a constant"));
}

// values
//
EsValAccessor::EsValAccessor(const EsVariant& val, const std::locale& loc) :
m_val(),
m_loc(loc)
{}

EsValAccessor::~EsValAccessor()
{}
	
// interface impl
ES_IMPL_INTF_METHOD(EsVariant, EsValAccessor::get)() const
{
	wxLogDebug(wxT("EsValAccessor::get() for '%s'"), EsScriptMachine::traceVariant(m_val).c_str());
	return m_val;
}

ES_IMPL_INTF_METHOD(EsVariant&, EsValAccessor::acc)()
{
	wxLogDebug(wxT("EsValAccessor::acc() for '%s'"), EsScriptMachine::traceVariant(m_val).c_str());
	return m_val;
}

ES_IMPL_INTF_METHOD(void, EsValAccessor::set)(const EsVariant& val)
{
	wxLogDebug(wxT("EsValAccessor::set('%s') for '%s'"), EsScriptMachine::traceVariant(val).c_str(), EsScriptMachine::traceVariant(m_val).c_str());
	m_val = val;
}

// items
//
EsItemAccessor::EsItemAccessor(EsAccessorIntf::Ptr valAcc, EsAccessorIntf::Ptr idxExprAcc, const std::locale& loc) :
m_loc(loc)
{
	wxASSERT(valAcc);
	wxASSERT(idxExprAcc);
	m_valAcc = valAcc;
	m_idxExprAcc = idxExprAcc;
}

EsItemAccessor::~EsItemAccessor()
{}

// interface impl
ES_IMPL_INTF_METHOD(EsVariant, EsItemAccessor::get)() const
{
	EsVariant idx = m_idxExprAcc->get();
	EsVariant item = m_valAcc->acc().getItem( idx );
	wxLogDebug(wxT("EsItemAccessor::get()['%s'] returns '%s'"), EsScriptMachine::traceVariant(idx).c_str(), EsScriptMachine::traceVariant(item).c_str());
	return item;
}

ES_IMPL_INTF_METHOD(EsVariant&, EsItemAccessor::acc)()
{
	EsException::Throw(0, EsException::Generic, wxT("Direct indexed item access is not supported"));
	return m_valAcc->acc();
}

ES_IMPL_INTF_METHOD(void, EsItemAccessor::set)(const EsVariant& val)
{
	EsVariant idx = m_idxExprAcc->get();
	wxLogDebug(wxT("EsItemAccessor::set('%s')['%s']"), EsScriptMachine::traceVariant(val).c_str(), EsScriptMachine::traceVariant(idx).c_str());
	return m_valAcc->acc().setItem( idx, val, m_loc );
}
