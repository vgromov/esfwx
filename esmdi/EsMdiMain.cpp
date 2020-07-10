#include "esmdipch.h"
#pragma hdrstop

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

ES_REFLECTION_REGISTRAR_BEGIN(esmdi)
  ES_REFLECTION_REGISTRAR_ENTRY(EsDocumentManager)
  ES_REFLECTION_REGISTRAR_ENTRY(EsDocument)
ES_REFLECTION_REGISTRAR_END

#if !(ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND && defined(ECC_USE_PACKAGE))
# if defined(WIN32) && defined(ES_USE_DLLS)
BOOL WINAPI DllMain(HINSTANCE hinstDLL,DWORD fdwReason, LPVOID lpReserved)
{
  switch( fdwReason )
  {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }

  return TRUE;
}
# endif
#endif

