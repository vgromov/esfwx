#include "escommpch.h"
#pragma hdrstop

#if ES_COMPILER_VENDOR != ES_COMPILER_VENDOR_BORLAND
# error This file must be used for Embarcadero projects only!
#endif

#pragma package(smart_init)
#include "autolink.cxx"

#pragma argsused
extern "C" int _libmain(unsigned long reason)
{
  return 1;
}
