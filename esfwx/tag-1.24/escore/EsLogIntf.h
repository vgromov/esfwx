#ifndef _es_log_intf_h_
#define _es_log_intf_h_

// logger interface
//
ES_DECL_INTF_BEGIN2( 70669FA1, 4A0B43ba, 9C474F2B, A9BC64C6, EsLogIntf, EsBaseIntf )
  ES_DECL_INTF_METHOD(void, logInfo)(const EsString& msg, const EsVariant& val = EsVariant::null()) = 0;
  ES_DECL_INTF_METHOD(void, logDebug)(const EsString& msg, const EsVariant& val = EsVariant::null()) = 0;
  ES_DECL_INTF_METHOD(void, logWarning)(const EsString& msg, const EsVariant& val = EsVariant::null()) = 0;
  ES_DECL_INTF_METHOD(void, logError)(const EsString& msg, const EsVariant& val = EsVariant::null()) = 0;
ES_DECL_INTF_END

#endif // _es_log_intf_h_