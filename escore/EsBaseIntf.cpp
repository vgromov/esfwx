#include "escorepch.h"
#pragma hdrstop

#include "EsBaseIntf.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsBase::EsBase() ES_NOTHROW {}
EsBase::~EsBase() {}
//---------------------------------------------------------------------------
