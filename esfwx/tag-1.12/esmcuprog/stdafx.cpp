#include "stdafx.h"
#pragma hdrstop

#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION)
#	include <vld.h>
#endif

#ifdef EKOSF_USEDLLS
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif // EKOSF_USEDLLS