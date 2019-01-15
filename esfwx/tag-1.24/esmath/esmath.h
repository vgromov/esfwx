#ifndef _es_math_h_
#define _es_math_h_

#ifndef _es_core_h_
#  error "escore.h must be included prior to this header!"
#endif

// math export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#  ifdef ESMATH_EXPORTS
#    define ESMATH_CLASS              ES_EXPORT_SPEC
#    define ESMATH_FUNC(type, decl)  ES_FUNCEXPORT_SPEC(type, decl)
#    define ESMATH_DATA(type, decl)   ES_DATAEXPORT_SPEC(type, decl)
#    define ESMATH_PKG                PACKAGE
#  else
#    define ESMATH_CLASS              ES_IMPORT_SPEC
#    define ESMATH_FUNC(type, decl)   ES_FUNCIMPORT_SPEC(type, decl)
#    define ESMATH_DATA(type, decl)   ES_DATAIMPORT_SPEC(type, decl)
#    define ESMATH_PKG                PACKAGE
#  endif
#else
#  define ESMATH_CLASS
#  define ESMATH_FUNC(type, decl)     type decl
#  define ESMATH_DATA(type, decl)     type decl
#  define ESMATH_PKG                  PACKAGE
#endif

#include <esmath/EsMathException.h>
#include <esmath/EsMathUtils.h>
#include <esmath/EsMathArrayReal.h>
#include <esmath/EsMathArrayComplex.h>
#include <esmath/EsMathArrayView.h>
#include <esmath/EsMathSpline.h>
#include <esmath/EsMathFft.h>
#include <esmath/EsMathConvolutionReal.h>
#include <esmath/EsMathFitConstraint.h>
#include <esmath/EsMathPolyFit.h>
#include <esmath/EsMathSplineFit.h>
#include <esmath/EsMathDelayLine.h>
#include <esmath/EsMathWindow.h>

ES_DECL_REFLECTION_REGISTRAR(ESMATH_FUNC, esmath)

#endif // _es_math_h_
