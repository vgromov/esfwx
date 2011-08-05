#ifndef _es_math_poly_fit_h_
#define _es_math_poly_fit_h_

class EsMathPolyFitSolver;

class EKOSF_MATH_CLASS EsMathPolyFit
{
public:
	EsMathPolyFit();
	~EsMathPolyFit();

	// validity check
	bool isOk() const
	{
		return 0 != m_solver &&
			m_poly.size() > 1 &&
			EsMathFitInfo_SUCCESS == m_info;
	}
	// build polyfit object 
	//
	void build(const EsMathFloatArray& x, const EsMathFloatArray& y, size_t power,
		const EsMathFitConstraints& constraints = EsMathFitConstraints());
	// x taken from y's indeces
	void build(const EsMathFloatArray& y, size_t power,
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
	// access calculated polynom
	const EsMathFloatArray& polyGet() const
	{
		return m_poly;
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
	// calculate polynom value at specified point
	double calculate(double x) const;
	// reset fitting polynom to unbuilt state
	void reset();

private:
	// prohibited functionality
	EsMathPolyFit(const EsMathPolyFit&);
	const EsMathPolyFit& operator=(const EsMathPolyFit&);
	// internal services
	void resetFittingErrors();

protected:
	EsMathPolyFitSolver* m_solver;
	EsMathFloatArray m_poly;
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