#include "esmathpch.h"
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathPolyFit.h"
#include "alglib/interpolation.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathPolyFit, NO_CLASS_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathPolyFit, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, build, void_Call_cr_EsVariant_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, build, void_Call_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, buildConstrained, void_Call_cr_EsVariant_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, buildConstrained, void_Call_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, calculate, double_CallConst_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, calculateV, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, calculateVectorInplace, void_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathPolyFit, reset, void_Call, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, minX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, maxX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, rangeX, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, info, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, rmsError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, avgError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, avgRelativeError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, maxError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathPolyFit, coefficients, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

// polyfit solver "black box" class
class EsMathPolyFitSolver
{
public:
  EsMathPolyFitSolver() :
  m_info(0)
  {}
  
  void build(const EsMathArrayReal& x, const EsMathArrayReal& y,
    size_t power, const EsMathFitConstraints& constraints)
  {
    size_t cnt = x.get_count();

    size_t constraintCnt = constraints.size();
    if( constraintCnt )
    {
      alglib::real_1d_array w;

      ES_ALGLIB_TRY

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
        yc[idx] = constraint.constraintGet();
        dc[idx] = static_cast<ulong>(constraint.kindGet());
      }
      // build cubic polynom in baricentric representation
      alglib::polynomialfitwc(alglibCastFromArray(x), alglibCastFromArray(y),
        w, xc, yc, dc, power+1, m_info, m_interpolant, m_report);

      ES_ALGLIB_CATCH
    }
    else
    {
      ES_ALGLIB_TRY

      alglib::polynomialfit(alglibCastFromArray(x), alglibCastFromArray(y),
        power+1, m_info, m_interpolant, m_report);

      ES_ALGLIB_CATCH
    }
  }

  void polyGet(EsMathArrayReal& poly) const
  {
    ES_ALGLIB_TRY

    // convert baricentric to power form
    alglib::polynomialbar2pow(m_interpolant, alglibCastFromArray(poly));

    ES_ALGLIB_CATCH
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
//---------------------------------------------------------------------------

// Poly fit class implamentation
//
EsMathPolyFit::EsMathPolyFit() :
m_solver(nullptr),
m_xmin(0),
m_xmax(0),
m_info(EsMathFitInfo::None),
m_rmsError(0),
m_avgError(0),
m_avgRelativeError(0),
m_maxError(0)
{
  m_solver = new EsMathPolyFitSolver;
  ES_ASSERT(m_solver);
}
//---------------------------------------------------------------------------

EsMathPolyFit::~EsMathPolyFit()
{
  ES_DELETE(m_solver);
}
//---------------------------------------------------------------------------

// build polyfit object
//
void EsMathPolyFit::resetFittingErrors()
{
  m_rmsError = m_avgError = m_avgRelativeError = m_maxError = 0;
}
//---------------------------------------------------------------------------

void EsMathPolyFit::build(const EsMathArrayReal& x, const EsMathArrayReal& y, ulong power,
           const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
  reset();

  ES_ASSERT(m_solver);
  if(x.countGet() != y.countGet())
    EsException::Throw(esT("x and y data arrays must be equally sized"));
  if(x.countGet() < 2)
    EsException::Throw(esT("Data array size is too small, must be at least 2"));

  EsNumericCheck::checkRangeInteger(1, 8, power, esT("Polynomial power"));

  m_xmin = x.get_min();
  m_xmax = x.get_max();
  ES_ASSERT(m_xmin != m_xmax);
  m_solver->build(x, y, power, constraints);
  m_solver->polyGet(m_poly);
  m_solver->fittingReportGet(m_rmsError, m_avgError, m_avgRelativeError, m_maxError);
  m_info = alglibInfoToFitInfo(m_solver->fittingInfoGet());
}
//---------------------------------------------------------------------------

// x taken from y's indeces
void EsMathPolyFit::build(const EsMathArrayReal& y, ulong power,
           const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
  EsMathArrayReal x( y.countGet() );
  for(ulong idx = 0; idx < x.countGet(); ++idx)
    x.itemSet(idx, idx);

  build(
    x, 
    y, 
    power, 
    constraints
  );
}
//---------------------------------------------------------------------------

// calculate polynom value at specified point
double EsMathPolyFit::calculate(double x) const
{
  ES_ASSERT(get_isOk());

  double result = m_poly.itemGet(0);
  double xpow = x;
  for( ulong idx = 1; idx < m_poly.countGet(); ++idx)
  {
    result += xpow*m_poly.itemGet(idx);
    xpow *= x;
  }

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsMathPolyFit::calculateV(cr_EsVariant in) const
{
  if( in.isCollection() || in.isObject() )
  {
    EsMathArrayReal* ar = nullptr;
    EsBaseIntfPtr result = EsMathArrayReal::baseIntfPtrCreate(
      in,
      &ar
    );

    if( !result )
      EsException::Throw(
        esT("%s call error, input parameter could not be interpreted as an array or collection"),
        EsString::fromAscii(__func__)
      );

    ulong cnt = ar->countGet();
    for(ulong idx = 0; idx < cnt; ++idx)
    {
      ar->itemSet(
        idx,
        calculate(
          ar->itemGet(
            idx
          )
        )
      );
    }

    return result;
  }
  else
    return calculate( in.asDouble() );
}
//---------------------------------------------------------------------------

void EsMathPolyFit::calculateVectorInplace(cr_EsVariant in) const
{
  EsMathArrayReal* ar = EsMathArrayReal::fromVariant(in);
  if( !ar )
    EsException::Throw(
      esT("%s call error, input parameter could not be interpreted as an array or collection"),
      EsString::fromAscii(__func__)
    );

  ulong cnt = ar->countGet();
  for(ulong idx = 0; idx < cnt; ++idx)
  {
    EsMathArrayReal::value_type val = ar->itemGet(idx);
    val = calculate( val );

    ar->itemSet(
      idx,
      val
    );
  }
}
//---------------------------------------------------------------------------

// reset fitting polynom to unbuilt state
void EsMathPolyFit::reset()
{
  m_info = EsMathFitInfo::None;
  resetFittingErrors();
  m_poly.clear();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathPolyFit::NEW()
{
  std::unique_ptr<EsMathPolyFit> f( new EsMathPolyFit );
  ES_ASSERT(f);
  
  f->m_dynamic = true;
  return f.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

void EsMathPolyFit::build(cr_EsVariant vx, cr_EsVariant vy, ulong power)
{
  buildConstrained(vx, vy, power, EsVariant::null());
}
//---------------------------------------------------------------------------

void EsMathPolyFit::build(cr_EsVariant vy, ulong power)
{
  buildConstrained(vy, power, EsVariant::null());
}
//---------------------------------------------------------------------------

void EsMathPolyFit::buildConstrained(cr_EsVariant vx, cr_EsVariant vy, ulong power, cr_EsVariant constraints)
{
  bool freePx = false;
  bool freePy = false;
  EsMathArrayReal* px = EsMathArrayReal::fromVariant(vx);
  EsMathArrayReal* py = EsMathArrayReal::fromVariant(vy);
  EsMathFitConstraints csx;
  constraintsFromVariant(csx, constraints);

  if( !px )
  {
    px = new EsMathArrayReal;
    ES_ASSERT(px);
    px->set_items(vx);
    freePx = true;
  }

  if( !py )
  {
    py = new EsMathArrayReal;
    ES_ASSERT(py);
    py->set_items(vy);
    freePy = true;
  }

  try
  {
    build(*px, *py, power, csx);
  }
  catch(...)
  {
    if( freePx )
      delete px;

    if( freePy )
      delete py;

    throw;
  }

  if( freePx )
    delete px;

  if( freePy )
    delete py;
}
//---------------------------------------------------------------------------

void EsMathPolyFit::buildConstrained(cr_EsVariant vy, ulong power, cr_EsVariant constraints)
{
  bool freePy = false;
  EsMathArrayReal* py = EsMathArrayReal::fromVariant(vy);
  EsMathFitConstraints csx;
  constraintsFromVariant(csx, constraints);

  if( !py )
  {
    py = new EsMathArrayReal;
    ES_ASSERT(py);
    py->set_items(vy);
    freePy = true;
  }

  try
  {
    build(*py, power, csx);
  }
  catch(...)
  {
    if( freePy )
      delete py;

    throw;
  }

  if( freePy )
    delete py;
}
//---------------------------------------------------------------------------

bool EsMathPolyFit::get_isOk() const
{
  return nullptr != m_solver &&
      m_poly.get_count() > 1 &&
      EsMathFitInfo::Success == m_info;
}
//---------------------------------------------------------------------------

double EsMathPolyFit::get_minX() const
{
  return m_xmin;
}
//---------------------------------------------------------------------------

double EsMathPolyFit::get_maxX() const
{
  return m_xmax;
}
//---------------------------------------------------------------------------

EsVariant EsMathPolyFit::get_rangeX() const
{
  EsVariant::Array result(2);

  result[0] = m_xmin;
  result[1] = m_xmax;

  return result;
}
//---------------------------------------------------------------------------

ulong EsMathPolyFit::get_info() const
{
  return static_cast<ulong>(m_info);
}
//---------------------------------------------------------------------------

double EsMathPolyFit::get_rmsError() const
{
  return m_rmsError;
}
//---------------------------------------------------------------------------

double EsMathPolyFit::get_avgError() const
{
  return m_avgError;
}
//---------------------------------------------------------------------------

double EsMathPolyFit::get_avgRelativeError() const
{
  return m_avgRelativeError;
}
//---------------------------------------------------------------------------

double EsMathPolyFit::get_maxError() const
{
  return m_maxError;
}
//---------------------------------------------------------------------------

EsVariant EsMathPolyFit::get_coefficients() const
{
  if( get_isOk() )
  {
    EsVariant::Array result( m_poly.countGet() );
    for( ulong idx = 0; idx < m_poly.countGet(); ++idx )
      result[idx] = m_poly.itemGet(idx);

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

