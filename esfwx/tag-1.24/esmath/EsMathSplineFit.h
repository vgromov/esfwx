#ifndef _es_math_spline_fit_h_
#define _es_math_spline_fit_h_

class EsMathSplineFitSolver;

class ESMATH_CLASS ES_INTF_IMPL1(EsMathSplineFit, EsReflectedClassIntf)

public:
	ES_DECL_REFLECTED_CLASS_BASE(EsMathSplineFit)
	ES_DECL_ATTR_HANDLING_STD

  /// Non-reflected services
  ///

	EsMathSplineFit();
	virtual ~EsMathSplineFit();

	/// Build spline fit object, using specified x, y data, required power
  /// @param x [in]           X values
  /// @param y [in]           Y values
  /// @param basisFunctionsCnt [in] maximal "flexibility" of fitting. Defaulting to count of input nodes+2,
  ///                         must be equal or greater, than 4
  /// @param constraints [in] optional set of fitting constraints
  ///
	void build(const EsMathArrayReal& x, const EsMathArrayReal& y, ulong basisFunctionsCnt,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());

	/// Same as above, but x is taken from y's indices
	void build(const EsMathArrayReal& y, ulong basisFunctionsCnt,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());

	/// Build spline fit object, using specified x, y data, required power,
  /// and penalty coefficient
  /// @param x [in]           X values
  /// @param y [in]           Y values
  /// @param basisFunctionsCnt [in] maximal "flexibility" of fitting. Defaulting to count of input nodes+2,
  ///                         must be equal or greater, than 4
  /// @param rho [in]         Penalty coefficient, may vary from -8 to 8, if below this range,
  ///                         no penalty is invoked, say, -15 will not linearize at all
  ///
	void build(const EsMathArrayReal& x, const EsMathArrayReal& y, ulong basisFunctionsCnt,
		double rho);

	/// Same as above, but x is taken from y's indices
	void build(const EsMathArrayReal& y, ulong basisFunctionsCnt,
		double rho);


	/// X Definition range access
  void definitionRangeGet(double& xmin, double& xmax) const
	{
		xmin = m_xmin; xmax = m_xmax;
	}

  // Get spline nodes && coefficients table
  void tableGet(EsMathSplineTable& out) const;

  /// Reflected services
  ///

  /// Constructors
  ///

  /// Default reflected constructor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  /// Methods
  ///

  /// Build unconstrained spline fit object, using specified x, y data, fitting flexibility
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p2 [in] maximal "flexibility" of fitting. Defaulting to count of input nodes+2,
  ///                must be equal or greater, than 4
  ///
  ES_DECL_REFLECTED_METHOD3(void, build, cr_EsVariant, cr_EsVariant, ulong);

	/// Same as above, but x points are taken from Y's indexes
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] maximal "flexibility" of fitting. Defaulting to count of input nodes+2,
  ///                must be equal or greater, than 4
  ///
  ES_DECL_REFLECTED_METHOD2(void, build, cr_EsVariant, ulong);

  /// Build constrained spline fit object, using specified x, y data, fitting flexibility, and constraints
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p2 [in] fitting flexibility
  /// @param p3 [in] contraint(s), which may be either variant collection, containing 3 items,
  ///                or variant collection, each item is a collection of 3 items,
  ///                which are interpreted as follows: [x, constraint, constraintKind]
  ///
  ES_DECL_REFLECTED_METHOD4(void, buildConstrained, cr_EsVariant, cr_EsVariant, ulong, cr_EsVariant);

  /// Same as above, but X points are taken from Y's indexes
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] fitting flexibility
  /// @param p2 [in] contraint(s), which may be either variant collection, containing 3 items,
  ///                or variant collection, each item is a collection of 3 items,
  ///                which are interpreted as follows: [x, constraint, constraintKind]
  ///
  ES_DECL_REFLECTED_METHOD3(void, buildConstrained, cr_EsVariant, ulong, cr_EsVariant);

  /// Build penalized spline fit object, using specified x, y data, fitting flexibility, and linearization parameter
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p2 [in] fitting flexibility
  /// @param p3 [in] linearization parameter
  ///
  ES_DECL_REFLECTED_METHOD4(void, buildPenalized, cr_EsVariant, cr_EsVariant, ulong, cr_EsVariant);

  /// Same as above, but X points are taken from Y's indexes
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] fitting flexibility
  /// @param p2 [in] linearization parameter
  ///
  ES_DECL_REFLECTED_METHOD3(void, buildPenalized, cr_EsVariant, ulong, cr_EsVariant);

	/// Calculate fitting spline value at specified point
  /// @param p0 [in] X value at which fitting spline is calculated
  /// @return        fitting spline value at specified point
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(double, calculate, double);

	/// Calculate fitting spline values on a single value, or vector|collection of values
  /// @param p0 [in] single value, or vector|collection of values at which polynom is calculated
  /// @return        calculated polynom values vector
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, calculateV, cr_EsVariant);

	/// Calculate fitting spline values on a vector of values inplace, replacing its original contents
  /// @param p0 [in, out] vector of values at which polynom is calculated
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(void, calculateVectorInplace, cr_EsVariant);

 	/// Reset fitting spline to unbuilt state
	ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Properties
  ///

  /// Return true if fitting spline object is built and valid
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

  /// Return fitting spline nodes as a collection [n0..nN], each item being, in turn,
  /// a fixed sized collection of node values, as follows:
  /// [xi,xi+1,a,b,c,d]
  /// If spline is not OK, null is returned
  ///
  ES_DECL_PROPERTY_RO(nodes, EsVariant)

private:
	EsMathSplineFit(const EsMathSplineFit&);
	const EsMathSplineFit& operator=(const EsMathSplineFit&);
	// internal services
	void resetFittingErrors();

protected:
	EsMathSplineFitSolver* m_solver;
	// definition range
	double m_xmin;
	double m_xmax;
	// splinefitting status
	EsMathFitInfo m_info;
	// splinefitting errors
	double m_rmsError;
  double m_avgError;
	double m_avgRelativeError;
  double m_maxError;
};

#endif // _es_math_spline_fit_h_