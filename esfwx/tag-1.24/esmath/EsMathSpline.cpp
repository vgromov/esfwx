#include "esmathpch.h"
#pragma hdrstop

#include "EsMathInternals.h"
#include "EsMathSpline.h"
#include "alglib/interpolation.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsMathSplineNode::EsMathSplineNode() :
m_x(0),
m_xp(0),
m_a(0),
m_b(0),
m_c(0),
m_d(0)
{}

EsMathSplineNode::EsMathSplineNode(double x, double xp, double a, double b, double c, double d) :
m_x(x),
m_xp(xp),
m_a(a),
m_b(b),
m_c(c),
m_d(d)
{}

EsMathSplineNode::EsMathSplineNode(const EsMathSplineNode& src) :
m_x(src.m_x),
m_xp(src.m_xp),
m_a(src.m_a),
m_b(src.m_b),
m_c(src.m_c),
m_d(src.m_d)
{}

EsMathSplineNode& EsMathSplineNode::operator=(const EsMathSplineNode& src)
{
  m_x = src.m_x;
  m_xp = src.m_xp;
  m_a = src.m_a;
  m_b = src.m_b;
  m_c = src.m_c;
  m_d = src.m_d;

  return *this;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

class EsMathSplineCubicSolver
{
public:
	void build(const EsMathArrayReal& x, const EsMathArrayReal& y)
	{
    ES_ALGLIB_TRY

		alglib::spline1dbuildcubic(	alglibCastFromArray(x),
			alglibCastFromArray(y), m_spline );

    ES_ALGLIB_CATCH
	}

	void akimaBuild(const EsMathArrayReal& x, const EsMathArrayReal& y)
	{
    ES_ALGLIB_TRY

		alglib::spline1dbuildcubic(	alglibCastFromArray(x),
			alglibCastFromArray(y), m_spline );

    ES_ALGLIB_CATCH
	}

	double calculate(double x) const
	{
    double result = 0;
    ES_ALGLIB_TRY

		result = alglib::spline1dcalc(m_spline, x);

    ES_ALGLIB_CATCH

    return result;
	}

  const alglib::spline1dinterpolant& splineGet() const { return m_spline; }

protected:
	alglib::spline1dinterpolant m_spline;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathSpline, NO_CLASS_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathSpline, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, build, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, build, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, akimaBuild, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, akimaBuild, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, calculate, double_CallConst_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, calculateV, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, calculateVectorInplace, void_CallConst_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathSpline, reset, void_Call, NO_METHOD_DESCR)
  // Properties
  ES_DECL_PROP_INFO_RO(             EsMathSpline, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(             EsMathSpline, minX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSpline, maxX, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSpline, rangeX, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathSpline, nodes, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsMathSpline::EsMathSpline() :
m_solver(0),
m_built(false),
m_xmin(0),
m_xmax(0)
{
	m_solver = new EsMathSplineCubicSolver;
}
//---------------------------------------------------------------------------

EsMathSpline::~EsMathSpline()
{
	ES_DELETE(m_solver);
}
//---------------------------------------------------------------------------

void EsMathSpline::prepareBuild(const EsMathArrayReal& x, const EsMathArrayReal& y)
{
	reset();

	if(x.countGet() != y.countGet())
		EsMathException::Throw(esT("x and y data arrays must be equally sized"));
	if(x.countGet() < 2)
		EsMathException::Throw(esT("Data array size is too small, must be at least 2"));

	m_xmin = x.get_min();
	m_xmax = x.get_max();
}
//---------------------------------------------------------------------------

void EsMathSpline::build(const EsMathArrayReal& x, const EsMathArrayReal& y)
{
  prepareBuild(x, y);

	ES_ASSERT(m_solver);
	m_solver->build(x, y);
	m_built = true;
}
//---------------------------------------------------------------------------

void EsMathSpline::xFromYinit(const EsMathArrayReal& y, EsMathArrayReal& x)
{
	x.countSet( y.countGet() );
	for(ulong idx = 0; idx < x.countGet(); ++idx)
		x.itemSet(idx, idx);
}
//---------------------------------------------------------------------------

void EsMathSpline::build(const EsMathArrayReal& y)
{
	EsMathArrayReal x;
  xFromYinit(y, x);

	build(x, y);
}
//---------------------------------------------------------------------------

void EsMathSpline::akimaBuild(const EsMathArrayReal& x, const EsMathArrayReal& y)
{
  prepareBuild(x, y);

	ES_ASSERT(m_solver);
	m_solver->akimaBuild(x, y);
	m_built = true;
}
//---------------------------------------------------------------------------

void EsMathSpline::akimaBuild(const EsMathArrayReal& y)
{
	EsMathArrayReal x;
  xFromYinit(y, x);

  akimaBuild(x, y);
}
//---------------------------------------------------------------------------

double EsMathSpline::calculate(double x) const
{
	ES_ASSERT(get_isOk());
//	EsNumericCheck::checkRangeFloat(m_xmin, m_xmax, x, esT("Spline definition range"));
	return m_solver->calculate(x);
}
//---------------------------------------------------------------------------

EsVariant EsMathSpline::calculateV(cr_EsVariant in) const
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

void EsMathSpline::calculateVectorInplace(cr_EsVariant in) const
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

void EsMathSpline::reset()
{
	m_built = false;
	m_xmin = 0;
	m_xmax = 0;
}
//---------------------------------------------------------------------------

void EsMathSpline::tableGet(EsMathSplineTable& out) const
{
	ES_ASSERT(get_isOk());

  alglib::ae_int_t n;
  alglib::real_2d_array tbl;
  alglib::spline1dunpack(m_solver->splineGet(), n, tbl);

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

// Reflected stuff
//
EsBaseIntfPtr EsMathSpline::NEW()
{
	std::unique_ptr<EsMathSpline> f( new EsMathSpline );
	f->m_dynamic = true;
	return f.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

void EsMathSpline::build(const EsVariant& vx, const EsVariant& vy)
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
    build(*px, *py);
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

void EsMathSpline::build(const EsVariant& vy)
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
    build(*py);
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

void EsMathSpline::akimaBuild(const EsVariant& vx, const EsVariant& vy)
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
    akimaBuild(*px, *py);
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

void EsMathSpline::akimaBuild(const EsVariant& vy)
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
    akimaBuild(*py);
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

bool EsMathSpline::get_isOk() const
{
  return 0 != m_solver && m_built;
}
//---------------------------------------------------------------------------

double EsMathSpline::get_minX() const
{
  return m_xmin;
}
//---------------------------------------------------------------------------

double EsMathSpline::get_maxX() const
{
  return m_xmax;
}
//---------------------------------------------------------------------------

EsVariant EsMathSpline::get_rangeX() const
{
  if( get_isOk() )
  {
    EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
    result.addToVariantCollection(m_xmin)
      .addToVariantCollection(m_xmax);

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsMathSpline::get_nodes() const
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

