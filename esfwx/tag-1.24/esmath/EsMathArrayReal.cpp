#include "esmathpch.h"
#pragma hdrstop

#include "EsMathArrayReal.h"
#include "EsMathInternals.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathArrayReal, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, clone, EsBaseIntfPtr_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, minimaxRecalc, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, clear, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fill, void_Call_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, add, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, add_inplace, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, subtract, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, subtract_inplace, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, multiply, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, multiply_inplace, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, divide, EsVariant_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, divide_inplace, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, countGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, countSet, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, itemGet, double_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, itemSet, void_Call_ulong_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromU8buffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromI8buffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromU16buffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromI16buffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromU32buffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromI32buffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromFloatBuffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromDoubleBuffer, bool_Call_cr_EsBinBuffer, NO_METHOD_DESCR)
   ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromU8buffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromI8buffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromU16buffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromI16buffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromU32buffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromI32buffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromFloatBuffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, fromDoubleBuffer, bool_Call_cr_EsBinBuffer_double_double_bool_bool, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, asString, EsString_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, streamRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(      EsMathArrayReal, streamWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  // Reflected properties
  ES_DECL_PROP_INFO(                      EsMathArrayReal, dirty, bool, NO_PROPERTY_LABEL, true, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsMathArrayReal, empty, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsMathArrayReal, min, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsMathArrayReal, max, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsMathArrayReal, minIdx, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsMathArrayReal, maxIdx, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsMathArrayReal, total, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsMathArrayReal, count, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsMathArrayReal, items, EsVariant, NO_PROPERTY_LABEL, EsVariant(EsVariant::VAR_VARIANT_COLLECTION), NO_PROPERTY_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(            EsMathArrayReal, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(            EsMathArrayReal, EsBaseIntfPtr_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  // Pseudo-constructors
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromU8buffer,     EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromI8buffer,     EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromU16buffer,    EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromI16buffer,    EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromU32buffer,    EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromI32buffer,    EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromFloatBuffer,  EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsMathArrayReal, newFromDoubleBuffer, EsBaseIntfPtr_ClassCall_cr_EsBinBuffer, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

// real data array
EsMathArrayReal::EsMathArrayReal(ulong size /*= 0*/, const double* data /*= 0*/) :
m_pimpl(0),
m_needRecalc(true),
m_minIdx(0),
m_maxIdx(0),
m_total(0)
{
  m_pimpl = new alglib::real_1d_array;
  ES_ASSERT(m_pimpl);

  if( size && data )
    dataSet(size, data);
  else if( size && 0 == data )
  {
    countSet( size );
    fill(0);
  }
}
//---------------------------------------------------------------------------

EsMathArrayReal::EsMathArrayReal(const EsMathArrayReal& src) :
m_pimpl(0),
m_needRecalc(true),
m_minIdx(0),
m_maxIdx(0),
m_total(0)
{
  m_pimpl = new alglib::real_1d_array;
  ES_ASSERT(m_pimpl);

  copy(src);
}
//---------------------------------------------------------------------------

EsMathArrayReal::~EsMathArrayReal()
{
  delete (alglib::real_1d_array*)m_pimpl;
  m_pimpl = 0;
}
//---------------------------------------------------------------------------

/// "Is dirty" flag
bool EsMathArrayReal::get_dirty() const
{
  return m_needRecalc;
}
//---------------------------------------------------------------------------

void EsMathArrayReal::set_dirty(const bool& dirty)
{
  m_needRecalc = dirty;
}
//---------------------------------------------------------------------------

/// emptiness check
bool EsMathArrayReal::get_empty() const
{
  return 0 == countGet();
}
//---------------------------------------------------------------------------

/// size access
ulong EsMathArrayReal::get_count() const
{
  return countGet();
}
//---------------------------------------------------------------------------

void EsMathArrayReal::set_count(const ulong& newCount)
{
  ES_ASSERT(m_pimpl);
  countSet( newCount );
}
//---------------------------------------------------------------------------

/// fill array with specified value
void EsMathArrayReal::fill(double val)
{
  ES_ASSERT(m_pimpl);
  double* pos = dataAccess();
  double* end = pos+countGet();

  while(pos < end)
    *pos++ = val;

  set_dirty(true);
}
//---------------------------------------------------------------------------

EsMathArrayReal* EsMathArrayReal::fromVariant(const EsVariant& in)
{
  if( in.isObject() )
  {
    EsReflectedClassIntf::Ptr obj  = in.asObject();
    if( obj && obj->isKindOf(classNameGetStatic()) )
      return ES_INTFPTR_TO_OBJECTPTR(obj, EsMathArrayReal);
  }

  return nullptr;
}
//---------------------------------------------------------------------------

template < typename directT >
bool fromBuffer(
  EsMathArrayReal* a,
  const EsBinBuffer& bb,
  bool doThrow = false,
  EsMathArrayReal::value_type rangeMin = 0,
  EsMathArrayReal::value_type rangeMax = 0,
  bool rangeReplace = false
)
{
  ES_ASSERT(a);

  if( bb.empty() )
  {
    a->clear();
    return true;
  }
  else
  {
    if( 0 == (bb.size() % sizeof(directT)) )
    {
      ulong cnt = static_cast<ulong>(bb.size() / sizeof(directT));
      a->countSet( cnt );
      const directT* dd = reinterpret_cast<const directT*>( &bb[0] );

      ES_ASSERT( !esIsNan(rangeMin) && !esIsInf(rangeMin) );
      ES_ASSERT( !esIsNan(rangeMax) && !esIsInf(rangeMax) );

      if( rangeMin > rangeMax )
      {
        EsMathArrayReal::value_type tmp = rangeMin;
        rangeMin = rangeMax;
        rangeMax = tmp;
      }

      bool checkRange = !EsUtilities::areEqualFloats( rangeMin, rangeMax );

      for(ulong idx = 0; idx < cnt; ++idx)
      {
        EsMathArrayReal::value_type val =
          static_cast<EsMathArrayReal::value_type>( dd[idx] );

        if( esIsNan(val) || esIsInf(val) )
        {
          if( doThrow )
            EsMathException::Throw(
              esT("NAN or INF encountered while assigning buffer to real array")
            );
          else
            return false;
        }

        if( checkRange )
        {
          if( rangeReplace )
          {
            if(val < rangeMin )
              val = rangeMin;
            else if( val > rangeMax )
              val = rangeMax;
          }
          else if( val < rangeMin || val > rangeMax )
          {
            if( doThrow )
              EsMathException::Throw(
                esT("Value out of range, while assigning buffer to real array")
              );
            else
              return false;
          }
        }

        a->itemSet(idx, val);
      }

      a->set_dirty(true);
      return true;
    }
    else if( doThrow )
      EsMathException::Throw(
        esT("Could not assign incorrectly aligned buffer to real array")
      );
  }

  // Pacify compilers
  return false;
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromU8buffer(const EsBinBuffer& src)
{
  return fromBuffer<esU8>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromI8buffer(const EsBinBuffer& src)
{
  return fromBuffer<esI8>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromU16buffer(const EsBinBuffer& src)
{
  return fromBuffer<esU16>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromI16buffer(const EsBinBuffer& src)
{
  return fromBuffer<esI16>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromU32buffer(const EsBinBuffer& src)
{
  return fromBuffer<esU32>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromI32buffer(const EsBinBuffer& src)
{
  return fromBuffer<esI32>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromFloatBuffer(const EsBinBuffer& src)
{
  return fromBuffer<float>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromDoubleBuffer(const EsBinBuffer& src)
{
  return fromBuffer<double>(this, src);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromU8buffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<esU8>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromI8buffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<esI8>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromU16buffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<esU16>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromI16buffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<esI16>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromU32buffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<esU32>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromI32buffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<esI32>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromFloatBuffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<float>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

bool EsMathArrayReal::fromDoubleBuffer(const EsBinBuffer& src, double min, double max, bool rangeReplace, bool doThrow)
{
  return fromBuffer<double>(this, src, doThrow, min, max, rangeReplace);
}
//---------------------------------------------------------------------------

EsString EsMathArrayReal::asString(const EsString& fmt, const EsString& sep) const
{
  if( get_empty() )
    return EsString::null();

  EsString fmtstr = esT("%f");
  if( !fmt.empty() )
    fmtstr = fmt;

  EsString sepstr = esT(";");
  if( !sep.empty() )
    sepstr = sep;

  ulong cnt = countGet();
  const double* dd = dataGet();

  EsString result;
  result.reserve(cnt*6);

  for(ulong idx = 0; idx < cnt; ++idx)
  {
    if( !result.empty() )
      result += sepstr;

    result += EsString::format(
      fmtstr.c_str(),
      dd[idx]
    );
  }

  return result;
}
//---------------------------------------------------------------------------

void EsMathArrayReal::streamWrite(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);

  const EsVariant& items = get_items();
  stream->valueWrite( items );
}
//---------------------------------------------------------------------------

void EsMathArrayReal::streamRead(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);

  const EsVariant& items = stream->valueRead();
  set_items(items);
}
//---------------------------------------------------------------------------

void EsMathArrayReal::set_items(const EsVariant& src)
{
  bool copied = false;

  if( src.isEmpty() )
  {
    clear();
    copied = true;
  }
  else
  {
    // Try copy from the object of the same type
    const EsMathArrayReal* sp = fromVariant(src);
    if( sp )
    {
      copy(*sp);
      copied = true;
    }
    else
    {
      if( src.isCollection() )
      {
        const EsVariant::Array& va = src.asVariantCollection();
        ulong cnt = static_cast<ulong>(va.size());
        countSet( cnt );

        for(ulong idx = 0; idx < cnt; ++idx)
          itemSet(idx, va[idx].asDouble());
        copied = true;
      }
      else if( EsVariant::VAR_BIN_BUFFER == src.typeGet() )
        copied = fromBuffer<double>(this, src.asBinBuffer());
      else if( src.isIndexed() ) // Other indexed cases
      {
        // Slow way of copying
        ulong cnt = src.countGet();
        countSet( cnt );
        for( ulong idx = 0; idx < cnt; ++idx )
          itemSet( idx, src.itemGet(idx).asDouble() );

        copied = true;
      }

      if( copied )
        set_dirty(true);
    }
  }

  if( !copied )
    EsException::Throw( esT("EsMathArrayReal items copying failed. Incompatible source data specified.") );
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayReal::get_items() const
{
  EsVariant::Array result( countGet() );

  for( ulong idx = 0; idx < result.size(); ++idx )
    result[idx] = itemGet(idx);

  return result;
}
//---------------------------------------------------------------------------

/// Array resize
/// NB! array contents are not guaranteed to be preserved
///
void EsMathArrayReal::countSet(ulong newSize)
{
  if( get_count() != newSize )
  {
    ES_ALGLIB_TRY
    ((alglib::real_1d_array*)m_pimpl)->setlength(newSize);
    set_dirty(true);
    ES_ALGLIB_CATCH
  }
}
//---------------------------------------------------------------------------

ulong EsMathArrayReal::countGet() const
{
  ES_ASSERT(m_pimpl);
  return static_cast<ulong>(((const alglib::real_1d_array*)m_pimpl)->length());
}
//---------------------------------------------------------------------------

/// minimax access
ulong EsMathArrayReal::get_minIdx() const
{
  const_cast<EsMathArrayReal*>(this)->minimaxRecalc();
  return m_minIdx;
}
//---------------------------------------------------------------------------

ulong EsMathArrayReal::get_maxIdx() const
{
  const_cast<EsMathArrayReal*>(this)->minimaxRecalc();
  return m_maxIdx;
}
//---------------------------------------------------------------------------

double EsMathArrayReal::get_total() const
{
  const_cast<EsMathArrayReal*>(this)->minimaxRecalc();
  return m_total;
}
//---------------------------------------------------------------------------

double EsMathArrayReal::get_min() const
{
  return itemGet(get_minIdx());
}
//---------------------------------------------------------------------------

double EsMathArrayReal::get_max() const
{
  return itemGet(get_maxIdx());
}
//---------------------------------------------------------------------------

/// indexed item acess operators
double EsMathArrayReal::itemGet(ulong idx) const
{
  ES_ASSERT(idx < get_count());
  return ((const alglib::real_1d_array*)m_pimpl)->operator[](idx);
}
//---------------------------------------------------------------------------

void EsMathArrayReal::itemSet(ulong idx, double val)
{
  ES_ASSERT(idx < get_count());
  ((alglib::real_1d_array*)m_pimpl)->operator[](idx) = val;
  set_dirty(true);
}
//---------------------------------------------------------------------------

/// minimax recalculation
void EsMathArrayReal::minimaxRecalc()
{
  if( m_needRecalc )
  {
    m_total = 0;
    m_minIdx = m_maxIdx = 0;
    if( !get_empty() )
    {
      double min = itemGet(0);
      double max = min;
      m_total = min;
      for(ulong idx = 1; idx < get_count(); ++idx)
      {
        double x = itemGet(idx);
        if( max < x )
        {
          max = x;
          m_maxIdx = idx;
        }

        if( min > x )
        {
          min = x;
          m_minIdx = idx;
        }

        m_total += x;
      }
    }

    m_needRecalc = false;
  }
}
//---------------------------------------------------------------------------

/// array cleanup
void EsMathArrayReal::clear()
{
  countSet(0);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::clone() const
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal(*this) );
  ES_ASSERT(p);
  
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::baseIntfPtrCreate(cr_EsVariant src, EsMathArrayReal** outp)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  if( !src.isEmpty() )
    p->set_items(src);

  if( outp )
    *outp = p.get();

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

/// Default constructor. Return empty array of size 0
EsBaseIntfPtr EsMathArrayReal::NEW()
{
  return baseIntfPtrCreate(
    EsVariant::null(),
    nullptr
  );
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::NEW(cr_EsVariant src)
{
  return baseIntfPtrCreate(
    src,
    nullptr
  );
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromU8buffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);

  p->m_dynamic = true;
  fromBuffer<esU8>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromI8buffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);

  p->m_dynamic = true;
  fromBuffer<esI8>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromU16buffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);

  p->m_dynamic = true;
  fromBuffer<esU16>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromI16buffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);

  p->m_dynamic = true;
  fromBuffer<esI16>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromU32buffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  fromBuffer<esU32>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromI32buffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);

  p->m_dynamic = true;
  fromBuffer<esI32>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromFloatBuffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  fromBuffer<float>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathArrayReal::newFromDoubleBuffer(const EsBinBuffer& src)
{
  std::unique_ptr<EsMathArrayReal> p( new EsMathArrayReal );
  ES_ASSERT(p);
  
  p->m_dynamic = true;
  fromBuffer<double>(p.get(), src, true);

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

const double* EsMathArrayReal::dataGet() const
{
  if( !get_empty() )
    return ((const alglib::real_1d_array*)m_pimpl)->getcontent();

  return nullptr;
}
//---------------------------------------------------------------------------

double* EsMathArrayReal::dataAccess()
{
  if( !get_empty() )
    return ((alglib::real_1d_array*)m_pimpl)->getcontent();

  return nullptr;
}
//---------------------------------------------------------------------------

void EsMathArrayReal::dataSet(ulong size, const double* data)
{
  ES_ASSERT(m_pimpl);
  ((alglib::real_1d_array*)m_pimpl)->setcontent(size, data);
  set_dirty(true);
}
//---------------------------------------------------------------------------

/// assignment operator
EsMathArrayReal& EsMathArrayReal::operator=(const EsMathArrayReal& src)
{
  copy(src);
  return *this;
}
//---------------------------------------------------------------------------

void EsMathArrayReal::copy(const EsMathArrayReal& src)
{
  if( &src != this )
  {
    alglibCastFromArray(*this).setcontent( src.get_count(),
    alglibCastFromArray(src).getcontent() );

    m_needRecalc = src.m_needRecalc;
    m_minIdx = src.m_minIdx;
    m_maxIdx = src.m_maxIdx;
    m_total = src.m_total;
  }
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayReal::add(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        arg+clone->itemGet(idx)
      );
    }

    return result;
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx) + src->itemGet(idx)
      );
    }

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsMathArrayReal::add_inplace(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      itemSet(
        idx,
        arg+itemGet(idx)
      );
    }
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      itemSet(
        idx,
        itemGet(idx) + src->itemGet(idx)
      );
    }
  }
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayReal::subtract(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx)-arg
      );
    }

    return result;
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx) - src->itemGet(idx)
      );
    }

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsMathArrayReal::subtract_inplace(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      itemSet(
        idx,
        itemGet(idx)-arg
      );
    }
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      itemSet(
        idx,
        itemGet(idx) - src->itemGet(idx)
      );
    }
  }
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayReal::multiply(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx) * arg
      );
    }

    return result;
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx) * src->itemGet(idx)
      );
    }

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsMathArrayReal::multiply_inplace(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      double val = itemGet(idx);
      val *= arg;

      itemSet(
        idx,
        val
      );
    }
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      double valOther = src->itemGet(idx);
      double val = itemGet(idx);
      val *= valOther;

      itemSet(
        idx,
        val
      );
    }
  }
}
//---------------------------------------------------------------------------

EsVariant EsMathArrayReal::divide(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx) / arg
      );
    }

    return result;
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    EsMathArrayReal* clone = nullptr;
    EsBaseIntfPtr result = baseIntfPtrCreate(
      asBaseIntfPtrWeak(),
      &clone
    );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      clone->itemSet(
        idx,
        clone->itemGet(idx) / src->itemGet(idx)
      );
    }

    return result;
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsMathArrayReal::divide_inplace(const EsVariant& v)
{
  ulong cnt = countGet();
  EsMathArrayReal* src = fromVariant(v);
  if( !src )
  {
    double arg = v.asDouble();

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      itemSet(
        idx,
        itemGet(idx)/arg
      );
    }
  }
  else
  {
    ulong srccnt = src->countGet();
    if( cnt != srccnt )
      EsMathException::ThrowArraySizesDoNotMatch( cnt, srccnt );

    for( ulong idx = 0; idx < cnt; ++idx )
    {
      itemSet(
        idx,
        itemGet(idx)/src->itemGet(idx)
      );
    }
  }
}
//---------------------------------------------------------------------------

