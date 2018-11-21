#ifndef _es_core_config_h_
#define _es_core_config_h_

/// @file EsCoreConfig_sample.h
/// Ekosf Framework components default configuration sample file. Its contents should be copied 
/// to the real framework configuration file, EsCoreConfig.h, which should be available anywhere on
/// the project search paths.
///

/// Use localization engine, based on gettext
#define ES_I18N
#define ES_I18N_RARE_LANGS

/// Use embarcadero zlib implementation from crtl
#define ES_USE_ECC_ZLIB

/// Use script compiler functionality
/// if 0 or not defined, compiler and scriptlet components will not be
/// included in target buid. but, there still will be ability to load
/// and execute pre-compiled script binaries
#define ES_USE_COMPILER_IMPLEMENTATION 1

/// Misc defines
///
// Use diagnotic linker messages when linking boost libraries to us
#define BOOST_LIB_DIAGNOSTIC
// Disable boost multi-thread support
#define BOOST_DISABLE_THREADS

/// baseintf-impl objects refcount change trace
//#define ES_USE_TRACE_BASEINTF_REFCNT

#endif // _es_core_config_h_

