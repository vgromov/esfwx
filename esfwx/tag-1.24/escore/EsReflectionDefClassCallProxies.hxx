// Generate class call proxies
//
#define REFL_METHOD_ENTRIES_BEGIN
#define	REFL_METHOD_ENTRY0( RetVal )
#define	REFL_METHOD_ENTRY1( RetVal, Param1 )
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define	REFL_METHOD_ENTRY_NR0
#define	REFL_METHOD_ENTRY_NR1( Param1 )
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 )
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define	REFL_CONST_METHOD_ENTRY0( RetVal )
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define	REFL_CONST_METHOD_ENTRY_NR0
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 )
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define REFL_METHOD_ENTRIES_END

#define REFL_CLASS_METHOD_ENTRIES_BEGIN \
namespace EsReflectionCallProxies {
#define	REFL_CLASS_METHOD_ENTRY0( RetVal ) \
static void GEN_SIG0(RetVal, ClassCall ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant&) { \
	_ret = From_ ## RetVal( (( GEN_SIG0(RetVal, ClassCall_T) )method.m_classMethod)()); }
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 ) \
static void GEN_SIG1(RetVal, ClassCall, Param1 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
	_ret = From_ ## RetVal( (( GEN_SIG1(RetVal, ClassCall, Param1 ##_T) )method.m_classMethod)(To_## Param1 (params.itemGet(0)))); }
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 ) \
static void GEN_SIG2(RetVal, ClassCall, Param1, Param2 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_ ## RetVal( (( GEN_SIG2(RetVal, ClassCall, Param1, Param2 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]))); }
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) \
static void GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_ ## RetVal( (( GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]))); }
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) \
static void GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_ ## RetVal( (( GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]))); }
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) \
static void GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_ ## RetVal( (( GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]), To_## Param5 (aparams[4]))); }
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) \
static void GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_ ## RetVal( (( GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]), To_## Param5 (aparams[4]), To_## Param6 (aparams[5]))); }
#define	REFL_CLASS_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) \
static void GEN_SIG7(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy)(EsVariant& _ret, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	_ret = From_ ## RetVal( (( GEN_SIG7(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]), To_## Param5 (aparams[4]), To_## Param6 (aparams[5]), To_## Param7 (aparams[6]))); }

#define	REFL_CLASS_METHOD_ENTRY_NR0 \
static void GEN_SIG0(void, ClassCall ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant&) { \
	(( GEN_SIG0(void, ClassCall_T) )method.m_classMethod)(); }
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 ) \
static void GEN_SIG1(void, ClassCall, Param1 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
	(( GEN_SIG1(void, ClassCall, Param1 ##_T) )method.m_classMethod)(To_## Param1 (params.itemGet(0))); }
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 ) \
static void GEN_SIG2(void, ClassCall, Param1, Param2 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(( GEN_SIG2(void, ClassCall, Param1, Param2 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1])); }
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) \
static void GEN_SIG3(void, ClassCall, Param1, Param2, Param3 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(( GEN_SIG3(void, ClassCall, Param1, Param2, Param3 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2])); }
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) \
static void GEN_SIG4(void, ClassCall, Param1, Param2, Param3, Param4 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(( GEN_SIG4(void, ClassCall, Param1, Param2, Param3, Param4 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3])); }
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) \
static void GEN_SIG5(void, ClassCall, Param1, Param2, Param3, Param4, Param5 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(( GEN_SIG5(void, ClassCall, Param1, Param2, Param3, Param4, Param5 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]), To_## Param5 (aparams[4])); }
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) \
static void GEN_SIG6(void, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(( GEN_SIG6(void, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]), To_## Param5 (aparams[4]), To_## Param6 (aparams[5])); }
#define	REFL_CLASS_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) \
static void GEN_SIG7(void, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy)(EsVariant&, const EsMethodT& method, const EsVariant& params) { \
  const EsVariant::Array& aparams = params.asVariantCollection(); \
	(( GEN_SIG7(void, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_T) )method.m_classMethod)(To_## Param1 (aparams[0]), To_## Param2 (aparams[1]), To_## Param3 (aparams[2]), To_## Param4 (aparams[3]), To_## Param5 (aparams[4]), To_## Param6 (aparams[5]), To_## Param7 (aparams[6])); }
#define REFL_CLASS_METHOD_ENTRIES_END }
#include <escore/EsReflection.hxx>

