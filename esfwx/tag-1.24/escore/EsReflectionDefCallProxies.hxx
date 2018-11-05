// generate member calls
//
#define REFL_METHOD_ENTRIES_BEGIN	\
namespace EsReflectionCallProxies {
#define	REFL_METHOD_ENTRY0( RetVal ) \
static void GEN_SIG0(RetVal, Call ##_proxy) (EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant&) { \
	_ret = From_##RetVal( (obj->*( GEN_SIG0(RetVal, Call_T) )method.m_method)()); }
#define	REFL_METHOD_ENTRY1( RetVal, Param1 ) \
static void GEN_SIG1(RetVal, Call, Param1 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
	_ret = From_##RetVal( (obj->*( GEN_SIG1(RetVal, Call, Param1 ##_T) )method.m_method)(To_##Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.typeGet()) ? params.itemGet(0) : params))); }
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 ) \
static void GEN_SIG2(RetVal, Call, Param1, Param2 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG2(RetVal, Call, Param1, Param2 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]))); }
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) \
static void GEN_SIG3(RetVal, Call, Param1, Param2, Param3 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG3(RetVal, Call, Param1, Param2, Param3 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]))); }
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) \
static void GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]))); }
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) \
static void GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]))); }
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) \
static void GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5]))); }
#define	REFL_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) \
static void GEN_SIG7(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG7(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5]), To_##Param7 (aparams[6]))); }

#define	REFL_CONST_METHOD_ENTRY0( RetVal ) \
static void GEN_SIG0(RetVal, CallConst ##_proxy) (EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant&) { \
	_ret = From_##RetVal( (obj->*( GEN_SIG0(RetVal, CallConst_T) )method.m_method)()); }
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 ) \
static void GEN_SIG1(RetVal, CallConst, Param1 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
	_ret = From_##RetVal( (obj->*( GEN_SIG1(RetVal, CallConst, Param1 ##_T) )method.m_method)(To_##Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.typeGet()) ? params.itemGet(0) : params))); }
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 ) \
static void GEN_SIG2(RetVal, CallConst, Param1, Param2 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG2(RetVal, CallConst, Param1, Param2 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]))); }
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) \
static void GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]))); }
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) \
static void GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]))); }
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) \
static void GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]))); }
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) \
static void GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5]))); }
#define	REFL_CONST_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) \
static void GEN_SIG7(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy)(EsVariant& _ret, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_##RetVal( (obj->*( GEN_SIG7(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5]), To_##Param7 (aparams[6]))); }

#define	REFL_METHOD_ENTRY_NR0 \
static void GEN_SIG0(void, Call ##_proxy) (EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant&) { \
	(obj->*( GEN_SIG0(void, Call_T) )method.m_method)(); }
#define	REFL_METHOD_ENTRY_NR1( Param1 ) \
static void GEN_SIG1(void, Call, Param1 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
	(obj->*( GEN_SIG1(void, Call, Param1 ##_T) )method.m_method)(To_##Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.typeGet()) ? params.itemGet(0) : params)); }
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 ) \
static void GEN_SIG2(void, Call, Param1, Param2 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG2(void, Call, Param1, Param2 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1])); }
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) \
static void GEN_SIG3(void, Call, Param1, Param2, Param3 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG3(void, Call, Param1, Param2, Param3 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2])); }
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) \
static void GEN_SIG4(void, Call, Param1, Param2, Param3, Param4 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG4(void, Call, Param1, Param2, Param3, Param4 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3])); }
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) \
static void GEN_SIG5(void, Call, Param1, Param2, Param3, Param4, Param5 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG5(void, Call, Param1, Param2, Param3, Param4, Param5 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4])); }
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) \
static void GEN_SIG6(void, Call, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG6(void, Call, Param1, Param2, Param3, Param4, Param5, Param6 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5])); }
#define	REFL_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) \
static void GEN_SIG7(void, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG7(void, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5]), To_##Param7 (aparams[6])); }

#define	REFL_CONST_METHOD_ENTRY_NR0 \
static void GEN_SIG0(void, CallConst ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant&) { \
	(obj->*( GEN_SIG0(void, CallConst_T) )method.m_method)(); }
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 ) \
static void GEN_SIG1(void, CallConst, Param1 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
	(obj->*( GEN_SIG1(void, CallConst, Param1 ##_T) )method.m_method)(To_##Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.typeGet()) ? params.itemGet(0) : params)); }
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 ) \
static void GEN_SIG2(void, CallConst, Param1, Param2 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG2(void, CallConst, Param1, Param2 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1])); }
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) \
static void GEN_SIG3(void, CallConst, Param1, Param2, Param3 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG3(void, CallConst, Param1, Param2, Param3 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2])); }
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) \
static void GEN_SIG4(void, CallConst, Param1, Param2, Param3, Param4 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG4(void, CallConst, Param1, Param2, Param3, Param4 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3])); }
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) \
static void GEN_SIG5(void, CallConst, Param1, Param2, Param3, Param4, Param5 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG5(void, CallConst, Param1, Param2, Param3, Param4, Param5 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4])); }
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) \
static void GEN_SIG6(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG6(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5])); }
#define	REFL_CONST_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) \
static void GEN_SIG7(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy)(EsVariant&, EsBaseIntf* obj, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(obj->*( GEN_SIG7(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_T) )method.m_method)(To_##Param1 (aparams[0]), To_##Param2 (aparams[1]), To_##Param3 (aparams[2]), To_##Param4 (aparams[3]), To_##Param5 (aparams[4]), To_##Param6 (aparams[5]), To_##Param7 (aparams[6])); }

#define REFL_METHOD_ENTRIES_END	}

#define REFL_CLASS_METHOD_ENTRIES_BEGIN
#define	REFL_CLASS_METHOD_ENTRY0( RetVal )
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CLASS_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define	REFL_CLASS_METHOD_ENTRY_NR0
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 )
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CLASS_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define REFL_CLASS_METHOD_ENTRIES_END
#include <escore/EsReflection.hxx>
