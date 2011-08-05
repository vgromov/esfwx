#include "stdafx.h"
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathSpline.h"
#include <alglib/interpolation.h>

class EsMathSplineCubicSolver
{
public:
	void build(const EsMathFloatArray& x, const EsMathFloatArray& y)
	{
		alglib::real_1d_array _x;
		alglib::real_1d_array _y;
		esMathFloatArrayToAlglib1dArray(x, _x);
		esMathFloatArrayToAlglib1dArray(y, _y);
		alglib::spline1dbuildcubic(	_x, _y, m_spline );
	}

	double calculate(double x) const
	{
		return alglib::spline1dcalc(m_spline, x);
	}

protected:
	alglib::spline1dinterpolant m_spline;
};

EsMathSplineCubic::EsMathSplineCubic() :
m_solver(0),
m_built(false),
m_xmin(0),
m_xmax(0)
{
	m_solver = new EsMathSplineCubicSolver;
}

EsMathSplineCubic::~EsMathSplineCubic()
{
	wxDELETE(m_solver);
}


void EsMathSplineCubic::build(const EsMathFloatArray& x, const EsMathFloatArray& y)
{
	reset();

	if(x.size() != y.size())
		EsException::Throw(0, EsException::Generic, wxT("x and y data arrays must be equally sized"));
	if(x.size() < 2)
		EsException::Throw(0, EsException::Generic, wxT("Data array size is too small, must be at least 2"));
	
	m_xmin = x.min();
	m_xmax = x.max();

	wxASSERT(m_solver);
	m_solver->build(x, y);
	m_built = true;
}

void EsMathSplineCubic::build(const EsMathFloatArray& y)
{
	EsMathFloatArray x( y.size() );
	for(size_t idx = 0; idx < x.size(); ++idx)
		x[idx] = idx;
	build(x, y);
}

double EsMathSplineCubic::calculate(double x) const
{
	wxASSERT(isOk());
	EsNumericCheck::checkRangeFloat(m_xmin, m_xmax, x, wxT("Cubic spline definition range"));
	return m_solver->calculate(x);
}

// internal services
void EsMathSplineCubic::reset()
{
	m_built = false;
	m_xmin = 0;
	m_xmax = 0;
}