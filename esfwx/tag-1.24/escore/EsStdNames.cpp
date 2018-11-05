#include "escorepch.h"
#pragma hdrstop

#include "EsStdNames.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

const EsString& EsStdNames::reflectedCtr() ES_NOTHROW
{
  static const EsString c_reflectedCtrName(esT("new"), EsString::doHashInit);
  return c_reflectedCtrName;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::reflectedDtr() ES_NOTHROW
{
  static const EsString c_reflectedDtrName(esT("delete"), EsString::doHashInit);
  return c_reflectedDtrName;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::_true() ES_NOTHROW
{
  static const EsString sc_trueString(esT("true"), EsString::doHashInit);
  return sc_trueString;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::_false() ES_NOTHROW
{
  static const EsString sc_falseString(esT("false"), EsString::doHashInit);
  return sc_falseString;
}
//---------------------------------------------------------------------------

const EsByteString& EsStdNames::_bfalse() ES_NOTHROW
{
  static const EsByteString sc_falseString("false");
  return sc_falseString;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::asString() ES_NOTHROW
{
  static const EsString s_asString(esT("asString"), EsString::doHashInit);
  return s_asString;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::compare() ES_NOTHROW
{
  static const EsString s_compare(esT("compare"), EsString::doHashInit);
  return s_compare;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::has() ES_NOTHROW
{
  static const EsString s_has(esT("has"), EsString::doHashInit);
  return s_has;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::add() ES_NOTHROW
{
  static const EsString s_add(esT("add"), EsString::doHashInit);
  return s_add;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::add_inplace() ES_NOTHROW
{
  static const EsString s_add(esT("add_inplace"), EsString::doHashInit);
  return s_add;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::subtract() ES_NOTHROW
{
  static const EsString s_subtract(esT("subtract"), EsString::doHashInit);
  return s_subtract;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::subtract_inplace() ES_NOTHROW
{
  static const EsString s_subtract(esT("subtract_inplace"), EsString::doHashInit);
  return s_subtract;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::multiply() ES_NOTHROW
{
  static const EsString s_multiply(esT("multiply"), EsString::doHashInit);
  return s_multiply;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::multiply_inplace() ES_NOTHROW
{
  static const EsString s_multiply(esT("multiply_inplace"), EsString::doHashInit);
  return s_multiply;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::divide() ES_NOTHROW
{
  static const EsString s_divide(esT("divide"), EsString::doHashInit);
  return s_divide;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::divide_inplace() ES_NOTHROW
{
  static const EsString s_divide(esT("divide_inplace"), EsString::doHashInit);
  return s_divide;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::countGet() ES_NOTHROW
{
  static const EsString s_countGet(esT("countGet"), EsString::doHashInit);
  return s_countGet;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::countSet() ES_NOTHROW
{
  static const EsString s_countSet(esT("countSet"), EsString::doHashInit);
  return s_countSet;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::itemGet() ES_NOTHROW
{
  static const EsString s_itemGet(esT("itemGet"), EsString::doHashInit);
  return s_itemGet;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::itemSet() ES_NOTHROW
{
  static const EsString s_itemSet(esT("itemSet"), EsString::doHashInit);
  return s_itemSet;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::itemDelete() ES_NOTHROW
{
  static const EsString s_itemDelete(esT("itemDelete"), EsString::doHashInit);
  return s_itemDelete;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::itemAppend() ES_NOTHROW
{
  static const EsString s_itemAppend(esT("itemAppend"), EsString::doHashInit);
  return s_itemAppend;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::setToNull() ES_NOTHROW
{
  static const EsString s_setToNull(esT("setToNull"), EsString::doHashInit);
  return s_setToNull;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::value() ES_NOTHROW
{
  static const EsString s_value(esT("value"), EsString::doHashInit);
  return s_value;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::version() ES_NOTHROW
{
  static const EsString s_version(esT("version"), EsString::doHashInit);
  return s_version;
}
//---------------------------------------------------------------------------

const EsString& EsStdNames::label() ES_NOTHROW
{
  static const EsString s_label(esT("label"), EsString::doHashInit);
  return s_label;
}
//---------------------------------------------------------------------------
