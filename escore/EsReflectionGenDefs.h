#ifndef _reflection_gen_defs_h_
#define _reflection_gen_defs_h_

// universal reflection signatures generation helper macros
#define  GEN_SIG0( RetVal, CallSpec )                                                                      ES_CONCAT3(RetVal, _, CallSpec)
#define  GEN_SIG1( RetVal, CallSpec, Param1 )                                                              ES_CONCAT3(GEN_SIG0( RetVal, CallSpec), _, Param1)
#define  GEN_SIG2( RetVal, CallSpec, Param1, Param2 )                                                      ES_CONCAT3(GEN_SIG1( RetVal, CallSpec, Param1), _, Param2)
#define  GEN_SIG3( RetVal, CallSpec, Param1, Param2, Param3 )                                              ES_CONCAT3(GEN_SIG2( RetVal, CallSpec, Param1, Param2), _, Param3)
#define  GEN_SIG4( RetVal, CallSpec, Param1, Param2, Param3, Param4 )                                      ES_CONCAT3(GEN_SIG3( RetVal, CallSpec, Param1, Param2, Param3), _, Param4)
#define  GEN_SIG5( RetVal, CallSpec, Param1, Param2, Param3, Param4, Param5 )                              ES_CONCAT3(GEN_SIG4( RetVal, CallSpec, Param1, Param2, Param3, Param4 ), _, Param5)
#define  GEN_SIG6( RetVal, CallSpec, Param1, Param2, Param3, Param4, Param5, Param6 )                      ES_CONCAT3(GEN_SIG5( RetVal, CallSpec, Param1, Param2, Param3, Param4, Param5 ), _, Param6)
#define  GEN_SIG7( RetVal, CallSpec, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )              ES_CONCAT3(GEN_SIG6( RetVal, CallSpec, Param1, Param2, Param3, Param4, Param5, Param6 ), _, Param7)

// universal type cast generator helper macros
#define NO_THIS_SPEC
#define  GEN_CALL_T0( RetVal, ThisSpec, TypeName )                                                          RetVal (ThisSpec *TypeName)()
#define  GEN_CALL_T1( RetVal, ThisSpec, TypeName, Param1 )                                                  RetVal (ThisSpec *TypeName)(Param1)
#define  GEN_CALL_T2( RetVal, ThisSpec, TypeName, Param1, Param2 )                                          RetVal (ThisSpec *TypeName)(Param1, Param2)
#define  GEN_CALL_T3( RetVal, ThisSpec, TypeName, Param1, Param2, Param3 )                                  RetVal (ThisSpec *TypeName)(Param1, Param2, Param3)
#define  GEN_CALL_T4( RetVal, ThisSpec, TypeName, Param1, Param2, Param3, Param4 )                          RetVal (ThisSpec *TypeName)(Param1, Param2, Param3, Param4)
#define  GEN_CALL_T5( RetVal, ThisSpec, TypeName, Param1, Param2, Param3, Param4, Param5 )                  RetVal (ThisSpec *TypeName)(Param1, Param2, Param3, Param4, Param5)
#define  GEN_CALL_T6( RetVal, ThisSpec, TypeName, Param1, Param2, Param3, Param4, Param5, Param6 )          RetVal (ThisSpec *TypeName)(Param1, Param2, Param3, Param4, Param5, Param6)
#define  GEN_CALL_T7( RetVal, ThisSpec, TypeName, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )  RetVal (ThisSpec *TypeName)(Param1, Param2, Param3, Param4, Param5, Param6, Param7)

#endif // _refletion_gen_defs_h_