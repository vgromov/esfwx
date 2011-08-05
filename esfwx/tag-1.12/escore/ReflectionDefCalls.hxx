// generate member calls
//
#define REFL_METHOD_ENTRIES_BEGIN	\
switch( m_signature ) {
#define	REFL_METHOD_ENTRY0( RetVal ) \
case GEN_SIG0(RetVal, Call): \
	result = From_ ## RetVal( (obj->*( GEN_SIG0(RetVal, Call) ##_T )m_method)()); break;
#define	REFL_METHOD_ENTRY1( RetVal, Param1 ) \
case GEN_SIG1(RetVal, Call, Param1): \
	result = From_ ## RetVal( (obj->*( GEN_SIG1(RetVal, Call, Param1) ##_T )m_method)(To_## Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.getType()) ? params.getItem(0) : params))); break;
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 ) \
case GEN_SIG2(RetVal, Call, Param1, Param2): \
	result = From_ ## RetVal( (obj->*( GEN_SIG2(RetVal, Call, Param1, Param2) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)))); break;
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) \
case GEN_SIG3(RetVal, Call, Param1, Param2, Param3): \
	result = From_ ## RetVal( (obj->*( GEN_SIG3(RetVal, Call, Param1, Param2, Param3) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)))); break;
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) \
case GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4): \
	result = From_ ## RetVal( (obj->*( GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)))); break;
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) \
case GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5): \
	result = From_ ## RetVal( (obj->*( GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)))); break;
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) \
case GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6): \
	result = From_ ## RetVal( (obj->*( GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)), To_## Param6 (params.getItem(5)))); break;

#define	REFL_CONST_METHOD_ENTRY0( RetVal ) \
case GEN_SIG0(RetVal, CallConst): \
	result = From_ ## RetVal( (obj->*( GEN_SIG0(RetVal, CallConst) ##_T )m_method)()); break;
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 ) \
case GEN_SIG1(RetVal, CallConst, Param1): \
	result = From_ ## RetVal( (obj->*( GEN_SIG1(RetVal, CallConst, Param1) ##_T )m_method)(To_## Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.getType()) ? params.getItem(0) : params))); break;
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 ) \
case GEN_SIG2(RetVal, CallConst, Param1, Param2): \
	result = From_ ## RetVal( (obj->*( GEN_SIG2(RetVal, CallConst, Param1, Param2) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)))); break;
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) \
case GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3): \
	result = From_ ## RetVal( (obj->*( GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)))); break;
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) \
case GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4): \
	result = From_ ## RetVal( (obj->*( GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)))); break;
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) \
case GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5): \
	result = From_ ## RetVal( (obj->*( GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)))); break;
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) \
case GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6): \
	result = From_ ## RetVal( (obj->*( GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)), To_## Param6 (params.getItem(5)))); break;

#define	REFL_METHOD_ENTRY_NR0 \
case GEN_SIG0(void, Call): \
	(obj->*( GEN_SIG0(void, Call) ##_T )m_method)(); break;
#define	REFL_METHOD_ENTRY_NR1( Param1 ) \
case GEN_SIG1(void, Call, Param1): \
	(obj->*( GEN_SIG1(void, Call, Param1) ##_T )m_method)(To_## Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.getType()) ? params.getItem(0) : params)); break;
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 ) \
case GEN_SIG2(void, Call, Param1, Param2): \
	(obj->*( GEN_SIG2(void, Call, Param1, Param2) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1))); break;
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) \
case GEN_SIG3(void, Call, Param1, Param2, Param3): \
	(obj->*( GEN_SIG3(void, Call, Param1, Param2, Param3) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2))); break;
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) \
case GEN_SIG4(void, Call, Param1, Param2, Param3, Param4): \
	(obj->*( GEN_SIG4(void, Call, Param1, Param2, Param3, Param4) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3))); break;
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) \
case GEN_SIG5(void, Call, Param1, Param2, Param3, Param4, Param5): \
	(obj->*( GEN_SIG5(void, Call, Param1, Param2, Param3, Param4, Param5) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4))); break;
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) \
case GEN_SIG6(void, Call, Param1, Param2, Param3, Param4, Param5, Param6): \
	(obj->*( GEN_SIG6(void, Call, Param1, Param2, Param3, Param4, Param5, Param6) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)), To_## Param6 (params.getItem(5))); break;

#define	REFL_CONST_METHOD_ENTRY_NR0 \
case GEN_SIG0(void, CallConst): \
	(obj->*( GEN_SIG0(void, CallConst) ##_T )m_method)(); break;
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 ) \
case GEN_SIG1(void, CallConst, Param1): \
	(obj->*( GEN_SIG1(void, CallConst, Param1) ##_T )m_method)(To_## Param1 ((EsVariant::VAR_VARIANT_COLLECTION == params.getType()) ? params.getItem(0) : params)); break;
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 ) \
case GEN_SIG2(void, CallConst, Param1, Param2): \
	(obj->*( GEN_SIG2(void, CallConst, Param1, Param2) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1))); break;
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) \
case GEN_SIG3(void, CallConst, Param1, Param2, Param3): \
	(obj->*( GEN_SIG3(void, CallConst, Param1, Param2, Param3) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2))); break;
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) \
case GEN_SIG4(void, CallConst, Param1, Param2, Param3, Param4): \
	(obj->*( GEN_SIG4(void, CallConst, Param1, Param2, Param3, Param4) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3))); break;
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) \
case GEN_SIG5(void, CallConst, Param1, Param2, Param3, Param4, Param5): \
	(obj->*( GEN_SIG5(void, CallConst, Param1, Param2, Param3, Param4, Param5) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4))); break;
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) \
case GEN_SIG6(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6): \
	(obj->*( GEN_SIG6(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6) ##_T )m_method)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)), To_## Param6 (params.getItem(5))); break;
#define REFL_METHOD_ENTRIES_END	\
default: wxASSERT(false); break; }
#define REFL_CLASS_METHOD_ENTRIES_BEGIN
#define	REFL_CLASS_METHOD_ENTRY0( RetVal )
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CLASS_METHOD_ENTRY_NR0
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 )
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define REFL_CLASS_METHOD_ENTRIES_END
#include <escore/Reflection.hxx>
