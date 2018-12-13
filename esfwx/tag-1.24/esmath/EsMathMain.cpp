#include "esmathpch.h"
#pragma hdrstop

#include "EsMathInternals.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

// ensure complex size is the same
ES_COMPILE_TIME_ASSERT(sizeof(EsMath::Complex)==sizeof(alglib::complex), sizeofEsMathComplex);
//---------------------------------------------------------------------------

ES_REFLECTION_REGISTRAR_BEGIN(esmath)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMath)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathArrayReal)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathArrayComplex)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathFft)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathConvolutionReal)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsMathConvolutionFlag)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsMathFitInfo)
  ES_REFLECTION_REGISTRAR_ENTRY(EsReflection::EsMathFitConstraintKind)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathPolyFit)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathSpline)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathSplineFit)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathDelayLine)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathDelayLineEx)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindow)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowBartlettHann)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowBlackman)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowBlackmanHarris)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowFlatTop)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowGaussian)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowGaussian)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowHamming)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowHann)
  ES_REFLECTION_REGISTRAR_ENTRY(EsMathWindowNuttall)
ES_REFLECTION_REGISTRAR_END
//---------------------------------------------------------------------------

