#ifndef _es_script_debug_h_
#define _es_script_debug_h_

ES_DECL_REFLECTED_SERIVES_BEGIN( EsScriptDebug )
	ES_DECL_REFLECTED_CLASS_METHOD1(void, log, cr_EsString);
	ES_DECL_REFLECTED_CLASS_METHOD2(void, log, cr_EsString, cr_EsVariant);
	ES_DECL_REFLECTED_CLASS_METHOD3(void, log, cr_EsString, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_CLASS_METHOD4(void, log, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_CLASS_METHOD5(void, log, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
	ES_DECL_REFLECTED_CLASS_METHOD6(void, log, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
ES_DECL_REFLECTED_SERIVES_END

#endif // _es_script_debug_h_
