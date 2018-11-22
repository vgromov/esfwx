#ifndef _es_string_defs_h_
#define _es_string_defs_h_

/// @file EsStringDefs.h
///

/// Initial char string configuration,
/// stringizer && string generalization stuff
///
#define ES_STRINGIZE_HELPER(x)  #x

/// String literal macros
///
#define ES_STRINGIZE8(x)		    ES_CONCAT(u8, ES_STRINGIZE_HELPER(x))
#define esT8(x)								  ES_CONCAT(u8, x)

#define ES_STRINGIZE16(x)		    ES_CONCAT(u, ES_STRINGIZE_HELPER(x))
#define esT16(x)								ES_CONCAT(u, x)

#define ES_STRINGIZE32(x)		    ES_CONCAT(U, ES_STRINGIZE_HELPER(x))
#define esT32(x)								ES_CONCAT(U, x)

#define ES_STRINGIZEW(x)		    ES_CONCAT(L, ES_STRINGIZE_HELPER(x))
#define esTW(x)								  ES_CONCAT(L, x)

/// wchar_t size macros
///
#if defined(__SIZE_OF_WCHAR_T__)
# define ES_WCHAR_T_SIZE        __SIZE_OF_WCHAR_T__
#elif defined(__SIZEOF_WCHAR_T__)
# define ES_WCHAR_T_SIZE        __SIZEOF_WCHAR_T__
#elif ES_OS == ES_OS_WINDOWS
# define ES_WCHAR_T_SIZE        2
#elif ES_OS == ES_OS_MAC
# define ES_WCHAR_T_SIZE        4
#elif ES_OS == ES_OS_ANDROID
# define ES_WCHAR_T_SIZE        4
#elif ES_OS == ES_OS_LINUX
# define ES_WCHAR_T_SIZE        4
#else
# error ES_WCHAR_T_SIZE is not defined
#endif

/// Map ES_CHAR type && generic string literal macro
///
#if defined(ES_USE_NARROW_ES_CHAR)
  /// Narrow char is used as EsString value_type
  typedef char					 				ES_CHAR;
# define ES_CHAR_SIZE           1
# define ES_WCHAR               wchar_t
# define ES_STRINGIZE		        ES_STRINGIZE_HELPER
# define esT(x)                 x
#elif (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND)
# if defined(WIDECHAR_IS_WCHAR)
    typedef wchar_t						  ES_CHAR;
#   define ES_CHAR_SIZE         ES_WCHAR_T_SIZE
#   define ES_CHAR_IS_WCHAR_T
#   define ES_WCHAR             wchar_t
#   define ES_STRINGIZE         ES_STRINGIZEW
#   define esT                  esTW
# else
#   define ES_CHAR              char16_t
#   define ES_CHAR_SIZE         2
#   define ES_WCHAR             char16_t
#   define ES_WCHAR_SIZE        2
#   define ES_WCHAR_IS_NOT_WCHAR_T
#   define ES_STRINGIZE         ES_STRINGIZE16
#   define esT                  esT16
# endif
#elif (ES_OS == ES_OS_WINDOWS)
  typedef wchar_t						    ES_CHAR;
# define ES_CHAR_SIZE           ES_WCHAR_SIZE
# define ES_CHAR_IS_WCHAR_T
# define ES_WCHAR               wchar_t
# define ES_STRINGIZE           ES_STRINGIZEW
# define esT                    esTW
#else ///< In all other cases we will use 32 bit chars
  typedef char32_t						  ES_CHAR;
# define ES_CHAR_SIZE           4
# define ES_WCHAR               wchar_t
# define ES_STRINGIZE           ES_STRINGIZE32
# define esT                    esT32
#endif

/// Char sequence const pointer type
#define ES_CTSTR                const ES_CHAR*

/// Post-define WCHAR_SIZE
#if !defined(ES_WCHAR_IS_NOT_WCHAR_T)
# define ES_WCHAR_SIZE          ES_WCHAR_T_SIZE
#endif

/// Define ES_UCHAR and ES_UWCHAR types
///
#if 2 == ES_WCHAR_SIZE
# define ES_UWCHAR              esU16
#elif 4 == ES_WCHAR_SIZE
# define ES_UWCHAR              esU32
#endif

#if 1 == ES_CHAR_SIZE
# define ES_UCHAR               esU8
#elif 2 == ES_CHAR_SIZE
# define ES_UCHAR               esU16
#elif 4 == ES_CHAR_SIZE
# define ES_UCHAR               esU32
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
#if !defined(ES_USE_NARROW_ES_CHAR)
# if !defined(ES_WCHAR_IS_NOT_WCHAR_T)
#   define esCout                 std::wcout
#   define esCin                  std::wcin
#   define esCerr                 std::wcerr
#   define esClog                 std::wclog
#   define esPrintf               wprintf
#   define esStrlen					      wcslen
#   define esStrcmp					      wcscmp
#   if (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS) || (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND)
#     define esStricmp			      _wcsicmp
#   else
#     define esStricmp            wcscasecmp
#   endif
#   define esStrncmp				      wcsncmp
#   define esStrchr					      wcsrchr
#   define esStrstr					      wcsstr
#   define esStrftime				      wcsftime
#   if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#    define esVscprintf_l	        _vscwprintf_l
#    define esVsprintf_l		      _vswprintf_l
#    define esVscprinf			      _vscwprintf
#    define esVsnprintf		        _vsnwprintf
#   else
#    define esVsnprintf		        vsnwprintf
#   endif
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
#else //< ES_USE_NARROW_ES_CHAR
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
