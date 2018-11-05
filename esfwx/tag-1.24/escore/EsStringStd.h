#ifndef _es_string_std_h_
#define _es_string_std_h_

typedef std::basic_string<ES_CHAR>            EsBasicStringT;
typedef std::basic_string<char16_t>           EsWideString16;
typedef std::basic_string<char32_t>           EsWideString32;

ESCORE_FUNC( const EsWideString16&)           nullWideString16() ES_NOTHROW;
ESCORE_FUNC( const EsWideString32&)           nullWideString32() ES_NOTHROW;

#if defined(ES_USE_WCHAR)
# define EsWideString                         EsString
# define nullWideString()                     EsString::null()
  typedef std::string					                EsByteString;
  ESCORE_FUNC( const EsByteString&)           nullByteString() ES_NOTHROW;
#else
  typedef std::basic_string<ES_WCHAR>         EsWideString;
# define EsByteString                         EsString
# define nullByteString()                     EsString::null()
  ESCORE_FUNC( const EsWideString&)           nullWideString() ES_NOTHROW;
#endif

ESCORE_FUNC( size_t) esBstrnlen(const char* str, size_t maxCnt = static_cast<size_t>(-1)) ES_NOTHROW;
ESCORE_FUNC( void)   esBtoupper(char* str, size_t maxCnt) ES_NOTHROW;
ESCORE_FUNC( void)   esBtoupper(EsByteString& bstr) ES_NOTHROW;

// Templated null strings accessors
//
template <typename StrT>
inline const StrT& nullStringT() ES_NOTHROW { return StrT::null(); }

// Specializations for known types
template <>
inline const EsByteString& nullStringT<EsByteString>() ES_NOTHROW { return nullByteString(); }

template <>
inline const EsWideString16& nullStringT<EsWideString16>() ES_NOTHROW { return nullWideString16(); }

template <>
inline const EsWideString32& nullStringT<EsWideString32>() ES_NOTHROW { return nullWideString32(); }

#endif // _es_string_std_h_
