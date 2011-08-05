// file used solely for precomiled symbols generation
#include "stdafx.h"
#pragma hdrstop

#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION)
#	include <vld.h>
#endif

ES_REFLECTION_REGISTRAR_BEGIN(escomm)
	ES_REFLECTION_REGISTRAR_ENTRY(UartIoChannel)
	ES_REFLECTION_REGISTRAR_ENTRY(EkonnectIoChannel)
	ES_REFLECTION_REGISTRAR_ENTRY(SocketClientIoChannel)
	ES_REFLECTION_REGISTRAR_ENTRY(SocketServerIoChannel)
	ES_REFLECTION_REGISTRAR_ENTRY(StubIoChannel)
	ES_REFLECTION_REGISTRAR_ENTRY(IoChannelFactory)
	ES_REFLECTION_REGISTRAR_ENTRY(EsRpcMaster)
ES_REFLECTION_REGISTRAR_END

#ifdef EKOSF_USEDLLS
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		reflectionInit();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif // EKOSF_USEDLLS