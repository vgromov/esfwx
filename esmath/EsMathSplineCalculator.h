#ifndef _es_math_spline_calculator_h_
#define _es_math_spline_calculator_h_

/// Spline calculator given input nodes table
///
class ESMATH_CLASS ES_INTF_IMPL1(EsMathSplineCalculator, EsReflectedClassIntf)

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsMathSplineCalculator)
  ES_DECL_ATTR_HANDLING_STD

  EsMathSplineCalculator() ES_NOTHROW {}

  /// Assign spline nodes && coefficients table
  /// NB! no assumption on input table is set, it must be sorted in ascending order by x,
  /// x range of nodes must not interleave each other.
  ///
  void tableSet(const EsMathSplineTable& src) ES_NOTHROW { m_table = src; }

  /// Retrieve internal spline nodes && coefficients table
  const EsMathSplineTable& tableGet() const ES_NOTHROW { return m_table; }

  /// Reflected services
  ///

  /// Constructors
  ///

  /// Default reflected constructor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Methods
  ///

  /// Calculate spline value at specified point
  /// @param p0 [in] X value at which spline is calculated
  /// @return        spline value at specified point
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(double, calculate, double);

  /// Calculate spline values on a single value, or vector|collection of values
  /// @param p0 [in] single value, or vector|collection of values at which polynom is calculated
  /// @return        calculated polynom values vector
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, calculateV, cr_EsVariant);

  /// Calculate spline values on a vector of values inplace, replacing its original contents
  /// @param p0 [in, out] vector of values at which polynom is calculated
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(void, calculateVectorInplace, cr_EsVariant);

  /// Properties
  ///

  /// Return true if Spline calculator table is not empty
  ES_DECL_PROPERTY_RO(isOk, bool)

  /// Return X range minimum
  ES_DECL_PROPERTY_RO(minX, double)

  /// Return X range maximum
  ES_DECL_PROPERTY_RO(maxX, double)

  /// Return spline X definition range as a collection of [min, max]
  ES_DECL_PROPERTY_RO(rangeX, EsVariant)

  /// Access/assign spline nodes as a collection [n0..nN], each item being, in turn,
  /// a fixed sized collection of node values, as follows:
  /// [xi,xi+1,a,b,c,d]
  /// If spline is not OK, null is returned
  ///
  ES_DECL_PROPERTY(nodes, EsVariant)

protected:
  // Find table node index given x value. If x is out of definition range,
  // a returned value is negative, if x is less than starting x,
  // positive, if x is greater than ending x, 0 otherwise.
  // If 0 is returned, an idx out parameter is valid, otherwise,
  // it should be considered invalid and unusable.
  //
  int nodeFind(size_t& idx, double x) const ES_NOTHROW;

  // Check if object is OK and if not, throw an exception
  void checkIsValid() const;

protected:
  EsMathSplineTable m_table;

  ES_NON_COPYABLE(EsMathSplineCalculator);
};

#endif // _es_math_spline_calculator_h_