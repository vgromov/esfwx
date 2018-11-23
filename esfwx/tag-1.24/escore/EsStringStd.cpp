#include "escorepch.h"
#pragma hdrstop

#include "EsStringStd.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if (defined(ES_USE_NARROW_ES_CHAR) || defined(ES_CHAR_IS_WCHAR_T)) || (4 == ES_CHAR_SIZE)
const EsWideString16& nullWideString16() ES_NOTHROW
{
  static const EsWideString16 sc_nullWideString16;
  return sc_nullWideString16;
}
//---------------------------------------------------------------------------
#endif

#if (defined(ES_USE_NARROW_ES_CHAR) || defined(ES_CHAR_IS_WCHAR_T)) || (2 == ES_CHAR_SIZE)
const EsWideString32& nullWideString32() ES_NOTHROW
{
  static const EsWideString32 sc_nullWideString32;
  return sc_nullWideString32;
}
//---------------------------------------------------------------------------
#endif

#if !defined(ES_USE_NARROW_ES_CHAR)
const EsByteString& nullByteString() ES_NOTHROW
{
  static const EsByteString sc_nullByteString;
  return sc_nullByteString;
}
#endif

#if !defined(ES_CHAR_IS_WCHAR_T)
const EsWideString& nullWideString() ES_NOTHROW
{
  static const EsWideString sc_nullWideString;
  return sc_nullWideString;
}
#endif

size_t esBstrnlen(const char* str, size_t maxCnt) ES_NOTHROW
{
	size_t len = 0;
	if( str )
		while( *str++ && ++len < maxCnt );

	return len;
}
//---------------------------------------------------------------------------

void esBtoupper(char* str, size_t cnt) ES_NOTHROW
{
  if( !str )
    return;

  while( cnt-- )
  {
    *str = std::toupper( *str
#if ES_COMPILER_VENDOR_MS == ES_COMPILER_VENDOR
      , EsLocale::locale()
#endif
    );
    ++str;
  }
}
//---------------------------------------------------------------------------

void esBtoupper(EsByteString& bstr) ES_NOTHROW
{
  if( bstr.empty() )
    return;

  size_t cnt = bstr.size();
  for(size_t idx = 0; idx < cnt; ++idx )
    bstr[idx] = std::toupper( bstr[idx]
#if ES_COMPILER_VENDOR_MS == ES_COMPILER_VENDOR
      , EsLocale::locale()
#endif
    );
}
//---------------------------------------------------------------------------
