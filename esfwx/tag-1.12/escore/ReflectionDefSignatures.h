#ifndef _reflection_def_signatures_h_
#define _reflection_def_signatures_h_

// reflected signatures generation
//
// method function signatures enumeration
#define REFL_METHOD_ENTRIES_BEGIN																													enum MethodSignature { invalidSignature = -1,
#define	REFL_METHOD_ENTRY0( RetVal )																											GEN_SIG0(RetVal, Call),
#define	REFL_METHOD_ENTRY1( RetVal, Param1 )																							GEN_SIG1(RetVal, Call, Param1),
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )																			GEN_SIG2(RetVal, Call, Param1, Param2),
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )															GEN_SIG3(RetVal, Call, Param1, Param2, Param3),
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )											GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4),
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )							GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5),
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )			GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6),
#define	REFL_METHOD_ENTRY_NR0																															REFL_METHOD_ENTRY0( void )																								
#define	REFL_METHOD_ENTRY_NR1( Param1 )																										REFL_METHOD_ENTRY1( void, Param1 )																				
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 )																						REFL_METHOD_ENTRY2( void, Param1, Param2 )																
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																		REFL_METHOD_ENTRY3( void, Param1, Param2, Param3 )												
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )														REFL_METHOD_ENTRY4( void, Param1, Param2, Param3, Param4 )								
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )										REFL_METHOD_ENTRY5( void, Param1, Param2, Param3, Param4, Param5 )				
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )						REFL_METHOD_ENTRY6( void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY0( RetVal )																								GEN_SIG0(RetVal, CallConst),
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )																				GEN_SIG1(RetVal, CallConst, Param1),
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )																GEN_SIG2(RetVal, CallConst, Param1, Param2),
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )												GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3),
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )								GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4),
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )				GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5),
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6) GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6),
#define	REFL_CONST_METHOD_ENTRY_NR0																												REFL_CONST_METHOD_ENTRY0( void )
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 )																							REFL_CONST_METHOD_ENTRY1( void, Param1 )																				
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )																			REFL_CONST_METHOD_ENTRY2( void, Param1, Param2 )																
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )															REFL_CONST_METHOD_ENTRY3( void, Param1, Param2, Param3 )												
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )											REFL_CONST_METHOD_ENTRY4( void, Param1, Param2, Param3, Param4 )								
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )							REFL_CONST_METHOD_ENTRY5( void, Param1, Param2, Param3, Param4, Param5 )				
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )			REFL_CONST_METHOD_ENTRY6( void, Param1, Param2, Param3, Param4, Param5, Param6)
#define REFL_METHOD_ENTRIES_END																														methodSignaturesEnd,
#define REFL_CLASS_METHOD_ENTRIES_BEGIN																										maxMethodSignature = 32000,
#define	REFL_CLASS_METHOD_ENTRY0( RetVal )																								GEN_SIG0(RetVal, ClassCall),
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )																				GEN_SIG1(RetVal, ClassCall, Param1),
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )																GEN_SIG2(RetVal, ClassCall, Param1, Param2),
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )												GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3),
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )								GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4),
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )				GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5),
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6)	GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6),
#define	REFL_CLASS_METHOD_ENTRY_NR0																												REFL_CLASS_METHOD_ENTRY0( void )																								
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 )																							REFL_CLASS_METHOD_ENTRY1( void, Param1 )																				
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )																			REFL_CLASS_METHOD_ENTRY2( void, Param1, Param2 )																
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )															REFL_CLASS_METHOD_ENTRY3( void, Param1, Param2, Param3 )												
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )											REFL_CLASS_METHOD_ENTRY4( void, Param1, Param2, Param3, Param4 )								
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )							REFL_CLASS_METHOD_ENTRY5( void, Param1, Param2, Param3, Param4, Param5 )				
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )			REFL_CLASS_METHOD_ENTRY6( void, Param1, Param2, Param3, Param4, Param5, Param6)
#define REFL_CLASS_METHOD_ENTRIES_END																											classMethodSignaturesEnd };
#include <escore/Reflection.hxx>

#endif // _reflection_def_signatures_h_