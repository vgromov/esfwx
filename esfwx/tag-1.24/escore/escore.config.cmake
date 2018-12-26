# Component version definitions
set(component_DESCRIPTION_STRING "ESFWX Core Library")
set(component_INTERNAL_NAME "escore")
set(component_ORIGINAL_NAME "escore")
set(component_BUILD_NUM 1)

# Core configuration parameters
#
# Locale && I18N
set(ESCORE_USE_EMBEDDED_LOCALE 1 CACHE BOOL "Default=1. Use our own std::locale facet implementations, with locale cultural data embedded directly into our core code.")
set(ESCORE_USE_I18N 1 CACHE BOOL "Default=1. Use I18N engine, based on gettext")
set(ESCORE_USE_I18N_RARE_LANGS 0 CACHE BOOL "Default=0. Use I18N extra rare languages support")

# ZLIB
set(ESCORE_USE_ZLIB 1 CACHE BOOL "Default=1. Use ZLIB")
set(ESCORE_USE_ZLIB_FLAVOUR "ZLIB_EMBEDDED" CACHE STRING "Default=ZLIB_EMBEDDED. Select implementation of ZLIB, our embedded ZLIB, or Embarcadero RTL")
set_property(CACHE ESCORE_USE_ZLIB_FLAVOUR PROPERTY STRINGS ZLIB_EMBEDDED ZLIB_ECC)

# Cryptography support
set(ESCORE_USE_CRYPTO 1 CACHE BOOL "Default=1. Use embedded implementation of CryptoPP functionality")
set(ESCORE_USE_CRYPTO_LEGACY 1 CACHE BOOL "Default=1. Use small simplified DES functionality, independent of CryptoPP")
set(ESCORE_USE_CRYPTO_DEBUG_TRACE 0 CACHE BOOL "Default=0. Use crypto functions diagnostics in Debug mode")

# Regex supprot
set(ESCORE_USE_REGEX 1 CACHE BOOL "Default=1. Use Regular expressions")
set(ESCORE_USE_REGEX_FLAVOUR "PCRE2_EMBEDDED" CACHE STRING "Default=PCRE2_EMBEDDED. Use STD to invoke Regular expressions implementation from standard C++ library. PCRE2_EMBEDDED selects tweaked PCRE library code")
set_property(CACHE ESCORE_USE_REGEX_FLAVOUR PROPERTY STRINGS STD PCRE2_EMBEDDED)

# Misc optional boost tuneup
set(ESCORE_BOOST_LIB_DIAGNOSTICS 1 CACHE BOOL "Default=1. Enable boost compilation diagnostic messages")
set(ESCORE_BOOST_DISABLE_THREADS 1 CACHE BOOL "Default=1. Disable boost threading support")

# Misc diagnostics
set(ESCORE_TRACE_BASEINTF_REFCNT 0 CACHE BOOL "Default=0. If = 1, Debug mode will use BASEINTF objects refcount changes diagnostic trace, where it's available. Will seriously impact application performance")
set(ESCORE_TRACE_STREAM 0 CACHE BOOL "Default=0. If = 1, Debug mode will show EsStream and related classes diagnostic trace, where it's available.")
set(ESCORE_TRACE_REFLECTED_CALLS 0 CACHE BOOL "Default=0. If = 1, Debug mode will show reflection calls diagnostic trace, where it's available.")

# Prepare variables for core defines
#
set(ES_USE_EMBEDDED_LOCALE_IMPL ${ESCORE_USE_EMBEDDED_LOCALE})

set(ES_I18N ${ESCORE_USE_I18N})
set(ES_I18N_RARE_LANGS ${ESCORE_USE_I18N_RARE_LANGS})

set(ES_USE_ZLIB ${ESCORE_USE_ZLIB})
if( ES_USE_ZLIB )
  if( ESCORE_USE_ZLIB_FLAVOUR STREQUAL "ZLIB_ECC" )
    set(ES_USE_ECC_ZLIB 1)
  endif()
endif()

set(ES_USE_CRYPTO ${ESCORE_USE_CRYPTO})
set(ES_USE_CRYPTO_LEGACY ${ESCORE_USE_CRYPTO_LEGACY})
if(ESCORE_USE_CRYPTO_DEBUG_TRACE)
  set(ES_USE_CRYPTODES_DEBUG_TRACE ${ESCORE_USE_CRYPTO_DEBUG_TRACE})
endif()

set(ES_USE_REGEX ${ESCORE_USE_REGEX})
if(ESCORE_USE_REGEX)
  if(ESCORE_USE_REGEX_FLAVOUR STREQUAL STD)
    set(ES_USE_REGEX_STD 1)
  elseif(ESCORE_USE_REGEX_FLAVOUR STREQUAL PCRE2_EMBEDDED)
    set(ES_USE_REGEX_PCRE 1)
  endif()
endif()

set(BOOST_LIB_DIAGNOSTIC ${ESCORE_BOOST_LIB_DIAGNOSTICS})
set(BOOST_DISABLE_THREADS ${ESCORE_BOOST_DISABLE_THREADS})

set(ES_USE_TRACE_BASEINTF_REFCNT ${ESCORE_TRACE_BASEINTF_REFCNT})
set(ES_USE_STREAM_TRACE ${ESCORE_TRACE_STREAM})
set(ES_USE_REFLECTED_CALL_TRACE ${ESCORE_TRACE_REFLECTED_CALLS})

# Set-up ESCORE binary version naming
set(ESCORE_VERSION_SUFFIX "${ESFWX_VERSION_SUFFIX}_${component_BUILD_NUM}" CACHE INTERNAL "")
