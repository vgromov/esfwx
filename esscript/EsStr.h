#ifndef _es_str_h_
#define _es_str_h_

// String flags
//
ES_DECL_ENUMERATION_BEGIN(EsStrFlag)
  ES_DECL_ENUMERATION_ITEM(None,                static_cast<ulong>(0))
  ES_DECL_ENUMERATION_ITEM(Quote,               EsString::StrQuote)
  ES_DECL_ENUMERATION_ITEM(NoCEscape,           EsString::StrNoCEscape)
  ES_DECL_ENUMERATION_ITEM(XML,                 EsString::StrXML)
  ES_DECL_ENUMERATION_ITEM(KeepSideBlanks,      EsString::StrKeepSideBlanks)
  ES_DECL_ENUMERATION_ITEM(EscapeNonAscii,      EsString::StrEscapeNonAscii)
ES_DECL_ENUMERATION_END

// String encodings
//
ES_DECL_ENUMERATION_BEGIN(EsStrByteEncoding)
  ES_DECL_ENUMERATION_ITEM(ASCII,               EsString::ASCII)
  ES_DECL_ENUMERATION_ITEM(CP1251,              EsString::CP1251)
  ES_DECL_ENUMERATION_ITEM(UTF8,                EsString::UTF8)
ES_DECL_ENUMERATION_END

/// Script string services
///
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN( EsStr, ESSCRIPT_CLASS )

  /// String formatting
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, format, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, format, cr_EsString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD3(EsString, format, cr_EsString, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD4(EsString, format, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD5(EsString, format, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD6(EsString, format, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD7(EsString, format, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, formatV, cr_EsString, cr_EsVariant);

  /// Locale-aware string formatting
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, lformat, cr_EsString, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD3(EsString, lformat, cr_EsString, cr_EsString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD4(EsString, lformat, cr_EsString, cr_EsString, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD5(EsString, lformat, cr_EsString, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD6(EsString, lformat, cr_EsString, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD7(EsString, lformat, cr_EsString, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD3(EsString, lformatV, cr_EsString, cr_EsString, cr_EsVariant);

  /// String processing
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isHexChar, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, fromString, cr_EsString, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, toString, cr_EsString, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, toLower, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, toUpper, cr_EsString);

  /// Convert between our default wide string and byte sequence
  /// direct conversion, wide-to-byte conversion may loose values, if incoming EsString contains Unicode-16 symbols
  /// with values > 255
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, toByteString, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, fromByteString, cr_EsBinBuffer);

  /// 8-byte encoding-aware conversions
  ES_DECL_REFLECTED_CLASS_METHOD2(EsBinBuffer, toByteString, cr_EsString, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, fromByteString, cr_EsBinBuffer, ulong);

  /// Convert hex char to binary value
  ES_DECL_REFLECTED_CLASS_METHOD1(long, hexToBinNibble, long);

  /// Convert hex string to binary buffer
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, hexToBin, cr_EsVariant);

  /// Convert hex substring to binary buffer, given start position, and count
  ES_DECL_REFLECTED_CLASS_METHOD3(EsVariant, hexToBin, cr_EsVariant, cr_EsVariant, cr_EsVariant);

  /// String comparison
  ES_DECL_REFLECTED_CLASS_METHOD2(long, compare, cr_EsString, cr_EsString);

  /// Misc string utilities
  ES_DECL_REFLECTED_CLASS_METHOD4(EsString, fromArray, cr_EsStringArray, cr_EsString, cr_EsString, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, versionsCompare, cr_EsVariant, cr_EsVariant);

  /// String <--> Numeric conversion
  ///

  /// long
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, fromLong, long, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(long, toLong, cr_EsString, ulong);

  /// ulong
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, fromULong, ulong, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(ulong, toULong, cr_EsString, ulong);

  /// long long
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, fromInt64, llong, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(llong, toInt64, cr_EsString, ulong);

  /// ulong long
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, fromUInt64, ullong, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(ullong, toUInt64, cr_EsString, ulong);

  /// double
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, fromDouble, double);
  ES_DECL_REFLECTED_CLASS_METHOD1(double, toDouble, cr_EsString);

  /// I18N stuff
  ///

  /// Get default (fallback) locale id
  ///
  ES_DECL_REFLECTED_CLASS_METHOD0(EsString, defaultLocaleIdGet);

  /// Set default (fallback) locale id
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(void, defaultLocaleIdSet, cr_EsString);

  /// Get currently enumerated (available) locale id strings
  ///
  ES_DECL_REFLECTED_CLASS_METHOD0(EsStringArray, localeIdsGet);

  /// Get the currently active locale id string
  ///
  ES_DECL_REFLECTED_CLASS_METHOD0(EsString, localeIdGet);

  /// Set the currently active locale id string
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(void, localeIdSet, cr_EsString);

  /// Get language code from locale id string code
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, languageCodeGet, cr_EsString);

  /// Get country code from locale id string. May return an empty string,
  /// if locale id does not contain country code part
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, countryCodeGet, cr_EsString);

  /// Return english language name from locale code
  /// or "Unknown", if language code is not recognized
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, languageNameGet, cr_EsString);

  /// Return Native language name from locale code
  /// or "Unknown", if language code is not recognized
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, nativeLanguageNameGet, cr_EsString);

  /// Return currently loaded translation domain names
  ES_DECL_REFLECTED_CLASS_METHOD0(EsStringArray, translationDomainsGet);

  /// Get source string translation using default domains lookup
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, translate, cr_EsString);

  /// Get source string translation using specified domain lookup first, then global (all) domains lookup
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, translate, cr_EsString, cr_EsString);

  /// Get plural source string translation using default (global) domain lookup
  ES_DECL_REFLECTED_CLASS_METHOD2(EsString, translatePlural, cr_EsString, cr_EsVariant);

  /// Get plural source string translation using specified domain lookup first, then global (all) domains lookup
  ES_DECL_REFLECTED_CLASS_METHOD3(EsString, translatePlural, cr_EsString, cr_EsVariant, cr_EsVariant);

  /// Get OS language ID part of current locale ID string
  ES_DECL_REFLECTED_CLASS_METHOD0(EsString, osLanguageIdGet);

  /// Get OS country ID part of current locale ID string
  ES_DECL_REFLECTED_CLASS_METHOD0(EsString, osCountryIdGet);

  /// Get entire OS locale ID string
  ES_DECL_REFLECTED_CLASS_METHOD0(EsString, osLocaleIdGet);

  /// Get entire OS locale ID string using specified separator string between language and countery part
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, osLocaleIdGet, cr_EsString);

  /// Set global translation storage search paths.
  /// As soon as this call is made, available locales will be re-enumerated,
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(void, translationSearchPathsSet, cr_EsVariant);

  /// Return array of translation search paths
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, translationSearchPathsGet);

  /// Reset translation search paths
  ES_DECL_REFLECTED_CLASS_METHOD0(void, translationSearchPathsReset);

  /// Cultural-specific locale info parts access (reflected EsLoclae delegates)
  ///

  /// Return all supported culture locale names. This functionality is supported only if escore is compiled with
  /// ES_USE_EMBEDDED_LOCALE_IMPL defined. Otherwise, an empty value is returned
  ///
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, supportedLocaleNamesGet);

  /// Access to some locale-specific data
  ///

  /// Numeric, list, and money separators
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, decimalSeparatorGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, thousandSeparatorGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, listSeparatorGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, moneyDecimalSeparatorGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, moneyThousandSeparatorGet, cr_EsString);

  /// Currency
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, moneySymbolGet, cr_EsString);

  /// Date-Time cultural-aware formatting
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, timeSeparatorGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, dateSeparatorGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, timeAmGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, timePmGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, timeFmtGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, dateFmtGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, timeFmtShortGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, dateFmtShortGet, cr_EsString);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, dateTimeFmtGet, cr_EsString);

ES_DECL_REFLECTED_SERVICES_END

#endif // _es_str_h_
