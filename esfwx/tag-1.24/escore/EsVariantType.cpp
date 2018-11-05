#include "escorepch.h"
#pragma hdrstop

#include "EsVariantType.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_ENUMERATION_INFO(EsVariantType, NO_CLASS_DESCR)
