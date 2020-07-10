#ifndef _es_exception_severity_facility_h_
#define _es_exception_severity_facility_h_

/// Reflected enumerations for exception severity and facility
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsExceptionSeverity)
  ES_DECL_ENUMERATION_ITEM(Warning,       EsException::severityWarning)
  ES_DECL_ENUMERATION_ITEM(Generic,       EsException::severityGeneric)
  ES_DECL_ENUMERATION_ITEM(Critical,      EsException::severityCritical)
ES_DECL_ENUMERATION_END

ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsExceptionFacility)
  ES_DECL_ENUMERATION_ITEM(Unknown,       EsException::facilityEsUnknown)
  ES_DECL_ENUMERATION_ITEM(Common,        EsException::facilityEsCommon)
  ES_DECL_ENUMERATION_ITEM(OS,            EsException::facilityEsOS)
  ES_DECL_ENUMERATION_ITEM(Numeric,       EsException::facilityEsNumeric)
  ES_DECL_ENUMERATION_ITEM(File,          EsException::facilityEsFile)
  ES_DECL_ENUMERATION_ITEM(Security,      EsException::facilityEsSecurity)
  ES_DECL_ENUMERATION_ITEM(RPC,           EsException::facilityEsRPC)
  ES_DECL_ENUMERATION_ITEM(Script,        EsException::facilityEsScript)
  ES_DECL_ENUMERATION_ITEM(XML,           EsException::facilityEsXml)
  ES_DECL_ENUMERATION_ITEM(Sockets,       EsException::facilityEsSockets)
  ES_DECL_ENUMERATION_ITEM(Math,          EsException::facilityEsMath)
  ES_DECL_ENUMERATION_ITEM(RegEx,         EsException::facilityEsRegEx)
  ES_DECL_ENUMERATION_ITEM(MDI,           EsException::facilityEsMDI)
  ES_DECL_ENUMERATION_END

#endif //< _es_exception_severity_facility_h_
