//---------------------------------------------------------------------------
// Get locale info, as an array of strings, 0 is string language ID, 1 is string country ID
//
static EsString::Array osLocaleInfoGet()
{
  TCHAR buff[LOCALE_NAME_MAX_LENGTH];
  EsString::Array result;

  int ret = ::GetLocaleInfo(
    LOCALE_USER_DEFAULT,
    LOCALE_SISO639LANGNAME,
    buff,
    LOCALE_NAME_MAX_LENGTH
  );

  if( ret )
  {
    result.push_back( buff );

    ret = ::GetLocaleInfo(
      LOCALE_USER_DEFAULT,
      LOCALE_SISO3166CTRYNAME,
      buff,
      LOCALE_NAME_MAX_LENGTH
    );

    if( ret )
    {
      result.push_back( buff );
      return result;
    }
  }

  // Fallback
  result.push_back( esT("en") );
  result.push_back( EsString::null() );

  return result;
}
//---------------------------------------------------------------------------

