#ifndef _es_core_locale_h_
#define _es_core_locale_h_

/// Default locale implementation for core char type.
/// If core char type is neither char nor wchar_t, as it may be in case of non-windows
/// platforms, like Android or iOS,
/// we have to implement our own facet type and inject it into default locale, which in turn
/// must be initialized at least once.
/// Otherwise, standatd classic C++ locale is used without any modification
///

namespace EsLocale
{
  /// Get default C++ locale object
  ESCORE_FUNC(const std::locale&, locale() ES_NOTHROW );

  /// Try to create, or get existing named locale object
  /// Throw an exception, if named locale is not supported by the current OS
  /// environment.
  ///
  /// @param [in]           loc                Requested locale name
  /// @param [in, optional] fallbackToClassic  If requested locale may not be created,
  ///                                          silently initialize it with copy of classic one instead
  /// @return                                  Requested locale object
  ///
  ESCORE_FUNC(const std::locale&, locale(const EsString& loc, bool fallbackToClassic = false) );

#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  /// Return all supported locale names. This functionality is supported only if escore is compiled with
  /// ES_USE_EMBEDDED_LOCALE_IMPL defined
  ///
  ESCORE_FUNC(const EsStringArray&, supportedLocaleNamesGet() );
#endif

  /// Access to some locale-specific data
  ///

  /// Numeric, list, and money separators
  ESCORE_FUNC(EsString::value_type, decimalSeparatorGet(const std::locale& loc) );
  ESCORE_FUNC(EsString::value_type, thousandSeparatorGet(const std::locale& loc) );
  ESCORE_FUNC(EsString::value_type, listSeparatorGet(const std::locale& loc) );
  ESCORE_FUNC(EsString::value_type, moneyDecimalSeparatorGet(const std::locale& loc) );
  ESCORE_FUNC(EsString::value_type, moneyThousandSeparatorGet(const std::locale& loc) );

  /// Currency
  ESCORE_FUNC(EsString, moneySymbolGet(const std::locale& loc) );

  /// Date-Time cultural-aware formatting
  ///
  ESCORE_FUNC(EsString::value_type, timeSeparatorGet(const std::locale& loc) );
  ESCORE_FUNC(EsString::value_type, dateSeparatorGet(const std::locale& loc) );

  ESCORE_FUNC(EsString, timeAmGet(const std::locale& loc) );
  ESCORE_FUNC(EsString, timePmGet(const std::locale& loc) );

  ESCORE_FUNC(EsString, timeFmtGet(const std::locale& loc) );
  ESCORE_FUNC(EsString, dateFmtGet(const std::locale& loc) );

  ESCORE_FUNC(EsString, timeFmtShortGet(const std::locale& loc) );
  ESCORE_FUNC(EsString, dateFmtShortGet(const std::locale& loc) );

  ESCORE_FUNC(EsString, dateTimeFmtGet(const std::locale& loc) );
}

#endif // _es_core_locale_h_
