#include "esscriptpch.h"
#pragma hdrstop

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#pragma argsused
extern "C" int _libmain(unsigned long reason)
{
  return 1;
}

