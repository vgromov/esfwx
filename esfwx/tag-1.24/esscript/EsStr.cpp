#include "esscriptpch.h"
#pragma hdrstop

#include "EsStr.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_ENUMERATION_INFO(EsStrFlag, esT("String processing flags"));
ES_DECL_ENUMERATION_INFO(EsStrByteEncoding, esT("String byte encoding types"));
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsStr, NO_CLASS_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, format, EsString_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, formatV, EsString_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)

	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformat, EsString_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformat, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformat, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformat, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformat, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformat, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, lformatV, EsString_ClassCall_cr_EsString_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)

	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, isHexChar, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromString, EsString_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toString, EsString_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toLower, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toUpper, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toByteString, EsBinBuffer_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromByteString, EsString_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toByteString, EsBinBuffer_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromByteString, EsString_ClassCall_cr_EsBinBuffer_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, hexToBinNibble, long_ClassCall_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, hexToBin, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, hexToBin, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, compare, long_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromArray, EsString_ClassCall_cr_EsStringArray_cr_EsString_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, versionsCompare, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromLong, EsString_ClassCall_long_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toLong, long_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromULong, EsString_ClassCall_ulong_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toULong, ulong_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromInt64, EsString_ClassCall_llong_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toInt64, llong_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromUInt64, EsString_ClassCall_ullong_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toUInt64, ullong_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, fromDouble, EsString_ClassCall_double, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, toDouble, double_ClassCall_cr_EsString, NO_METHOD_DESCR)

	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, defaultLocaleIdGet, EsString_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, defaultLocaleIdSet, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, localeIdsGet, EsStringArray_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, localeIdGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, localeIdSet, void_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, languageCodeGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, countryCodeGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, languageNameGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, nativeLanguageNameGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translationDomainsGet, EsStringArray_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translate, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translate, EsString_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translatePlural, EsString_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translatePlural, EsString_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, osLanguageIdGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, osCountryIdGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, osLocaleIdGet, EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, osLocaleIdGet, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translationSearchPathsSet, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translationSearchPathsGet, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, translationSearchPathsReset, void_ClassCall, NO_METHOD_DESCR)

  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, decimalSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, thousandSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, listSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, moneyDecimalSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, moneyThousandSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, moneySymbolGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, timeSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, dateSeparatorGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, timeAmGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, timePmGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, timeFmtGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, dateFmtGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, timeFmtShortGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, dateFmtShortGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsStr, dateTimeFmtGet, EsVariant_ClassCall_cr_EsString, NO_METHOD_DESCR)

ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt)
{
	return EsString::format(fmt.c_str());
}
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt, const EsVariant& v0)
{
  return EsString::format(fmt.c_str(), v0);
}
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt, const EsVariant& v0, const EsVariant& v1)
{
  return EsString::format(fmt.c_str(), v0, v1);
}
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2)
{
  return EsString::format(fmt.c_str(), v0, v1, v2);
}
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2, const EsVariant& v3)
{
  return EsString::format(fmt.c_str(), v0, v1, v2, v3);
}
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2, const EsVariant& v3, const EsVariant& v4)
{
  return EsString::format(fmt.c_str(), v0, v1, v2, v3, v4);
}
//---------------------------------------------------------------------------

EsString EsStr::format(const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2, const EsVariant& v3, const EsVariant& v4, const EsVariant& v5)
{
  return EsString::format(fmt.c_str(), v0, v1, v2, v3, v4, v5);
}
//---------------------------------------------------------------------------

EsString EsStr::formatV(const EsString& fmt, const EsVariant& args)
{
  if( EsVariant::VAR_VARIANT_COLLECTION == args.typeGet() )
    return EsString::formatV(
      EsLocale::locale(),
      fmt.c_str(),
      args
    );
  else
    return EsString::format(
      fmt.c_str(),
      args
    );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsString EsStr::lformat(const EsString& loc, const EsString& fmt)
{
	return EsString::format(EsLocale::locale(loc, true), fmt.c_str());
}
//---------------------------------------------------------------------------

EsString EsStr::lformat(const EsString& loc, const EsString& fmt, const EsVariant& v0)
{
  return EsString::format(EsLocale::locale(loc, true), fmt.c_str(), v0);
}
//---------------------------------------------------------------------------

EsString EsStr::lformat(const EsString& loc, const EsString& fmt, const EsVariant& v0, const EsVariant& v1)
{
  return EsString::format(EsLocale::locale(loc, true), fmt.c_str(), v0, v1);
}
//---------------------------------------------------------------------------

EsString EsStr::lformat(const EsString& loc, const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2)
{
  return EsString::format(EsLocale::locale(loc, true), fmt.c_str(), v0, v1, v2);
}
//---------------------------------------------------------------------------

EsString EsStr::lformat(const EsString& loc, const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2, const EsVariant& v3)
{
  return EsString::format(EsLocale::locale(loc, true), fmt.c_str(), v0, v1, v2, v3);
}
//---------------------------------------------------------------------------

EsString EsStr::lformat(const EsString& loc, const EsString& fmt, const EsVariant& v0, const EsVariant& v1, const EsVariant& v2, const EsVariant& v3, const EsVariant& v4)
{
  return EsString::format(EsLocale::locale(loc, true), fmt.c_str(), v0, v1, v2, v3, v4);
}
//---------------------------------------------------------------------------

EsString EsStr::lformatV(const EsString& loc, const EsString& fmt, const EsVariant& args)
{
  if( EsVariant::VAR_VARIANT_COLLECTION == args.typeGet() )
  {
    return EsString::formatV(
      EsLocale::locale(loc, true),
      fmt.c_str(),
      args
    );
  }
  else
    return EsString::format(
      EsLocale::locale(loc, true),
      fmt.c_str(),
      args
    );
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool EsStr::isHexChar(cr_EsVariant ch)
{
	return EsString::isHexChar(ch.asChar());
}

EsString EsStr::fromString(const EsString& src, ulong flags)
{
	return EsString::fromString(src, flags);
}

EsString EsStr::toString(const EsString& src, ulong flags)
{
	return EsString::toString(src, flags);
}

EsString EsStr::toLower(const EsString& src)
{
	return EsString::toLower(src);
}

EsString EsStr::toUpper(const EsString& src)
{
	return EsString::toUpper(src);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsBinBuffer EsStr::toByteString(cr_EsString src)
{
	const EsByteString& bs = EsString::toAscii(src);
	return EsBinBuffer(bs.begin(), bs.end());
}
//---------------------------------------------------------------------------

EsString EsStr::fromByteString(cr_EsBinBuffer src)
{
  if( src.empty() )
    return EsString::null();

  EsByteString::const_pointer csrc = reinterpret_cast<EsByteString::const_pointer>(src.data());

	return EsString::fromAscii(
    csrc,
    esBstrnlen(csrc, src.size())
  );
}
//---------------------------------------------------------------------------

EsBinBuffer EsStr::toByteString(cr_EsString src, ulong encoding)
{
  if( src.empty() )
    return EsBinBuffer::null();

	const EsByteString& bs = EsString::toByteString(src, encoding);

	return EsBinBuffer(
    bs.begin(),
    bs.end()
  );
}
//---------------------------------------------------------------------------

EsString EsStr::fromByteString(cr_EsBinBuffer src, ulong encoding)
{
  if( src.empty() )
    return EsString::null();

  EsByteString::const_pointer csrc = reinterpret_cast<EsByteString::const_pointer>(src.data());

	EsByteString bs(
    csrc,
    esBstrnlen(csrc, src.size())
  );

	return EsString::fromByteString(bs, encoding);
}
//---------------------------------------------------------------------------

long EsStr::hexToBinNibble(long ch)
{
	esU8 nibble = 0;
	if( !EsString::hexToBinNibble(ch, nibble) )
		EsException::Throw(0, EsException::severityGeneric, EsException::facilityEsScript,
			esT("Invalid hex character encountered: '%c'"), ch);

	return nibble;
}
//---------------------------------------------------------------------------

EsVariant EsStr::hexToBin(cr_EsVariant hex)
{
  return hexToBin(
    hex, 
    EsVariant::null(),
    EsVariant::null()
  );
}
//--------------------------------------------------------------------------------

EsVariant EsStr::hexToBin(cr_EsVariant hex, cr_EsVariant offs, cr_EsVariant cnt)
{
  ulong _offs = offs.isEmpty() ? 0 : offs.asULong();

  if(hex.isEmpty())
    return EsBinBuffer::null();

  ulong hexCnt = hex.countGet();

  if( hexCnt <= _offs )
    return EsBinBuffer::null();

  ulong _cnt = cnt.isEmpty() ? 
    (hexCnt-_offs) : 
    esMin(cnt.asULong(), hexCnt - _offs);

  if(!_cnt)
    return EsBinBuffer::null();

  if(_cnt % 2)
    EsException::Throw(
      esT("hexToBin count parameter must be an even value, got %d after precalculation"),
      _cnt
    );
    
  size_t converted = 0;

  EsBinBuffer bin;
  bin.resize(_cnt / 2); //< Prepare binary buffer

  if(hex.isBinBuffer())
  {
    const EsBinBuffer& bb = hex.doInterpretAsBinBuffer();
    converted = EsString::hexToBin<EsBinBuffer::value_type>(
      bb.data() + _offs,
      _cnt,
      bin.data(),
      bin.size()
    );
  }
  else
  {
    const EsString& str = hex.asString();
    converted = EsString::hexToBin<EsString::value_type>(
      str.data() + _offs,
      _cnt,
      bin.data(),
      bin.size()
    );
  }

  if(converted != _cnt)
    EsException::Throw(
      esT("hexToBin could not convert entire sequence to binary. Converted: %d, starting from offset: %d, requested for conversion: %d"),
      converted,
      _offs,
      _cnt
    );

  return bin;
}
//--------------------------------------------------------------------------------

long EsStr::compare(cr_EsString _1, cr_EsString _2)
{
	return _1.compare(_2);
}
//---------------------------------------------------------------------------

EsString EsStr::fromArray(cr_EsStringArray a, cr_EsString delimiter, cr_EsString leftEnclosure, cr_EsString rightEnclosure)
{
	return EsString::fromStringArray(a, delimiter, leftEnclosure, rightEnclosure);
}
//---------------------------------------------------------------------------

EsVariant EsStr::versionsCompare(cr_EsVariant _1, cr_EsVariant _2)
{
  return EsUtilities::versionStrCompare(
    _1.asString(), 
    _2.asString()
  );
}
//--------------------------------------------------------------------------- 
//---------------------------------------------------------------------------

EsString EsStr::fromLong(long l, ulong radix)
{
  return EsString::fromLong(l, radix);
}
//---------------------------------------------------------------------------

long EsStr::toLong(const EsString& str, ulong radix)
{
  return EsString::toLong(str, radix);
}
//---------------------------------------------------------------------------

EsString EsStr::fromULong(ulong u, ulong radix)
{
  return EsString::fromULong(u, radix);
}
//---------------------------------------------------------------------------

ulong EsStr::toULong(const EsString& str, ulong radix)
{
  return EsString::toULong(str, radix);
}
//---------------------------------------------------------------------------

EsString EsStr::fromInt64(llong i64, ulong radix)
{
  return EsString::fromInt64(i64, radix);
}
//---------------------------------------------------------------------------

llong EsStr::toInt64(const EsString& str, ulong radix)
{
  return EsString::toInt64(str, radix);
}
//---------------------------------------------------------------------------

EsString EsStr::fromUInt64(ullong u64, ulong radix)
{
  return EsString::fromUInt64(u64, radix);
}
//---------------------------------------------------------------------------

ullong EsStr::toUInt64(const EsString& str, ulong radix)
{
  return EsString::toUInt64(str, radix);
}
//---------------------------------------------------------------------------

EsString EsStr::fromDouble(double d)
{
  return EsString::fromDouble(d);
}
//---------------------------------------------------------------------------

double EsStr::toDouble(const EsString& str)
{
  return EsString::toDouble(str);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsString EsStr::defaultLocaleIdGet()
{
  return EsStringI18n::instanceGet().defaultLocaleGet();
}
//---------------------------------------------------------------------------

void EsStr::defaultLocaleIdSet(cr_EsString loc)
{
  EsStringI18n::instanceGet().defaultLocaleSet(loc);
}
//---------------------------------------------------------------------------

EsStringArray EsStr::localeIdsGet()
{
  return EsStringI18n::instanceGet().availableLocalesGet();
}
//---------------------------------------------------------------------------

EsString EsStr::localeIdGet()
{
  return EsStringI18n::instanceGet().currentLocaleGet();
}
//---------------------------------------------------------------------------

void EsStr::localeIdSet(cr_EsString loc)
{
  EsStringI18n::instanceGet().currentLocaleSet(loc);
}
//---------------------------------------------------------------------------

EsString EsStr::languageCodeGet(cr_EsString loc)
{
  return EsStringI18n::languageCodeGet(loc);
}
//---------------------------------------------------------------------------

EsString EsStr::countryCodeGet(cr_EsString loc)
{
  return EsStringI18n::countryCodeGet(loc);
}
//---------------------------------------------------------------------------

EsString EsStr::languageNameGet(cr_EsString loc)
{
  return EsStringI18n::languageNameGet(loc);
}
//---------------------------------------------------------------------------

EsString EsStr::nativeLanguageNameGet(cr_EsString loc)
{
  return EsStringI18n::nativeLanguageNameGet(loc);
}
//---------------------------------------------------------------------------

EsStringArray EsStr::translationDomainsGet()
{
  return EsStringI18n::domains();
}
//---------------------------------------------------------------------------

EsString EsStr::translate(cr_EsString src)
{
  return EsStringI18n::translate(src);
}
//---------------------------------------------------------------------------

EsString EsStr::translate(cr_EsString src, cr_EsString domain)
{
  return EsStringI18n::translate(src, domain);
}
//---------------------------------------------------------------------------

EsString EsStr::translatePlural(cr_EsString src, cr_EsVariant num)
{
  return EsStringI18n::translatePlural(src, num.asULong());
}
//---------------------------------------------------------------------------

EsString EsStr::translatePlural(cr_EsString src, cr_EsVariant num, cr_EsVariant domain)
{
  return EsStringI18n::translatePlural(src, num.asULong(), domain.asString());
}
//---------------------------------------------------------------------------

EsString EsStr::osLanguageIdGet()
{
  return EsStringI18n::osLanguageIdGet();
}
//---------------------------------------------------------------------------

EsString EsStr::osCountryIdGet()
{
  return EsStringI18n::osCountryIdGet();
}
//---------------------------------------------------------------------------

EsString EsStr::osLocaleIdGet()
{
  return EsStringI18n::osLocaleIdGet();
}
//---------------------------------------------------------------------------

EsString EsStr::osLocaleIdGet(const EsString& sep)
{
  return EsStringI18n::osLocaleIdGet(sep);
}
//---------------------------------------------------------------------------

void EsStr::translationSearchPathsSet(cr_EsVariant paths)
{
  if( paths.isEmpty() )
    translationSearchPathsReset();
  else
    EsStringI18n::instanceGet().translationSearchPathsSet(
      paths.asStringCollection()
    );
}
//---------------------------------------------------------------------------

EsVariant EsStr::translationSearchPathsGet()
{
  return EsStringI18n::instanceGet().translationSearchPathsGet();
}
//---------------------------------------------------------------------------

void EsStr::translationSearchPathsReset()
{
  EsStringI18n::instanceGet().translationSearchPathsReset();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsVariant EsStr::supportedLocaleNamesGet()
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  return EsLocale::supportedLocaleNamesGet();
#else
  return EsVariant::null();
#endif
}
//---------------------------------------------------------------------------

EsVariant EsStr::decimalSeparatorGet(cr_EsString loc)
{
  return EsLocale::decimalSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::thousandSeparatorGet(cr_EsString loc)
{
  return EsLocale::thousandSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::listSeparatorGet(cr_EsString loc)
{
  return EsLocale::listSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::moneyDecimalSeparatorGet(cr_EsString loc)
{
  return EsLocale::moneyDecimalSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::moneyThousandSeparatorGet(cr_EsString loc)
{
  return EsLocale::moneyThousandSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::moneySymbolGet(cr_EsString loc)
{
  return EsLocale::moneySymbolGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::timeSeparatorGet(cr_EsString loc)
{
  return EsLocale::timeSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::dateSeparatorGet(cr_EsString loc)
{
  return EsLocale::dateSeparatorGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::timeAmGet(cr_EsString loc)
{
  return EsLocale::timeAmGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::timePmGet(cr_EsString loc)
{
  return EsLocale::timePmGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::timeFmtGet(cr_EsString loc)
{
  return EsLocale::timeFmtGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::dateFmtGet(cr_EsString loc)
{
  return EsLocale::dateFmtGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::timeFmtShortGet(cr_EsString loc)
{
  return EsLocale::timeFmtShortGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::dateFmtShortGet(cr_EsString loc)
{
  return EsLocale::dateFmtShortGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

EsVariant EsStr::dateTimeFmtGet(cr_EsString loc)
{
  return EsLocale::dateTimeFmtGet(
    EsLocale::locale(loc, true)
  );
}
//---------------------------------------------------------------------------

