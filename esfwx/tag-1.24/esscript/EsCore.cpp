#include "esscriptpch.h"
#pragma hdrstop

#include "EsCore.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsCore, NO_CLASS_DESCR)
  // global logging wrappers
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logInfo, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logInfo, void_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logDebug, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logDebug, void_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logWarning, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logWarning, void_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logError, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsCore, logError, void_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END

// global logging wrappers
void EsCore::logInfo(cr_EsString msg)
{
  EsUtilities::logInfo(msg);
}

void EsCore::logInfo(cr_EsString msg, cr_EsVariant val)
{
  EsUtilities::logInfo(msg, val);
}

void EsCore::logDebug(cr_EsString msg)
{
  EsUtilities::logDebug(msg);
}

void EsCore::logDebug(cr_EsString msg, cr_EsVariant val)
{
  EsUtilities::logDebug(msg, val);
}

void EsCore::logWarning(cr_EsString msg)
{
  EsUtilities::logWarning(msg);
}

void EsCore::logWarning(cr_EsString msg, cr_EsVariant val)
{
  EsUtilities::logWarning(msg, val);
}

void EsCore::logError(cr_EsString msg)
{
  EsUtilities::logError(msg);
}

void EsCore::logError(cr_EsString msg, cr_EsVariant val)
{
  EsUtilities::logError(msg, val);
}

//---------------------------------------------------------------------------
// More escore EsUtilities wrappers
//
ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsUtils, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, GUIDtoStr, EsString_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, GUIDtoCanonicalStr, EsString_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, GUIDfromStr, EsBinBuffer_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, bToE, EsBinBuffer_ClassCall_cr_EsBinBuffer_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, eToB, EsBinBuffer_ClassCall_cr_EsBinBuffer_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, sToE, EsString_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, eToS, EsString_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, inverse2, esU16_ClassCall_esU16, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, inverse4, esU32_ClassCall_esU32, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, inverse8, esU64_ClassCall_esU64, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, round, double_ClassCall_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, areEqualFloats, bool_ClassCall_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsUtils, areEqualFloats, bool_ClassCall_double_double_ulong, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END

EsString EsUtils::GUIDtoStr(cr_EsBinBuffer guid)
{
  if( sizeof(GUID) == guid.size() )
    return EsUtilities::GUIDtoStr( *reinterpret_cast<const GUID*>(&guid[0]), false );
  else
    EsException::ThrowWrongObjectSize(esT("GUID"), sizeof(GUID), guid.size());

  return EsString::null();
}

EsString EsUtils::GUIDtoCanonicalStr(cr_EsBinBuffer guid)
{
  if( sizeof(GUID) == guid.size() )
    return EsUtilities::GUIDtoStr( *reinterpret_cast<const GUID*>(&guid[0]), true );
  else
    EsException::ThrowWrongObjectSize(esT("GUID"), sizeof(GUID), guid.size());

  return EsString::null();
}

EsBinBuffer EsUtils::GUIDfromStr(cr_EsString str)
{
  GUID guid = EsUtilities::GUIDfromStr(str);

  return EsBinBuffer(
    reinterpret_cast<EsBinBuffer::const_pointer>(&guid),
    reinterpret_cast<EsBinBuffer::const_pointer>(&guid) + sizeof(GUID)
    );
}

EsBinBuffer EsUtils::bToE(cr_EsBinBuffer b, cr_EsString key)
{
  return EsUtilities::bToE(b, key);
}

EsBinBuffer EsUtils::eToB(cr_EsBinBuffer e, cr_EsString key)
{
  return EsUtilities::eToB(e, key);
}

EsString EsUtils::sToE(cr_EsString s, cr_EsString key)
{
  return EsUtilities::sToE(s, key);
}

EsString EsUtils::eToS(cr_EsString e, cr_EsString key)
{
  return EsUtilities::eToS(e, key);
}

esU16 EsUtils::inverse2(esU16 in)
{
  return EsUtilities::inverse2(in);
}

esU32 EsUtils::inverse4(esU32 in)
{
  return EsUtilities::inverse4(in);
}

esU64 EsUtils::inverse8(esU64 in)
{
  return EsUtilities::inverse8(in);
}

double EsUtils::round(double in)
{
  return EsUtilities::round0(in);
}

bool EsUtils::areEqualFloats(double _1, double _2)
{
  return EsUtilities::areEqualFloats(_1, _2);
}

bool EsUtils::areEqualFloats(double _1, double _2, ulong ulps)
{
  return EsUtilities::areEqualFloats(_1, _2, ulps);
}


