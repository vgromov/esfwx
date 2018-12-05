#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptEnumeration.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsScriptEnumeration, EsEnumeration, NO_CLASS_DESCR )
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsEnumerationIntf::Ptr EsScriptEnumeration::create(EsScriptMachine& machine, const EsString& enumTypeName)
{
  std::unique_ptr<EsScriptEnumeration> ptr(
    new EsScriptEnumeration(
      machine,
      enumTypeName
    )
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

