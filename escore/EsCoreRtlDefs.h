#ifndef _es_rtl_defs_h_
#define _es_rtl_defs_h_

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#  define esFiniteF                    es_finitef
#  define esIsInfF(x)                  (!es_finitef(x))
#  define esIsNanF                      es_isnanf
#  define esFinite                      es_finite
#  define esIsInf(x)                    (!es_finite(x))
#  define esIsNan                      es_isnan
# define esCeil                       es_ceil
# define esLog                        es_log
# define esLog10                      es_log10
# define esLog2                       es_log2
# define esLogN                       es_logN
# define esPow                        es_pow
# define esExp                        es_exp
# define esSqrt                       es_sqrt
# define esModf                       es_modf
# define esAbs                        es_abs
# define esSin                        es_sin
# define esSinh                       es_sinh
# define esAsin                       es_asin
# define esCos                        es_cos
# define esCosh                       es_cosh
# define esAcos                       es_acos
# define esTan                        es_tan
# define esTanh                       es_tanh
# define esAtan                       es_atan
# define esAtanYX                     es_atan_yx
#else
#  ifdef __cplusplus
#   include <cmath>
#    include <cfloat>
#    define esFiniteF(f)               std::isfinite(static_cast<float>(f))
#    define esIsInfF(f)                std::isinf(static_cast<float>(f))
#    define esIsInf(d)                 std::isinf(static_cast<double>(d))
#    define esIsNanF(f)                std::isnan(static_cast<float>(f))
#    define esIsNan(d)                  std::isnan(static_cast<double>(d))
#   define esCeil                     std::ceil
#   define esLog                      std::log
#   define esLog10                    std::log10
#   define esLog2                     std::log2
#   define esPow                      std::pow
#   define esExp                      std::exp
#   define esSqrt                     std::sqrt
#   define esModf                     std::modf
#   define esAbs                      std::fabs
#   define esSin                      std::sin
#   define esSinh                     std::sinh
#   define esAsin                     std::asin
#   define esCos                      std::cos
#   define esCosh                     std::cosh
#   define esAcos                     std::acos
#   define esTan                      std::tan
#   define esTanh                     std::tanh
#   define esAtan                     std::atan
#   define esAtanYX                   std::atan2
# else
#   include <math.h>
#    include <float.h>
#   define esFiniteF                  _finite
#   define esIsInfF(x)                (0 == _finite(x))
#   ifdef isnan
#     define esIsNanF                  isnan
#   else
#     define esIsNanF                  _isnanf
#   endif
#   define esIsNan                    _isnan
#   define esCeil                     ceil
#   define esLog                      log
#   define esLog10                    log10
#   define esLog2                     log2
#   define esPow                      pow
#   define esExp                      exp
#   define esSqrt                     sqrt
#   define esModf                     modf
#   define esAbs                      fabs
#   define esSin                      sin
#   define esSinh                     sinh
#   define esAsin                     asin
#   define esCos                      cos
#   define esCosh                     cosh
#   define esAcos                     acos
#   define esTan                      tan
#   define esTanh                     tanh
#   define esAtan                     atan
#   define esAtanYX                   atan2
# endif
# define esLogN                       es_logN
#endif

// Host integers to BE integers conversion routines,
// taking host endianness into account
#if ES_ENDIAN != ES_BIG_ENDIAN
# define esHostToBe2(h)               EsUtilities::inverse2(h)
# define esHostToBe4(h)               EsUtilities::inverse4(h)
# define esHostToBe8(h)               EsUtilities::inverse8(h)
# define esBeToHost2(b)               EsUtilities::inverse2(b)
# define esBeToHost4(b)               EsUtilities::inverse4(b)
# define esBeToHost8(b)               EsUtilities::inverse8(b)
#else
# define esHostToBe2(h)               (h)
# define esHostToBe4(h)               (h)
# define esHostToBe8(h)               (h)
# define esBeToHost2(b)               (b)
# define esBeToHost4(b)               (b)
# define esBeToHost8(b)               (b)
#endif

// redirect min|max to std::min|std::max
#ifdef max
#  undef max
#endif
#ifdef min
# undef min
#endif

#define esMax(a, b)                    std::max((a), (b))
#define esMin(a, b)                    std::min((a), (b))

/// Additional Core types definitions
///
#if (ES_OS_WINDOWS != ES_OS) && !defined(__ANDROID__)
/// GUID data compatibility declaration for non-Windows OSes
struct EsGUID
{
  uint32_t Data1;
  uint16_t Data2;
  uint16_t Data3;
  uint8_t Data4[ 8 ];

  static const EsGUID& null() ES_NOTHROW;
};
typedef EsGUID GUID;

# ifndef GUID_NULL
#   define GUID_NULL EsGUID::null()
# endif

ES_COMPILE_TIME_ASSERT(sizeof(EsGUID) == 16, _EsGUID_size_is_equal_to_16_bytes_);
#endif //

#endif // _es_rtl_defs_h_
