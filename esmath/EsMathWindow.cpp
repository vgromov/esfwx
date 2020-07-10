#include "esmathpch.h"
#pragma hdrstop

#include "EsMathWindow.h"
//---------------------------------------------------------------------------

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathWindow, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathWindow, generate, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathWindow, generateNormalized, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsMathWindow::EsMathWindow() :
m_cacheInvalid(true)
{}
//---------------------------------------------------------------------------

ulong EsMathWindow::cachePrepare(cr_EsVariant cnt) const
{
  ulong _cnt = cnt.asULong();
  if( !_cnt )
    EsMathException::Throw(
      esT("Requested Window count must be a positive integer number")
    );

  if( m_aw.countGet() != _cnt )
  {
    m_aw.countSet(_cnt);
    m_aw.fill(1);
    m_cacheInvalid = true;
  }

  return _cnt;
}
//---------------------------------------------------------------------------

EsVariant EsMathWindow::generate(cr_EsVariant cnt) const
{
  ulong _cnt = cachePrepare(cnt);
  if( 1 != _cnt && m_cacheInvalid )
  {
    doGenerate();
    m_cacheInvalid = false;
  }

  return m_aw.clone();
}
//---------------------------------------------------------------------------

EsVariant EsMathWindow::generateNormalized(cr_EsVariant cnt) const
{
  ulong _cnt = cachePrepare(cnt);
  if( 1 != _cnt && m_cacheInvalid )
  {
    doGenerate();
    m_cacheInvalid = false;
  }

  EsMathArrayReal norm = m_aw;
  norm.divide_inplace( m_aw.get_total() );

  return norm.clone();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowBartlettHann, EsMathWindow, _i("Modified Bartlett-Hann window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowBartlettHann, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowBartlettHann::NEW()
{
  std::unique_ptr<EsMathWindowBartlettHann> p = ES_MAKE_UNIQUE( EsMathWindowBartlettHann );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline bartlettHannGenerator(double x)
{
  return 0.62 -
    0.48 * EsMath::abs(x) +
    0.38 * EsMath::cos(
      EsMath::_2pi() * x
    );
}
//---------------------------------------------------------------------------

void EsMathWindowBartlettHann::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = 1./static_cast<double>(cnt);

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = static_cast<double>(idx)*_div - 0.5;

    m_aw.itemSet(
      idx,
      bartlettHannGenerator(x)
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowBlackman, EsMathWindow, _i("Blackman window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowBlackman, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowBlackman::NEW()
{
  std::unique_ptr<EsMathWindowBlackman> p = ES_MAKE_UNIQUE( EsMathWindowBlackman );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline blackmannGenerator(double x)
{
  return 0.42 -
    0.5 * EsMath::cos(
      EsMath::_2pi() * x
    ) +
    0.08 * EsMath::cos(
      2. * EsMath::_2pi() * x
    );
}
//---------------------------------------------------------------------------

void EsMathWindowBlackman::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = 1./static_cast<double>(cnt-1);
  ulong mid = (cnt % 2) ?
    (cnt+1)/2 :
    cnt/2;

  for( ulong idx = 0; idx < mid; ++idx )
  {
    double x = static_cast<double>(idx)*_div;

    m_aw.itemSet(
      idx,
      blackmannGenerator(x)
    );
  }

  for( ulong idx = mid; idx < cnt; ++idx )
  {
    m_aw.itemSet(
      idx,
      m_aw.itemGet(
        cnt-1-idx
      )
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowBlackmanHarris, EsMathWindow, _i("Minimum 4-term Blackman-Harris window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowBlackmanHarris, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowBlackmanHarris::NEW()
{
  std::unique_ptr<EsMathWindowBlackmanHarris> p = ES_MAKE_UNIQUE( EsMathWindowBlackmanHarris );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline blackmanHarrisGenerator(double x)
{
  const double a0 = 0.35875;
  const double a1 = 0.48829;
  const double a2 = 0.14128;
  const double a3 = 0.01168;

  return a0 -
    a1 * EsMath::cos(
      EsMath::_2pi() * x
    ) +
    a2 * EsMath::cos(
      2. * EsMath::_2pi() * x
    ) -
    a3 * EsMath::cos(
      3. * EsMath::_2pi() * x
    );
}
//---------------------------------------------------------------------------

void EsMathWindowBlackmanHarris::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = 1./static_cast<double>(cnt-1);

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = static_cast<double>(idx)*_div;

    m_aw.itemSet(
      idx,
      blackmanHarrisGenerator(x)
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowFlatTop, EsMathWindow, _i("Flat-Top window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowFlatTop, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowFlatTop::NEW()
{
  std::unique_ptr<EsMathWindowFlatTop> p = ES_MAKE_UNIQUE( EsMathWindowFlatTop );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline flatTopGenerator(double x)
{
  const double a0 = 0.21557895;
  const double a1 = 0.41663158;
  const double a2 = 0.277263158;
  const double a3 = 0.083578947;
  const double a4 = 0.006947368;

  return a0 -
    a1 * EsMath::cos(
      EsMath::_2pi() * x
    ) +
    a2 * EsMath::cos(
      2. * EsMath::_2pi() * x
    ) -
    a3 * EsMath::cos(
      3. * EsMath::_2pi() * x
    ) +
    a4 * EsMath::cos(
      4. * EsMath::_2pi() * x
    );
}
//---------------------------------------------------------------------------

void EsMathWindowFlatTop::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = 1./static_cast<double>(cnt-1);

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = static_cast<double>(idx)*_div;

    m_aw.itemSet(
      idx,
      flatTopGenerator(x)
    );
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowGaussian, EsMathWindow, _i("Gaussian window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowGaussian, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_PROP_INFO(          EsMathWindowGaussian, alpha, double, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsMathWindowGaussian::EsMathWindowGaussian() :
m_alpha(0)
{}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowGaussian::NEW()
{
  std::unique_ptr<EsMathWindowGaussian> p = ES_MAKE_UNIQUE( EsMathWindowGaussian );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

void EsMathWindowGaussian::set_alpha(const double& val)
{
  if( !EsMath::areEqualFloats(val, m_alpha) )
  {
    m_cacheInvalid = true;
    m_alpha = val;
  }
}
//---------------------------------------------------------------------------

double EsMathWindowGaussian::get_alpha() const
{
  return m_alpha;
}
//---------------------------------------------------------------------------

static double inline gaussianGenerator(double x, double _div)
{
  return EsMath::exp(
    x*x*_div
  );
}
//---------------------------------------------------------------------------

void EsMathWindowGaussian::doGenerate() const
{
  ulong cnt = m_aw.countGet();

  if( cnt < 8 )
    EsMathException::Throw(
      esT("Minimum node count for Gaussian window calculation must not be less than 8")
    );

  if( m_alpha > static_cast<double>(cnt/8) )
    m_alpha = static_cast<double>(cnt/8);

  double sig = static_cast<double>(cnt-1)/(2.* m_alpha);
  double _div = -1./(2.*sig*sig);
  double beg = -static_cast<double>(cnt-1)/2.;

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = beg + static_cast<double>(idx);

    m_aw.itemSet(
      idx,
      gaussianGenerator(x, _div)
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowHamming, EsMathWindow, _i("Hamming window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowHamming, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowHamming::NEW()
{
  std::unique_ptr<EsMathWindowHamming> p = ES_MAKE_UNIQUE( EsMathWindowHamming );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline hammingGenerator(double x)
{
  return 0.54 -
    0.46 * EsMath::cos( x );
}
//---------------------------------------------------------------------------

void EsMathWindowHamming::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = EsMath::_2pi()/static_cast<double>(cnt-1);

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = static_cast<double>(idx)*_div;

    m_aw.itemSet(
      idx,
      hammingGenerator(x)
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowHann, EsMathWindow, _i("Hann window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowHann, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowHann::NEW()
{
  std::unique_ptr<EsMathWindowHann> p = ES_MAKE_UNIQUE( EsMathWindowHann );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline hannGenerator(double x)
{
  return 0.5 * (
    1. -
    EsMath::cos( x )
  );
}
//---------------------------------------------------------------------------

void EsMathWindowHann::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = EsMath::_2pi()/static_cast<double>(cnt-1);

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = static_cast<double>(idx)*_div;

    m_aw.itemSet(
      idx,
      hannGenerator(x)
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

ES_DECL_CLASS_INFO_DERIVED_BEGIN( EsMathWindowNuttall, EsMathWindow, _i("Nuttall window") )
  ES_DECL_REFLECTED_CTOR_INFO(EsMathWindowNuttall, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathWindowNuttall::NEW()
{
  std::unique_ptr<EsMathWindowNuttall> p = ES_MAKE_UNIQUE( EsMathWindowNuttall );
  ES_ASSERT(p);
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

static double inline nuttallGenerator(double x)
{
  const double a0 = 0.3635819;
  const double a1 = 0.4891775;
  const double a2 = 0.1365995;
  const double a3 = 0.0106411;

  return a0 -
    a1 * EsMath::cos(
      EsMath::_2pi() * x
    ) +
    a2 * EsMath::cos(
      2. * EsMath::_2pi() * x
    ) -
    a3 * EsMath::cos(
      3. * EsMath::_2pi() * x
    );
}
//---------------------------------------------------------------------------

void EsMathWindowNuttall::doGenerate() const
{
  ulong cnt = m_aw.countGet();
  double _div = 1./static_cast<double>(cnt-1);

  for( ulong idx = 0; idx < cnt; ++idx )
  {
    double x = static_cast<double>(idx)*_div;

    m_aw.itemSet(
      idx,
      nuttallGenerator(x)
    );
  }
}
//---------------------------------------------------------------------------
