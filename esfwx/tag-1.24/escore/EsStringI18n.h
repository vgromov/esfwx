#ifndef _es_string_i18n_h_
#define _es_string_i18n_h_

#ifdef ES_I18N

/// I18N event catergory
#define ES_EVTC_I18N esT("i18n")

enum class EsEvtI18n : ulong
{
  LocaleChanged = 0 //< Payload: new locale name
};

/// String internationalization (i18n) support,
/// based on gettext MO translation storage
///
class ESCORE_CLASS EsStringI18n
{
public:
	typedef std::map<EsString, EsString> DomainFilesMap;

protected:
	// source|translation entry. main form + optional plural forms array ptr
	class TranslationEntry
	{
	public:
		TranslationEntry(const EsString& org = EsString::null(),
			const EsString& translationMain = EsString::null(),
			const EsString::Array& pluralForms = EsString::nullArray()) :
		m_org(org),
		m_trMain(translationMain),
		m_trPlurals(pluralForms)
		{}

		bool isOk() const { return !m_org.empty(); }

		EsString m_org;
		EsString m_trMain;
		EsString::Array m_trPlurals;
	};

	// translated strings, mapped by original main string hash
	typedef std::map< EsString::HashT, TranslationEntry > MapT;
	// translated strings map, mapped by domain name
	typedef std::map< EsString, MapT > DomainTranslationsT;

protected:
	EsStringI18n(bool isDefault);

public:
	virtual ~EsStringI18n();

	/// Set translation storage search paths.
	/// As soon as this call is made, available locales will be re-enumerated,
	///
	void translationSearchPathsSet(const EsString::Array& paths);

	/// Return array of translation search paths
	///
	const EsString::Array& translationSearchPathsGet() const;

	/// Reset translation search paths
	///
	void translationSearchPathsReset();

	/// (try to) load specified domain translations for default and current locales
	///
	void domainTranslationsLoad(const EsString& domain);

	/// Unload specified domain translations from default and current locales
	///
	void domainTranslationsUnload(const EsString& domain);

	/// Append singular translation to the storage
	///
	void translationAppend(const EsString& source, const EsString& translation, const EsString& domain);

	/// append singular and optional plural translations to the storage
	///
	void translationPluralAppend(const EsString& source, const EsString& translationMain, const EsString::Array& translationPluralForms, const EsString& domain);

	/// Get default (fallback) locale id
	///
	const EsString& defaultLocaleGet() const;

	/// Set default (fallback) locale id
	///
	void defaultLocaleSet(const EsString& def);

	/// Return locales available for i18n, by searching and
	/// enumerating translation files in search path list.
	/// fallback locale set in defaultLocaleSet call is
	/// always available, even if no translation files were enumerated
	/// for it
	///
	const EsString::Array& availableLocalesGet();

	/// Return currently active translation locale id
	///
	const EsString& currentLocaleGet() const;

	/// Set currently active translation locale id
	/// If locale id differs from default one, all translation files, found in
	/// translation search paths list for this locale, will be loaded.
	/// If the locale being set is the same as default locale, all
	/// non-defaule translations will be unloaded from translation memory.
	///
	void currentLocaleSet(const EsString& loc);

	/// Get language code from locale id
	///
	static EsString languageCodeGet(const EsString& loc);

	/// Get country code from locale id. may return empty string,
	/// if locale id does not contain country code part
	///
	static EsString countryCodeGet(const EsString& loc);

	/// Return english language name from locale code
	/// or "Unknown", if language code is not recognized
	///
	static EsString languageNameGet(const EsString& loc);

	/// Return Native language name from locale code
	/// or "Unknown", if language code is not recognized
	///
	static EsString nativeLanguageNameGet(const EsString& loc);

	/// Return an array of translation domains found and loaded for the
	/// currently set locale
	///
	EsString::Array translationDomainsGet() const;

	/// String translation get, for the specific translation domain.
	/// if no domain is specified, all domains are searched for the translation
	/// if no translation is found, source string is returned
	///
	EsString translationGet(const EsString& src, const EsString& domain = EsString::null()) const;

	/// Get plural translation for the source string, for the specific translation domain.
	/// if no domain is specified, all domains are searched for the translation
	/// if no translation is found, source string is returned
	///
	EsString translationPluralGet(const EsString& src, size_t cnt, const EsString& domain = EsString::null()) const;

	/// Return singleton instance of the i18n class
	///
	static EsStringI18n& instanceGet();

	/// Static services and short-cuts
  ///
	static EsString::Array domains() { return instanceGet().translationDomainsGet(); }
	static EsString translate(const EsString& src, const EsString& domain = EsString::null()) { return instanceGet().translationGet(src, domain); }
	static EsString translatePlural(const EsString& src, size_t cnt, const EsString& domain = EsString::null()) { return instanceGet().translationPluralGet(src, cnt, domain); }

  /// Get OS language ID part of locale ID
  static EsString osLanguageIdGet();

  /// Get OS country ID part of locale ID
  static EsString osCountryIdGet();

  /// Get entire OS locale ID
  static EsString osLocaleIdGet(const EsString& sep = esT("_"));

protected:
	// internal services
	void translationLocalesEnumerate();
	void translationsLoad(const EsString& newLocale);
	void moFileLoad(const EsString& moFile, const EsString& domain);
	const EsStringI18n::TranslationEntry& translationFind(const EsString& src, const EsString& domain) const;
	void checkPluralFormsCount( int count, const EsString::Array& plurals ) const;
	EsString pluralEval(const EsStringI18n::TranslationEntry& entry, size_t cnt) const;
	static EsStringI18n& defaultGet();

private:
	static const EsStringI18n::TranslationEntry& nullEntry();

protected:
	mutable EsCriticalSection m_cs;
	EsString::Array m_translationPaths;
	EsString::Array m_availableLocales;
	EsString m_locale;
	// translation file paths mapping
	DomainFilesMap m_domainFiles;
	// loaded translations
	DomainTranslationsT m_domainTranslations;
	// default translations
	bool m_isDefault;
	// language code (internal)
	int m_langCode;

private:
	EsStringI18n(const EsStringI18n&) ES_REMOVEDECL;
	EsStringI18n& operator=( const EsStringI18n& ) ES_REMOVEDECL;
};

#endif // ES_I18N

#endif // _es_string_i18n_h_

