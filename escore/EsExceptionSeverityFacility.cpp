#include "escorepch.h"
#pragma hdrstop

#include "EsExceptionSeverityFacility.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_ENUMERATION_INFO(EsExceptionSeverity, esT("Known exception severity codes"))
ES_DECL_ENUMERATION_INFO(EsExceptionFacility, esT("Known exception origins"))
//--------------------------------------------------------------------------------
