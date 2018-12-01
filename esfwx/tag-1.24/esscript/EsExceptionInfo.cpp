#include "esscriptpch.h"
#pragma hdrstop

#include "EsExceptionInfo.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_ENUMERATION_INFO(EsExceptionSeverity, esT("Known exception severity codes"))
ES_DECL_ENUMERATION_INFO(EsExceptionFacility, esT("Known exception origins"))

ES_DECL_BASE_CLASS_INFO_BEGIN(EsExceptionInfo, NO_CLASS_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, severity, ulong, esT("Exception severity"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, facility, ulong, esT("Exception source facility"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, code, ulong, esT("Exception code"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, reason, cr_EsString, esT("Exception reason"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, data, cr_EsVariant, esT("Exception payload data"), NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

EsExceptionInfo::EsExceptionInfo(ulong severity, ulong facility, ulong code, 
  const EsString& reason, const EsVariant& data, const EsScriptDebugInfoIntf::Ptr& dbg) :
m_severity(severity),
m_facility(facility),
m_code(code),
m_reason(reason),
m_data(data),
m_dbg(dbg)
{
  m_dynamic = true;
}

EsReflectedClassIntf::Ptr EsExceptionInfo::create(ulong severity, ulong facility, ulong code, const EsString& reason, 
    const EsVariant& data /*= EsVariant::s_null*/, const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  return EsReflectedClassIntf::Ptr( new EsExceptionInfo(severity, facility, code, reason, data, dbg) );
}

EsReflectedClassIntf::Ptr EsExceptionInfo::create(const EsException& ex, const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/)
{
  return create(ex.severityGet(), ex.facilityGet(), ex.codeGet(), ex.messageGet(), ex.dataGet(), dbg);
}

EsReflectedClassIntf::Ptr EsExceptionInfo::create(const std::exception& ex, const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/,  
  const EsVariant& data /*= EsVariant::s_null*/)
{
  const EsString& reason = EsString::fromUtf8(ex.what());
  return EsReflectedClassIntf::Ptr( new EsExceptionInfo(EsException::severityGeneric,
    EsException::facilityEsCommon, 0, reason, EsVariant::null(), dbg) );
}

ulong EsExceptionInfo::get_severity() const
{
  return m_severity;
}

ulong EsExceptionInfo::get_facility() const
{
  return m_facility;
}

ulong EsExceptionInfo::get_code() const
{
  return m_code;
}

cr_EsString EsExceptionInfo::get_reason() const
{
  return m_reason;
}

cr_EsVariant EsExceptionInfo::get_data() const
{
  return m_data;
}