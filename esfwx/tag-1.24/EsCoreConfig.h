#ifndef _es_core_config_h_
#define _es_core_config_h_

/// @file EsCoreConfig.h
/// Ekosf Framework components configuration. BIGBOI project - specific configuration
///

/// Use our own std::locale facet implementations, with locale cultural data embedded directly into our core code
#define ES_USE_EMBEDDED_LOCALE_IMPL 1

/// unicode build. main string class uses wchar_t under WIN platforms
#define ES_UNICODE

/// use localization engine, based on gettext
#define ES_I18N
//# define ES_I18N_RARE_LANGS

/// Use embarcadero zlib implementation from crtl
#define ES_USE_ZLIB
//# define ES_USE_ECC_ZLIB

// Cryptography configuration
#ifndef __APPLE__
# define ES_USE_CRYPTO
#endif

#define ES_USE_CRYPTO_LEGACY

/// Regex engine configuration
///
#define ES_USE_REGEX
//# define ES_USE_REGEX_STD
# define ES_USE_REGEX_PCRE

/// Misc defines
///
// Use diagnotic linker messages when linking boost libraries to us
#define BOOST_LIB_DIAGNOSTIC
// Disable boost multi-thread support
#define BOOST_DISABLE_THREADS

/// baseintf-impl objects refcount change trace
//#define ES_USE_TRACE_BASEINTF_REFCNT

/// Uncomment to output debug trace information for EsStream internal operations
//#define ES_USE_STREAM_TRACE

#endif // _es_core_config_h_


