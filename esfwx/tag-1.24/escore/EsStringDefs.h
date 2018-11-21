#ifndef _es_string_defs_h_
#define _es_string_defs_h_

// Initial char string configuration,
// stringizer && string generalization stuff
//
#define ES_STRINGIZE_HELPER(x)  #x

#if defined(ES_UNICODE)
# if (ES_OS == ES_OS_WINDOWS) || (ES_OS == ES_OS_LINUX)
#	  define ES_USE_WCHAR
    typedef wchar_t							ES_CHAR;
#   define ES_CHAR_IS_WCHAR_T
  // else use compiler-specific system widechar type, for compatibility with
  // rtl's UnicodeString
# elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#	  define ES_USE_WCHAR
#   if defined(WIDECHAR_IS_WCHAR)
      typedef wchar_t						 ES_CHAR;
#     define ES_CHAR_IS_WCHAR_T
#   else
#     define ES_WCHAR_IS_NOT_WCHAR_T
#     define ES_CHAR             char16_t
#     define ES_WCHAR            char16_t
#     define ES_CHAR_SIZE        2
#   endif
# endif
#endif

// No wchar string implementation is used
#if !defined(ES_USE_WCHAR)
  typedef char					 				ES_CHAR;
  typedef unsigned char         ES_UCHAR;
# define ES_CHAR_SIZE           1
#endif

// Define char sequence const pointer type
#define ES_CTSTR                const ES_CHAR*

// Finalize string configuration mess
//
#if !defined(ES_WCHAR)
# define ES_WCHAR               wchar_t
// Getting to know the default OS wchar_t size
# if ES_OS == ES_OS_WINDOWS
#   define ES_WCHAR_SIZE        2
# elif defined(__SIZE_OF_WCHAR_T__)
#   define ES_WCHAR_SIZE        __SIZE_OF_WCHAR_T__
# elif defined(__SIZEOF_WCHAR_T__)
#   define ES_WCHAR_SIZE        __SIZEOF_WCHAR_T__
# elif ES_OS == ES_OS_MAC
#   define ES_WCHAR_SIZE        4
# elif ES_OS == ES_OS_ANDROID
#   define ES_WCHAR_SIZE        4
# elif ES_OS == ES_OS_LINUX
#   define ES_WCHAR_SIZE        4
# endif
#else
// We're using non standard wide char type
# define ES_WCHAR_SIZE          2
#endif

#if defined(ES_UNICODE)
# if (1 == ES_CHAR_SIZE)
#   define ES_STRINGIZE(x)			ES_CONCAT(u8, ES_STRINGIZE_HELPER(x))
#   define esT(x)								ES_CONCAT(u8, x)
# elif (2 == ES_CHAR_SIZE)
#   define ES_STRINGIZE(x)			ES_CONCAT(u, ES_STRINGIZE_HELPER(x))
#   define esT(x)								ES_CONCAT(u, x)
# elif (4 == ES_CHAR_SIZE)
#   define ES_STRINGIZE(x)			ES_CONCAT(U, ES_STRINGIZE_HELPER(x))
#   define esT(x)								ES_CONCAT(U, x)
# else
#   error ES_CHAR_SIZE is not defined!
# endif
#else
# define ES_STRINGIZE(x)			  ES_STRINGIZE_HELPER(x)
# define esT(x)								  x
#endif

#ifndef ES_WCHAR_SIZE
# error ES_WCHAR_SIZE is unknown|undefined!
#else
# if 2 == ES_WCHAR_SIZE
#   define ES_UWCHAR            esU16
# elif 4 == ES_WCHAR_SIZE
#   define ES_UWCHAR            esU32
# endif
# if defined(ES_USE_WCHAR)
#   define ES_UCHAR             ES_UWCHAR
# endif
#endif

// String I18n macros
//
#ifdef ES_I18N
# define esTranslationGet(x)		EsStringI18n::translate( x )
#	define _(x)										EsStringI18n::translate( esT(x) ).c_str()
#else
#define esTranslationGet(x)			x
#	define _(x)										esT(x)
#endif

// Misc commonly used sting macros
//
#define ES_COMMA esT(",")

// Interpret string literal as variant
//
#define esVT(x) EsVariant(esT(x), EsVariant::ACCEPT_STRING)

// CRT string utilities generalization
//
#if defined(ES_UNICODE) && defined(ES_USE_WCHAR)
# if !defined(ES_WCHAR_IS_NOT_WCHAR_T)
#   define esCout           std::wcout
#   define esCin            std::wcin
#   define esCerr           std::wcerr
#   define esClog           std::wclog
#   define esPrintf         wprintf
#   define esStrlen					wcslen
#   define esStrcmp					wcscmp
#   if (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS) || (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND)
#     define esStricmp			_wcsicmp
#   else
#     define esStricmp      wcscasecmp
#   endif
#	  define esStrncmp				wcsncmp
#   define esStrchr					wcsrchr
#   define esStrstr					wcsstr
#   define esStrftime				wcsftime
#	  if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#		  define esVscprintf_l	_vscwprintf_l
#		  define esVsprintf_l		_vswprintf_l
#		  define esVscprinf			_vscwprintf
# 	  define esVsnprintf		_vsnwprintf
#	  else
# 	  define esVsnprintf		vsnwprintf
#	  endif
# else // ES_WCHAR_IS_NOT_WCHAR_T
  // We use not wchar_t for our wide strings, but char16_t instead
  // so ensure we use borland runtime, and re-route to it
#   if ES_COMPILER_VENDOR != ES_COMPILER_VENDOR_BORLAND
#     error Non standard wide string functions are for Borland|Embarcadero runtime only!
#   endif
//  All non-standard functions are defined in EsCoreRtl module
#   define esPrintf         es_printf
#   define esStrlen				  es_strlen
#   define esStrcmp				  es_strcmp
#   define esStricmp			  es_stricmp
#	  define esStrncmp			  es_strncmp
#   define esStrchr					es_strchr
#   define esStrstr					es_strstr
#   define esStrftime			  es_strftime
# endif // ES_WCHAR_IS_NOT_WCHAR_T
#else
# define esCout           std::cout
# define esCin            std::cin
# define esCerr           std::cerr
# define esClog           std::clog
# define esPrintf         printf
#	define esStrlen					strlen
# define esStrcmp					strcmp
# define esStricmp				stricmp
#	define esStrncmp        strncmp
# define esStrchr					strchr
# define esStrstr					strstr
# define esStrftime				strftime
#	if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#		define esVscprintf_l	_vscprintf_l
#		define esVsprintf_l		_vsnprintf_l
#		define esVscprinf			_vscprintf
# 	define esVsnprintf		_vsnprintf
#	else
# 	define esVsnprintf		vsnprintf
#	endif
#endif

#endif // _es_string_defs_h_
