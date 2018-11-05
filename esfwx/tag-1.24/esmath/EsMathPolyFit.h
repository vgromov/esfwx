#ifndef _es_math_poly_fit_h_
#define _es_math_poly_fit_h_

class EsMathPolyFitSolver;

class ESMATH_CLASS ES_INTF_IMPL1(EsMathPolyFit, EsReflectedClassIntf)

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsMathPolyFit)
	ES_DECL_ATTR_HANDLING_STD

  /// Non-reflected services
  ///
	EsMathPolyFit();
	virtual ~EsMathPolyFit();

  /// Build fitting polynom, using specified x, y data, required power, and optional constraints
  /// @param x [in]     X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param y [in]     Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param power [in] requested polynom power
  /// @param constraints [in, optional] Optional fitting constraints
  ///
  void build(const EsMathArrayReal& x, const EsMathArrayReal& y, size_t power,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());

	/// Same as above, but x points are taken from y's indeces
	void build(const EsMathArrayReal& y, size_t power,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());

	/// X Definition range access
	void definitionRangeGet(double& xmin, double& xmax) const
	{
		xmin = m_xmin; xmax = m_xmax;
	}

	/// Get calculated polynom
	const EsMathArrayReal& polyGet() const
	{
		return m_poly;
	}

  /// Reflected services
  ///

  /// Constructors
  ///

  /// Default reflected constructor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Methods
  ///

  /// Build unconstrained polynom, using specified x, y data, required power
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p2 [in] requested polynom power
  ///
  ES_DECL_REFLECTED_METHOD3(void, build, cr_EsVariant, cr_EsVariant, ulong);

	/// Same as above, but x points are taken from Y's indexes
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] requested polynom power
  ///
  ES_DECL_REFLECTED_METHOD2(void, build, cr_EsVariant, ulong);

  /// Build constrained polynom, using specified x, y data, required power, and constraints
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p2 [in] requested polynom power
  /// @param p3 [in] contraint(s), which may be either variant collection, containing 3 items,
  ///                or variant collection, each item is a collection of 3 items,
  ///                which are interpreted as follows: [x, constraint, constraintKind]
  ///
  ES_DECL_REFLECTED_METHOD4(void, buildConstrained, cr_EsVariant, cr_EsVariant, ulong, cr_EsVariant);

  /// Same as above, but X points are taken from Y's indexes
  /// Build constrained polynom, using specified x, y data, required power, and constraints
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] requested polynom power
  /// @param p2 [in] contraint(s), which may be either variant collection, containing 3 items,
  ///                or variant collection, each item is a collection of 3 items,
  ///                which are interpreted as follows: [x, constraint, constraintKind]
  ///
  ES_DECL_REFLECTED_METHOD3(void, buildConstrained, cr_EsVariant, ulong, cr_EsVariant);

	/// Calculate polynom value at specified point
  /// @param p0 [in] X value at which polynom is calculated
  /// @return        polynom value at specified point
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(double, calculate, double);

	/// Calculate polynom values on a single value, or vector|collection of values
  /// @param p0 [in] single value, or vector|collection of values at which polynom is calculated
  /// @return        calculated polynom values vector
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, calculateV, cr_EsVariant);

	/// Calculate polynom values on a vector of values inplace, replacing its original contents
  /// @param p0 [in, out] vector of values at which polynom is calculated
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(void, calculateVectorInplace, cr_EsVariant);

 	/// Reset fitting polynom to unbuilt state
	ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Properties
  ///

  /// Return true if polynom is built
  ES_DECL_PROPERTY_RO(isOk, bool)

  /// Return X range minimum
	ES_DECL_PROPERTY_RO(minX, double)

  /// Return X range maximum
	ES_DECL_PROPERTY_RO(maxX, double)

  /// Return X range as [min, max] variant collection
  ES_DECL_PROPERTY_RO(rangeX, EsVariant)

  /// Return fitting status info
	ES_DECL_PROPERTY_RO(info, ulong)

	/// Return fitting RMS error
	ES_DECL_PROPERTY_RO(rmsError, double)

  /// Return average fitting error
	ES_DECL_PROPERTY_RO(avgError, double)

  /// Return relative average fitting error
	ES_DECL_PROPERTY_RO(avgRelativeError, double)

  /// Return maximum fitting error
	ES_DECL_PROPERTY_RO(maxError, double)

  /// Return calculated polynom coefficients, or empty variant,
  /// if polynom was not successfully built.
  ///
  ES_DECL_PROPERTY_RO(coefficients, EsVariant)

private:
	// prohibited functionality
	EsMathPolyFit(const EsMathPolyFit&);
	const EsMathPolyFit& operator=(const EsMathPolyFit&);
	// internal services
	void resetFittingErrors();

protected:
	EsMathPolyFitSolver* m_solver;
	EsMathArrayReal m_poly;
	double m_xmin;
	double m_xmax;
	// polyfitting status
	EsMathFitInfo m_info;
	// polyfitting errors
	double m_rmsError;
  double m_avgError;
	double m_avgRelativeError;
  double m_maxError;
};

#endif // _es_math_poly_fit_h_