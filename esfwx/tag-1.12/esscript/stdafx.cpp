// file used solely for precomiled symbols generation
#include "stdafx.h"
#pragma hdrstop

#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION)
#	include <vld.h>
#endif

ES_REFLECTION_REGISTRAR_BEGIN(esscript)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptDebug)
	ES_REFLECTION_REGISTRAR_ENTRY(EsScriptObject)
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