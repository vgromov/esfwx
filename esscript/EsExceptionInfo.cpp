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

class EsExceptionInfoCtorAcc : public EsExceptionInfo
{
public:
  EsExceptionInfoCtorAcc(
    ulong severity,
    ulong facility,
    ulong code,
    const EsString& reason,
    const EsVariant& data,
    const EsVariant& trace,
    const EsScriptDebugInfoIntf::Ptr& dbg
  ) :
  EsExceptionInfo(
    severity,
    facility,
    code,
    reason,
    data,
    trace,
    dbg
  )
  {}
};
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsExceptionInfo, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsExceptionInfo, asString, EsString_CallConst, NO_METHOD_DESCR)

  ES_DECL_PROP_INFO_RO(EsExceptionInfo, severity, ulong, esT("Exception severity"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, facility, ulong, esT("Exception source facility"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, code, ulong, esT("Exception code"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, reason, cr_EsString, esT("Exception reason"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, data, cr_EsVariant, esT("Exception payload data"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsExceptionInfo, trace, cr_EsVariant, esT("Exception stack trace data"), NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

EsExceptionInfo::EsExceptionInfo(
  ulong severity, 
  ulong facility, 
  ulong code, 
  const EsString& reason, 
  const EsVariant& data, 
  const EsVariant& trace, 
  const EsScriptDebugInfoIntf::Ptr& dbg
) :
m_severity(severity),
m_facility(facility),
m_code(code),
m_reason(reason),
m_data(data),
m_trace(trace),
m_dbg(dbg)
{
  m_dynamic = true;
}
//--------------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsExceptionInfo::create(
  ulong severity, 
  ulong facility, 
  ulong code, 
  const EsString& reason, 
  const EsVariant& data /*= EsVariant::null()*/, 
  const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/,
  const EsVariant& trace /*= EsVariant::null()*/
)
{
  std::unique_ptr<EsExceptionInfoCtorAcc> ptr = ES_MAKE_UNIQUE(
    EsExceptionInfoCtorAcc,
    severity, 
    facility, 
    code, 
    reason, 
    data,
    trace,
    dbg
  );
  ES_ASSERT(ptr);
  
  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsExceptionInfo::create(
  const EsException& ex, 
  const EsScriptDebugInfoIntf::Ptr& dbg /*= EsScriptDebugInfoIntf::Ptr()*/,
  const EsVariant& trace /*= EsVariant::null()*/
)
{
  return create(
    ex.severityGet(), 
    ex.facilityGet(), 
    ex.codeGet(), 
    ex.messageGet(), 
    ex.dataGet(),
    dbg,
    trace
  );
}
//--------------------------------------------------------------------------------

EsReflectedClassIntf::Ptr EsExceptionInfo::create(
  const std::exception& ex, 
  const EsVariant& data,
  const EsScriptDebugInfoIntf::Ptr& dbg,  
  const EsVariant& trace /*= EsVariant::null()*/
)
{
  EsString reason = EsString::fromUtf8(ex.what());

  return create(
    EsException::severityGeneric,
    EsException::facilityEsCommon, 
    EsException::codeNone, 
    reason, 
    data, 
    dbg,
    trace
  );
}
//--------------------------------------------------------------------------------

ulong EsExceptionInfo::get_severity() const
{
  return m_severity;
}
//--------------------------------------------------------------------------------

ulong EsExceptionInfo::get_facility() const
{
  return m_facility;
}
//--------------------------------------------------------------------------------

ulong EsExceptionInfo::get_code() const
{
  return m_code;
}
//--------------------------------------------------------------------------------

cr_EsString EsExceptionInfo::get_reason() const
{
  return m_reason;
}
//--------------------------------------------------------------------------------

cr_EsVariant EsExceptionInfo::get_data() const
{
  return m_data;
}
//--------------------------------------------------------------------------------

cr_EsVariant EsExceptionInfo::get_trace() const
{
  return m_trace;
}
//--------------------------------------------------------------------------------

EsString EsExceptionInfo::asString() const
{
  return m_reason;
}
//---------------------------------------------------------------------------

