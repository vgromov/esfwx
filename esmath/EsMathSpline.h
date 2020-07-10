#ifndef _es_math_spline_h_
#define _es_math_spline_h_

/// Forward declarations
class EsMathSplineCubicSolver;

/// Cubic Spline node class, used as read-only node in
/// EsMathSplineTable container
///
class ESMATH_CLASS EsMathSplineNode
{
public:
  EsMathSplineNode() ES_NOTHROW;
  EsMathSplineNode(double x, double xp, double a, double b, double c, double d) ES_NOTHROW;
  EsMathSplineNode(const EsMathSplineNode& src) ES_NOTHROW;
  EsMathSplineNode& operator=(const EsMathSplineNode& src) ES_NOTHROW;

  inline double x() const ES_NOTHROW { return m_x; }
  inline double xp() const ES_NOTHROW { return m_xp; }
  inline void xp(double val) ES_NOTHROW { m_xp = val; }
  inline double a() const ES_NOTHROW { return m_a; }
  inline double b() const ES_NOTHROW { return m_b; }
  inline double c() const ES_NOTHROW { return m_c; }
  inline double d() const ES_NOTHROW { return m_d; }

protected:
  double m_x;     // (x[i])
  double m_xp;    // (x[i+1])
  double m_a;     // pow 0
  double m_b;     // pow 1
  double m_c;     // pow 2
  double m_d;     // pow 3
};

/// Calculated cubic spline table
typedef std::vector<EsMathSplineNode> EsMathSplineTable;

/// Cubic spline calculator
///
class ESMATH_CLASS ES_INTF_IMPL1(EsMathSpline, EsReflectedClassIntf)

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsMathSpline)
  ES_DECL_ATTR_HANDLING_STD

  /// Non-reflected services
  ///
  EsMathSpline();
  virtual ~EsMathSpline();

  void build(const EsMathArrayReal& x, const EsMathArrayReal& y);
  void build(const EsMathArrayReal& y);

  void akimaBuild(const EsMathArrayReal& x, const EsMathArrayReal& y);
  void akimaBuild(const EsMathArrayReal& y);

  /// Return spline definition range in single call
  void inline definitionRangeGet(double& xmin, double& xmax) const
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

  /// Build spline, using specified x, y data
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  ///
  ES_DECL_REFLECTED_METHOD2(void, build, cr_EsVariant, cr_EsVariant);

  /// Same as above, but x points are taken from Y's indexes
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  ///
  ES_DECL_REFLECTED_METHOD1(void, build, cr_EsVariant);

  /// Build Akima spline, using specified x, y data
  /// @param p0 [in] X values, which may be either EsMathArrayReal object, or collection of doubles
  /// @param p1 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  ///
  ES_DECL_REFLECTED_METHOD2(void, akimaBuild, cr_EsVariant, cr_EsVariant);

  /// Same as above, but x points are taken from Y's indexes
  /// @param p0 [in] Y values, which may be either EsMathArrayReal object, or collection of doubles
  ///
  ES_DECL_REFLECTED_METHOD1(void, akimaBuild, cr_EsVariant);

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

  /// Reset spline to unbuilt state
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Properties
  ///

  /// Return true if Spline object is built ok
  ES_DECL_PROPERTY_RO(isOk, bool)

  /// Return X range minimum
  ES_DECL_PROPERTY_RO(minX, double)

  /// Return X range maximum
  ES_DECL_PROPERTY_RO(maxX, double)

  /// Return spline X definition range as a collection of [min, max]
  ES_DECL_PROPERTY_RO(rangeX, EsVariant)

  /// Return spline nodes as a collection [n0..nN], each item being, in turn,
  /// a fixed sized collection of node values, as follows:
  /// [xi,xi+1,a,b,c,d]
  /// If spline is not OK, null is returned
  ///
  ES_DECL_PROPERTY_RO(nodes, EsVariant)

protected:
  void prepareBuild(const EsMathArrayReal& x, const EsMathArrayReal& y);
  static void xFromYinit(const EsMathArrayReal& y, EsMathArrayReal& x);

protected:
  EsMathSplineCubicSolver* m_solver;
  bool m_built;
  double m_xmin;
  double m_xmax;

  ES_NON_COPYABLE(EsMathSpline);
};

#endif // _es_math_spline_h_