#ifndef _es_math_utils_h_
#define _es_math_utils_h_

ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN( EsMath, ESMATH_CLASS )

  /// Reflected math constants
  ///

  /// PI
  ES_DECL_REFLECTED_CLASS_METHOD0(double, pi);

  /// 2*PI
  ES_DECL_REFLECTED_CLASS_METHOD0(double, _2pi);

  /// PI/2
  ES_DECL_REFLECTED_CLASS_METHOD0(double, pi_2);

  /// PI/180
  ES_DECL_REFLECTED_CLASS_METHOD0(double, pi_180);

  /// e
  ES_DECL_REFLECTED_CLASS_METHOD0(double, e);

  /// Reflected validity check functions
  ///

  /// Is infinite float check
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isInf, double);

  /// Is NAN float check
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isNaN, double);

  /// Comparison
  ///

  /// Double internals-aware equality comparison. Comparison accuracy used is 1 ULPs
  /// (Units in the Last Place)
  ///
  ES_DECL_REFLECTED_CLASS_METHOD2(bool, areEqualFloats, double, double);

  /// Double internals-aware equality comparison, an additional parameter
  /// is comparison accuracy, in ULPs (Units in the Last Place)
  ///
  ES_DECL_REFLECTED_CLASS_METHOD3(bool, areEqualFloats, double, double, ulong);

  /// Reflected standard math functions
  ///

  /// Effective 10^n calculation
  ES_DECL_REFLECTED_CLASS_METHOD1(double, pow10, long);

  /// Effective 2^n calculation
  ES_DECL_REFLECTED_CLASS_METHOD1(double, pow2, long);

  /// n^m calculation powNM(n, m)
  ES_DECL_REFLECTED_CLASS_METHOD2(double, powNM, double, double);

  /// e^m calculation exp(m)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, exp, double);

  /// Square root sqrt(val)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, sqrt, double);

  /// Round the floating number, using the number of the floating point digits
  /// after comma. Note that the function can yield imprecise result.
  /// If numDecimalPlaces is zero, the behavior is equivalent to Round0.
  ///
  /// Here is the usage:
  ///       double x = EsMath::round(0.555, 1);    // 0.6
  ///       double y = EsMath::round(1.234567, 2); // 1.23
  ///
  ES_DECL_REFLECTED_CLASS_METHOD2(double, round, double, ulong);

  /// Same as above, but use 0 as decimal places count, effectively rounding to interal
  ES_DECL_REFLECTED_CLASS_METHOD1(double, round, double);

  /// Convert deg to rad and vice versa
  ES_DECL_REFLECTED_CLASS_METHOD1(double, degToRad, double);
  ES_DECL_REFLECTED_CLASS_METHOD1(double, radToDeg, double);

  /// Split floating point argument into fractional and integer parts.
  /// Returned is [int, frac]
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, modf, double);

  /// Absolute value of argument
  ES_DECL_REFLECTED_CLASS_METHOD1(double, abs, double);

  /// Natural logarithm
  ES_DECL_REFLECTED_CLASS_METHOD1(double, log, double);

  /// 2-base logarithm
  ES_DECL_REFLECTED_CLASS_METHOD1(double, log2, double);

  /// 10-base logarithm
  ES_DECL_REFLECTED_CLASS_METHOD1(double, log10, double);

  /// N-base logarithm logN(base, val)
  ES_DECL_REFLECTED_CLASS_METHOD2(double, logN, double, double);

  /// Sine sin(rad)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, sin, double);

  /// Hyperbolic sine sinh(rad)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, sinh, double);

  /// Arc sine asin(val)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, asin, double);

  /// Cosine cos(rad)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, cos, double);

  /// Hyperbolic cosine cosh(rad)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, cosh, double);

  /// Arc cosine acos(val)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, acos, double);

  /// Tangent tan(rad)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, tan, double);

  /// Hyperbolical tangent tanh(rad)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, tanh, double);

  /// Arc tangent atan(val)
  ES_DECL_REFLECTED_CLASS_METHOD1(double, atan, double);

  /// Arc tangent of Y/X atanYX(y, x)
  ES_DECL_REFLECTED_CLASS_METHOD2(double, atanYX, double, double);

  /// GEO-specific math
  ///

  /// Distance estimate in meters between lat1,lon1 .. lat2,lon2
  ES_DECL_REFLECTED_CLASS_METHOD4(double, geoDistanceEstimate, double, double, double, double);

  /// Fast distance estimate in meters between lat1,lon1 .. lat2,lon2
  ES_DECL_REFLECTED_CLASS_METHOD4(double, geoDistanceEstimateFast, double, double, double, double);

  /// Bearing between two points, in degrees
  ES_DECL_REFLECTED_CLASS_METHOD4(double, geoBearing, double, double, double, double);

  /// Calculate the first derivative of an input vector|numeric collection
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, diff, cr_EsVariant);

  /// Non-reflected stuff
  ///

  /// Complex value type
  ///
  struct Complex
  {
    Complex(double re = 0, double im = 0) :
    m_re(re),
    m_im(im)
    {}

    double m_re;
    double m_im;
  };

  /// Directly accessible math constants
  ///
  static const double c_pi;
  static const double c_2pi;
  static const double c_pi_2;
  static const double c_pi_180;
  static const double c_e;

ES_DECL_REFLECTED_SERVICES_END

#endif // _es_math_utils_h_
