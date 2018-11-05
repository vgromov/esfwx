#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptStdNames.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// startup code name
const EsString& EsScriptStdNames::startup() ES_NOTHROW
{
  static const EsString s_startup(esT("__startup"), EsString::doHashInit);
  return s_startup;
}
// ---------------------------------------------------------------------------

// scripting host version buil-in constant name
const EsString& EsScriptStdNames::version() ES_NOTHROW
{
  static const EsString s_version(esT("__version"), EsString::doHashInit);
  return s_version;
}
// ---------------------------------------------------------------------------

const EsString& EsScriptStdNames::metaclass() ES_NOTHROW
{
  static const EsString s_metaclass(esT("__metaclass"), EsString::doHashInit);
  return s_metaclass;
}
// ---------------------------------------------------------------------------

const EsString& EsScriptStdNames::scriptHost() ES_NOTHROW
{
  static const EsString s_scriptHost(esT("__scriptHost"), EsString::doHashInit);
  return s_scriptHost;
}
// ---------------------------------------------------------------------------

const EsString& EsScriptStdNames::null() ES_NOTHROW
{
  static const EsString s_null(esT("null"), EsString::doHashInit);
  return s_null;
}
//---------------------------------------------------------------------------

const EsString& EsScriptStdNames::exception() ES_NOTHROW
{
  static const EsString s_exception(esT("__exception"), EsString::doHashInit);
  return s_exception;
}
//---------------------------------------------------------------------------

const EsString& EsScriptStdNames::_this() ES_NOTHROW
{
  static const EsString s_this(esT("this"), EsString::doHashInit);
  return s_this;
}
//---------------------------------------------------------------------------
