#ifndef _ekosf_math_h_
#define _ekosf_math_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included prior to this header!"
#endif

// communication stuff
//
// core export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef ESMATH_EXPORTS
#		define EKOSF_MATH_CLASS	__declspec(dllexport)
#		define EKOSF_MATH_FUNC	__declspec(dllexport)
#		define EKOSF_MATH_DATA	__declspec(dllexport)
#	else
#		define EKOSF_MATH_CLASS	__declspec(dllimport)
#		define EKOSF_MATH_FUNC	__declspec(dllimport)
#		define EKOSF_MATH_DATA	__declspec(dllimport)
#	endif
#else
#	define EKOSF_MATH_CLASS
#	define EKOSF_MATH_FUNC
#	define EKOSF_MATH_DATA
#endif

#include <valarray>

//ES_DECL_REFLECTION_REGISTRAR(esmath)

enum EsMathFitInfo
{
	EsMathFitInfo_NONE,
	EsMathFitInfo_SUCCESS,
	EsMathFitInfo_INCONVERGENCE_OCCURED,
	EsMathFitInfo_INCONSISTENT_CONSTRAINTS,
	EsMathFitInfo_FAIL
};

typedef std::valarray<double> EsMathFloatArray;
typedef std::valarray<int> EsMathIntArray;

#include <esmath/EsMath.h>
#include <esmath/EsMathSpline.h>
#include <esmath/EsMathFitConstraint.h>
#include <esmath/EsMathPolyFit.h>
#include <esmath/EsMathCubicSplineFit.h>

#endif // _ekosf_math_h_
