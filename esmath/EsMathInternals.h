#ifndef _es_math_internals_h_
#define _es_math_internals_h_

// alglib stuff
#include "alglib/ap.h"

EsMathFitInfo alglibInfoToFitInfo(alglib::ae_int_t info);

// internal array type cast helpers
//
inline alglib::real_1d_array& alglibCastFromArray(EsMathArrayReal& src)
{
  return *(alglib::real_1d_array*)src.pimplGet();
}

inline const alglib::real_1d_array& alglibCastFromArray(const EsMathArrayReal& src)
{
  return *(const alglib::real_1d_array*)src.pimplGet();
}

inline alglib::complex_1d_array& alglibCastFromArray(EsMathArrayComplex& src)
{
  return *(alglib::complex_1d_array*)src.pimplGet();
}

inline const alglib::complex_1d_array& alglibCastFromArray(const EsMathArrayComplex& src)
{
  return *(const alglib::complex_1d_array*)src.pimplGet();
}

#define ES_ALGLIB_TRY \
try {

#define ES_ALGLIB_CATCH \
} catch( const alglib::ap_error& aerr ) { \
  EsMathException::Throw( EsString::fromUtf8(aerr.msg) ); \
}

#endif // _es_math_internals_h_