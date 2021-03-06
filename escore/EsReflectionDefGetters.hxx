// generate property getter calls
//
#define REFL_METHOD_ENTRIES_BEGIN  \
switch( m_readerSignature ) {
#define  REFL_METHOD_ENTRY0( RetVal)
#define  REFL_METHOD_ENTRY1( RetVal, Param1 )
#define  REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define  REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define  REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define  REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define  REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define  REFL_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define  REFL_CONST_METHOD_ENTRY0( RetVal)   \
case GEN_SIG0(RetVal, CallConst): \
  result = (obj->*esCallCastToCall< EsMemberCallT, GEN_SIG0(RetVal, CallConst_T) >(m_reader))(); break;
#define  REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )
#define  REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )
#define  REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
#define  REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
#define  REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
#define  REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
#define  REFL_CONST_METHOD_ENTRY7( RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define  REFL_METHOD_ENTRY_NR0
#define  REFL_METHOD_ENTRY_NR1( Param1 )
#define  REFL_METHOD_ENTRY_NR2( Param1, Param2 )
#define  REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define  REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define  REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define  REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define  REFL_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define  REFL_CONST_METHOD_ENTRY_NR0
#define  REFL_CONST_METHOD_ENTRY_NR1( Param1 )
#define  REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )
#define  REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
#define  REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
#define  REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
#define  REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
#define  REFL_CONST_METHOD_ENTRY_NR7( Param1, Param2, Param3, Param4, Param5, Param6, Param7 )
#define REFL_METHOD_ENTRIES_END  \
default: ES_ASSERT(false); break; }
#define REFL_CLASS_METHOD_ENTRIES_BEGIN
#define  REFL_CLASS_METHOD_ENTRY0( RetVal)
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
