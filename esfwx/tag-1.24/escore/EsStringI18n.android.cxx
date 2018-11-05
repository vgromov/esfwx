#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND

//---------------------------------------------------------------------------
static EsString::Array osLocaleInfoGet()
{
  EsString::Array result;

  _di_JLocale loc = TJLocale::JavaClass->getDefault();

  const EsString& countryId = EsString::jstringToString(
    loc->getCountry()
  );

  const EsString& langId = EsString::jstringToString(
    loc->getLanguage()
  );

  if( !langId.empty() && !countryId.empty() )
  {
    result.push_back( langId );
    result.push_back( countryId );
  }
  else
  {
    // Fallback
    result.push_back( esT("en") );
    result.push_back( EsString::null() );
  }

  return result;
}

#else

#include <java/util/Locale.hpp>
#define J2CPP_INCLUDE_IMPLEMENTATION
#include <java/util/Locale.hpp>

//---------------------------------------------------------------------------
// Get locale info, as an array of strings, 0 is string language ID, 1 is string country ID
//
static EsString::Array osLocaleInfoGet()
{
  EsString::Array result;

  j2cpp::local_ref< java::util::Locale > loc =
    java::util::Locale::getDefault();

  j2cpp::local_ref< java::lang::String > countryId = loc.getCountry();
  j2cpp::local_ref< java::lang::String > langId = loc.getLanguage();

  if(lsvc)
  {

  }
  else
  {
    // Fallback
    result.push_back( esT("en") );
    result.push_back( EsString::null() );
  }

  return result;
}
//---------------------------------------------------------------------------

#endif

