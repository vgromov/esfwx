#ifndef _es_exception_info_h_
#define _es_exception_info_h_

// reflected enumerations for exception type and kind checking in script
//
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESSCRIPT_CLASS, EsExceptionSeverity)
	ES_DECL_ENUMERATION_ITEM(Warning,			EsException::severityWarning)
	ES_DECL_ENUMERATION_ITEM(Generic,			EsException::severityGeneric)
	ES_DECL_ENUMERATION_ITEM(Critical,		EsException::severityCritical)
ES_DECL_ENUMERATION_END

ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESSCRIPT_CLASS, EsExceptionFacility)
	ES_DECL_ENUMERATION_ITEM(Unknown,			EsException::facilityEsUnknown)
	ES_DECL_ENUMERATION_ITEM(Common,			EsException::facilityEsCommon)
	ES_DECL_ENUMERATION_ITEM(OS,					EsException::facilityEsOS)
	ES_DECL_ENUMERATION_ITEM(Numeric,			EsException::facilityEsNumeric)
	ES_DECL_ENUMERATION_ITEM(File,				EsException::facilityEsFile)
	ES_DECL_ENUMERATION_ITEM(Security,		EsException::facilityEsSecurity)
	ES_DECL_ENUMERATION_ITEM(RPC,					EsException::facilityEsRPC)
	ES_DECL_ENUMERATION_ITEM(Script,			EsException::facilityEsScript)
ES_DECL_ENUMERATION_END

// proxy class for passing caught exception information 
// to the script code
//
class ESSCRIPT_CLASS ES_INTF_IMPL1(EsExceptionInfo, EsReflectedClassIntf)

protected:
	EsExceptionInfo(ulong severity, ulong facility, ulong code, const EsString& reason, const EsVariant& data,
		const EsScriptDebugInfoIntf::Ptr& dbg);
	static EsReflectedClassIntf::Ptr create(ulong severity, ulong facility, ulong code, const EsString& reason, 
		const EsVariant& data = EsVariant::null(), const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());
	static EsReflectedClassIntf::Ptr create(const EsException& ex, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr());
	static EsReflectedClassIntf::Ptr create(const std::exception& ex, const EsScriptDebugInfoIntf::Ptr& dbg = EsScriptDebugInfoIntf::Ptr(),	
		const EsVariant& data = EsVariant::null());

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsExceptionInfo)
	ES_DECL_ATTR_HANDLING_STD

	// read-only exception properties
	ES_DECL_PROPERTY_RO_STD(severity, ulong)
	ES_DECL_PROPERTY_RO_STD(facility, ulong)
	ES_DECL_PROPERTY_RO_STD(code, ulong)
	ES_DECL_PROPERTY_RO_STD(reason, cr_EsString)
	ES_DECL_PROPERTY_RO_STD(data, cr_EsVariant)

private:
	EsExceptionInfo();
	EsExceptionInfo(const EsExceptionInfo&);
	const EsExceptionInfo& operator=(const EsExceptionInfo&);

protected:
	EsScriptDebugInfoIntf::Ptr m_dbg;
	
	friend class EsScriptTryCatchBlock;
};

#endif // _es_exception_info_h_
