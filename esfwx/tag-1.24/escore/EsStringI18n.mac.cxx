//---------------------------------------------------------------------------
// Get locale info, as an array of strings, 0 is string language ID, 1 is string country ID
//
static EsString::Array osLocaleInfoGet()
{
  EsString::Array result;

  CFLocaleRef loc = CFLocaleGetSystem();
  if( loc )
  {
    CFTypeRef langId = CFLocaleGetValue(
      loc,
      kCFLocaleLanguageCode
    );

    CFTypeRef countryId = CFLocaleGetValue(
      loc,
      kCFLocaleCountryCode
    );

    if( langId &&
        countryId &&
        CFGetTypeID(langId) == CFStringGetTypeID() &&
        CFGetTypeID(countryId) == CFStringGetTypeID()
    )
    {
      result.push_back(
        EsString::cfStringRefToString(
          static_cast<CFStringRef>(langId)
        )
      );

      result.push_back(
        EsString::cfStringRefToString(
          static_cast<CFStringRef>(countryId)
        )
      );
    }

    if( langId )
      CFRelease( langId );

    if( countryId )
      CFRelease( countryId );

    if( !result.empty() )
      return result;
  }

  // Fallback
  result.push_back( esT("en") );
  result.push_back( EsString::null() );

  return result;
}
//---------------------------------------------------------------------------
