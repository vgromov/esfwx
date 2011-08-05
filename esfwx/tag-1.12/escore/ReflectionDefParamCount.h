#ifndef _reflection_def_param_count_h_
#define _reflection_def_param_count_h_

// reflected method parameters count generation
//
#define REFL_METHOD_ENTRIES_BEGIN																														enum MethodParamsCount {
#define	REFL_METHOD_ENTRY0( RetVal )																												GEN_SIG0(RetVal, Call) ##_paramsCount = 0,
#define	REFL_METHOD_ENTRY1( RetVal, Param1 )																								GEN_SIG1(RetVal, Call, Param1) ##_paramsCount = 1,
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )																				GEN_SIG2(RetVal, Call, Param1, Param2) ##_paramsCount = 2,
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )																GEN_SIG3(RetVal, Call, Param1, Param2, Param3) ##_paramsCount = 3,
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )												GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4) ##_paramsCount = 4,
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )								GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5) ##_paramsCount = 5,
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )				GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6) ##_paramsCount = 6,
#define	REFL_METHOD_ENTRY_NR0																																REFL_METHOD_ENTRY0( void )																								
#define	REFL_METHOD_ENTRY_NR1( Param1 )																											REFL_METHOD_ENTRY1( void, Param1 )																				
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 )																							REFL_METHOD_ENTRY2( void, Param1, Param2 )																
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																			REFL_METHOD_ENTRY3( void, Param1, Param2, Param3 )												
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )															REFL_METHOD_ENTRY4( void, Param1, Param2, Param3, Param4 )								
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )											REFL_METHOD_ENTRY5( void, Param1, Param2, Param3, Param4, Param5 )				
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )							REFL_METHOD_ENTRY6( void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY0( RetVal )																									GEN_SIG0(RetVal, CallConst) ##_paramsCount = 0,
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )																					GEN_SIG1(RetVal, CallConst, Param1) ##_paramsCount = 1,
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )																	GEN_SIG2(RetVal, CallConst, Param1, Param2) ##_paramsCount = 2,
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )													GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3) ##_paramsCount = 3,
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )									GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4) ##_paramsCount = 4,
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )					GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5) ##_paramsCount = 5,
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )	GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6) ##_paramsCount = 6,
#define	REFL_CONST_METHOD_ENTRY_NR0																													REFL_CONST_METHOD_ENTRY0( void )																								
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 )																								REFL_CONST_METHOD_ENTRY1( void, Param1 )																				
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )																				REFL_CONST_METHOD_ENTRY2( void, Param1, Param2 )																
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																REFL_CONST_METHOD_ENTRY3( void, Param1, Param2, Param3 )												
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )												REFL_CONST_METHOD_ENTRY4( void, Param1, Param2, Param3, Param4 )								
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )								REFL_CONST_METHOD_ENTRY5( void, Param1, Param2, Param3, Param4, Param5 )				
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )				REFL_CONST_METHOD_ENTRY6( void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define REFL_METHOD_ENTRIES_END
#define REFL_CLASS_METHOD_ENTRIES_BEGIN
#define	REFL_CLASS_METHOD_ENTRY0( RetVal )																									GEN_SIG0(RetVal, ClassCall) ##_paramsCount = 0,
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )																					GEN_SIG1(RetVal, ClassCall, Param1) ##_paramsCount = 1,
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )																	GEN_SIG2(RetVal, ClassCall, Param1, Param2) ##_paramsCount = 2,
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )													GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3) ##_paramsCount = 3,
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )									GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4) ##_paramsCount = 4,
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )					GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5) ##_paramsCount = 5,
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )	GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6) ##_paramsCount = 6,
#define	REFL_CLASS_METHOD_ENTRY_NR0																													REFL_CLASS_METHOD_ENTRY0( void )																								
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 )																								REFL_CLASS_METHOD_ENTRY1( void, Param1 )																				
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )																				REFL_CLASS_METHOD_ENTRY2( void, Param1, Param2 )																
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																REFL_CLASS_METHOD_ENTRY3( void, Param1, Param2, Param3 )												
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )												REFL_CLASS_METHOD_ENTRY4( void, Param1, Param2, Param3, Param4 )								
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )								REFL_CLASS_METHOD_ENTRY5( void, Param1, Param2, Param3, Param4, Param5 )				
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )				REFL_CLASS_METHOD_ENTRY6( void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define REFL_CLASS_METHOD_ENTRIES_END																												};
#include <escore/Reflection.hxx>

#endif // _reflection_def_param_count_h_