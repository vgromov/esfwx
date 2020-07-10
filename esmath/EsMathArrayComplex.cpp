#include "esmathpch.h"
#pragma hdrstop

#include "EsMathArrayComplex.h"
#include "EsMathInternals.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathArrayComplex, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathArrayComplex, clear, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathArrayComplex, fill, void_Call_double_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathArrayComplex, fill, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathArrayComplex, countGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathArrayComplex, countSet, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(    EsMathArrayComplex, _itemGet, itemGet, EsVariant_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathArrayComplex, itemSet, void_Call_ulong_cr_EsVariant, NO_METHOD_DESCR)
  // Reflected properties
  ES_DECL_PROP_INFO_RO(             EsMathArrayComplex, empty, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathArrayComplex, count, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(     EsMathArrayComplex, items, EsVariant, NO_PROPERTY_LABEL, EsVariant(EsVariant::VAR_VARIANT_COLLECTION), NO_PROPERTY_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathArrayComplex, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathArrayComplex, EsBaseIntfPtr_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

//---------------------------------------------------------------------------
/// Complex array implementation
///

/// math-specific array of complex values
EsMathArrayComplex::EsMathArrayComplex(ulong size /*= 0*/) :
m_pimpl(0)
{
  m_pimpl = new alglib::complex_1d_array;
  ES_ASSERT(m_pimpl);

  countSet(size);
}
//---------------------------------------------------------------------------

EsMathArrayComplex::EsMathArrayComplex(const EsMathArrayComplex& src) :
m_pimpl(0)
{
  m_pimpl = new alglib::complex_1d_array;
  ES_ASSERT(m_pimpl);

  copy(src);
}
//---------------------------------------------------------------------------

EsMathArrayComplex::~EsMathArrayComplex()
{
  delete (alglib::complex_1d_array*)m_pimpl;
  m_pimpl = 0;
}
//---------------------------------------------------------------------------

/// indexed item acess operators
const EsMath::Complex& EsMathArrayComplex::itemGet(ulong idx) const
{
  ES_ASSERT(idx < countGet());
  return *(const EsMath::Complex*)&(((const alglib::complex_1d_array*)m_pimpl)->operator[](idx));
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::itemSet(ulong idx, const EsMath::Complex& val)
{
  ES_ASSERT(idx < countGet());
  ((alglib::complex_1d_array*)m_pimpl)->operator[](idx) = (const alglib::complex&)val;
}
//---------------------------------------------------------------------------

/// direct data access
const EsMath::Complex* EsMathArrayComplex::dataGet() const
{
  if( !get_empty() )
    return (const EsMath::Complex*)(((const alglib::complex_1d_array*)m_pimpl)->getcontent());

  return 0;
}
//---------------------------------------------------------------------------

EsMath::Complex* EsMathArrayComplex::dataAccess()
{
  if( !get_empty() )
    return (EsMath::Complex*)(((alglib::complex_1d_array*)m_pimpl)->getcontent());

  return 0;
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::dataSet(ulong size, const EsMath::Complex* data)
{
  ES_ASSERT(m_pimpl);
  alglibCastFromArray(*this).setcontent( 
    size, 
    (const alglib::complex*)data 
  );
}
//---------------------------------------------------------------------------

/// assignment operator
EsMathArrayComplex& EsMathArrayComplex::operator=(const EsMathArrayComplex& src)
{
  copy(src);
  return *this;
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::copy(const EsMathArrayComplex& src)
{
  if( &src != this )
  {
    alglibCastFromArray(*this).setcontent( src.countGet(),
      alglibCastFromArray(src).getcontent() );
  }
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayComplex::clone() const
{
  std::unique_ptr<EsMathArrayComplex> p = ES_MAKE_UNIQUE(
    EsMathArrayComplex,
    *this
  );
  ES_ASSERT(p);
  
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayComplex::NEW()
{
  std::unique_ptr<EsMathArrayComplex> p = ES_MAKE_UNIQUE( EsMathArrayComplex );
  ES_ASSERT(p);
  
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayComplex::NEW(cr_EsVariant src)
{
  std::unique_ptr<EsMathArrayComplex> p = ES_MAKE_UNIQUE( EsMathArrayComplex );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  p->set_items(src);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

bool EsMathArrayComplex::get_empty() const
{
  return 0 == countGet();
}
//---------------------------------------------------------------------------

ulong EsMathArrayComplex::get_count() const
{
  return countGet();
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::set_count(const ulong& size)
{
  countSet(size);
}
//---------------------------------------------------------------------------

ulong EsMathArrayComplex::countGet() const
{
  ES_ASSERT(m_pimpl);
  return static_cast<ulong>(((const alglib::complex_1d_array*)m_pimpl)->length());
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::countSet(ulong newSize)
{
  if( countGet() != newSize )
  {
    ES_ALGLIB_TRY
    ((alglib::complex_1d_array*)m_pimpl)->setlength(newSize);
    ES_ALGLIB_CATCH
  }
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::fill(double re, double im)
{
  EsMath::Complex* pc = dataAccess();
  for(ulong idx = 0; idx < countGet(); ++idx)
  {
    pc->m_re = re; pc->m_im = im;
    ++pc;
  }
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::fill(cr_EsVariant val)
{
  fill( val.itemGet(0).asDouble(), val.itemGet(1).asDouble() );
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::clear()
{
  countSet(0);
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayComplex::_itemGet(ulong idx) const
{
  const EsMath::Complex& val = itemGet(idx);
  EsVariant::Array va(2);
  va[0] = val.m_re;
  va[1] = val.m_im;

  return va;
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::itemSet(ulong idx, cr_EsVariant val)
{
  EsMath::Complex cv(val.itemGet(0).asDouble(), val.itemGet(1).asDouble());

  itemSet(idx, cv);
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayComplex::get_items() const
{
  EsVariant::Array result( countGet() );

  for( ulong idx = 0; idx < result.size(); ++idx )
  {
    const EsVariant& node = _itemGet(idx);
    result[idx] = node;
  }

  return result;
}
//---------------------------------------------------------------------------

void EsMathArrayComplex::set_items(const EsVariant& src)
{
  bool copied = false;

  const EsMathArrayComplex* sp = ES_VARIANT_TO_OBJECTPTR<EsMathArrayComplex>(src);
  // Try copy from object of the same type
  if( sp )
  {
    copy(*sp);
    copied = true;
  }

  // Try copy from indexed source
  if( !copied && src.isIndexed() )
  {
    // Slow way of copying
    countSet( src.countGet() );
    for( ulong idx = 0; idx < get_count(); ++idx )
      itemSet( idx, src.itemGet(idx) );

    copied = true;
  }

  if( !copied )
    EsException::Throw(0, EsException::severityGeneric, EsException::facilityEsMath,
      esT("EsMathArrayComplex copy failed. Incompatible source data specified."));
}
//---------------------------------------------------------------------------

