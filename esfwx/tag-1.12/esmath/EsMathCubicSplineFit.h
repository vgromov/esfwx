#ifndef _es_math_spline_fit_h_
#define _es_math_spline_fit_h_

class EsMathCubicSplineFitSolver;

class EKOSF_MATH_CLASS EsMathCubicSplineFit
{
public:
	EsMathCubicSplineFit();
	~EsMathCubicSplineFit();

	// validity check
	bool isOk() const
	{
		return 0 != m_solver &&
			EsMathFitInfo_SUCCESS == m_info;
	}
	// build spline fit object 
	//
	void build(const EsMathFloatArray& x, const EsMathFloatArray& y, size_t basisFunctionsCnt,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());
	// x taken from y's indeces
	void build(const EsMathFloatArray& y, size_t basisFunctionsCnt,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());
	// definition range access
	double minXget() const
	{
		return m_xmin;
	}
	double maxXget() const
	{
		return m_xmax;
	}
	void definitionRangeGet(double& xmin, double& xmax) const
	{
		xmin = m_xmin; xmax = m_xmax;
	}
	// info access
	EsMathFitInfo infoGet() const
	{
		return m_info;
	}
	// fitting errors access
	double rmsErrorGet() const
	{
		return m_rmsError;
	}
	double avgErrorGet() const
	{
		return m_avgError;
	}
	double avgRelativeErrorGet() const
	{
		return m_avgRelativeError;
	}
	double maxErrorGet() const
	{
		return m_maxError;
	}
	// calculate spline value at specified point
	double calculate(double x) const;
	// reset spline fit to unbuilt state
	void reset();

private:
	EsMathCubicSplineFit(const EsMathCubicSplineFit&);
	const EsMathCubicSplineFit& operator=(const EsMathCubicSplineFit&);
	// internal services
	void resetFittingErrors();

protected:
	EsMathCubicSplineFitSolver* m_solver;
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