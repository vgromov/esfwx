#ifndef _es_math_window_h_
#define _es_math_window_h_

/// Window function generator class base
///
class ESMATH_CLASS ES_INTF_IMPL1( EsMathWindow, EsReflectedClassIntf )

protected:
  EsMathWindow();

public:
  /// Reflection declaration
  ///
  ES_DECL_REFLECTED_CLASS_BASE( EsMathWindow )
  ES_DECL_ATTR_HANDLING_STD

  /// Generate window values of N samples, and return it as EsMathRealArray object
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, generate, cr_EsVariant);

  /// Generate normalized window values of N samples, and return it as EsMathRealArray object
  /// Values get normalized to window function integral
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, generateNormalized, cr_EsVariant);

protected:
  ulong cachePrepare(cr_EsVariant cnt) const;
  virtual void doGenerate() const = 0;

protected:
  // Window values cache
  //
  mutable EsMathArrayReal m_aw;
  mutable bool m_cacheInvalid;
};
//---------------------------------------------------------------------------
/// Window classes specialization implementations
///

/// Bartlett-Hann
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowBartlettHann, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowBartlettHann, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

/// Blackman
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowBlackman, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowBlackman, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

/// Blackman-Harris
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowBlackmanHarris, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowBlackmanHarris, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }
  
  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

/// Flat-top
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowFlatTop, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowFlatTop, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

/// Gaussian
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowGaussian, EsMathWindow )

public:
  EsMathWindowGaussian();

  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowGaussian, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Properties
  ///

  /// Alpha (a) is related to sigma as s = (cnt-1)/(2*a)
  /// Due to Gaussian nodes width limitation, a may not be greater than (cnt / 8)
  /// If alpha is set to 0, or exceedes boundaries above, the value is calculated
  /// automatically, as cnt/8.
  /// Similarly, minimum count for gaussian window calculation is restricted to 8,
  /// to maintain minimum accuracy.
  ///
  ES_DECL_PROPERTY(alpha, double)

protected:
  virtual void doGenerate() const;

protected:
  mutable double m_alpha;
};
//---------------------------------------------------------------------------

/// Hamming
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowHamming, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowHamming, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

/// Hann
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowHann, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowHann, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

/// Nuttall
///
class ESMATH_CLASS ES_INTF_IMPL_DERIVED( EsMathWindowNuttall, EsMathWindow )

public:
  /// Reflected stuff
  ///

  ES_DECL_REFLECTED_CLASS( EsMathWindowNuttall, EsMathWindow )

  // EsBaseIntf implementation
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classNameGetStatic(); }

  /// Default constructor.
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
  virtual void doGenerate() const;
};
//---------------------------------------------------------------------------

#endif
