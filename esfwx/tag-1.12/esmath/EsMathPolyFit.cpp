#include "stdafx.h"
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathPolyFit.h"
#include <alglib/interpolation.h>

// polyfit solver "black box" class
class EsMathPolyFitSolver
{
public:
	EsMathPolyFitSolver()
	{}
	
	void build(const EsMathFloatArray& x, const EsMathFloatArray& y,
		size_t power, const EsMathFitConstraints& constraints)
	{
		size_t cnt = x.size();
		alglib::real_1d_array _x;
		alglib::real_1d_array _y;
		esMathFloatArrayToAlglib1dArray(x, _x);
		esMathFloatArrayToAlglib1dArray(y, _y);

		size_t constraintCnt = constraints.size();
		if( constraintCnt )
		{
			alglib::real_1d_array w;
			w.setlength(cnt);
			for( size_t idx = 0; idx < cnt; ++idx)
				w[idx] = 1.;
 
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
			// build cubic polynom in baricentric representation
			alglib::polynomialfitwc(_x, _y, w, xc, yc, dc, power+1, m_info, m_interpolant, m_report);
		}
		else
			alglib::polynomialfit(_x, _y, power+1, m_info, m_interpolant, m_report);
	}

	void polyGet(EsMathFloatArray& poly) const
	{
		// convert baricentric to power form
		alglib::real_1d_array _poly;
		alglib::polynomialbar2pow(m_interpolant, _poly);
		poly = EsMathFloatArray(_poly.getcontent(), _poly.length());
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
	alglib::barycentricinterpolant m_interpolant;
	alglib::ae_int_t m_info;
  alglib::polynomialfitreport m_report;
};

// poly fit class implamentation
//
EsMathPolyFit::EsMathPolyFit() :
m_solver(0),
m_xmin(0),
m_xmax(0),
m_info(EsMathFitInfo_NONE),
m_rmsError(0),
m_avgError(0),
m_avgRelativeError(0),
m_maxError(0)
{
	m_solver = new EsMathPolyFitSolver;
	wxASSERT(m_solver);
}

EsMathPolyFit::~EsMathPolyFit()
{
	wxDELETE(m_solver);
}

// build polyfit object 
//
void EsMathPolyFit::resetFittingErrors()
{
	m_rmsError = m_avgError = m_avgRelativeError = m_maxError = 0;
}

void EsMathPolyFit::build(const EsMathFloatArray& x, const EsMathFloatArray& y, size_t power,
					 const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
	reset();

	wxASSERT(m_solver);
	if(x.size() != y.size())
		EsException::Throw(0, EsException::Generic, wxT("x and y data arrays must be equally sized"));
	if(x.size() < 2)
		EsException::Throw(0, EsException::Generic, wxT("Data array size is too small, must be at least 2"));
	EsNumericCheck::checkRangeInteger(1, 8, power, wxT("Polynomial power"));

	m_xmin = x.min();
	m_xmax = x.max();
	wxASSERT(m_xmin != m_xmax);
	m_solver->build(x, y, power, constraints);
	m_solver->polyGet(m_poly);
	m_solver->fittingReportGet(m_rmsError, m_avgError, m_avgRelativeError, m_maxError);
	m_info = alglibInfoToFitInfo(m_solver->fittingInfoGet());
}

// x taken from y's indeces
void EsMathPolyFit::build(const EsMathFloatArray& y, size_t power,
					 const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
	EsMathFloatArray x( y.size() );
	for(size_t idx = 0; idx < x.size(); ++idx)
		x[idx] = idx;
	build(x, y, power, constraints);
}

// calculate polynom value at specified point
double EsMathPolyFit::calculate(double x) const
{
	wxASSERT(isOk());
	wxASSERT(x <= m_xmax);
	wxASSERT(x >= m_xmin);

	double result = m_poly[0];
	double xpow = x;
	for( size_t idx = 1; idx < m_poly.size(); ++idx)
	{
		result += xpow*m_poly[idx];
		xpow *= x;
	}

	return result;
}

// reset fitting polynom to unbuilt state
void EsMathPolyFit::reset()
{
	m_info = EsMathFitInfo_NONE;
	resetFittingErrors();
	(m_poly.free)();
}