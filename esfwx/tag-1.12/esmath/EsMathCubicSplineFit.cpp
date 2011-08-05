#include "stdafx.h"
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathCubicSplineFit.h"
#include <alglib/interpolation.h>

// cubic spline solver "black box"
class EsMathCubicSplineFitSolver
{
public:
	void build(const EsMathFloatArray& x, const EsMathFloatArray& y,
		size_t basisFunctionsCount, const EsMathFitConstraints& constraints)
	{
		size_t cnt = x.size();
		alglib::real_1d_array _x;
		alglib::real_1d_array _y;
		esMathFloatArrayToAlglib1dArray(x, _x);
		esMathFloatArrayToAlglib1dArray(y, _y);

		alglib::real_1d_array w;
		w.setlength(cnt);
		for( size_t idx = 0; idx < cnt; ++idx)
			w[idx] = 1.;

		size_t constraintCnt = constraints.size();
		alglib::real_1d_array xc;
		xc.setlength(constraintCnt);
		alglib::real_1d_array yc;
		yc.setlength(constraintCnt);
		alglib::integer_1d_array dc;
		dc.setlength(constraintCnt);
		for(size_t idx = 0; idx < constraintCnt; ++idx)
		{
			const EsMathFitConstraint& constraint = constraints[idx];
			xc[idx] = constraint.xGet();
			yc[idx] = constraint.valGet();
			dc[idx] = constraint.kindGet();
		}

		alglib::spline1dfitcubicwc(_x, _y, w,
			xc, yc, dc,	basisFunctionsCount,
			m_info,	m_interpolant, m_report);
	}

	double calculate(double x) const
	{
		return alglib::spline1dcalc(m_interpolant, x);
	}

	void fittingReportGet(double& rmsErr, double& avgErr, double& avgRelErr, double& maxErr) const
	{
		rmsErr = m_report.rmserror; 
		avgErr = m_report.avgerror;
		avgRelErr = m_report.avgrelerror;
		maxErr = m_report.maxerror;
	}

	alglib::ae_int_t fittingInfoGet() const
	{
		return m_info;
	}

protected:
	alglib::spline1dinterpolant m_interpolant;
	alglib::ae_int_t m_info;
	alglib::spline1dfitreport m_report;
};

// cubic spline fitter implementation
EsMathCubicSplineFit::EsMathCubicSplineFit() :
m_solver(0),
m_xmin(0),
m_xmax(0),
m_info(EsMathFitInfo_NONE),
m_rmsError(0),
m_avgError(0),
m_avgRelativeError(0),
m_maxError(0)
{
	m_solver = new EsMathCubicSplineFitSolver;
	wxASSERT(m_solver);
}

EsMathCubicSplineFit::~EsMathCubicSplineFit()
{
	wxDELETE(m_solver);
}

// build spline fit object 
//
void EsMathCubicSplineFit::resetFittingErrors()
{
	m_rmsError = m_avgError = m_avgRelativeError = m_maxError = 0;
}

void EsMathCubicSplineFit::build(const EsMathFloatArray& x, const EsMathFloatArray& y, size_t basisFunctionsCnt,
					 const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
	reset();
	wxASSERT(m_solver);
	
	if(x.size() != y.size())
		EsException::Throw(0, EsException::Generic, wxT("x and y data arrays must be equally sized"));
	if(x.size() < 2)
		EsException::Throw(0, EsException::Generic, wxT("Data array size is too small, must be at least 2"));
	EsNumericCheck::checkRangeInteger(4, x.size()+2, basisFunctionsCnt, wxT("Basis functions count"));

	m_xmin = x.min();
	m_xmax = x.max();
	wxASSERT(m_xmin != m_xmax);
	m_solver->build(x, y, basisFunctionsCnt, constraints);
	m_solver->fittingReportGet(m_rmsError, m_avgError, m_avgRelativeError, m_maxError);
	m_info = alglibInfoToFitInfo(m_solver->fittingInfoGet());
}

// x taken from y's indeces
void EsMathCubicSplineFit::build(const EsMathFloatArray& y, size_t basisFunctionsCnt,
					 const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
	EsMathFloatArray x( y.size() );
	for(size_t idx = 0; idx < x.size(); ++idx)
		x[idx] = idx;
	build(x, y, basisFunctionsCnt, constraints);
}

// calculate spline value at specified point
double EsMathCubicSplineFit::calculate(double x) const
{
	wxASSERT(isOk());
	wxASSERT(x <= m_xmax);
	wxASSERT(x >= m_xmin);
	return m_solver->calculate(x);
}

// reset spline fit to unbuilt state
void EsMathCubicSplineFit::reset()
{
	m_info = EsMathFitInfo_NONE;
	resetFittingErrors();
}