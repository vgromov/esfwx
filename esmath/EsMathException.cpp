#include "esmathpch.h"
#pragma hdrstop

#include "EsMathException.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsMathException::EsMathException(const EsMathException& src) :
EsException(src)
{}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsMathException::EsMathException(const EsString& msg, const EsScriptDebugInfoIntf::Ptr& dbg) :
EsException(
  0,
  EsException::severityGeneric,
  EsException::facilityEsMath,
  msg,
  dbg
)
{
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsMathException::EsMathException(const EsMathException& src, const EsScriptDebugInfoIntf::Ptr& dbg) :
EsException(src, dbg)
{
  logEsException();
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall EsMathException::~EsMathException()
{}
#endif
//---------------------------------------------------------------------------

void EsMathException::Throw(const EsString& msg, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  throw EsMathException(msg, dbg);
}
//---------------------------------------------------------------------------

void EsMathException::ThrowArraySizesDoNotMatch( ulong cnt, ulong othercnt, const EsScriptDebugInfoIntf::Ptr& dbg)
{
  const EsString& msg = EsString::format(
    esT("Array sizes do not match. %d != %d."),
    cnt,
    othercnt
  );

  throw EsMathException(msg, dbg);
}
//---------------------------------------------------------------------------

