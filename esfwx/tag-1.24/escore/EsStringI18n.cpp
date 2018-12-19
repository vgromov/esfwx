#include "escorepch.h"
#pragma hdrstop

#include "EsStringI18n.h"

#ifdef ES_I18N

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// mo file parser helper class
//
const esU32 c_moMagicNumber        = 0x950412DE;
const esU32 c_moMagicNumberSwapped = 0xDE120495;
//---------------------------------------------------------------------------

// define language codes enumeration
#define ES_LANG_MAP_BEGIN         enum EsI18nLangCode {
#define ES_LANG_MAP_END            i18nLangCodesCount };
#define ES_LANG_ENTRY(code, name, nameNative) i18n_## code,
#include "EsStringI18n.def"
//---------------------------------------------------------------------------

// define language codes to language names map
#define ES_LANG_MAP_BEGIN \
  static const struct { \
    EsString::const_pointer code; EsString::const_pointer name; EsString::const_pointer nameNative;} \
    sc_langCodesMap[] = {
#define ES_LANG_MAP_END  \
  {0, 0, 0} };
#define ES_LANG_ENTRY(code, name, nameNative) \
  {ES_STRINGIZE(code), ES_STRINGIZE(name), esT( nameNative )},
#include "EsStringI18n.def"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// mo file parser helper class
class GettextMoParser
{
public:
  struct MoFileHeader {
    esU32 magic;                    // offset +00:  magic id
    esU32 revision;                 //        +04:  revision
    esU32 numStrings;               //        +08:  number of strings in the file
    esU32 offsetOriginalStrings;    //        +0C:  start of original string table
    esU32 offsetTranslatedStrings;  //        +10:  start of translated string table
    esU32 hashTableSize;            //        +14:  hash table size
    esU32 offsetHashTable;          //        +18:  offset of hash table start
  };

  struct MoOffsetTableItem {
    esU32 length;    // length of the string
    esU32 offset;    // offset to the beginning of the string
  };

public:
  GettextMoParser(EsStringI18n& translationStorage, const EsBinBuffer& data, const EsString& domain);

  inline bool isOk() const ES_NOTHROW { return m_parsed; }
  bool parse();

  inline esU32 swapIfNeeded(esU32 in) const ES_NOTHROW
  {
    if( m_swapped )
      return EsUtilities::inverse4(in);
    else
      return in;
  }

  // return mo file charset string
  inline const EsByteString& charsetGet() const ES_NOTHROW { return m_charset; }

private:
  void charsetExtract();
  void translationsExtract();
  void stringsExtract(const char* str, size_t strLen, EsStringConverter& conv,
    EsString& main, EsString::Array& plurals);

private:
  EsStringI18n& m_storage;
  const EsBinBuffer& m_data;

  bool m_swapped;
  MoFileHeader m_hdr;
  EsByteString m_charset;
  EsString m_domain;
  bool m_parsed;
};
//---------------------------------------------------------------------------

GettextMoParser::GettextMoParser(EsStringI18n& translationStorage, const EsBinBuffer& data, const EsString& domain) :
m_storage(translationStorage),
m_data(data),
m_swapped(false),
m_domain(domain),
m_parsed(false)
{
  m_hdr = {0};
}
//---------------------------------------------------------------------------

bool GettextMoParser::parse()
{
  if( !m_parsed && m_data.size() >= sizeof(MoFileHeader) )
  {
    memcpy((esU8*)&m_hdr, &m_data[0], sizeof(m_hdr));
    m_swapped = m_hdr.magic == c_moMagicNumberSwapped;
    if(m_hdr.magic == c_moMagicNumber || m_hdr.magic == c_moMagicNumberSwapped)
    {
      m_hdr.magic = swapIfNeeded(m_hdr.magic);
      m_hdr.revision = swapIfNeeded(m_hdr.revision);
      m_hdr.numStrings = swapIfNeeded(m_hdr.numStrings);
      m_hdr.offsetOriginalStrings = swapIfNeeded(m_hdr.offsetOriginalStrings);
      m_hdr.offsetTranslatedStrings = swapIfNeeded(m_hdr.offsetTranslatedStrings);
      m_hdr.hashTableSize = swapIfNeeded(m_hdr.hashTableSize);
      m_hdr.offsetHashTable = swapIfNeeded(m_hdr.offsetHashTable);

      // extract charset
      charsetExtract();

      // extract translations
      translationsExtract();

      m_parsed = true;
    }
  }

  return m_parsed;
}
//---------------------------------------------------------------------------

void GettextMoParser::charsetExtract()
{
  const esU8* data = &m_data[0];
  const MoOffsetTableItem* translationTable =
    (const MoOffsetTableItem*)(data + m_hdr.offsetTranslatedStrings);
  size_t length = swapIfNeeded(translationTable->length);
  size_t offset = swapIfNeeded(translationTable->offset);

  if( length )
  {
    const char* infoBuffer = (const char*)(data + offset);
    EsByteString info(infoBuffer);
    size_t contentTypePos = info.find("Content-Type: text/plain; charset=");
    if( contentTypePos != EsByteString::npos )
    {
      size_t stringStart = contentTypePos + 34; // strlen("Content-Type: text/plain; charset=")
      size_t stringEnd = info.find('\n', stringStart);
      if(stringEnd != EsByteString::npos &&
         stringEnd > stringStart )
      {
        const EsByteString& cs = info.substr(stringStart, stringEnd-stringStart);
        if( 0 != cs.compare("CHARSET") )
          m_charset = cs;
      }
    }
  }
}
//---------------------------------------------------------------------------

void GettextMoParser::stringsExtract(
  const char* str,
  size_t strLen,
  EsStringConverter& conv,
  EsString& main,
  EsString::Array& plurals
)
{
  ES_ASSERT(str);
  ES_ASSERT(strLen);
  EsByteString src(str, strLen);
  size_t pos0 = src.find('\0');
  if( pos0 != EsByteString::npos )
  {
    main =
#if !defined(ES_USE_NARROW_ES_CHAR)
    conv.cToW_T<EsString>(
#else
    conv.cToC(
#endif
      src.substr(0, pos0)
    );

    do
    {
      size_t offs = pos0+1;
      pos0 = src.find('\0', offs);

      plurals.push_back(
#if !defined(ES_USE_NARROW_ES_CHAR)
        conv.cToW_T<EsString>(
#else
        conv.cToC(
#endif
          src.substr(
            offs,
            (pos0 == EsByteString::npos) ?
              EsByteString::npos :
              pos0 - offs
          )
        )
      );

    } while( pos0 != EsByteString::npos );
  }
  else
    main =
#if !defined(ES_USE_NARROW_ES_CHAR)
      conv.cToW_T<EsString>(
#else
      conv.cToC(
#endif
      src
    );
}
//---------------------------------------------------------------------------

void GettextMoParser::translationsExtract()
{
  ES_ASSERT( !m_charset.empty() );

  EsStringConverter::Ptr conv = EsStringConverter::convGet(
    EsString::defEncoding(),
    m_charset
  );

  const esU8* data = &m_data[0];
  const MoOffsetTableItem* originalTable =
    reinterpret_cast<const MoOffsetTableItem*>(data + m_hdr.offsetOriginalStrings);

  size_t orgLength = swapIfNeeded(originalTable->length);
  size_t orgOffset = swapIfNeeded(originalTable->offset);

  for( size_t idx = 0; idx < m_hdr.numStrings; ++idx )
  {
    if( orgLength )
    {
      // extract original string
      EsByteString org((const char*)(data + orgOffset), orgLength);

      // extract corresponding translation with optional plurals
      const MoOffsetTableItem* translationTable =
        reinterpret_cast<const MoOffsetTableItem*>(data + m_hdr.offsetTranslatedStrings);

      translationTable += idx;
      size_t trLength = swapIfNeeded(translationTable->length);
      size_t trOffset = swapIfNeeded(translationTable->offset);

      EsString trMain;
      EsString::Array trPlurals;

      if( trLength )
        stringsExtract(
          reinterpret_cast<const char*>(data + trOffset),
          trLength,
          *conv,
          trMain,
          trPlurals
        );

      // append translation entry
      m_storage.translationPluralAppend(
        EsString::fromAscii(org),
        trMain,
        trPlurals,
        m_domain
      );
    }

    ++originalTable;
    orgLength = swapIfNeeded(originalTable->length);
    orgOffset = swapIfNeeded(originalTable->offset);
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsStringI18n::EsStringI18n(bool isDefault) :
m_isDefault(isDefault),
m_langCode(-1)
{
}
//---------------------------------------------------------------------------

EsStringI18n::~EsStringI18n()
{
}
//---------------------------------------------------------------------------

void EsStringI18n::translationSearchPathsSet(const EsString::Array& paths)
{
  EsCriticalSectionLocker lock(m_cs);
  m_translationPaths = paths;

  if( !m_isDefault )
  {
    translationLocalesEnumerate();
    defaultGet().translationSearchPathsSet(paths);
  }
}
//---------------------------------------------------------------------------

const EsString::Array& EsStringI18n::translationSearchPathsGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_translationPaths;
}
//---------------------------------------------------------------------------

void EsStringI18n::translationSearchPathsReset()
{
  EsCriticalSectionLocker lock(m_cs);
  m_translationPaths.clear();
  m_availableLocales.clear();
  m_domainFiles.clear();
  if( !m_isDefault )
    defaultGet().translationSearchPathsReset();
}
//---------------------------------------------------------------------------

const EsString::Array& EsStringI18n::availableLocalesGet()
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_isDefault )
  {
    const EsString& dl = defaultLocaleGet();
    if(
      !dl.empty() &&
      m_availableLocales.end() == std::find(
        m_availableLocales.begin(),
        m_availableLocales.end(),
        dl
      )
    )
      m_availableLocales.insert(
        m_availableLocales.begin(),
        dl
      );
  }

  return m_availableLocales;
}
//---------------------------------------------------------------------------

const EsString& EsStringI18n::defaultLocaleGet() const
{
  if( !m_isDefault )
    return defaultGet().currentLocaleGet();

  return m_locale;
}
//---------------------------------------------------------------------------

void EsStringI18n::defaultLocaleSet(const EsString& def)
{
  if( !m_isDefault )
    defaultGet().currentLocaleSet(def);
}
//---------------------------------------------------------------------------

const EsString& EsStringI18n::currentLocaleGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_locale;
}
//---------------------------------------------------------------------------

static int internalLangCodeGetFromLocale(const EsString& loc)
{
  const EsString& langCodeStr = EsStringI18n::languageCodeGet(loc);
  if( !langCodeStr.empty() )
  {
    for(int idx = 0; idx < i18nLangCodesCount; ++idx)
    {
      if( langCodeStr == sc_langCodesMap[idx].code )
        return idx;
    }
  }

  return -1;
}
//---------------------------------------------------------------------------

void EsStringI18n::currentLocaleSet(const EsString& loc)
{
  EsCriticalSectionLocker lock(m_cs);
  if( m_isDefault )
  {
    m_availableLocales.clear();
    m_availableLocales.push_back(loc);
  }
  m_langCode = internalLangCodeGetFromLocale(loc);
  translationsLoad(loc);

  EsEventDispatcher::eventPost(
    ES_EVTC_I18N,
    static_cast<ulong>(EsEvtI18n::LocaleChanged),
    loc
  );
}
//---------------------------------------------------------------------------

EsString::Array EsStringI18n::translationDomainsGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  EsString::Array result;
  result.reserve( m_domainFiles.size() );
  for(DomainFilesMap::const_iterator cit = m_domainFiles.begin();
    cit != m_domainFiles.end(); ++cit)
      result.push_back( (*cit).first );

  return result;
}
//---------------------------------------------------------------------------

EsString EsStringI18n::languageCodeGet(const EsString& loc)
{
  static EsRegEx s_re(esT("([a-z]{2,3})(?:[_-][A-Z]{2})?"));
  s_re.set_text(loc);
  if( s_re.get_matches() )
    return s_re.matchGet(1);

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsStringI18n::countryCodeGet(const EsString& loc)
{
  static EsRegEx s_re(esT("[a-z]{2,3}[_-]([A-Z]{2})"));
  s_re.set_text(loc);
  if( s_re.get_matches() )
    return s_re.matchGet(1);

  return EsString::null();
}
//---------------------------------------------------------------------------

static EsString::const_pointer internalLanguageNameGet(int langCode)
{
  if( langCode > -1 && langCode < i18nLangCodesCount )
    return sc_langCodesMap[langCode].name;

  return esT("Unknown");
}
//---------------------------------------------------------------------------

static EsString::const_pointer internalNativeLanguageNameGet(int langCode)
{
  if( langCode > -1 && langCode < i18nLangCodesCount )
    return sc_langCodesMap[langCode].nameNative;

  return esT("Unknown");
}
//---------------------------------------------------------------------------

EsString EsStringI18n::languageNameGet(const EsString& loc)
{
  return internalLanguageNameGet(
    internalLangCodeGetFromLocale(loc)
  );
}
//---------------------------------------------------------------------------

EsString EsStringI18n::nativeLanguageNameGet(const EsString& loc)
{
  return internalNativeLanguageNameGet(
    internalLangCodeGetFromLocale(loc)
  );
}
//---------------------------------------------------------------------------

void EsStringI18n::translationAppend(const EsString& source, const EsString& translation, const EsString& domain)
{
  translationPluralAppend(source, translation, EsString::nullArray(), domain);
}
//---------------------------------------------------------------------------

static int pluralFormIdxGet(int langCode, int& formsCnt, size_t n)
{
  switch(langCode)
  {
  case i18n_en: case i18n_de: case i18n_nl: case i18n_sv: case i18n_da:  case i18n_no:
  case i18n_fo: case i18n_es:  case i18n_pt: case i18n_it: case i18n_bg: case i18n_el:
  case i18n_fi: case i18n_et: case i18n_he: case i18n_eo: case i18n_hu: case i18n_tr:
    formsCnt = 2;
    return n != 1;
  case i18n_fr:// case i18n_pt: brazilian portuguese requires full locale code, ie pt_BR
    formsCnt = 2;
    return n>1;
  case i18n_lv:
    formsCnt = 3;
    return n%10==1 && n%100!=11 ? 0 : n != 0 ? 1 : 2;
  case i18n_ga: case i18n_gd:
    formsCnt = 3;
    return n==1 ? 0 : n==2 ? 1 : 2;
  case i18n_lt:
    formsCnt = 3;
    return n%10==1 && n%100!=11 ? 0 :
      n%10>=2 && (n%100<10 || n%100>=20) ? 1 : 2;
  case i18n_hr: case i18n_cs: case i18n_ru: case i18n_sk: case i18n_uk:
    formsCnt = 3;
    return n%10==1 && n%100!=11 ? 0 :
      n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;
  case i18n_pl:
    formsCnt = 3;
    return n==1 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2;
  case i18n_sl:
    formsCnt = 4;
    return n%100==1 ? 0 : n%100==2 ? 1 : n%100==3 || n%100==4 ? 2 : 3;
  default:
    formsCnt = 0;
    return 0; // no plural form, only singular
  }
}
//---------------------------------------------------------------------------

void EsStringI18n::checkPluralFormsCount( int count, const EsString::Array& plurals ) const
{
  if( count != (int)plurals.size() )
    EsException::Throw(
      esT("Plural forms count differs from language '%s' specification; expected %d, got %d"),
      languageNameGet(m_langCode),
      count,
      plurals.size()
    );
}
//---------------------------------------------------------------------------

void EsStringI18n::translationPluralAppend(const EsString& source,
  const EsString& translationMain, const EsString::Array& translationPluralForms, const EsString& domain)
{
  ES_ASSERT( !source.empty() );
  ES_ASSERT( !translationMain.empty() );
  ES_ASSERT( !domain.empty() );

  EsCriticalSectionLocker lock(m_cs);

  TranslationEntry entry(source, translationMain, translationPluralForms);

  if( !translationPluralForms.empty() )
  {
    int pluralsCnt;
    pluralFormIdxGet(m_langCode, pluralsCnt, 0);
    checkPluralFormsCount(pluralsCnt, translationPluralForms);
  }

  EsString::HashT hash = source.hashGet();

  // append entry to the specific domain
  DomainTranslationsT::iterator it = m_domainTranslations.find( domain );
  if( it != m_domainTranslations.end() )
  {
    MapT& map = (*it).second;
    MapT::const_iterator cit = map.find(hash);
    if( cit == map.end() )
      map[hash] = entry;
  }
  else
  {
    MapT newMap;
    newMap[hash] = entry;
    m_domainTranslations[domain] = newMap;
  }
}
//---------------------------------------------------------------------------

const EsStringI18n::TranslationEntry& EsStringI18n::nullEntry()
{
  static const TranslationEntry sc_nullEntry;

  return sc_nullEntry;
}
//---------------------------------------------------------------------------

const EsStringI18n::TranslationEntry& EsStringI18n::translationFind(const EsString& src, const EsString& domain) const
{
  DomainTranslationsT::const_iterator cit = m_domainTranslations.find(domain);
  EsString::HashT hash = src.hashGet();

  if( cit != m_domainTranslations.end() )
  {
    const MapT& map = (*cit).second;
    MapT::const_iterator mcit = map.find( hash );
    if( mcit != map.end() )
      return (*mcit).second;
  }

  // search all domains otherwise
  for( DomainTranslationsT::const_iterator ait = m_domainTranslations.begin();
    ait != m_domainTranslations.end(); ++ait )
  {
    if( ait != cit )
    {
      const MapT& map = (*ait).second;
      MapT::const_iterator mcit = map.find( hash );
      if( mcit != map.end() )
        return (*mcit).second;
    }
  }

  return nullEntry();
}
//---------------------------------------------------------------------------

EsString EsStringI18n::translationGet(const EsString& src, const EsString& domain /*= EsString::null()*/) const
{
  EsCriticalSectionLocker lock(m_cs);

  const TranslationEntry& result = translationFind( src, domain );
  if( result.isOk() )
    return result.m_trMain;

  // fallback to default locale translation, or original string
  if( !m_isDefault )
    return defaultGet().translationGet(src, domain);
  else
    return src;
}
//---------------------------------------------------------------------------

EsString EsStringI18n::pluralEval(const TranslationEntry& entry, size_t cnt) const
{
  if( !entry.m_trPlurals.empty() )
  {
    int pluralsCnt;
    int idx = pluralFormIdxGet(m_langCode, pluralsCnt, cnt);

    checkPluralFormsCount(pluralsCnt, entry.m_trPlurals);
    return entry.m_trPlurals[idx];
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsStringI18n::translationPluralGet(const EsString& src, size_t cnt, const EsString& domain /*= EsString::null()*/) const
{
  EsCriticalSectionLocker lock(m_cs);

  const TranslationEntry& result = translationFind(src, domain);
  if( result.isOk() )
    return pluralEval(result, cnt);

  // fallback to default locale translation, or original string
  if( !m_isDefault )
    return defaultGet().translationPluralGet(src, cnt, domain);
  else
    return src;
}
//---------------------------------------------------------------------------

EsStringI18n& EsStringI18n::instanceGet()
{
  static EsStringI18n s_i18n(false);
  return s_i18n;
}
//---------------------------------------------------------------------------

EsStringI18n& EsStringI18n::defaultGet()
{
  static EsStringI18n s_i18nDefault(true);
  return s_i18nDefault;
}
//---------------------------------------------------------------------------

class EsLocaleEnumerator : public EsPathEnumerator
{
public:
  EsLocaleEnumerator(const EsString& searchPath, EsRegEx& re, EsString::Array& locales) :
  EsPathEnumerator(
    searchPath,
    esT("*.*.mo")
  ),
  m_re(re),
  m_locales(locales)
  {}

protected:
  virtual bool onObject(const EsString& ES_UNUSED(curPath), const EsString& name, bool isDir)
  {
    if( !isDir )
    {
      // extract locale part from file name
      m_re.set_text(name);
      if( m_re.get_matches() )
      {
        const EsString& locale = m_re.matchGet(1);
        if( std::find(m_locales.begin(), m_locales.end(), locale) == m_locales.end() )
          m_locales.push_back(locale);
      }
    }

    return true;
  }

private:
  EsRegEx& m_re;
  EsString::Array& m_locales;
};
//---------------------------------------------------------------------------

void EsStringI18n::translationLocalesEnumerate()
{
  m_availableLocales.clear();

  if( m_isDefault )
    return;

  if( !defaultLocaleGet().empty() )
    m_availableLocales.push_back( defaultLocaleGet() );

  if( !m_translationPaths.empty() )
  {
    EsRegEx re(esT("^[^\\.]+\\.([a-z]{2,3}(?:[_-][A-Z]{2})?)\\.mo$"));
    for(size_t idx = 0; idx < m_translationPaths.size(); ++idx)
    {
      EsLocaleEnumerator enumerator(
        m_translationPaths[idx],
        re,
        m_availableLocales
      );

      enumerator.execute();
    }
  }
}
//---------------------------------------------------------------------------

class EsLocaleDomainTranslationsFinder : public EsPathEnumerator
{
public:
  EsLocaleDomainTranslationsFinder(const EsString& searchPath, const EsString& locale, const EsString& domain ) :
  EsPathEnumerator(searchPath, domain + esT(".") + locale + esT(".mo"))
  {}

  const EsString& domainFileGet() const { return m_domainFile; }

protected:
  virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
  {
    if( !isDir )
    {
      m_domainFile = curPath + name;
      return false;
    }

    return true;
  }

private:
  EsString m_domainFile;
};
//---------------------------------------------------------------------------

void EsStringI18n::moFileLoad(const EsString& moFile, const EsString& domain)
{
  EsFile f(moFile, static_cast<ulong>(EsFileFlag::Read));
  if( f.open() )
  {
    const EsBinBuffer& data = f.readAllAsBinBuffer();
    GettextMoParser moParser(*this, data, domain);
    moParser.parse();
    ES_ASSERT(moParser.isOk());

    f.close();
  }
}
//---------------------------------------------------------------------------

void EsStringI18n::domainTranslationsLoad(const EsString& domain)
{
  ES_ASSERT( !domain.empty() );
  EsCriticalSectionLocker lock(m_cs);

  if( m_domainTranslations.end() == m_domainTranslations.find(domain) )
  {
    EsString domainFile;
    for(size_t idx = 0; idx < m_translationPaths.size(); ++idx)
    {
      EsLocaleDomainTranslationsFinder finder(m_translationPaths[idx], m_locale, domain);
      finder.execute();
      domainFile = finder.domainFileGet();
      if( !domainFile.empty() )
        break;
    }

    if( !domainFile.empty() )
    {
      moFileLoad(domainFile, domain);
      m_domainFiles[domain] = domainFile;
    }
  }

  if( !m_isDefault )
    defaultGet().domainTranslationsLoad(domain);
}
//---------------------------------------------------------------------------

void EsStringI18n::domainTranslationsUnload(const EsString& domain)
{
  ES_ASSERT( !domain.empty() );
  EsCriticalSectionLocker lock(m_cs);

  DomainFilesMap::iterator fit = m_domainFiles.find(domain);
  if( m_domainFiles.end() != fit )
    m_domainFiles.erase(fit);

  DomainTranslationsT::iterator it = m_domainTranslations.find(domain);
  if( m_domainTranslations.end() != it )
    m_domainTranslations.erase(it);

  if( !m_isDefault )
    defaultGet().domainTranslationsUnload(domain);
}
//---------------------------------------------------------------------------

// this helper class enumerates all domain files for given locale in
// search path, and adds it to map container. if domain file already exists in
// map, it's skipped, so if domain translation file is encountered in multiple locations,
// only the first one is enumerated
//
class EsLocaleTranslationsEnumerator : public EsPathEnumerator
{
public:
  EsLocaleTranslationsEnumerator(const EsString& searchPath, const EsString& locale,
    EsRegEx& re, EsStringI18n::DomainFilesMap& map ) :
  EsPathEnumerator(searchPath, esT("*.") + locale + esT(".mo")),
  m_re(re),
  m_map(map)
  {}

protected:
  virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
  {
    if( !isDir )
    {
      m_re.set_text(name);
      if( m_re.get_matches() )
      {
        const EsString& domain = m_re.matchGet(1);
        if( m_map.end() == m_map.find(domain) )
          m_map[domain] = curPath + name;
      }
    }

    return true;
  }

private:
  EsRegEx& m_re;
  EsStringI18n::DomainFilesMap& m_map;
};
//---------------------------------------------------------------------------

void EsStringI18n::translationsLoad(const EsString& newLocale)
{
  ES_ASSERT( !newLocale.empty() );
  if( m_locale != newLocale )
  {
    m_locale = newLocale;
    m_domainTranslations.clear();
    m_domainFiles.clear();
    if( !m_isDefault && newLocale == defaultLocaleGet() )
    {
      defaultGet().translationsLoad(newLocale);
      return;
    }

    bool availableLocaleMatch = false;
    const EsString& newLang = languageCodeGet(newLocale);
    for(size_t idx = 0; idx < m_availableLocales.size(); ++idx )
    {
      const EsString& loc = m_availableLocales[idx];
      if( languageCodeGet(loc) == newLang )
      {
        m_locale = loc; //< Potentially fix-up the language-only locale name
        availableLocaleMatch = true;
        break;
      }
    }

    if( availableLocaleMatch )
    {
      EsRegEx re(esT("^([^\\.]+)\\.[a-z]{2,3}(?:[_-][A-Z]{2})?\\.mo$"));

      for(size_t idx = 0; idx < m_translationPaths.size(); ++idx)
      {
        EsLocaleTranslationsEnumerator enumerator(m_translationPaths[idx], m_locale, re, m_domainFiles);
        enumerator.execute();
      }

      for( DomainFilesMap::const_iterator cit = m_domainFiles.begin(); cit != m_domainFiles.end(); ++cit )
      {
        const EsString& domain = (*cit).first;
        const EsString& moFile = (*cit).second;

        moFileLoad(moFile, domain);
      }
    }
  }
}
//---------------------------------------------------------------------------

static int internalLangCodeGetFromLanguage(const EsString& lang)
{
  if( !lang.empty() )
  {
    for(int idx = 0; idx < i18nLangCodesCount; ++idx)
    {
      if( lang == sc_langCodesMap[idx].name )
        return idx;
    }
  }

  return -1;
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsStringI18n.win.cxx"
#elif ES_OS == ES_OS_MAC || ES_OS == ES_OS_IOS
# include "EsStringI18n.mac.cxx"
#elif ES_OS == ES_OS_ANDROID
# include "EsStringI18n.android.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsStringI18n.posix.cxx"
#else
# error "OS locale services are not implemented for the current platform"
#endif
//---------------------------------------------------------------------------

EsString EsStringI18n::osLocaleIdGet(const EsString& sep /*= esT("_")*/)
{
  const EsString::Array& info = osLocaleInfoGet();
  if( !info[1].empty() )
    return info[0] + sep + info[1];

  return info[0];
}
//---------------------------------------------------------------------------

EsString EsStringI18n::osLanguageIdGet()
{
  const EsString::Array& info = osLocaleInfoGet();
  return info[0];
}
//---------------------------------------------------------------------------

EsString EsStringI18n::osCountryIdGet()
{
  const EsString::Array& info = osLocaleInfoGet();
  return info[1];
}
//---------------------------------------------------------------------------

#endif // ES_I18N

