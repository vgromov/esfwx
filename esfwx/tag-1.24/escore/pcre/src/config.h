#ifndef _pcre_2_config_h_
#define _pcre_2_config_h_

// PCRE2 10.23 config.h for escore build
//
#include <escore/EsCoreDefs.h>

//#undef HAVE_DIRENT_H
#define HAVE_INTTYPES_H           1
#define HAVE_STDINT_H             1
#define HAVE_STRERROR             1
#define HAVE_SYS_STAT_H           1
#define HAVE_SYS_TYPES_H          1
//#undef HAVE_UNISTD_H
#ifdef _WIN32
# define HAVE_WINDOWS_H           1
#endif

//#undef HAVE_BCOPY
#define HAVE_MEMMOVE              1

#define PCRE2_STATIC              1

#ifdef ES_USE_WCHAR
# if 2 == ES_WCHAR_SIZE
#   define SUPPORT_PCRE2_16       1
#   define PCRE2_CODE_UNIT_WIDTH  16
# elif 4 == ES_WCHAR_SIZE
#   define SUPPORT_PCRE2_32       1
#   define PCRE2_CODE_UNIT_WIDTH  32
# endif
# else
#   define SUPPORT_PCRE2_8        1
#   define PCRE2_CODE_UNIT_WIDTH  8
#endif
//#undef PCRE2_DEBUG

//#undef SUPPORT_LIBBZ2
//#undef SUPPORT_LIBEDIT
//#undef SUPPORT_LIBREADLINE
//#undef SUPPORT_LIBZ

#define SUPPORT_JIT               1
//#undef SUPPORT_PCRE2GREP_JIT
#define SUPPORT_UNICODE           1
//#undef SUPPORT_VALGRIND

//#undef BSR_ANYCRLF
//#undef EBCDIC
//#undef EBCDIC_NL25
//#undef HEAP_MATCH_RECURSE
//#undef NEVER_BACKSLASH_C

#define LINK_SIZE		              2
#define MATCH_LIMIT		            10000000
#define MATCH_LIMIT_RECURSION    	MATCH_LIMIT
#define NEWLINE_DEFAULT           PCRE2_NEWLINE_ANYCRLF
#define PARENS_NEST_LIMIT         250
#define PCRE2GREP_BUFSIZE         20480
#define PCRE2GREP_MAX_BUFSIZE     1048576

#define MAX_NAME_SIZE	            32
#define MAX_NAME_COUNT	          10000

#endif // _pcre_2_config_h_

