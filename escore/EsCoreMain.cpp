#include "escorepch.h"
#pragma hdrstop

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

// compile-time asserts on wchar_t size definitions
#if (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND)
  ES_COMPILE_TIME_ASSERT(sizeof(ES_WCHAR)==sizeof(System::WideChar), sizeofWideChar);
  ES_COMPILE_TIME_ASSERT(ES_WCHAR_SIZE==sizeof(System::WideChar), sizeofSystemWideChar);
#else
  ES_COMPILE_TIME_ASSERT(ES_WCHAR_SIZE==sizeof(wchar_t), sizeofWcharT);
#endif

// compile-time asserts on reflected data types
ES_COMPILE_TIME_ASSERT(sizeof(esU8)  == 1,  sizeof_esU8);
ES_COMPILE_TIME_ASSERT(sizeof(esU16) == 2,  sizeof_esU16);
ES_COMPILE_TIME_ASSERT(sizeof(esU32) == 4,  sizeof_esU32);
ES_COMPILE_TIME_ASSERT(sizeof(esU64) == 8,  sizeof_esU64);
ES_COMPILE_TIME_ASSERT(sizeof(esI8)  == 1,  sizeof_esI8);
ES_COMPILE_TIME_ASSERT(sizeof(esI16) == 2,  sizeof_esI16);
ES_COMPILE_TIME_ASSERT(sizeof(esI32) == 4,  sizeof_esI32);
ES_COMPILE_TIME_ASSERT(sizeof(esI64) == 8,  sizeof_esI64);
ES_COMPILE_TIME_ASSERT(sizeof(esF)   == 4,  sizeof_esF);
ES_COMPILE_TIME_ASSERT(sizeof(esDT)  == 8,  sizeof_esDT);

ES_REFLECTION_REGISTRAR_BEGIN(escore)
  ES_REFLECTION_REGISTRAR_ENTRY(EsAssocContainer)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCRC8)
  ES_REFLECTION_REGISTRAR_ENTRY(EsChecksum16)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCRC16_CCIITT)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCRC32_IEEE802_3)
  ES_REFLECTION_REGISTRAR_ENTRY(EsTimeSpan)
  ES_REFLECTION_REGISTRAR_ENTRY(EsDateTime)
  ES_REFLECTION_REGISTRAR_ENTRY(EsEnumeration)
  ES_REFLECTION_REGISTRAR_ENTRY(EsEvent)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsPathFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsPath)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsFileFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsFile)
  ES_REFLECTION_REGISTRAR_ENTRY(EsProgressMonitor)
  ES_REFLECTION_REGISTRAR_ENTRY(EsProgressMonitorTask)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsStreamFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsStream)
  ES_REFLECTION_REGISTRAR_ENTRY(EsStreamXml)
  ES_REFLECTION_REGISTRAR_ENTRY(EsStringTokenizer)
  ES_REFLECTION_REGISTRAR_ENTRY(EsRange)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsUtilities)
#if defined(ES_USE_REGEX)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsRegExCompileFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsRegExMatchFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsRegExReplaceFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsRegEx)
#endif
  ES_REFLECTION_REGISTRAR_ENTRY(EsThreadStateProxy)
  ES_REFLECTION_REGISTRAR_ENTRY(EsVariantType)
  ES_REFLECTION_REGISTRAR_ENTRY(EsNumericCheck)
  ES_REFLECTION_REGISTRAR_ENTRY(EsExceptionSeverity)
  ES_REFLECTION_REGISTRAR_ENTRY(EsExceptionFacility)
  ES_REFLECTION_REGISTRAR_ENTRY(EsPathEnumeratorHelper)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsPathEnumeratorFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMetaclass)
  ES_REFLECTION_REGISTRAR_ENTRY(EsUUID)
  ES_REFLECTION_REGISTRAR_ENTRY(EsOsInfo)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsOsPlatfrom)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsOsArchitecture)
#if defined(ES_USE_CRYPTO)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCryptoHash)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCryptoDes)
  ES_REFLECTION_REGISTRAR_ENTRY(EsCryptoAes)
#endif
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

