#ifndef _es_core_rtl_h_
#define _es_core_rtl_h_

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND

#ifdef __cplusplus
  extern "C" {
#endif

ESCORE_FUNC( int,     es_finite(double d) );
ESCORE_FUNC( int,     es_isnan(double d)  );
ESCORE_FUNC( int,     es_finitef(float f) );
ESCORE_FUNC( int,     es_isnanf(float f)  );

ESCORE_FUNC( double,  es_abs(double f)    );
ESCORE_FUNC( float,   es_ceilf(float f)   );
ESCORE_FUNC( double,  es_ceil(double d)   );
ESCORE_FUNC( float,   es_logf(float f)    );
ESCORE_FUNC( double,  es_log(double d)    );
ESCORE_FUNC( float,   es_log10f(float f)  );
ESCORE_FUNC( double,  es_log10(double d)  );
ESCORE_FUNC( double,  es_log2(double x)   );
ESCORE_FUNC( double,  es_logN(double n, double d) );
ESCORE_FUNC( float,   es_powf(float x, float n)   );
ESCORE_FUNC( double,  es_pow(double x, double n)  );
ESCORE_FUNC( double,  es_exp(double n)    );
ESCORE_FUNC( double,  es_sqrt(double x)   );
ESCORE_FUNC( double,  es_modf(double x, double* ipart)  );
ESCORE_FUNC( float ,  es_modff(float x, float* intpart) );

ESCORE_FUNC( double,  es_sin(double rad)  );
ESCORE_FUNC( double,  es_sinh(double val) );
ESCORE_FUNC( double,  es_asin(double val) );
ESCORE_FUNC( double,  es_cos(double rad)  );
ESCORE_FUNC( double,  es_cosh(double val) );
ESCORE_FUNC( double,  es_acos(double val) );
ESCORE_FUNC( double,  es_tan(double val)  );
ESCORE_FUNC( double,  es_tanh(double val) );
ESCORE_FUNC( double,  es_atan(double val) );
ESCORE_FUNC( double,  es_atan_yx(double y, double x)   );

#ifdef __cplusplus
  }
#endif

#else

#ifdef __cplusplus
  extern "C" {
#endif

ESCORE_FUNC( double, es_logN(double n, double d) );

#ifdef __cplusplus
  }
#endif

#endif

#if (ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND) && \
    defined(ES_WCHAR_IS_NOT_WCHAR_T)

#ifdef __cplusplus
  extern "C" {
#endif

ESCORE_FUNC( size_t, es_strlen(ES_CTSTR s) );
ESCORE_FUNC( int, es_strcmp(ES_CTSTR s1, ES_CTSTR s2)       );
ESCORE_FUNC( int, es_stricmp(ES_CTSTR s1, ES_CTSTR s2)      );
ESCORE_FUNC( int, es_strncmp(ES_CTSTR s1, ES_CTSTR s2, size_t n) );
ESCORE_FUNC( ES_CTSTR, es_strchr(ES_CTSTR s, ES_CHAR c)     );
ESCORE_FUNC( ES_CTSTR, es_strstr(ES_CTSTR s1, ES_CTSTR s2)  );
ESCORE_FUNC( size_t, es_strftime(ES_CHAR* buff, size_t nsize, ES_CTSTR format, const struct tm *timeptr) );

#ifdef __cplusplus
  }
#endif

#endif

// Special short-cut for exception throwing, before actual exception class is declared
ESCORE_FUNC( void, EsExceptionThrow(ES_CTSTR error) );

#endif // _es_core_rtl_h_
