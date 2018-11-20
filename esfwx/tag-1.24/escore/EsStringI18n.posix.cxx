//---------------------------------------------------------------------------
// Get locale info, as an array of strings, 0 is string language ID, 1 is string country ID
//
static EsString::Array osLocaleInfoGet()
{
  EsString::Array result;

  const std::locale& defloc = std::locale("");
  EsString name = EsString::fromAscii(
    defloc.name()
  );

  // Omit name part after dot '.', if any
  size_t dotpos = name.find(esT('.'));
  if( dotpos != EsString::npos )
    name = name.substr(0, dotpos);

  const EsString& lang = EsStringI18n::languageCodeGet(name);
  const EsString& ccode = EsStringI18n::countryCodeGet(name);

  if( !lang.empty() )
  {
    result.push_back( lang );

    if( !ccode.empty() )
      result.push_back( ccode );
    else
      result.push_back( EsString::null() );
  }
  else
  {
    // Fallback
    result.push_back( esT("en") );
    result.push_back( EsString::null() );
  }

  return result;
}
