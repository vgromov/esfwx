// method|class method type casts generation
//
#ifndef _reflection_def_type_casts_h_
#define _reflection_def_type_casts_h_

#define REFL_METHOD_ENTRIES_BEGIN
#define	REFL_METHOD_ENTRY0( RetVal )																												typedef GEN_CALL_T0(RetVal, EsBaseIntf::, GEN_SIG0(RetVal, Call) ## _T);
#define	REFL_METHOD_ENTRY1( RetVal, Param1 )																								typedef GEN_CALL_T1(RetVal, EsBaseIntf::, GEN_SIG1(RetVal, Call, Param1) ## _T, Param1);
#define	REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )																				typedef GEN_CALL_T2(RetVal, EsBaseIntf::, GEN_SIG2(RetVal, Call, Param1, Param2) ## _T, Param1, Param2);
#define	REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )																typedef GEN_CALL_T3(RetVal, EsBaseIntf::, GEN_SIG3(RetVal, Call, Param1, Param2, Param3) ## _T, Param1, Param2, Param3);
#define	REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )												typedef GEN_CALL_T4(RetVal, EsBaseIntf::, GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4) ## _T, Param1, Param2, Param3, Param4);
#define	REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )								typedef GEN_CALL_T5(RetVal, EsBaseIntf::, GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5) ## _T, Param1, Param2, Param3, Param4, Param5);
#define	REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )				typedef GEN_CALL_T6(RetVal, EsBaseIntf::, GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6) ## _T, Param1, Param2, Param3, Param4, Param5, Param6);
#define	REFL_METHOD_ENTRY_NR0																																REFL_METHOD_ENTRY0(void )
#define	REFL_METHOD_ENTRY_NR1( Param1 )																											REFL_METHOD_ENTRY1(void, Param1 )
#define	REFL_METHOD_ENTRY_NR2( Param1, Param2 )																							REFL_METHOD_ENTRY2(void, Param1, Param2 )
#define	REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																			REFL_METHOD_ENTRY3(void, Param1, Param2, Param3 )
#define	REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )															REFL_METHOD_ENTRY4(void, Param1, Param2, Param3, Param4 )
#define	REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )											REFL_METHOD_ENTRY5(void, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )							REFL_METHOD_ENTRY6(void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define	REFL_CONST_METHOD_ENTRY0( RetVal )																									typedef GEN_CALL_T0(RetVal, EsBaseIntf::, GEN_SIG0(RetVal, CallConst) ## _T) const;
#define	REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )																					typedef GEN_CALL_T1(RetVal, EsBaseIntf::, GEN_SIG1(RetVal, CallConst, Param1) ## _T, Param1) const;
#define	REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )																	typedef GEN_CALL_T2(RetVal, EsBaseIntf::, GEN_SIG2(RetVal, CallConst, Param1, Param2) ## _T, Param1, Param2) const;
#define	REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )													typedef GEN_CALL_T3(RetVal, EsBaseIntf::, GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3) ## _T, Param1, Param2, Param3) const;
#define	REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )									typedef GEN_CALL_T4(RetVal, EsBaseIntf::, GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4) ## _T, Param1, Param2, Param3, Param4) const;
#define	REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )					typedef GEN_CALL_T5(RetVal, EsBaseIntf::, GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5) ## _T, Param1, Param2, Param3, Param4, Param5) const;
#define	REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )	typedef GEN_CALL_T6(RetVal, EsBaseIntf::, GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6) ## _T, Param1, Param2, Param3, Param4, Param5, Param6) const;
#define	REFL_CONST_METHOD_ENTRY_NR0																													REFL_CONST_METHOD_ENTRY0(void )
#define	REFL_CONST_METHOD_ENTRY_NR1( Param1 )																								REFL_CONST_METHOD_ENTRY1(void, Param1 )
#define	REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )																				REFL_CONST_METHOD_ENTRY2(void, Param1, Param2 )
#define	REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																REFL_CONST_METHOD_ENTRY3(void, Param1, Param2, Param3 )
#define	REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )												REFL_CONST_METHOD_ENTRY4(void, Param1, Param2, Param3, Param4 )
#define	REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )								REFL_CONST_METHOD_ENTRY5(void, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )				REFL_CONST_METHOD_ENTRY6(void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define REFL_METHOD_ENTRIES_END
#define REFL_CLASS_METHOD_ENTRIES_BEGIN
#define	REFL_CLASS_METHOD_ENTRY0( RetVal )																									typedef GEN_CALL_T0(RetVal, NO_THIS_SPEC, GEN_SIG0(RetVal, ClassCall) ## _T);
#define	REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )																					typedef GEN_CALL_T1(RetVal, NO_THIS_SPEC, GEN_SIG1(RetVal, ClassCall, Param1) ## _T, Param1);
#define	REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )																	typedef GEN_CALL_T2(RetVal, NO_THIS_SPEC, GEN_SIG2(RetVal, ClassCall, Param1, Param2) ## _T, Param1, Param2);
#define	REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )													typedef GEN_CALL_T3(RetVal, NO_THIS_SPEC, GEN_SIG3(RetVal, ClassCall, Param1, Param2, Param3) ## _T, Param1, Param2, Param3);
#define	REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )									typedef GEN_CALL_T4(RetVal, NO_THIS_SPEC, GEN_SIG4(RetVal, ClassCall, Param1, Param2, Param3, Param4) ## _T, Param1, Param2, Param3, Param4);
#define	REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )					typedef GEN_CALL_T5(RetVal, NO_THIS_SPEC, GEN_SIG5(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5) ## _T, Param1, Param2, Param3, Param4, Param5);
#define	REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )	typedef GEN_CALL_T6(RetVal, NO_THIS_SPEC, GEN_SIG6(RetVal, ClassCall, Param1, Param2, Param3, Param4, Param5, Param6) ## _T, Param1, Param2, Param3, Param4, Param5, Param6);
#define	REFL_CLASS_METHOD_ENTRY_NR0																													REFL_CLASS_METHOD_ENTRY0(void )
#define	REFL_CLASS_METHOD_ENTRY_NR1( Param1 )																								REFL_CLASS_METHOD_ENTRY1(void, Param1 )
#define	REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )																				REFL_CLASS_METHOD_ENTRY2(void, Param1, Param2 )
#define	REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )																REFL_CLASS_METHOD_ENTRY3(void, Param1, Param2, Param3 )
#define	REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )												REFL_CLASS_METHOD_ENTRY4(void, Param1, Param2, Param3, Param4 )
#define	REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )								REFL_CLASS_METHOD_ENTRY5(void, Param1, Param2, Param3, Param4, Param5 )
#define	REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )				REFL_CLASS_METHOD_ENTRY6(void, Param1, Param2, Param3, Param4, Param5, Param6 )
#define REFL_CLASS_METHOD_ENTRIES_END
#include <escore/Reflection.hxx>

#endif // _reflection_def_type_casts_h_
