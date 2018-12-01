// Generate member call proxies jump table
//
#define REFL_METHOD_ENTRIES_BEGIN  \
namespace EsReflectionCallProxies { \
static const _proxyT sc_memberProxies[] = {
#define  REFL_METHOD_ENTRY0( RetVal ) GEN_SIG0(RetVal, Call ##_proxy),
#define  REFL_METHOD_ENTRY1( RetVal, Param1 ) GEN_SIG1(RetVal, Call, Param1 ##_proxy),
#define  REFL_METHOD_ENTRY2( RetVal, Param1, Param2 ) GEN_SIG2(RetVal, Call, Param1, Param2 ##_proxy),
#define  REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) GEN_SIG3(RetVal, Call, Param1, Param2, Param3 ##_proxy),
#define  REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) GEN_SIG4(RetVal, Call, Param1, Param2, Param3, Param4 ##_proxy),
#define  REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) GEN_SIG5(RetVal, Call, Param1, Param2, Param3, Param4, Param5 ##_proxy),
#define  REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) GEN_SIG6(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy),
#define  REFL_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) GEN_SIG7(RetVal, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy),

#define  REFL_CONST_METHOD_ENTRY0( RetVal ) GEN_SIG0(RetVal, CallConst ##_proxy),
#define  REFL_CONST_METHOD_ENTRY1( RetVal, Param1 ) GEN_SIG1(RetVal, CallConst, Param1 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 ) GEN_SIG2(RetVal, CallConst, Param1, Param2 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 ) GEN_SIG3(RetVal, CallConst, Param1, Param2, Param3 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 ) GEN_SIG4(RetVal, CallConst, Param1, Param2, Param3, Param4 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 ) GEN_SIG5(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 ) GEN_SIG6(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) GEN_SIG7(RetVal, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy),

#define  REFL_METHOD_ENTRY_NR0 GEN_SIG0(void, Call ##_proxy),
#define  REFL_METHOD_ENTRY_NR1( Param1 ) GEN_SIG1(void, Call, Param1 ##_proxy),
#define  REFL_METHOD_ENTRY_NR2( Param1, Param2 ) GEN_SIG2(void, Call, Param1, Param2 ##_proxy),
#define  REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) GEN_SIG3(void, Call, Param1, Param2, Param3 ##_proxy),
#define  REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) GEN_SIG4(void, Call, Param1, Param2, Param3, Param4 ##_proxy),
#define  REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) GEN_SIG5(void, Call, Param1, Param2, Param3, Param4, Param5 ##_proxy),
#define  REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) GEN_SIG6(void, Call, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy),
#define  REFL_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) GEN_SIG7(void, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy),

#define  REFL_CONST_METHOD_ENTRY_NR0 GEN_SIG0(void, CallConst ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR1( Param1 ) GEN_SIG1(void, CallConst, Param1 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 ) GEN_SIG2(void, CallConst, Param1, Param2 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 ) GEN_SIG3(void, CallConst, Param1, Param2, Param3 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 ) GEN_SIG4(void, CallConst, Param1, Param2, Param3, Param4 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 ) GEN_SIG5(void, CallConst, Param1, Param2, Param3, Param4, Param5 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 ) GEN_SIG6(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6 ##_proxy),
#define  REFL_CONST_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) GEN_SIG7(void, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7 ##_proxy),

#define REFL_METHOD_ENTRIES_END  \
nullptr }; }

#define REFL_CLASS_METHOD_ENTRIES_BEGIN
#define  REFL_CLASS_METHOD_ENTRY0( RetVal )
#define  REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )
#define  REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define  REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define  REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define  REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define  REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define  REFL_CLASS_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define  REFL_CLASS_METHOD_ENTRY_NR0
#define  REFL_CLASS_METHOD_ENTRY_NR1( Param1 )
#define  REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )
#define  REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define  REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define  REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define  REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define  REFL_CLASS_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define REFL_CLASS_METHOD_ENTRIES_END
#include <escore/EsReflection.hxx>
