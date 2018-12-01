#ifndef _es_core_utils_h_
#define _es_core_utils_h_

/// Reflected core && OS utilities subset
///
ES_DECL_REFLECTED_SERVICES_BEGIN( EsCore )
  /// Global logging wrappers
  ES_DECL_REFLECTED_CLASS_METHOD1(void, logInfo, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, logInfo, cr_EsString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, logDebug, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, logDebug, cr_EsString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, logWarning, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, logWarning, cr_EsString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, logError, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, logError, cr_EsString, cr_EsVariant);
ES_DECL_REFLECTED_SERVICES_END

ES_DECL_REFLECTED_SERVICES_BEGIN( EsUtils )
  /// Some core utilities
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString,     GUIDtoStr, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString,     GUIDtoCanonicalStr, cr_EsBinBuffer);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer,  GUIDfromStr, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsBinBuffer,  bToE, cr_EsBinBuffer, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsBinBuffer,  eToB, cr_EsBinBuffer, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString,     sToE, cr_EsString, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString,     eToS, cr_EsString, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(esU16,        inverse2, esU16);
  ES_DECL_REFLECTED_CLASS_METHOD1(esU32,        inverse4, esU32);
  ES_DECL_REFLECTED_CLASS_METHOD1(esU64,        inverse8, esU64);
  ES_DECL_REFLECTED_CLASS_METHOD1(double,       round, double);
  ES_DECL_REFLECTED_CLASS_METHOD2(bool,         areEqualFloats, double, double);
  ES_DECL_REFLECTED_CLASS_METHOD3(bool,         areEqualFloats, double, double, ulong);
ES_DECL_REFLECTED_SERVICES_END


#endif // _es_core_utils_h_