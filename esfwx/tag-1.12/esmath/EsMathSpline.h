#ifndef _es_math_spline_h_
#define _es_math_spline_h_

class EsMathSplineCubicSolver;

class EKOSF_MATH_CLASS EsMathSplineCubic
{
public:
	EsMathSplineCubic();
	~EsMathSplineCubic();

	bool isOk() const
	{
		return 0 != m_solver && m_built;
	}
	void build(const EsMathFloatArray& x, const EsMathFloatArray& y);
	void build(const EsMathFloatArray& y);
	double xminGet() const
	{
		return m_xmin;
	}
	double xmaxGet() const
	{
		return m_xmax;
	}
	void definitionRangeGet(double& xmin, double& xmax) const
	{
		xmin = m_xmin; xmax = m_xmax;
	}
	double calculate(double x) const;
	// reset spline to unbuilt state
	void reset();

private:
	// prohibited functionality
	EsMathSplineCubic(const EsMathSplineCubic&);
	const EsMathSplineCubic& operator=(const EsMathSplineCubic&);

protected:
	EsMathSplineCubicSolver* m_solver;
	bool m_built;
	double m_xmin;
	double m_xmax;
};

#endif // _es_math_spline_h_