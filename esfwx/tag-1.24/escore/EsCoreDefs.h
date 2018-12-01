#ifndef _es_core_defs_h_
#define _es_core_defs_h_

/// @file EsCoreDefs.h
///  common, most basic, esfwx framework definitions go here
///

/// Configuration definitions must go first
#include <EsCoreConfig.h>

#if defined(DEBUG) || defined(_DEBUG)
# ifndef _DEBUG
#   define _DEBUG 1
# endif
# ifndef DEBUG
#   define DEBUG 1
# endif
# undef NDEBUG
# define ES_DEBUG
#endif

/// Locale-specific stuff
/// Use our embedded locale implementation by default
///
#if !defined(ES_USE_EMBEDDED_LOCALE_IMPL)
# define ES_USE_EMBEDDED_LOCALE_IMPL 1
#endif

/// OS - specific macros
///
/// Posix compatible OS flag
/// ES_POSIX_COMPAT
/// Supported OSes
#define ES_OS_UNDEFINED      0
#define ES_OS_WINDOWS        1
#define ES_OS_MAC           2
#define ES_OS_IOS           3
#define ES_OS_ANDROID        4
#define ES_OS_LINUX         5
#define ES_OS_UNIX          6

#if defined(__WINDOWS__) || defined(__WIN32__) || _WIN32 || _WIN64
#  define ES_OS              ES_OS_WINDOWS
#elif defined(__APPLE__)
# include "TargetConditionals.h"
# if defined(TARGET_OS_IPHONE) || defined(TARGET_IPHONE_SIMULATOR)
#   define ES_OS            ES_OS_IOS
#   define ES_IS_MOBILE_OS
# else
#   define ES_OS            ES_OS_MAC
# endif
# define ES_POSIX_COMPAT
#elif defined(__ANDROID__)
#  define ES_OS              ES_OS_ANDROID
# define ES_IS_MOBILE_OS
# define ES_POSIX_COMPAT
#elif defined(linux) || defined(__LINUX__) || defined(__linux__) || defined(__linux)
# define ES_OS              ES_OS_LINUX
# define ES_POSIX_COMPAT
#elif defined(unix) || defined(__UNIX__) || defined(__unix__) || defined(__unix)
# define ES_OS              ES_OS_UNIX
# define ES_POSIX_COMPAT
#else
#  define ES_OS              ES_OS_UNDEFINED
#endif

#if ES_OS == ES_OS_UNDEFINED
# error OS is not defined!
#endif

// compiler vendor specifics
//
#define ES_COMPILER_VENDOR_UNDEFINED    0
#define ES_COMPILER_VENDOR_BORLAND      1
#define ES_COMPILER_VENDOR_MS            2
#define ES_COMPILER_VENDOR_INTEL        3
#define ES_COMPILER_VENDOR_GNUC          4
#define ES_COMPILER_VENDOR_DIGITALMARS  5

#if defined(__BORLANDC__)
#  define ES_COMPILER_VENDOR    ES_COMPILER_VENDOR_BORLAND
#elif defined(_MSC_VER)
#  define ES_COMPILER_VENDOR    ES_COMPILER_VENDOR_MS
#elif defined(__INTEL_COMPILER)
#  define ES_COMPILER_VENDOR    ES_COMPILER_VENDOR_INTEL
#elif defined(__GNUC__)
#  define ES_COMPILER_VENDOR    ES_COMPILER_VENDOR_GNUC
#elif defined(__DMC__)
#  define ES_COMPILER_VENDOR    ES_COMPILER_VENDOR_DIGITALMARS
#else
#  define ES_COMPILER_VENDOR    ES_COMPILER_VENDOR_UNDEFINED
#endif

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_UNDEFINED
#  error Compiler vendor is not defined!
#endif

#ifdef __clang__
# define ES_CLANG
#endif

#if (__cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201103L)
# define ES_MODERN_CPP
# define ES_HAVE_NOEXCEPT
#endif

#if ES_COMPILER_VENDOR_MS == ES_COMPILER_VENDOR
// In MSVC we have almost anything needed for modern constructs used in our code since VC2013
# if !defined(ES_MODERN_CPP) && _MSC_VER >= 1800
#   define ES_MODERN_CPP
// In MSVC noexcept is supported since VC2015
#   if !defined(ES_HAVE_NOEXCEPT) && _MSC_VER >= 1900
#     define ES_HAVE_NOEXCEPT
#   endif
# endif
#endif

#if !defined(ES_MODERN_CPP) && defined(ES_CLANG)
# define ES_MODERN_CPP
# define ES_HAVE_NOEXCEPT
#endif

/// Export-import specifiers
///
#if defined(ECC_PACKAGE) || defined(ES_USE_DLLS)
# define ES_DYNAMIC_LINKAGE
#endif

// ES_DYNAMIC_LINKAGE override for static package linkage under Borland toolchain
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND && defined(ECC_PACKAGE)
# ifdef ES_DYNAMIC_LINKAGE
#   ifndef USEPACKAGES
#     undef ES_DYNAMIC_LINKAGE
#     undef ES_USE_DLLS
#   endif
# endif
#endif

// Export-import specs adaptation for toolchains|platforms
#ifdef ES_DYNAMIC_LINKAGE
# if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#   if !defined(ES_CLANG) && defined(ECC_PACKAGE)
#     pragma warn -8118
#     define ES_EXPORT_SPEC                  PACKAGE
#     define ES_IMPORT_SPEC                  PACKAGE
#     define ES_FUNCEXPORT_SPEC(type, decl)  type ES_EXPORT_SPEC decl
#     define ES_FUNCIMPORT_SPEC(type, decl)  type ES_IMPORT_SPEC decl
#     define ES_DATAEXPORT_SPEC(type, decl)  type ES_EXPORT_SPEC decl
#     define ES_DATAIMPORT_SPEC(type, decl)  type ES_IMPORT_SPEC decl
#   else
#     if __CODEGEARC__ < 0x0700
#       define ES_EXPORT_SPEC                 __export
#       define ES_IMPORT_SPEC                 __import
#       define ES_FUNCEXPORT_SPEC(type, decl) type ES_EXPORT_SPEC decl
#       define ES_FUNCIMPORT_SPEC(type, decl) type ES_IMPORT_SPEC decl
#       define ES_DATAEXPORT_SPEC(type, decl) type ES_EXPORT_SPEC decl
#       define ES_DATAIMPORT_SPEC(type, decl) type ES_IMPORT_SPEC decl
#     elif __CODEGEARC__ < 0x0710
        // XE8
#       define ES_EXPORT_SPEC                 __declspec(dllexport)
#       define ES_IMPORT_SPEC                 __declspec(dllimport)
#       define ES_FUNCEXPORT_SPEC(type, decl) ES_EXPORT_SPEC type decl
#       define ES_FUNCIMPORT_SPEC(type, decl) ES_IMPORT_SPEC type decl
#       define ES_DATAEXPORT_SPEC(type, decl) ES_EXPORT_SPEC type decl
#       define ES_DATAIMPORT_SPEC(type, decl) ES_IMPORT_SPEC type decl
#     elif __CODEGEARC__ < 0x0740
        // XE9, XE10, XE11
#       define ES_EXPORT_SPEC                 __declspec(dllexport)
#       define ES_IMPORT_SPEC                 __declspec(dllimport)
#       define ES_FUNCEXPORT_SPEC(type, decl) type ES_EXPORT_SPEC decl
#       define ES_FUNCIMPORT_SPEC(type, decl) type ES_IMPORT_SPEC decl
#       define ES_DATAEXPORT_SPEC(type, decl) type ES_EXPORT_SPEC decl
#       define ES_DATAIMPORT_SPEC(type, decl) type ES_IMPORT_SPEC decl
#     endif
#   endif
# elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC
#   if ES_OS == ES_OS_WINDOWS
#     define ES_EXPORT_SPEC                   __attribute__((dllexport))
#     define ES_IMPORT_SPEC                   __attribute__((dllimport))
#   else
#     define ES_EXPORT_SPEC                   __attribute__((visibility("default")))
#     define ES_IMPORT_SPEC                   __attribute__((visibility("default")))
#   endif
#   define ES_FUNCEXPORT_SPEC(type, decl)   type decl ES_EXPORT_SPEC
#   define ES_FUNCIMPORT_SPEC(type, decl)   type decl ES_IMPORT_SPEC
#   define ES_DATAEXPORT_SPEC(type, decl)   type decl ES_EXPORT_SPEC
#   define ES_DATAIMPORT_SPEC(type, decl)   type decl ES_IMPORT_SPEC
# else // Not using borland toolchain - prefer platform specs
#   define ES_EXPORT_SPEC                     __declspec(dllexport)
#   define ES_IMPORT_SPEC                     __declspec(dllimport)
#   define ES_FUNCEXPORT_SPEC(type, decl)     ES_EXPORT_SPEC type decl
#   define ES_FUNCIMPORT_SPEC(type, decl)     ES_IMPORT_SPEC type decl
#   define ES_DATAEXPORT_SPEC(type, decl)     ES_EXPORT_SPEC type decl
#   define ES_DATAIMPORT_SPEC(type, decl)     ES_IMPORT_SPEC type decl
# endif
#else
# define ES_EXPORT_SPEC
# define ES_IMPORT_SPEC
# define ES_FUNCEXPORT_SPEC(type, decl)       type decl
# define ES_FUNCIMPORT_SPEC(type, decl)       type decl
# define ES_DATAEXPORT_SPEC(type, decl)       type decl
# define ES_DATAIMPORT_SPEC(type, decl)       type decl
#endif

// endianness detection
#include "EsCoreEndiannessDefs.h"

// compiler-specific defs and tune-ups
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// disable W8024 warning which occurs in many places due to our multiple interface inheritance technique
# pragma warn -8024
// Do not use annotations for modern MS code (MS SAL is not supported here)
#   if ES_OS == ES_OS_WINDOWS
#     include <no_sal2.h>
#   endif
#elif ES_COMPILER_VENDOR ==  ES_COMPILER_VENDOR_MS
// Disable C4191 warning for reflected class static method pointer types typecasts
#  pragma warning( once: 4191 )
// Disable C4584 warning which occurs in many places due to our multiple interface inheritance technique
#  pragma warning( disable: 4584 )
// Disable DLL warnings for inline objects && data
#  pragma warning( disable: 4251 )
# ifndef _CRT_SECURE_NO_WARNINGS
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#elif ES_COMPILER_VENDOR == ESCOMPILER_VENDOR_GNUC
// Disable inaccessible direct base for multiple inheritance classes
# pragma GCC diagnostic ignored "-W"
#endif

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC
# define ES_ABSTRACT
#  define ES_NO_INLINE __attribute__((noinline))
#elif (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND)
#  define ES_ABSTRACT  __declspec(novtable)
#  define ES_NO_INLINE
#elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#  define ES_ABSTRACT  __declspec(novtable)
#  define ES_NO_INLINE __declspec(noinline)
#else
#  define ES_NO_INLINE
#  define ES_ABSTRACT
#endif

/// Always use unicode build
///
#ifndef _UNICODE
#  define _UNICODE
#endif
#ifndef UNICODE
#  define UNICODE
#endif

/// Standard C includes,
/// OS-specific includes,
/// standard OS includes
///
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# ifdef __cplusplus
#   include <System.hpp>
#   include <System.Types.hpp>
# endif
#endif

#if ES_OS == ES_OS_WINDOWS
# include <tchar.h>
#  include <stdlib.h>
#  include <stdio.h>

// Prevent winsock.h to be included from windows.h, we use winsock2.h internally in comm, no need to spoil defs and structs there
# define _WINSOCKAPI_
#  include <windows.h>
  // Undef wingdi's GetObject macro, to prevent clash with FMX headers GetObject object member service(s)
# undef GetObject
#elif defined(ES_POSIX_COMPAT)
# if (ES_OS == ES_OS_MAC || ES_OS == ES_OS_IOS) && defined(__cplusplus)
#   include <CoreFoundation/CoreFoundation.h>
# elif ES_OS == ES_OS_ANDROID
#   ifdef __cplusplus
#     if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#       include <Androidapi.JNI.JavaTypes.hpp>
        typedef Androidapi::Jni::Javatypes::_di_JString JStringPtr;
#     else
#       include <j2cpp/j2cpp.hpp>
#     endif
#   endif // __cplusplus
# endif
# if ESCOMPILER_VENDOR_GCC == ES_COMPILER_VENDOR
#   define _BSD_SOURCE //< Ensure we wiil gettimeofday in time.h
# endif
#  include <unistd.h>
#  include <stdlib.h>
# include <stdio.h>
# include <errno.h>
#  include <time.h>
#  include <pthread.h>
#  include <semaphore.h>
#  if ES_OS == ES_OS_MAC
#       define ES_PMUTEX_NO_ROBUST
#    define ES_PTHREAD_NO_TIMEDLOCK
#    define ES_PTHREAD_NO_PRIO
#       define ES_PTHREAD_NO_YIELD
#   elif ES_OS == ES_OS_ANDROID
#       define ES_PMUTEX_NO_ROBUST
#    define ES_PTHREAD_NO_TIMEDLOCK
#    define ES_PTHREAD_NO_PRIO
#       define ES_PTHREAD_NO_YIELD
#  endif
# if ES_OS == ES_OS_ANDROID && ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// undefine some macros(ed) errno-s, as soon as
// these are re-declared as consts in ECC system headers
#   ifdef ENOTSUP
#     undef ENOTSUP
#   endif
# endif
# ifndef SEM_VALUE_MAX
#   define SEM_VALUE_MAX 0x3FFFFFFF
# endif
#endif

// stringizer and concatenation helper macros
//
#define ES_CONCAT_HELPER(x1, x2) x1 ## x2

#define ES_CONCAT(x1, x2) \
  ES_CONCAT_HELPER(x1, x2)

#define ES_CONCAT3(x1, x2, x3) \
  ES_CONCAT(ES_CONCAT(x1, x2), x3)

#define ES_CONCAT4(x1, x2, x3, x4) \
  ES_CONCAT(ES_CONCAT3(x1, x2, x3), x4)

#define ES_CONCAT5(x1, x2, x3, x4, x5) \
  ES_CONCAT(ES_CONCAT4(x1, x2, x3, x4), x5)

#define ES_CONCAT6(x1, x2, x3, x4, x5, x6) \
  ES_CONCAT(ES_CONCAT5(x1, x2, x3, x4, x5), x6)

#define ES_CONCAT7(x1, x2, x3, x4, x5, x6, x7) \
  ES_CONCAT(ES_CONCAT6(x1, x2, x3, x4, x5, x6), x7)

#define ES_CONCAT8(x1, x2, x3, x4, x5, x6, x7, x8) \
  ES_CONCAT(ES_CONCAT7(x1, x2, x3, x4, x5, x6, x7), x8)

#define ES_CONCAT9(x1, x2, x3, x4, x5, x6, x7, x8, x9) \
  ES_CONCAT(ES_CONCAT8(x1, x2, x3, x4, x5, x6, x7, x8), x9)

// Chars, Strings confiduration & defines
#include "EsStringDefs.h"

// Other RTL configuration & defines
#include "EsCoreRtlDefs.h"

// Misc defines
//
#if defined(ES_MODERN_CPP) && defined(ES_HAVE_NOEXCEPT)
# define ES_NOTHROW    noexcept
#else
# define ES_NOTHROW    throw()
#endif

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC
# define ES_UNUSED(x) x __attribute__((unused))
#elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
# define ES_UNUSED(x) __pragma(warning(suppress: 4100)) x
#else
# define ES_UNUSED(x) x
#endif

// Non-copyable class
//
#ifdef ES_MODERN_CPP
# define ES_REMOVEDECL = delete
# define ES_OVERRIDE override
#else
# define ES_REMOVEDECL
# define ES_OVERRIDE
# define nullptr NULL
#endif

# define ES_NON_COPYABLE(ClassName) \
private:\
  ClassName(const ClassName&) REMOVEDECL; \
  ClassName& operator=(const ClassName&) REMOVEDECL

#ifdef __cplusplus
// pointer deletion with cleanup
template <typename T>
inline void ES_DELETE(T*& ptr)
{
  ES_UNUSED(typedef char TypeIsCompleteCheck[sizeof(T)]);

  if( nullptr != ptr )
  {
    delete ptr;
    ptr = nullptr;
  }
}
#endif

// Deprecated features macro
//
#ifndef ES_DEPRECATED
#  if defined(__GNUC__)
#    define ES_DEPRECATED __attribute__((deprecated))
#  elif defined(_MSC_VER) && _MSC_VER >= 1300
#    define ES_DEPRECATED __declspec(deprecated)
#  else
#    define ES_DEPRECATED
#  endif
#endif

// Debugging stuff
//
// Assertion macros
//
#ifndef NDEBUG
#  include <assert.h>
#  define ES_ASSERT( x )    assert( x )
# define ES_FAIL          ES_ASSERT( 0 )
#  define ES_FAIL_MSG( x )  ES_ASSERT( 0 == (x) )
#else
#  define ES_ASSERT( x )
# define ES_FAIL          abort()
#  define ES_FAIL_MSG( x ) abort()
#endif

#ifdef __cplusplus
# ifdef ES_MODERN_CPP
#   define ES_COMPILE_TIME_ASSERT(expr, msg) static_assert(expr, ES_STRINGIZE_HELPER( ES_CONCAT4(COMPILE_TIME_ASSERTION_, __LINE__, _, msg) ) )
# else
    // Compile-time assertions
    namespace _es_debug_ns_
    {
      template<bool b> struct EsCompileTimeAssert {};
      template<> struct EsCompileTimeAssert<true> { typedef void (* triggerT)(void); };
    }
#  define ES_COMPILE_TIME_ASSERT(expr, msg) typedef _es_debug_ns_::EsCompileTimeAssert< (bool)(expr) >::triggerT ES_CONCAT4(COMPILE_TIME_ASSERTION_, __LINE__, _, msg)
# endif
#endif

// Debugging trace stuff
//
#ifdef ES_DEBUG
# if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS && defined(__cplusplus)
#   define ES_DEBUG_TRACE_INIT      do{ _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG); _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_WNDW); } while(0)
#   define ES_DEBUG_TRACE(...)      OutputDebugString(EsString(EsString::format(__VA_ARGS__) + esT("\n")).c_str())
#   define ES_DEBUG_TRACE_RAW(...)  do{ char _tmpdbg[256]; snprintf(_tmpdbg, 255, __VA_ARGS__); _tmpdbg[255] = 0; OutputDebugStringA(_tmpdbg); } while(0)
#   define USE_MEMLEAK_DETECTION  0
# elif (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND || ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_GNUC) && ES_OS == ES_OS_WINDOWS
#    define ES_DEBUG_TRACE_INIT     ((void)0)
#    define ES_DEBUG_TRACE(...)      OutputDebugString(EsString::format(__VA_ARGS__).c_str())
#    define ES_DEBUG_TRACE_RAW(...) do{ char _tmpdbg[256]; snprintf(_tmpdbg, 255, __VA_ARGS__); _tmpdbg[255] = 0; OutputDebugStringA(_tmpdbg); } while(0)
#    define USE_MEMLEAK_DETECTION   0
# else
#   define ES_DEBUG_TRACE_INIT      ((void)0)
#   define ES_DEBUG_TRACE(...)      ((void)0)
#   define ES_DEBUG_TRACE_RAW(...)  ((void)0)
#   define USE_MEMLEAK_DETECTION    0
# endif
#else
# define ES_DEBUG_TRACE_INIT        ((void)0)
# define ES_DEBUG_TRACE(...)        ((void)0)
# define ES_DEBUG_TRACE_RAW(...)    ((void)0)
# define USE_MEMLEAK_DETECTION      0
#endif

// Use advanced memleak detection
#if defined(ES_DEBUG) && !defined(USE_MEMLEAK_DETECTION)
# define USE_MEMLEAK_DETECTION      1
#endif

// Memory leak detection
#if defined(ES_DEBUG) && (1 == USE_MEMLEAK_DETECTION) && (ES_OS == ES_OS_WINDOWS)
# define _CRTDBG_MAP_ALLOC
# include <stdlib.h>
# include <crtdbg.h>
#endif

#define ES_MACRO_TO_STR( m ) ES_STRINGIZE_HELPER( m )

#endif // _es_core_defs_h_
