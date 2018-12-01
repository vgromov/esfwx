#include "esmathpch.h"
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathSplineFit.h"
#include "alglib/interpolation.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// cubic spline solver "black box"
class EsMathSplineFitSolver
{
public:
  void build(const EsMathArrayReal& x, const EsMathArrayReal& y,
    size_t basisFunctionsCount, const EsMathFitConstraints& constraints)
  {
    size_t cnt = x.countGet();

    alglib::real_1d_array w;

    ES_ALGLIB_TRY

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
      yc[idx] = constraint.constraintGet();
      dc[idx] = static_cast<ulong>(constraint.kindGet());
    }

    alglib::spline1dfitcubicwc(*(const alglib::real_1d_array*)x.pimplGet(), *(const alglib::real_1d_array*)y.pimplGet(), w,
      xc, yc, dc,  basisFunctionsCount,
      m_info,  m_interpolant, m_report);

    ES_ALGLIB_CATCH
  }

  void build(const EsMathArrayReal& x, const EsMathArrayReal& y,
    size_t basisFunctionsCount, double rho)
  {
    ES_ALGLIB_TRY

    alglib::spline1dfitpenalized(*(const alglib::real_1d_array*)x.pimplGet(), *(const alglib::real_1d_array*)y.pimplGet(),
      basisFunctionsCount,
      rho,
      m_info,  m_interpolant, m_report);

    ES_ALGLIB_CATCH
  }

  double calculate(double x) const
  {
    double result = 0;

    ES_ALGLIB_TRY

    result = alglib::spline1dcalc(m_interpolant, x);

    ES_ALGLIB_CATCH

    return result;
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

  const alglib::spline1dinterpolant& splinesGet() const { return m_interpolant; }

protected:
  alglib::spline1dinterpolant m_interpolant;
  alglib::ae_int_t m_info;
  alglib::spline1dfitreport m_report;
};
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathSplineFit, NO_CLASS_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathSplineFit, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, build, void_Call_cr_EsVariant_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, build, void_Call_cr_EsVariant_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, buildConstrained, void_Call_cr_EsVariant_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, buildConstrained, void_Call_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, buildPenalized, void_Call_cr_EsVariant_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, buildPenalized, void_Call_cr_EsVariant_ulong_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, calculate, double_CallConst_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, calculateV, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, calculateVectorInplace, void_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSplineFit, reset, void_Call, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, minX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, maxX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, rangeX, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, info, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, rmsError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, avgError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, avgRelativeError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, maxError, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSplineFit, nodes, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

// cubic spline fitter implementation
EsMathSplineFit::EsMathSplineFit() :
m_solver(0),
m_xmin(0),
m_xmax(0),
m_info( EsMathFitInfo::None ),
m_rmsError(0),
m_avgError(0),
m_avgRelativeError(0),
m_maxError(0)
{
  m_solver = new EsMathSplineFitSolver;
  ES_ASSERT(m_solver);
}
//---------------------------------------------------------------------------

EsMathSplineFit::~EsMathSplineFit()
{
  ES_DELETE(m_solver);
}
//---------------------------------------------------------------------------

// build spline fit object
//
void EsMathSplineFit::resetFittingErrors()
{
  m_rmsError = m_avgError = m_avgRelativeError = m_maxError = 0;
}
//---------------------------------------------------------------------------

void EsMathSplineFit::build(const EsMathArrayReal& x, const EsMathArrayReal& y,
  ulong basisFunctionsCnt, const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
  reset();
  ES_ASSERT(m_solver);

  if(x.countGet() != y.countGet())
    EsMathException::Throw(esT("x and y data arrays must be equally sized"));
  if(x.countGet() < 2)
    EsMathException::Throw(esT("Data array size is too small, must be at least 2"));
  EsNumericCheck::checkRangeInteger(
    4, 
    x.countGet()+2, 
    basisFunctionsCnt, 
    esT("Basis functions count")
  );

  m_xmin = x.get_min();
  m_xmax = x.get_max();
  ES_ASSERT(m_xmin != m_xmax);
  m_solver->build(x, y, basisFunctionsCnt, constraints);
  m_solver->fittingReportGet(m_rmsError, m_avgError, m_avgRelativeError, m_maxError);
  m_info = alglibInfoToFitInfo(m_solver->fittingInfoGet());
}
//---------------------------------------------------------------------------

// x taken from y's indeces
void EsMathSplineFit::build(const EsMathArrayReal& y, ulong basisFunctionsCnt, const EsMathFitConstraints& constraints /*= EsMathFitConstraints()*/)
{
  EsMathArrayReal x( y.countGet() );
  for(ulong idx = 0; idx < x.countGet(); ++idx)
    x.itemSet(idx, idx);
  
  build(
    x, 
    y, 
    basisFunctionsCnt, 
    constraints
  );
}
//---------------------------------------------------------------------------

void EsMathSplineFit::build(const EsMathArrayReal& x, const EsMathArrayReal& y,  ulong basisFunctionsCnt, double rho)
{
  reset();
  ES_ASSERT(m_solver);

  if(x.countGet() != y.countGet())
    EsMathException::Throw(esT("x and y data arrays must be equally sized"));
  if(x.countGet() < 2)
    EsMathException::Throw(esT("Data array size is too small, must be at least 2"));
  
  EsNumericCheck::checkRangeInteger(
    4, 
    x.countGet()+2, 
    basisFunctionsCnt, 
    esT("Basis functions count")
  );

  m_xmin = x.get_min();
  m_xmax = x.get_max();
  ES_ASSERT(m_xmin != m_xmax);
  m_solver->build(
    x, 
    y, 
    basisFunctionsCnt, 
    rho
  );
  m_solver->fittingReportGet(
    m_rmsError, 
    m_avgError, 
    m_avgRelativeError, 
    m_maxError
  );
  m_info = alglibInfoToFitInfo(m_solver->fittingInfoGet());
}
//---------------------------------------------------------------------------

// x taken from y's indeces
void EsMathSplineFit::build(const EsMathArrayReal& y, ulong basisFunctionsCnt, double rho)
{
  EsMathArrayReal x( y.countGet() );
  for(ulong idx = 0; idx < x.countGet(); ++idx)
    x.itemSet(idx, idx);
  
  build(
    x, 
    y, 
    basisFunctionsCnt, 
    rho
  );
}
//---------------------------------------------------------------------------

// calculate spline value at specified point
double EsMathSplineFit::calculate(double x) const
{
  ES_ASSERT(get_isOk());

  return m_solver->calculate(x);
}
//---------------------------------------------------------------------------

EsVariant EsMathSplineFit::calculateV(cr_EsVariant in) const
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

void EsMathSplineFit::calculateVectorInplace(cr_EsVariant in) const
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
    ar->itemSet(
      idx,
      calculate(
        ar->itemGet(
          idx
        )
      )
    );
  }
}
//---------------------------------------------------------------------------

// reset spline fit to unbuilt state
void EsMathSplineFit::reset()
{
  m_info = EsMathFitInfo::None;
  resetFittingErrors();
}
//---------------------------------------------------------------------------

void EsMathSplineFit::tableGet(EsMathSplineTable& out) const
{
  ES_ASSERT(get_isOk());

  alglib::ae_int_t n;
  alglib::real_2d_array tbl;
  alglib::spline1dunpack(m_solver->splinesGet(), n, tbl);

  out.clear();
  for(alglib::ae_int_t idx = 0; idx < n-1; ++idx )
  {
    out.push_back(
      EsMathSplineNode(
        tbl[idx][0],
        tbl[idx][1],
        tbl[idx][2],
        tbl[idx][3],
        tbl[idx][4],
        tbl[idx][5]
      )
    );
  }
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathSplineFit::NEW()
{
  std::unique_ptr<EsMathSplineFit> f( new EsMathSplineFit );
  f->m_dynamic = true;
  return f.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

void EsMathSplineFit::build(cr_EsVariant vx, cr_EsVariant vy, ulong basisCnt)
{
  buildConstrained(vx, vy, basisCnt, EsVariant::null());
}
//---------------------------------------------------------------------------

void EsMathSplineFit::build(cr_EsVariant vy, ulong basisCnt)
{
  buildConstrained(vy, basisCnt, EsVariant::null());
}
//---------------------------------------------------------------------------

void EsMathSplineFit::buildConstrained(cr_EsVariant vx, cr_EsVariant vy, ulong basisCnt, cr_EsVariant constraints)
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
    build(*px, *py, basisCnt, csx);
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

void EsMathSplineFit::buildConstrained(cr_EsVariant vy, ulong basisCnt, cr_EsVariant constraints)
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
    build(*py, basisCnt, csx);
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

void EsMathSplineFit::buildPenalized(cr_EsVariant vx, cr_EsVariant vy, ulong basisCnt, cr_EsVariant rho)
{
  bool freePx = false;
  bool freePy = false;
  EsMathArrayReal* px = EsMathArrayReal::fromVariant(vx);
  EsMathArrayReal* py = EsMathArrayReal::fromVariant(vy);

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
    build(*px, *py, basisCnt, rho.asDouble());
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

void EsMathSplineFit::buildPenalized(cr_EsVariant vy, ulong basisCnt, cr_EsVariant rho)
{
  bool freePy = false;
  EsMathArrayReal* py = EsMathArrayReal::fromVariant(vy);

  if( !py )
  {
    py = new EsMathArrayReal;
    ES_ASSERT(py);
    py->set_items(vy);
    freePy = true;
  }

  try
  {
    build(*py, basisCnt, rho.asDouble());
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

bool EsMathSplineFit::get_isOk() const
{
  return 0 != m_solver &&
      EsMathFitInfo::Success == m_info;
}
//---------------------------------------------------------------------------

double EsMathSplineFit::get_minX() const
{
  return m_xmin;
}
//---------------------------------------------------------------------------

double EsMathSplineFit::get_maxX() const
{
  return m_xmax;
}
//---------------------------------------------------------------------------

EsVariant EsMathSplineFit::get_rangeX() const
{
  EsVariant::Array result(2);

  result[1] = m_xmin;
  result[2] = m_xmax;

  return result;
}
//---------------------------------------------------------------------------

ulong EsMathSplineFit::get_info() const
{
  return static_cast<ulong>(m_info);
}
//---------------------------------------------------------------------------

double EsMathSplineFit::get_rmsError() const
{
  return m_rmsError;
}
//---------------------------------------------------------------------------

double EsMathSplineFit::get_avgError() const
{
  return m_avgError;
}
//---------------------------------------------------------------------------

double EsMathSplineFit::get_avgRelativeError() const
{
  return m_avgRelativeError;
}
//---------------------------------------------------------------------------

double EsMathSplineFit::get_maxError() const
{
  return m_maxError;
}
//---------------------------------------------------------------------------

EsVariant EsMathSplineFit::get_nodes() const
{
  if( get_isOk() )
  {
    EsMathSplineTable tbl;
    tableGet(tbl);

    EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
    for(size_t idx = 0; idx < tbl.size(); ++idx)
    {
      const EsMathSplineNode& node = tbl[idx];
      EsVariant::Array vn(6);
      vn[0] = node.x();
      vn[1] = node.xp();
      vn[2] = node.a();
      vn[3] = node.b();
      vn[4] = node.c();
      vn[5] = node.d();

      result.addToVariantCollection(vn);
    }

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------


