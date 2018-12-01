#include "esmathpch.h"
#pragma hdrstop

#include "EsMathConvolutionReal.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsMathConvolutionFlag, NO_CLASS_DESCR)
}

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathConvolutionReal, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathConvolutionReal, process, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  /// Properties
  ES_DECL_PROP_INFO_RO(             EsMathConvolutionReal, ok, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathConvolutionReal, flags, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathConvolutionReal, substL, double, NO_PROPERTY_LABEL, 0., NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathConvolutionReal, substR, double, NO_PROPERTY_LABEL, 0., NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathConvolutionReal, filter, EsVariant, NO_PROPERTY_LABEL, EsVariant(EsVariant::VAR_VARIANT_COLLECTION), NO_PROPERTY_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathConvolutionReal, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathConvolutionReal, EsBaseIntfPtr_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathConvolutionReal, EsBaseIntfPtr_ClassCall_cr_EsVariant_ulong, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

// Convolution implementation
//
EsMathConvolutionReal::EsMathConvolutionReal() :
m_flags(0),
m_substL(0),
m_substR(0)
{}

EsMathConvolutionReal::EsMathConvolutionReal(const EsMathArrayReal& filter, ulong flags) :
m_flags(flags),
m_substL(0),
m_substR(0),
m_filter(filter)
{
}

EsMathConvolutionReal::EsMathConvolutionReal(ulong filterSize, EsMathArrayReal::const_pointer filter, ulong flags) :
m_flags(flags),
m_substL(0),
m_substR(0)
{
  m_filter.dataSet(
    filterSize, 
    filter
  );
}

/// filter access
///
void EsMathConvolutionReal::filterSet(const EsMathArrayReal& filter, ulong flags /*= 0*/)
{
  m_filter = filter;
  // NB! change only filter-related flags
  m_flags &= ~static_cast<ulong>(EsMathConvolutionFlag::FilterMask);
  m_flags |= (flags & static_cast<ulong>(EsMathConvolutionFlag::FilterMask));
}

void EsMathConvolutionReal::filterSet(ulong filterSize, EsMathArrayReal::const_pointer filter, ulong flags /*= 0*/)
{
  m_filter.dataSet(
    filterSize, 
    filter
  );
  
  // NB! change only filter-related flags
  m_flags &= ~static_cast<ulong>(EsMathConvolutionFlag::FilterMask);
  m_flags |= (flags & static_cast<ulong>(EsMathConvolutionFlag::FilterMask));
}

/// real 1-dimentional convolution
/// if signal is stable, we may use signal itself to define data at T < 0 domains
/// if filter is symmetrical, we may use half of its length as offset from the
/// signal sequence, to the first filtered point value
///
void EsMathConvolutionReal::process(const EsMathArrayReal& sig, EsMathArrayReal& out) const
{
  ES_ASSERT(!m_filter.get_empty());
  ES_ASSERT(sig.countGet()/2 > m_filter.countGet());

  ulong fCnt = 0;
  if( m_flags & static_cast<ulong>(EsMathConvolutionFlag::FilterIsSymmetrical) )
    fCnt = m_filter.countGet()/2;
  else
    fCnt = m_filter.countGet();

  double lsubstVal = m_substL;
  double rsubstVal = m_substR;

  if( m_flags & static_cast<ulong>(EsMathConvolutionFlag::SignalIsStable) )
  {
    if( !(m_flags & static_cast<ulong>(EsMathConvolutionFlag::UseFixedLsubst)) )
    {
      // in case of stable signal, calculate lsubstVal and rsubstVal as average per
      // lCnt and rCnt signal points
      lsubstVal = 0;
      for(ulong idx = 0; idx < fCnt; ++idx)
        lsubstVal += sig.itemGet(idx);
      lsubstVal /= (double)fCnt;
    }

    if( !(m_flags & static_cast<ulong>(EsMathConvolutionFlag::UseFixedRsubst)) )
    {
      rsubstVal = 0;
      for(ulong idx = sig.countGet()-fCnt; idx < sig.countGet(); ++idx)
        rsubstVal += sig.itemGet(idx);
      rsubstVal /= (double)fCnt;
    }
  }

  ulong start = 0;
  ulong end = sig.countGet();
  if( m_flags & static_cast<ulong>(EsMathConvolutionFlag::SignalIsStable) )
  {
    out.countSet( sig.countGet() );
  }
  else
  {
    out.countSet( sig.countGet() - 2*fCnt );
    start = fCnt;
    end = sig.countGet()-fCnt;
  }

  ulong idx = 0;
  for( ulong sidx = start; sidx < end; ++sidx )
  {
    double acc = 0;
    for( ulong fidx = 0; fidx < m_filter.countGet(); ++fidx )
    {
      double sv;
      ulong ssidx = sidx+fidx;
      if( ssidx < fCnt )
        sv = lsubstVal;
      else
      {
        ssidx -= fCnt;
        if( ssidx >= end )
          sv = rsubstVal;
        else
          sv = sig.itemGet(ssidx);
      }

      acc += sv * m_filter.itemGet(fidx);
    }

    if( !(m_flags & static_cast<ulong>(EsMathConvolutionFlag::FilterIsNormalized)) )
      acc /= (double)m_filter.countGet();

    out.itemSet(idx++, acc);
  }
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathConvolutionReal::NEW()
{
  std::unique_ptr<EsMathConvolutionReal> p( new EsMathConvolutionReal );
  ES_ASSERT(p.get());
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathConvolutionReal::NEW(cr_EsVariant filter)
{
  std::unique_ptr<EsMathConvolutionReal> p( new EsMathConvolutionReal );
  ES_ASSERT(p.get());
  p->m_dynamic = true;
  p->set_filter(filter);

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathConvolutionReal::NEW(cr_EsVariant filter, ulong flags)
{
  std::unique_ptr<EsMathConvolutionReal> p( new EsMathConvolutionReal );
  ES_ASSERT(p.get());
  p->m_dynamic = true;
  p->set_filter(filter);
  p->set_flags(flags);

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsVariant EsMathConvolutionReal::process(cr_EsVariant in) const
{
  std::unique_ptr<EsMathArrayReal> out( new EsMathArrayReal );
  ES_ASSERT(out);
  out->m_dynamic = true;

  EsMathArrayReal* sig = EsMathArrayReal::fromVariant(in);
  bool needFree = false;
  if( !sig )
  {
    sig = new EsMathArrayReal;
    ES_ASSERT(sig);
    sig->set_items(in);
    needFree = true;
  }

  try
  {
    process(*sig, *out.get());
  }
  catch(...)
  {
    if( needFree )
      delete sig;

    throw;
  }

  if( needFree )
    delete sig;

  return out.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

bool EsMathConvolutionReal::get_ok() const
{
  return isOk();
}
//---------------------------------------------------------------------------

ulong EsMathConvolutionReal::get_flags() const
{
  return m_flags;
}
//---------------------------------------------------------------------------

void EsMathConvolutionReal::set_flags(const ulong& flags)
{
  m_flags = flags;
}
//---------------------------------------------------------------------------

double EsMathConvolutionReal::get_substL() const
{
  return m_substL;
}
//---------------------------------------------------------------------------

void EsMathConvolutionReal::set_substL(const double& l)
{
  m_substL = l;
}
//---------------------------------------------------------------------------

double EsMathConvolutionReal::get_substR() const
{
  return m_substR;
}
//---------------------------------------------------------------------------

void EsMathConvolutionReal::set_substR(const double& r)
{
  m_substR = r;
}
//---------------------------------------------------------------------------

EsVariant EsMathConvolutionReal::get_filter() const
{
  return m_filter.get_items();
}
//---------------------------------------------------------------------------

void EsMathConvolutionReal::set_filter(const EsVariant& filter)
{
  m_filter.set_items(filter);
}
//---------------------------------------------------------------------------

