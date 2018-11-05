#ifndef _es_math_fft_h_
#define _es_math_fft_h_

ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN( EsMathFft, ESMATH_CLASS )
  /// Reflected services
  ///

  /// Real FFT transformation
  /// @param p0 [in]  input variant with real array values
  /// @return         variant with EsMathArrayComplex object instance
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, transformReal, cr_EsVariant);

  /// Non-reflected services
  ///
  static void transform(const EsMathArrayReal& in, EsMathArrayComplex& out);

ES_DECL_REFLECTED_SERVICES_END

#endif // _es_math_fft_h_

