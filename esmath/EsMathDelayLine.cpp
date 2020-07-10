#include "esmathpch.h"
#pragma hdrstop

#include "EsMathDelayLine.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathDelayLine, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, isEmpty, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, isSaturated, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, itemCountGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, accumulatorGet, double_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, clear, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, itemAppend, void_Call_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLine, valueGet, double_CallConst, NO_METHOD_DESCR)
  // Reflected properties
  ES_DECL_PROP_INFO_RO(             EsMathDelayLine, empty, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLine, saturated, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLine, count, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLine, accumulator, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLine, value, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathDelayLine, length, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathDelayLine, EsBaseIntfPtr_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsMathDelayLine::EsMathDelayLine(ulong length) :
m_length(0),
m_accum(0)
{
  internalLengthSet(length);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathDelayLine::NEW( cr_EsVariant length )
{
  std::unique_ptr<EsMathDelayLine> p = ES_MAKE_UNIQUE(
    EsMathDelayLine,
    length.asULong()
  );
  ES_ASSERT(p);

  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

bool EsMathDelayLine::isEmpty() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_buff.empty();
}
//---------------------------------------------------------------------------

bool EsMathDelayLine::isSaturated() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_length == m_buff.size();
}
//---------------------------------------------------------------------------

ulong EsMathDelayLine::itemCountGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return static_cast<ulong>(m_buff.size());
}
//---------------------------------------------------------------------------

double EsMathDelayLine::accumulatorGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_accum;
}
//---------------------------------------------------------------------------

void EsMathDelayLine::clear()
{
  EsCriticalSectionLocker lock(m_cs);
  internalClear();
}
//---------------------------------------------------------------------------

void EsMathDelayLine::internalClear()
{
  m_accum = 0;
  m_buff.clear();
}
//---------------------------------------------------------------------------

void EsMathDelayLine::internalItemAppend(double val)
{
  if( m_length == m_buff.size() )
  {
    double old = m_buff.front();
    m_buff.pop_front();
    m_accum -= old;
  }

  m_buff.push_back(val);
  m_accum += val;
}
//---------------------------------------------------------------------------

void EsMathDelayLine::itemAppend(double val)
{
  EsCriticalSectionLocker lock(m_cs);
  internalItemAppend(val);
}
//---------------------------------------------------------------------------

double EsMathDelayLine::internalValueGet() const
{
  ulong n = static_cast<ulong>(m_buff.size());
  if( 0 == n )
    n = 1;

  return m_accum / static_cast<double>(n);
}
//---------------------------------------------------------------------------

void EsMathDelayLine::internalLengthSet(ulong length)
{
  if( !length )
    EsMathException::Throw(
      esT("Could not create delay line of zero length")
    );

  if( m_length != length )
  {
    internalClear();
    m_length = length;
  }
}
//---------------------------------------------------------------------------

double EsMathDelayLine::valueGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return internalValueGet();
}
//---------------------------------------------------------------------------

bool EsMathDelayLine::get_empty() const
{
  return isEmpty();
}
//---------------------------------------------------------------------------

bool EsMathDelayLine::get_saturated() const
{
  return isSaturated();
}
//---------------------------------------------------------------------------

ulong EsMathDelayLine::get_count() const
{
  return itemCountGet();
}
//---------------------------------------------------------------------------

double EsMathDelayLine::get_accumulator() const
{
  return accumulatorGet();
}
//---------------------------------------------------------------------------

double EsMathDelayLine::get_value() const
{
  return valueGet();
}
//---------------------------------------------------------------------------

ulong EsMathDelayLine::get_length() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_length;
}
//---------------------------------------------------------------------------

void EsMathDelayLine::set_length(const ulong& length)
{
  EsCriticalSectionLocker lock(m_cs);
  internalLengthSet(length);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
ES_DECL_BASE_CLASS_INFO_BEGIN(EsMathDelayLineEx, NO_CLASS_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, isEmpty, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, isSaturated, bool_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, itemCountGet, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, accumulatorGet, double_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, clear, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, itemAppend, void_Call_double, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, valueGet, double_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsMathDelayLineEx, minimaxReset, void_Call, NO_METHOD_DESCR)
  // Reflected properties
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, empty, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, saturated, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, count, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, accumulator, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, value, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, min, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, max, double, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(             EsMathDelayLineEx, minimaxCount, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathDelayLineEx, minimaxResetThreshold, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0xFFFFFFFF), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                EsMathDelayLineEx, length, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  // Constructors
  ES_DECL_REFLECTED_CTOR_INFO(      EsMathDelayLineEx, EsBaseIntfPtr_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsMathDelayLineEx::EsMathDelayLineEx(ulong length) :
m_length( 0 ),
m_accum(0),
m_min( 0 ),
m_max( 0 ),
m_minimaxCnt( 0 ),
m_minimaxResetThreshold( 0xFFFFFFFF ),
m_minimaxReset( true )
{
  internalLengthSet(length);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsMathDelayLineEx::NEW( cr_EsVariant length )
{
  std::unique_ptr<EsMathDelayLineEx> p = ES_MAKE_UNIQUE(
    EsMathDelayLineEx,
    length.asULong()
  );
  ES_ASSERT(p);
  
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

bool EsMathDelayLineEx::isEmpty() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_buff.empty();
}
//---------------------------------------------------------------------------

bool EsMathDelayLineEx::isSaturated() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_length == m_buff.size();
}
//---------------------------------------------------------------------------

ulong EsMathDelayLineEx::itemCountGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return static_cast<ulong>(m_buff.size());
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::accumulatorGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_accum;
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::clear()
{
  EsCriticalSectionLocker lock(m_cs);
  internalClear();
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::internalClear()
{
  m_accum = 0;
  m_buff.clear();
  internalMinimaxReset();
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::internalItemAppend(double val)
{
  // Automatic minimax reset
  if( m_minimaxResetThreshold <= m_minimaxCnt )
    internalMinimaxReset();

  if( m_buff.empty() )
  {
    m_min = val;
    m_max = val;
    m_minimaxReset = false;
  }
  else
  {
    calcMinimaxIfReset();

    if( val > m_max )
      m_max = val;
    if( val < m_min )
      m_min = val;
  }

  if( m_length == m_buff.size() )
  {
    double old = m_buff.front();
    m_buff.pop_front();
    m_accum -= old;
  }

  m_buff.push_back(val);
  m_accum += val;

  ++m_minimaxCnt;
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::itemAppend(double val)
{
  EsCriticalSectionLocker lock(m_cs);
  internalItemAppend(val);
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::internalValueGet() const
{
  ulong n = static_cast<ulong>(m_buff.size());
  if( 0 == n )
    n = 1;

  return m_accum / static_cast<double>(n);
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::valueGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return internalValueGet();
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::internalLengthSet(ulong length)
{
  if( !length )
    EsMathException::Throw(
      esT("Could not create delay line of zero length")
    );

  if( m_length != length )
  {
    internalClear();
    m_length = length;
  }
}
//---------------------------------------------------------------------------

bool EsMathDelayLineEx::get_empty() const
{
  return isEmpty();
}
//---------------------------------------------------------------------------

bool EsMathDelayLineEx::get_saturated() const
{
  return isSaturated();
}
//---------------------------------------------------------------------------

ulong EsMathDelayLineEx::get_count() const
{
  return itemCountGet();
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::get_accumulator() const
{
  return accumulatorGet();
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::get_value() const
{
  return valueGet();
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::get_min() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_min;
}
//---------------------------------------------------------------------------

double EsMathDelayLineEx::get_max() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_max;
}
//---------------------------------------------------------------------------

ulong EsMathDelayLineEx::get_minimaxCount() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_minimaxCnt;
}
//---------------------------------------------------------------------------

ulong EsMathDelayLineEx::get_length() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_length;
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::set_length(const ulong& length)
{
  EsCriticalSectionLocker lock(m_cs);
  internalLengthSet(length);
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::internalMinimaxReset()
{
  m_minimaxReset = true;
  m_minimaxCnt = 0;
  if( m_buff.empty() )
    m_min = m_max = 0;
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::minimaxReset()
{
  EsCriticalSectionLocker lock(m_cs);
  internalMinimaxReset();
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::set_minimaxResetThreshold(const ulong& threshold)
{
  EsCriticalSectionLocker lock(m_cs);

  ulong tr = threshold;
  if( 0 == tr )
    tr = 0xFFFFFFFF;

  if( tr != m_minimaxResetThreshold )
  {
    m_minimaxResetThreshold = tr;
    internalMinimaxReset();
  }
}
//---------------------------------------------------------------------------

ulong EsMathDelayLineEx::get_minimaxResetThreshold() const
{
  return m_minimaxResetThreshold;
}
//---------------------------------------------------------------------------

void EsMathDelayLineEx::calcMinimaxIfReset()
{
  if( m_minimaxReset )
  {
    BuffT::const_iterator cit = m_buff.begin();
    m_min = m_max = (*cit++);
    ++m_minimaxCnt;
    while( cit != m_buff.end() )
    {
      double v = (*cit++);
      if( m_min > v )
        m_min = v;
      if( m_max < v )
        m_max = v;
      ++m_minimaxCnt;
    }

    m_minimaxReset = false;
  }
}
//---------------------------------------------------------------------------

