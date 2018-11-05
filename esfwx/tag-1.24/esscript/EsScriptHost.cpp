#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptHost.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsScriptHost, NO_CLASS_DESCR)
	// services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptHost, eventPost, void_Call_cr_EsString_ulong_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptHost, eventPostUrgent, void_Call_cr_EsString_ulong_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptHost, eventPost, void_Call_ulong_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptHost, eventPostUrgent, void_Call_ulong_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsScriptHost, marshal, void_Call_cr_EsVariant, NO_METHOD_DESCR)
	// properties
	ES_DECL_PROP_INFO_RO(							EsScriptHost, script, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
	ES_DECL_PROP_INFO(								EsScriptHost, progressMonitor, EsVariant, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(             EsScriptHost, aborting, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsScriptHost::EsScriptHost(const EsScriptMachine& owner) :
m_owner(owner)
{
	ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsVariant EsScriptHost::get_script() const
{
	EsBaseIntf::Ptr tmp( const_cast<EsScript&>(m_owner.m_owner).asBaseIntf(), false, false );

	return tmp;
}
//---------------------------------------------------------------------------

void EsScriptHost::set_progressMonitor(const EsVariant& val)
{
	if( !val.isEmpty() )
	{
		EsProgressMonitorIntf::Ptr mon = val.asExistingObject();
		if( !mon )
			EsException::Throw(esT("Could not assign progress monitor. Object is of incompatible type."));
	}

	m_progressMonitor = val;
}
//---------------------------------------------------------------------------

EsVariant EsScriptHost::get_progressMonitor() const
{
	return m_progressMonitor;
}
//---------------------------------------------------------------------------

bool EsScriptHost::get_aborting() const
{
  return m_owner.m_abort;
}
//---------------------------------------------------------------------------

void EsScriptHost::marshal(const EsVariant& data)
{
  m_owner.marshalData(data);
}
//---------------------------------------------------------------------------

// post event from script, given category, id, data
void EsScriptHost::eventPost(cr_EsString category, ulong id, cr_EsVariant payload)
{
  m_owner.marshalData(payload);

	EsEventDispatcher::eventPost(category, id, payload);
}
//---------------------------------------------------------------------------

// post urget event from script
void EsScriptHost::eventPostUrgent(cr_EsString category, ulong id, cr_EsVariant payload)
{
  m_owner.marshalData(payload);

	EsEventDispatcher::eventPostUrgent(category, id, payload );
}
//---------------------------------------------------------------------------

// post event from script, given category, id, data
void EsScriptHost::eventPost(ulong id, cr_EsVariant payload)
{
	eventPost(esT("generic"), id, payload);
}
//---------------------------------------------------------------------------

// post urget event from script
void EsScriptHost::eventPostUrgent(ulong id, cr_EsVariant payload)
{
	eventPostUrgent(esT("generic"), id, payload);
}
//---------------------------------------------------------------------------
