// supplementary file containing reflected method declaration entries
// must not be used directly in project build. instead, it's included
// and preprocessed in Reflection.cpp & Reflection.h files
//
// macros templates for copy-paste implementation
//
//#define REFL_METHOD_ENTRIES_BEGIN
//#define REFL_METHOD_ENTRY0( RetVal )
//#define REFL_METHOD_ENTRY1( RetVal, Param1 )
//#define REFL_METHOD_ENTRY2( RetVal, Param1, Param2 )
//#define REFL_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
//#define REFL_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
//#define REFL_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
//#define REFL_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
//#define REFL_METHOD_ENTRY_NR0
//#define REFL_METHOD_ENTRY_NR1( Param1 )
//#define REFL_METHOD_ENTRY_NR2( Param1, Param2 )
//#define REFL_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
//#define REFL_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
//#define REFL_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
//#define REFL_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
//#define REFL_CONST_METHOD_ENTRY0( RetVal )
//#define REFL_CONST_METHOD_ENTRY1( RetVal, Param1 )
//#define REFL_CONST_METHOD_ENTRY2( RetVal, Param1, Param2 )
//#define REFL_CONST_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
//#define REFL_CONST_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
//#define REFL_CONST_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
//#define REFL_CONST_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
//#define REFL_CONST_METHOD_ENTRY_NR0
//#define REFL_CONST_METHOD_ENTRY_NR1( Param1 )
//#define REFL_CONST_METHOD_ENTRY_NR2( Param1, Param2 )
//#define REFL_CONST_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
//#define REFL_CONST_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
//#define REFL_CONST_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
//#define REFL_CONST_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
//#define REFL_METHOD_ENTRIES_END
//#define REFL_CLASS_METHOD_ENTRIES_BEGIN
//#define REFL_CLASS_METHOD_ENTRY0( RetVal )
//#define REFL_CLASS_METHOD_ENTRY1( RetVal, Param1 )
//#define REFL_CLASS_METHOD_ENTRY2( RetVal, Param1, Param2 )
//#define REFL_CLASS_METHOD_ENTRY3( RetVal, Param1, Param2, Param3 )
//#define REFL_CLASS_METHOD_ENTRY4( RetVal, Param1, Param2, Param3, Param4 )
//#define REFL_CLASS_METHOD_ENTRY5( RetVal, Param1, Param2, Param3, Param4, Param5 )
//#define REFL_CLASS_METHOD_ENTRY6( RetVal, Param1, Param2, Param3, Param4, Param5, Param6 )
//#define REFL_CLASS_METHOD_ENTRY_NR0
//#define REFL_CLASS_METHOD_ENTRY_NR1( Param1 )
//#define REFL_CLASS_METHOD_ENTRY_NR2( Param1, Param2 )
//#define REFL_CLASS_METHOD_ENTRY_NR3( Param1, Param2, Param3 )
//#define REFL_CLASS_METHOD_ENTRY_NR4( Param1, Param2, Param3, Param4 )
//#define REFL_CLASS_METHOD_ENTRY_NR5( Param1, Param2, Param3, Param4, Param5 )
//#define REFL_CLASS_METHOD_ENTRY_NR6( Param1, Param2, Param3, Param4, Param5, Param6 )
//#define REFL_CLASS_METHOD_ENTRIES_END
//#include <core/Reflection.hxx>

// reflected member functions go here. NB! add new definitions at the end of list only!
//															RetVal											Param1						Param2				Param3				Param4				Param5				Param6
REFL_METHOD_ENTRIES_BEGIN
	REFL_METHOD_ENTRY_NR0
	REFL_METHOD_ENTRY0(						bool)
	REFL_CONST_METHOD_ENTRY0(			bool)
	REFL_CONST_METHOD_ENTRY0(			long)
	REFL_METHOD_ENTRY_NR1(																		cr_long )
	REFL_CONST_METHOD_ENTRY0(			EsString)
	REFL_METHOD_ENTRY_NR1(																		cr_EsString )
	REFL_METHOD_ENTRY1(						EsString,										long)
	REFL_METHOD_ENTRY_NR1(																		cr_bool)
	REFL_METHOD_ENTRY3(						long,												cp_void,					long,					long)
	REFL_METHOD_ENTRY3(						long,												p_void,						long,					long)
	REFL_METHOD_ENTRY_NR1(																		p_void)
	REFL_CONST_METHOD_ENTRY1(			EsString,										long)
	REFL_CONST_METHOD_ENTRY2(			EsString,										long,							bool)
	REFL_CONST_METHOD_ENTRY_NR0
	REFL_CONST_METHOD_ENTRY0(			EsBaseIntfPtr)
	REFL_CONST_METHOD_ENTRY0(			double)
	REFL_CONST_METHOD_ENTRY0(			EsVariant)
	REFL_CONST_METHOD_ENTRY0(			EsBinBuffer)
	REFL_CONST_METHOD_ENTRY1(			EsVariant,									long)
	REFL_METHOD_ENTRY1(						bool,												long)	
	REFL_CONST_METHOD_ENTRY1(			bool,												long)
	REFL_METHOD_ENTRY2(						ulong,											cr_EsBinBuffer,		ulong)
	REFL_METHOD_ENTRY2(						EsBinBuffer,								ulong,						ulong)
	REFL_METHOD_ENTRY_NR1(																		cr_EsVariant)
	REFL_CONST_METHOD_ENTRY1(			bool,												cr_EsString)
	REFL_METHOD_ENTRY_NR3(																		cr_EsString,			cr_EsString,	p_wxEvtHandler)
	REFL_METHOD_ENTRY_NR1(																		long)
	REFL_METHOD_ENTRY0(						EsVariant)
	REFL_METHOD_ENTRY0(						ulong)
	REFL_METHOD_ENTRY1(						bool,												cr_EsVariant)
	REFL_METHOD_ENTRY_NR1(																		ulong)
	REFL_CONST_METHOD_ENTRY0(			ulong)
	REFL_CONST_METHOD_ENTRY1(			EsString,										ulong)
	REFL_METHOD_ENTRY_NR1(																		cr_ulong)	
	REFL_METHOD_ENTRY0(						EsBinBuffer)
	REFL_CONST_METHOD_ENTRY1(			bool,												cr_EsVariant)
	REFL_CONST_METHOD_ENTRY1(			int,												cr_EsVariant)
	REFL_METHOD_ENTRY1(						UINT8,											UINT16)
	REFL_METHOD_ENTRY1(						INT8,												UINT16)
	REFL_METHOD_ENTRY1(						UINT16,											UINT16)
	REFL_METHOD_ENTRY1(						INT16,											UINT16)
	REFL_METHOD_ENTRY1(						UINT32,											UINT16)
	REFL_METHOD_ENTRY1(						INT32,											UINT16)
	REFL_METHOD_ENTRY1(						UINT64,											UINT16)
	REFL_METHOD_ENTRY1(						INT64,											UINT16)
	REFL_METHOD_ENTRY1(						FLOAT,											UINT16)
	REFL_METHOD_ENTRY1(						BOOL,												UINT16)
	REFL_METHOD_ENTRY1(						DATETIME,										UINT16)
	REFL_METHOD_ENTRY1(						EsBinBuffer,								UINT16)
	REFL_METHOD_ENTRY_NR1(																		UINT16)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							BYTEARRAY)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							UINT16)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							FLOAT)
	REFL_METHOD_ENTRY2(						BOOL,												UINT16,							DATETIME)
	REFL_METHOD_ENTRY2(						EsBinBuffer,								UINT16,							UINT16)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							UINT8)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							UINT32)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							BOOL)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							UINT8,				FLOAT)
	REFL_METHOD_ENTRY2(						FLOAT,											UINT16,							UINT8)
	REFL_METHOD_ENTRY2(						UINT16,											UINT16,							BYTEARRAY)
	REFL_METHOD_ENTRY3(						UINT16,											UINT16,							BYTEARRAY,		BOOL)
	REFL_METHOD_ENTRY2(						EsBinBuffer,								UINT16,							BYTEARRAY)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							UINT8,				BYTEARRAY)
	REFL_METHOD_ENTRY2(						bool,												ulong,							bool)
	REFL_METHOD_ENTRY1(						bool,												ulong)
	REFL_METHOD_ENTRY_NR2(																		cr_EsString,				bool)
	REFL_METHOD_ENTRY2(						EsBinBuffer,								UINT16,							UINT32)
	REFL_METHOD_ENTRY3(						EsBinBuffer,								UINT16,							UINT32, 			UINT32)	
	REFL_METHOD_ENTRY2(						EsVariant,									ulong,							ulong)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							UINT32,				BYTEARRAY)
	REFL_METHOD_ENTRY2(						EsBinBuffer,								UINT16,							UINT8)
	REFL_CONST_METHOD_ENTRY0(			int)
	REFL_CONST_METHOD_ENTRY1(			EsVariant,									cr_EsVariant)
	REFL_METHOD_ENTRY_NR2(																		cr_EsVariant,				cr_EsVariant)
	REFL_METHOD_ENTRY3(						ulong,											cr_EsBinBuffer,			ulong,				EsBaseIntfPtr)
	REFL_METHOD_ENTRY3(						EsBinBuffer,								ulong,							ulong,				EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						UINT8,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						INT8,												UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						UINT16,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						INT16,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						UINT32,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						INT32,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						UINT64,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						INT64,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						FLOAT,											UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						BOOL,												UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						DATETIME,										UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						EsBinBuffer,								UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY_NR2(																		UINT16,							EsBaseIntfPtr)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							EsBaseIntfPtr, BYTEARRAY)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							EsBaseIntfPtr,	UINT16)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							EsBaseIntfPtr,	FLOAT)
	REFL_METHOD_ENTRY3(						BOOL,												UINT16,							EsBaseIntfPtr, DATETIME)
	REFL_METHOD_ENTRY3(						EsBinBuffer,								UINT16,							EsBaseIntfPtr,	UINT16)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							EsBaseIntfPtr,	UINT8)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							EsBaseIntfPtr,	UINT32)
	REFL_METHOD_ENTRY_NR3(																		UINT16,							EsBaseIntfPtr,	BOOL)
	REFL_METHOD_ENTRY_NR4(																		UINT16,							EsBaseIntfPtr,	UINT8,				FLOAT)
	REFL_METHOD_ENTRY3(						FLOAT,											UINT16,							EsBaseIntfPtr,	UINT8)
	REFL_METHOD_ENTRY3(						UINT16,											UINT16,							EsBaseIntfPtr,	BYTEARRAY)
	REFL_METHOD_ENTRY4(						UINT16,											UINT16,							EsBaseIntfPtr,	BYTEARRAY,		BOOL)
	REFL_METHOD_ENTRY3(						EsBinBuffer,								UINT16,							EsBaseIntfPtr,	BYTEARRAY)
	REFL_METHOD_ENTRY_NR4(																		UINT16,							EsBaseIntfPtr,	UINT8,				BYTEARRAY)
	REFL_METHOD_ENTRY3(						EsBinBuffer,								UINT16,							EsBaseIntfPtr,	UINT32)
	REFL_METHOD_ENTRY4(						EsBinBuffer,								UINT16,							EsBaseIntfPtr,	UINT32, 			UINT32)	
	REFL_METHOD_ENTRY_NR4(																		UINT16,							EsBaseIntfPtr,	UINT32,				BYTEARRAY)
	REFL_METHOD_ENTRY3(						EsBinBuffer,								UINT16,							EsBaseIntfPtr,	UINT8)
	REFL_METHOD_ENTRY1(						bool,												EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						bool,												EsBaseIntfPtr,		ulong)
	REFL_METHOD_ENTRY3(						EsVariant,									EsBaseIntfPtr,		ulong,						ulong)
	REFL_METHOD_ENTRY3(						bool,												EsBaseIntfPtr,		ulong,						bool)
	REFL_METHOD_ENTRY1(						EsVariant,									EsBaseIntfPtr)
	REFL_METHOD_ENTRY1(						ulong,											EsBaseIntfPtr)	
	REFL_METHOD_ENTRY_NR1(																		EsBaseIntfPtr)
	REFL_METHOD_ENTRY2(						bool,												EsBaseIntfPtr,		cr_EsVariant)	
	REFL_METHOD_ENTRY_NR2(																		EsBaseIntfPtr,		ulong)
	REFL_METHOD_ENTRY1(						EsBinBuffer,								EsBaseIntfPtr)
	REFL_CONST_METHOD_ENTRY1(			ulong,											ulong)
REFL_METHOD_ENTRIES_END

// reflected class functions go here. NB! add new definitions at the end of list only!
REFL_CLASS_METHOD_ENTRIES_BEGIN
	REFL_CLASS_METHOD_ENTRY_NR0
	REFL_CLASS_METHOD_ENTRY0(			EsBaseIntfPtr)
	REFL_CLASS_METHOD_ENTRY1(			EsBaseIntfPtr,							p_wxObject )
	REFL_CLASS_METHOD_ENTRY0(			EsVariant)	
	REFL_CLASS_METHOD_ENTRY2(			EsVariant,									EsBaseIntfPtr, long)	
	REFL_CLASS_METHOD_ENTRY4(			ulong,											ulong,						ulong,				bool,					ulong)
	REFL_CLASS_METHOD_ENTRY_NR1(															cr_EsString )
	REFL_CLASS_METHOD_ENTRY_NR2(															cr_EsString,			cr_EsVariant )	
	REFL_CLASS_METHOD_ENTRY_NR3(															cr_EsString,			cr_EsVariant,	cr_EsVariant )
	REFL_CLASS_METHOD_ENTRY_NR4(															cr_EsString,			cr_EsVariant,	cr_EsVariant,	cr_EsVariant )
	REFL_CLASS_METHOD_ENTRY_NR5(															cr_EsString,			cr_EsVariant,	cr_EsVariant,	cr_EsVariant,	cr_EsVariant )
	REFL_CLASS_METHOD_ENTRY_NR6(															cr_EsString,			cr_EsVariant,	cr_EsVariant,	cr_EsVariant,	cr_EsVariant,	cr_EsVariant )
REFL_CLASS_METHOD_ENTRIES_END

// undefine all. do not change these lines
#undef REFL_METHOD_ENTRIES_BEGIN
#undef REFL_METHOD_ENTRY0
#undef REFL_METHOD_ENTRY1
#undef REFL_METHOD_ENTRY2
#undef REFL_METHOD_ENTRY3
#undef REFL_METHOD_ENTRY4
#undef REFL_METHOD_ENTRY5
#undef REFL_METHOD_ENTRY6
#undef REFL_METHOD_ENTRY_NR0
#undef REFL_METHOD_ENTRY_NR1
#undef REFL_METHOD_ENTRY_NR2
#undef REFL_METHOD_ENTRY_NR3
#undef REFL_METHOD_ENTRY_NR4
#undef REFL_METHOD_ENTRY_NR5
#undef REFL_METHOD_ENTRY_NR6
#undef REFL_CONST_METHOD_ENTRY0
#undef REFL_CONST_METHOD_ENTRY1
#undef REFL_CONST_METHOD_ENTRY2
#undef REFL_CONST_METHOD_ENTRY3
#undef REFL_CONST_METHOD_ENTRY4
#undef REFL_CONST_METHOD_ENTRY5
#undef REFL_CONST_METHOD_ENTRY6
#undef REFL_CONST_METHOD_ENTRY_NR0
#undef REFL_CONST_METHOD_ENTRY_NR1
#undef REFL_CONST_METHOD_ENTRY_NR2
#undef REFL_CONST_METHOD_ENTRY_NR3
#undef REFL_CONST_METHOD_ENTRY_NR4
#undef REFL_CONST_METHOD_ENTRY_NR5
#undef REFL_CONST_METHOD_ENTRY_NR6
#undef REFL_METHOD_ENTRIES_END
#undef REFL_CLASS_METHOD_ENTRIES_BEGIN
#undef REFL_CLASS_METHOD_ENTRY0
#undef REFL_CLASS_METHOD_ENTRY1
#undef REFL_CLASS_METHOD_ENTRY2
#undef REFL_CLASS_METHOD_ENTRY3
#undef REFL_CLASS_METHOD_ENTRY4
#undef REFL_CLASS_METHOD_ENTRY5
#undef REFL_CLASS_METHOD_ENTRY6
#undef REFL_CLASS_METHOD_ENTRY_NR0
#undef REFL_CLASS_METHOD_ENTRY_NR1
#undef REFL_CLASS_METHOD_ENTRY_NR2
#undef REFL_CLASS_METHOD_ENTRY_NR3
#undef REFL_CLASS_METHOD_ENTRY_NR4
#undef REFL_CLASS_METHOD_ENTRY_NR5
#undef REFL_CLASS_METHOD_ENTRY_NR6
#undef REFL_CLASS_METHOD_ENTRIES_END