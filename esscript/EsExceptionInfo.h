#ifndef _es_exception_info_h_
#define _es_exception_info_h_

// proxy class for passing caught exception information 
// to the script code
//
class ESSCRIPT_CLASS ES_INTF_IMPL1(EsExceptionInfo, EsReflectedClassIntf)

protected:
  EsExceptionInfo(
    ulong severity, 
    ulong facility, 
    ulong code, 
    const EsString& reason, 
    const EsVariant& data,
    const EsVariant& trace,
    const EsScriptDebugInfoIntf::Ptr& dbg
  );

  static EsReflectedClassIntf::Ptr create(
    ulong severity, 
    ulong facility, 
    ulong code, 
    const EsString& reason, 
    const EsVariant& data = EsVariant::null(), 
    const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr(),
    const EsVariant& trace = EsVariant::null()
  );
  
  static EsReflectedClassIntf::Ptr create(
    const EsException& ex, 
    const EsScriptDebugInfoIntf::Ptr& dbg,
    const EsVariant& trace = EsVariant::null()
  );
  
  static EsReflectedClassIntf::Ptr create(
    const std::exception& ex, 
    const EsVariant& data,
    const EsScriptDebugInfoIntf::Ptr& dbg,  
    const EsVariant& trace = EsVariant::null()
  );

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsExceptionInfo)
  ES_DECL_ATTR_HANDLING_STD

  // Standard conversion to string
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString);

  // read-only exception properties
  ES_DECL_PROPERTY_RO_STD(severity, ulong)
  ES_DECL_PROPERTY_RO_STD(facility, ulong)
  ES_DECL_PROPERTY_RO_STD(code, ulong)
  ES_DECL_PROPERTY_RO_STD(reason, cr_EsString)
  ES_DECL_PROPERTY_RO_STD(data, cr_EsVariant)
  ES_DECL_PROPERTY_RO_STD(trace, cr_EsVariant)

protected:
  EsScriptDebugInfoIntf::Ptr m_dbg;

  ES_NO_DEFAULT_CTR( EsExceptionInfo );
  ES_NON_COPYABLE( EsExceptionInfo );

  friend class EsScriptTryCatchBlock;
};

#endif // _es_exception_info_h_
