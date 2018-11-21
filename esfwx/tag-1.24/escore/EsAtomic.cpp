#include "escorepch.h"
#pragma hdrstop

#include "EsAtomic.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsAtomic.win.cxx"
#elif ES_OS == ES_OS_ANDROID
# include "EsAtomic.android.cxx"
#elif ES_OS == ES_OS_MAC
# include "EsAtomic.mac.cxx"
#else
# include "EsAtomic.default.cxx"
#endif

