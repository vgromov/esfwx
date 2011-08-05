	// generate class calls
	//
#define REFL_METHOD_ENTRIES_BEGIN
#define	REFL_METHOD_ENTRY0( RetVal )
#define	REFL_METHOD_ENTRY1( RetVal, Param1 )
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_METHOD_ENTRY_NR0
#define	REFL_METHOD_ENTRY_NR1( Param1 )
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 )
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY0( RetVal )
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY_NR0
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 )
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define REFL_METHOD_ENTRIES_END

#define REFL_CLASS_METHOD_ENTRIES_BEGIN \
switch( m_signature ) {
#define	REFL_CLASS_METHOD_ENTRY0( RetVal ) \
case GEN_SIG0(RetVal, ClassCall): \
	result = From_ ## RetVal( (( GEN_SIG0(RetVal, ClassCall) ##_T )m_classMethod)()); break;
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 ) \
case GEN_SIG1(RetVal, ClassCall, Param1): \
	result = From_ ## RetVal( (( GEN_SIG1(RetVal, ClassCall, Param1) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)))); break;
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 ) \
case GEN_SIG2(RetVal, ClassCall, Param1, Param2): \
	result = From_ ## RetVal( (( GEN_SIG2(RetVal, ClassCall, Param1, Param2) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)))); break;
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) \
case GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3): \
	result = From_ ## RetVal( (( GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)))); break;
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) \
case GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4): \
	result = From_ ## RetVal( (( GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)))); break;
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) \
case GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5): \
	result = From_ ## RetVal( (( GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)))); break;
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) \
case GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6): \
	result = From_ ## RetVal( (( GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)), To_## Param6 (params.getItem(5)))); break;

#define	REFL_CLASS_METHOD_ENTRY_NR0 \
case GEN_SIG0(void, ClassCall): \
	(( GEN_SIG0(void, ClassCall) ##_T )m_classMethod)(); break;
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 ) \
case GEN_SIG1(void, ClassCall, Param1): \
	(( GEN_SIG1(void, ClassCall, Param1) ##_T )m_classMethod)(To_## Param1 (params.getItem(0))); break;
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 ) \
case GEN_SIG2(void, ClassCall, Param1, Param2): \
	(( GEN_SIG2(void, ClassCall, Param1, Param2) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1))); break;
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) \
case GEN_SIG3(void, ClassCall, Param1, Param2, Param3): \
	(( GEN_SIG3(void, ClassCall, Param1, Param2, Param3) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2))); break;
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) \
case GEN_SIG4(void, ClassCall, Param1, Param2, Param3, Param4): \
	(( GEN_SIG4(void, ClassCall, Param1, Param2, Param3, Param4) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3))); break;
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) \
case GEN_SIG5(void, ClassCall, Param1, Param2, Param3, Param4, Param5): \
	(( GEN_SIG5(void, ClassCall, Param1, Param2, Param3, Param4, Param5) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4))); break;
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) \
case GEN_SIG6(void, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6): \
	(( GEN_SIG6(void, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6) ##_T )m_classMethod)(To_## Param1 (params.getItem(0)), To_## Param2 (params.getItem(1)), To_## Param3 (params.getItem(2)), To_## Param4 (params.getItem(3)), To_## Param5 (params.getItem(4)), To_## Param6 (params.getItem(5))); break;
#define REFL_CLASS_METHOD_ENTRIES_END \
default: wxASSERT(false); break; }
#include <escore/Reflection.hxx>

