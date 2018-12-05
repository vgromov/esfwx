#include "escorepch.h"
#pragma hdrstop

#include <esfwxe/datetime.h>
#include <esfwxe/datetime.c>

#include "EsDateTime.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

/// internal flags
namespace EsInternal {

enum {
  flagNativeChanged     = 0x02, ///< we've changed native storage, need re-decompose.
  flagDecompChanged     = 0x04,  ///< we've changed decomposed storage, need to re-compose it.
  flagInitialized        = 0x08,  ///< object initialization complete
  flagDayOfYearInvalid  = 0x10  ///< Day of year DateTime field is not valid - need to recompose and re-decompose
};

}
//---------------------------------------------------------------------------

// date time span class
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsTimeSpan, NO_CLASS_DESCR)
  // services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, asString, EsString_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, abs, EsVariant_CallConst, NO_METHOD_DESCR)
  // reflected operators
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, compare, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, add, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, subtract, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, multiply, EsVariant_CallConst_llong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, divide, EsVariant_CallConst_llong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, clone, EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, streamRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsTimeSpan, streamWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  // reflected properties
  ES_DECL_PROP_INFO(EsTimeSpan, value, EsVariant, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsTimeSpan, negative, bool, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsTimeSpan, days, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsTimeSpan, hours, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsTimeSpan, minutes, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsTimeSpan, seconds, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsTimeSpan, milliseconds, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsTimeSpan, allDays, llong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsTimeSpan, allHours, llong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsTimeSpan, allMinutes, llong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsTimeSpan, allSeconds, llong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsTimeSpan, allMilliseconds, llong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // constructors
  ES_DECL_REFLECTED_CTOR_INFO(EsTimeSpan, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsTimeSpan::EsTimeSpan() :
m_flags(EsInternal::flagNativeChanged),
m_dt(0),
m_days(0),
m_hours(0),
m_mins(0),
m_secs(0),
m_msecs(0)
{
  ES_REFLECTED_PROPERTIES_RESET;
  m_flags |= EsInternal::flagInitialized;
}
//---------------------------------------------------------------------------

EsTimeSpan::EsTimeSpan(const EsTimeSpan& other) :
m_flags(other.m_flags),
m_dt(other.m_dt),
m_days(other.m_days),
m_hours(other.m_hours),
m_mins(other.m_mins),
m_secs(other.m_secs),
m_msecs(other.m_msecs)
{
}
//---------------------------------------------------------------------------

EsTimeSpan::EsTimeSpan(esDT dt) :
m_flags(EsInternal::flagNativeChanged),
m_dt(dt),
m_days(0),
m_hours(0),
m_mins(0),
m_secs(0),
m_msecs(0)
{
  if( dt == 0 )
    ES_REFLECTED_PROPERTIES_RESET;

   m_flags |= EsInternal::flagInitialized;
}
//---------------------------------------------------------------------------

EsTimeSpan::EsTimeSpan(const EsVariant& src) :
m_flags(EsInternal::flagNativeChanged),
m_dt(0),
m_days(0),
m_hours(0),
m_mins(0),
m_secs(0),
m_msecs(0)
{
  m_dt = fromVariant(src);

  if( m_dt == 0 )
    ES_REFLECTED_PROPERTIES_RESET;

   m_flags |= EsInternal::flagInitialized;
}
//---------------------------------------------------------------------------

// internal initializer
EsBaseIntfPtr EsTimeSpan::create(esDT dt)
{
  std::unique_ptr<EsTimeSpan> p( new EsTimeSpan(dt) );
  ES_ASSERT(p.get());
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::clone() const
{
  return create(
    this->operator esDT()
  );
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsTimeSpan::NEW()
{
  return create(0);
}
//---------------------------------------------------------------------------

// composition|decomposition checkers
void EsTimeSpan::checkComposed() const
{
  if( m_flags & EsInternal::flagDecompChanged )
  {
    // compose time span from components
    m_dt =
      (esDT)m_days * IMMEDIATE_INT64(86400000) +
      (esDT)m_hours * IMMEDIATE_INT64(3600000) +
      (esDT)m_mins * IMMEDIATE_INT64(60000) +
      (esDT)m_secs * IMMEDIATE_INT64(1000) +
      (esDT)m_msecs;

    m_flags &= ~EsInternal::flagDecompChanged;
  }
}
//---------------------------------------------------------------------------

void EsTimeSpan::checkDecomposed() const
{
  if( m_flags & EsInternal::flagNativeChanged )
  {
    // decompose native into components
    esDT dt = m_dt;
    long tmp = (long)(dt / IMMEDIATE_INT64(86400000));
    dt = dt % IMMEDIATE_INT64(86400000);
    if( tmp < 0 )
      m_days = -tmp;
    else
      m_days = tmp;
    tmp = (long)(dt / IMMEDIATE_INT64(3600000));
    dt = dt % IMMEDIATE_INT64(3600000);
    if( tmp < 0 )
      m_hours = -tmp;
    else
      m_hours = tmp;
    tmp = (long)(dt / IMMEDIATE_INT64(60000));
    dt = dt % IMMEDIATE_INT64(60000);
    if( tmp < 0 )
      m_mins = -tmp;
    else
      m_mins = tmp;
    tmp = (long)(dt / IMMEDIATE_INT64(1000));
    dt = dt % IMMEDIATE_INT64(1000);
    if( tmp < 0 )
      m_secs = -tmp;
    else
      m_secs = tmp;
    tmp = (long)dt;
    if( tmp < 0 )
      m_msecs = -tmp;
    else
      m_msecs = tmp;

    m_flags &= ~EsInternal::flagNativeChanged;
  }
}
//---------------------------------------------------------------------------

long EsTimeSpan::cmp(const EsTimeSpan* other) const
{
  ES_ASSERT(other);
  checkComposed();
  other->checkComposed();
  return (m_dt < other->m_dt) ? -1 : ((m_dt == other->m_dt) ? 0 : 1);
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::abs() const
{
  checkComposed();
  return EsTimeSpan::create( m_dt < 0 ? -m_dt : m_dt );
}
//---------------------------------------------------------------------------

void EsTimeSpan::streamWrite(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);
  checkComposed();
  stream->valueWrite( m_dt );
}
//---------------------------------------------------------------------------

void EsTimeSpan::streamRead(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);
  m_dt = stream->valueRead().asLLong();
  m_flags |= EsInternal::flagNativeChanged;
  m_flags &= ~EsInternal::flagDecompChanged;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

enum EsTimeSpanPart
{
  week,
  day,
  h,
  m,
  s,
  ms
};
//---------------------------------------------------------------------------

EsString EsTimeSpan::asString(const EsString& fmt) const
{
  if( fmt.empty() )
    EsException::Throw( esT("TimeSpan format string must not be empty") );

  checkDecomposed();

  EsString str;
  str.reserve( fmt.size() );

  // set the most significant part found so far
  EsTimeSpanPart mspart = EsTimeSpanPart::ms;
  for( size_t idx = 0; idx < fmt.size(); ++idx )
  {
    EsString::value_type ch = fmt[idx];

    if( esT('%') == ch )
    {
      // the start of the format specification of the printf() below
      EsString fmtPfx( esT('%') );
      // the number being formatted
      ulong n;
      // the number of digits for the format string, 0 if unused
      ulong digits = 0;

      ch = fmt[++idx];    // get the format spec char
      switch( ch )
      {
      default:
        EsException::Throw( esT("TimeSpan invalid format character encountered: '%c'"), ch );
        // NB! fallthrough
      case esT('%'):
        str += ch;
        // skip to the nest iteration
        continue;
      case esT('D'):
        n = get_days();
        if( mspart < EsTimeSpanPart::day )
          n %= 7;
        else
          mspart = EsTimeSpanPart::day;
        break;
      case esT('E'):
        mspart = EsTimeSpanPart::week;
        n = get_weeks();
        break;
      case esT('H'):
        n = get_hours();
        if( mspart < EsTimeSpanPart::h )
          n %= 24;
        else
          mspart = EsTimeSpanPart::h;
        digits = 2;
        break;
      case esT('l'):
        n = get_milliseconds();
        if( mspart < EsTimeSpanPart::ms )
          n %= 1000;
        //else: no need to reset partBiggest to Part_MSec, it is
        //      the least significant one anyhow
        digits = 3;
        break;
      case esT('M'):
        n = get_minutes();
        if( mspart < EsTimeSpanPart::m )
          n %= 60;
        else
          mspart = EsTimeSpanPart::m;
        digits = 2;
        break;
      case esT('S'):
        n = get_seconds();
        if( mspart < EsTimeSpanPart::s )
          n %= 60;
        else
          mspart = EsTimeSpanPart::s;
        digits = 2;
        break;
      }

      if( digits )
        fmtPfx += esT("0") + EsString::fromULong(digits);
      fmtPfx += esT("lu");

      str += EsString::format(fmtPfx.c_str(), n);
    }
    else // normal character, just copy
      str += ch;
  }

  if( get_negative() )
    str = esT("-") + str;

  return str;
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::get_value() const
{
  return get_allMilliseconds();
}
//--------------------------------------------------------------------------------

void EsTimeSpan::set_value(const EsVariant& val)
{
  m_dt = fromVariant(val);
  checkDecomposed();
}
//--------------------------------------------------------------------------------

bool EsTimeSpan::get_negative() const
{
  checkComposed();
  return m_dt < IMMEDIATE_INT64(0);
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_negative(const bool& negative)
{
  checkComposed();
  if( negative != (m_dt < IMMEDIATE_INT64(0)) )
    m_dt = -m_dt;
}
//---------------------------------------------------------------------------

ulong EsTimeSpan::get_weeks() const
{
  checkDecomposed();
  return m_days/7;
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_weeks(cr_ulong val)
{
  m_days = val*7;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

llong EsTimeSpan::get_allWeeks() const
{
  checkComposed();
  return m_dt/IMMEDIATE_INT64(604800000);
}
//---------------------------------------------------------------------------

ulong EsTimeSpan::get_days() const
{
  checkDecomposed();
  return m_days;
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_days(cr_ulong val)
{
  m_days = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

llong EsTimeSpan::get_allDays() const
{
  checkComposed();
  return m_dt/IMMEDIATE_INT64(86400000);
}
//---------------------------------------------------------------------------

ulong EsTimeSpan::get_hours() const
{
  checkDecomposed();
  return m_hours;
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_hours(cr_ulong val)
{
  m_hours = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

llong EsTimeSpan::get_allHours() const
{
  checkComposed();
  return m_dt/IMMEDIATE_INT64(3600000);
}
//---------------------------------------------------------------------------

ulong EsTimeSpan::get_minutes() const
{
  checkDecomposed();
  return m_mins;
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_minutes(cr_ulong val)
{
  m_mins = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

llong EsTimeSpan::get_allMinutes() const
{
  checkComposed();
  return m_dt/IMMEDIATE_INT64(60000);
}
//---------------------------------------------------------------------------

ulong EsTimeSpan::get_seconds() const
{
  checkDecomposed();
  return m_secs;
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_seconds(cr_ulong val)
{
  m_secs = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

llong EsTimeSpan::get_allSeconds() const
{
  checkComposed();
  return m_dt/IMMEDIATE_INT64(1000);
}
//---------------------------------------------------------------------------

ulong EsTimeSpan::get_milliseconds() const
{
  checkDecomposed();
  return m_msecs;
}
//---------------------------------------------------------------------------

void EsTimeSpan::set_milliseconds(cr_ulong val)
{
  m_msecs = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

llong EsTimeSpan::get_allMilliseconds() const
{
  checkComposed();
  return m_dt;
}
//---------------------------------------------------------------------------

static EsTimeSpan* timeSpanPtrFromObj(const EsReflectedClassIntf::Ptr& obj)
{
  if(obj && obj->isKindOf(EsTimeSpan::classNameGetStatic()))
    return ES_INTFPTR_TO_OBJECTPTR(obj, EsTimeSpan);

  return nullptr;
}
//---------------------------------------------------------------------------

esDT EsTimeSpan::fromVariant(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsTimeSpan* dts = timeSpanPtrFromObj(var.asExistingObject());
    if(dts)
      return static_cast<esDT>(*dts);

    EsException::Throw(esT("Could not convert variant to EsTimeSpan value"));
  }
  else
    return var.asLLong();

  return 0; // pacify compilers
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::toVariant(esDT dt)
{
  return EsTimeSpan::create(dt);
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::compare(cr_EsVariant spanOrLlong) const
{
  if( spanOrLlong.isObject() && !spanOrLlong.isEmpty() )
  {
    EsTimeSpan* dts = timeSpanPtrFromObj(spanOrLlong.asExistingObject());
    if(dts)
      return cmp(dts);
  }
  else
  {
    llong other = spanOrLlong.asLLong();
    return (m_dt < other) ? -1 : ((m_dt == other) ? 0 : 1);
  }

  EsException::Throw(esT("Could not compare EsTimeSpan object with incompatible value"));
  // pacify compilers
  return 0;
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::add(cr_EsVariant spanOrLlong) const
{
  checkComposed();
  if( spanOrLlong.isObject() && !spanOrLlong.isEmpty() )
  {
    EsTimeSpan* dts = timeSpanPtrFromObj(spanOrLlong.asExistingObject());
    if(dts)
      return create(m_dt + static_cast<esDT>(*dts));
  }
  else
    return create(m_dt + spanOrLlong.asLLong());

  EsException::Throw(esT("Could not add incompatible value and EsTimeSpan object"));
  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::subtract(cr_EsVariant spanOrLlong) const
{
  checkComposed();
  if( spanOrLlong.isObject() && !spanOrLlong.isEmpty() )
  {
    EsTimeSpan* dts = timeSpanPtrFromObj(spanOrLlong.asExistingObject());
    if(dts)
      return create(m_dt - static_cast<esDT>(*dts));
  }
  else
    return create(m_dt - spanOrLlong.asLLong());

  EsException::Throw(esT("Could not subtract incompatible value from EsTimeSpan object"));
  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::multiply(llong factor) const
{
  checkComposed();
  return create(m_dt * factor);
}
//---------------------------------------------------------------------------

EsVariant EsTimeSpan::divide(llong divisor) const
{
  checkComposed();
  if( !divisor )
    EsException::ThrowDivisionByZero();
  return create(m_dt / divisor);
}
//---------------------------------------------------------------------------

EsTimeSpan::operator esDT () const
{
  checkComposed();
  return m_dt;
}
//---------------------------------------------------------------------------

EsTimeSpan& EsTimeSpan::operator= (esDT src)
{
  if( m_dt != src )
  {
    m_dt = src;
    m_flags |= EsInternal::flagNativeChanged;
    m_flags &= ~EsInternal::flagDecompChanged;
  }

  return *this;
}
//---------------------------------------------------------------------------

bool EsTimeSpan::operator== (const EsTimeSpan& other) const
{
  return 0 == cmp(&other);
}
//---------------------------------------------------------------------------

bool EsTimeSpan::operator< (const EsTimeSpan& other) const
{
  return -1 == cmp(&other);
}
//---------------------------------------------------------------------------

EsTimeSpan operator+ (const EsTimeSpan& _1, const EsTimeSpan& _2)
{
  _1.checkComposed();
  _2.checkComposed();
  return EsTimeSpan( _1.m_dt+_2.m_dt );
}
//---------------------------------------------------------------------------

EsTimeSpan operator+ (const EsTimeSpan& _1, llong _2)
{
  _1.checkComposed();
  return EsTimeSpan(_1.m_dt+_2);
}
//---------------------------------------------------------------------------

EsTimeSpan operator- (const EsTimeSpan& _1, const EsTimeSpan& _2)
{
  _1.checkComposed();
  _2.checkComposed();
  return EsTimeSpan(_1.m_dt-_2.m_dt);
}
//---------------------------------------------------------------------------

EsTimeSpan operator- (const EsTimeSpan& _1, llong _2)
{
  _1.checkComposed();
  return EsTimeSpan(_1.m_dt-_2);
}
//---------------------------------------------------------------------------

EsTimeSpan operator* (const EsTimeSpan& _1, llong _2)
{
  _1.checkComposed();
  return EsTimeSpan(_1.m_dt*_2);
}
//---------------------------------------------------------------------------

EsTimeSpan operator/ (const EsTimeSpan& _1, llong _2)
{
  _1.checkComposed();
  if( !_2 )
    EsException::ThrowDivisionByZero();
  return EsTimeSpan(_1.m_dt/_2);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// date time class
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsDateTime, NO_CLASS_DESCR)
  // services
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, asString, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, asDateString, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, asTimeString, EsString_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, asString, EsString_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, asString, EsString_CallConst_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, clone, EsVariant_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, streamRead, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, streamWrite, void_Call_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  // class services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, isLeapYear, bool_ClassCall_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, daysInMonthGet, ulong_ClassCall_long_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, daysInYearGet, ulong_ClassCall_long, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, dayOfWeekGet, ulong_ClassCall_long_ulong_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(    EsDateTime, biasReflected, bias, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, isDstActive, bool_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsDateTime, universalFmtToStrftime, EsString_ClassCall_cr_EsString, NO_METHOD_DESCR)
  // reflected operators
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, compare, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, add, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsDateTime, subtract, EsVariant_CallConst_cr_EsVariant, NO_METHOD_DESCR)
  // reflected properties
  ES_DECL_PROP_INFO_RO(EsDateTime, leapYear, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsDateTime, daysInMonth, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsDateTime, daysInYear, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsDateTime, dayOfYear, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsDateTime, dayOfWeek, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(   EsDateTime, value, EsVariant, NO_PROPERTY_LABEL, static_cast<ulong>(0), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(   EsDateTime, year, long, NO_PROPERTY_LABEL, static_cast<ulong>(DT_YEAR0), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(EsDateTime, month, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(1), static_cast<ulong>(1), static_cast<ulong>(12), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(EsDateTime, dayOfMonth, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(1), static_cast<ulong>(1), static_cast<ulong>(31), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(EsDateTime, hours, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), static_cast<ulong>(0), static_cast<ulong>(23), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(EsDateTime, minutes, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), static_cast<ulong>(0), static_cast<ulong>(59), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(EsDateTime, seconds, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), static_cast<ulong>(0), static_cast<ulong>(59), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE(EsDateTime, milliseconds, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(0), static_cast<ulong>(0), static_cast<ulong>(999), NO_PROPERTY_DESCR)
  // constructors
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(EsDateTime, reflectedNow, now, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsDateTime, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsDateTime::EsDateTime() :
m_flags(EsInternal::flagNativeChanged),
m_dt(0)
{
  ES_REFLECTED_PROPERTIES_RESET;
  m_flags |= EsInternal::flagInitialized;
}
//---------------------------------------------------------------------------

EsDateTime::EsDateTime(const EsDateTime& other) :
m_flags(other.m_flags),
m_dt(other.m_dt),
m_year(other.m_year),
m_dayOfYear(other.m_dayOfYear),
m_month(other.m_month),
m_dayOfMonth(other.m_dayOfMonth),
m_hour(other.m_hour),
m_min(other.m_min),
m_sec(other.m_sec),
m_msec(other.m_msec)
{
}
//---------------------------------------------------------------------------

EsDateTime::EsDateTime(esDT dt) :
m_flags(EsInternal::flagNativeChanged),
m_dt(dt)
{
  if( dt == 0 )
    ES_REFLECTED_PROPERTIES_RESET;

  m_flags |= EsInternal::flagInitialized;

  if( dt != 0 )
    checkDecomposed(false);
}
//---------------------------------------------------------------------------

EsDateTime::EsDateTime(const EsVariant& src) :
m_flags(EsInternal::flagNativeChanged),
m_dt(0)
{
  esDT dt = fromVariant(src);
  m_dt = dt;

  if( dt == 0 )
    ES_REFLECTED_PROPERTIES_RESET;

  m_flags |= EsInternal::flagInitialized;

  if( dt != 0 )
    checkDecomposed(false);
}
//---------------------------------------------------------------------------

const EsDateTime& EsDateTime::null()
{
  static const EsDateTime sc_null;
  return sc_null;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsDateTime::create(esDT dt)
{
  std::unique_ptr<EsDateTime> p( new EsDateTime(dt) );
  ES_ASSERT(p.get());
  p->m_dynamic = true;

  return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsVariant EsDateTime::clone() const
{
  return create(
    this->operator esDT()
  );
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsDateTime.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsDateTime.posix.cxx"
#endif
//---------------------------------------------------------------------------

EsDateTime EsDateTime::now()
{
  return EsDateTime(internalNow());
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsDateTime::reflectedNow()
{
  return create(internalNow());
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsDateTime::NEW()
{
  return create(0);
}
//---------------------------------------------------------------------------

bool EsDateTime::isOk() const
{
  checkComposed(false);
  checkDecomposed(false);

  return 0 != dtIsValid(m_year, m_month, m_dayOfMonth, m_hour, m_min, m_sec, m_msec);
}
//---------------------------------------------------------------------------

void EsDateTime::compose(long year, ulong month, ulong dayOfMonth, ulong hour, ulong minute, ulong second, ulong millisecond)
{
  if( !dtIsValid(year, month, dayOfMonth, hour, minute, second, millisecond) )
    EsException::Throw(esT("Could not compose valid date time value from date time components"));

  m_year = year;
  m_month = month;
  m_dayOfMonth = dayOfMonth;
  m_hour = hour;
  m_min = minute;
  m_sec = second;
  m_msec = millisecond;
  // we need to recalc dayOfYear member
  m_flags |= EsInternal::flagNativeChanged|EsInternal::flagDecompChanged|EsInternal::flagDayOfYearInvalid;
  checkComposed();
  checkDecomposed();
}
//---------------------------------------------------------------------------

void EsDateTime::checkDecomposed(bool doThrow /*= true*/) const
{
  if( m_flags & (EsInternal::flagNativeChanged|EsInternal::flagDayOfYearInvalid) )
  {
    // Zero all outbound members, in case their platform size > sizeof(int) and therefore, not all their conents would be
    // properly assigned inside dtDecomposeDateTime calculator
    //
    m_year = m_dayOfYear = m_month = m_dayOfMonth = m_hour = m_min = m_sec = m_msec = 0;
    dtDecomposeDateTime(
      &m_dt,
      (int*)&m_year,
      (int*)&m_dayOfYear,
      (int*)&m_month,
      (int*)&m_dayOfMonth,
      (int*)&m_hour,
      (int*)&m_min,
      (int*)&m_sec,
      (int*)&m_msec
    );
    if( !dtIsValid(m_year, m_month, m_dayOfMonth, m_hour, m_min, m_sec, m_msec) )
    {
      if( doThrow )
        EsException::Throw(esT("Could not decompose esDT value into valid date time components"));
    }
    else
      m_flags &= ~(EsInternal::flagNativeChanged|EsInternal::flagDayOfYearInvalid);
  }
}
//---------------------------------------------------------------------------

void EsDateTime::checkComposed(bool doThrow /*= true*/) const
{
  if( m_flags & EsInternal::flagDecompChanged )
  {
    if( !dtComposeDateTime(&m_dt, m_year, m_month, m_dayOfMonth, m_hour, m_min, m_sec, m_msec) )
    {
      if( doThrow )
        EsException::Throw(esT("Could not compose date time comonents into valid esDT value"));
    }
    else
      m_flags &= ~EsInternal::flagDecompChanged;
  }
}
//---------------------------------------------------------------------------

EsDateTime::operator esDT () const
{
  checkComposed();
  return m_dt;
}
//---------------------------------------------------------------------------

static EsDateTime* dateTimePtrFromObj(const EsReflectedClassIntf::Ptr& obj)
{
  if(obj && obj->isKindOf(EsDateTime::classNameGetStatic()))
    return ES_INTFPTR_TO_OBJECTPTR(obj, EsDateTime);

  return nullptr;
}
//---------------------------------------------------------------------------

esDT EsDateTime::fromVariant(const EsVariant& var)
{
  if( var.isObject() )
  {
    EsDateTime* dt = dateTimePtrFromObj(var.asExistingObject());
    if(dt)
      return static_cast<esDT>(*dt);

    EsException::Throw(esT("Could not convert variant to esDT value"));
  }
  else
    return var.asLLong();

  return 0; // pacify compilers
}
//---------------------------------------------------------------------------

EsVariant EsDateTime::toVariant(esDT dt)
{
  return EsDateTime::create(dt);
}
//---------------------------------------------------------------------------

EsString EsDateTime::asString() const
{
  // return ISO 8601 Date and Time with space as date-time separator
  return asString(
    EsLocale::locale(),
    esT("%F %T")
  );
}
//---------------------------------------------------------------------------

EsString EsDateTime::asDateString() const
{
  // Return ISO 8601 Date
  return asString(
    EsLocale::locale(),
    esT("%F")
  );
}
//---------------------------------------------------------------------------

EsString EsDateTime::asTimeString() const
{
  // Return ISO 8601 Time
  return asString(
    EsLocale::locale(),
    esT("%T")
  );
}
//---------------------------------------------------------------------------

bool EsDateTime::isLeapYear(long year)
{
  return 0 != dtIsLeapYear(year);
}
//---------------------------------------------------------------------------

ulong EsDateTime::daysInMonthGet(long year, ulong month)
{
  return dtGetMonthDayCount(year, month);
}
//---------------------------------------------------------------------------

ulong EsDateTime::daysInYearGet(long year)
{
  return dtGetDaysInYear(year);
}
//---------------------------------------------------------------------------

ulong EsDateTime::dayOfWeekGet(long year, ulong month, ulong dayOfMonth)
{
  return dtGetDayOfWeek(year, month, dayOfMonth);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool EsDateTime::get_leapYear() const
{
  return EsDateTime::isLeapYear(get_year());
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_daysInMonth() const
{
  return EsDateTime::daysInMonthGet(get_year(), get_month());
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_daysInYear() const
{
  return EsDateTime::daysInYearGet(get_year());
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_dayOfYear() const
{
  // If we have set any property values directly prior to this call, -
  // we got to re-compose, and then re-decompose all other values in DateTime
  //
  checkComposed();
  checkDecomposed();

  return m_dayOfYear;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_dayOfWeek() const
{
  return EsDateTime::dayOfWeekGet(get_year(), get_month(), get_dayOfMonth());
}
//---------------------------------------------------------------------------

EsVariant EsDateTime::get_value() const
{
  checkComposed();
  return m_dt;
}
//--------------------------------------------------------------------------------

void EsDateTime::set_value(const EsVariant& val)
{
  m_dt = fromVariant(val);
  checkDecomposed();
}
//--------------------------------------------------------------------------------

long EsDateTime::get_year() const
{
  checkDecomposed();
  return m_year;
}
//---------------------------------------------------------------------------

void EsDateTime::set_year(cr_long val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(val, m_month, m_dayOfMonth, m_hour, m_min, m_sec, m_msec) )
    EsException::Throw(esT("Setting year to %d would result in invalid date time object"), val);
  m_year = val;
  m_flags |= (EsInternal::flagDecompChanged|EsInternal::flagDayOfYearInvalid);
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_month() const
{
  checkDecomposed();
  return m_month;
}
//---------------------------------------------------------------------------

void EsDateTime::set_month(cr_ulong val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(m_year, val, m_dayOfMonth, m_hour, m_min, m_sec, m_msec) )
    EsException::Throw(esT("Setting month to %d would result in invalid date time object"), val);
  m_month = val;
  m_flags |= (EsInternal::flagDecompChanged|EsInternal::flagDayOfYearInvalid);
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_dayOfMonth() const
{
  checkDecomposed();
  return m_dayOfMonth;
}
//---------------------------------------------------------------------------

void EsDateTime::set_dayOfMonth(cr_ulong val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(m_year, m_month, val, m_hour, m_min, m_sec, m_msec) )
    EsException::Throw(esT("Setting day of month to %d would result in invalid date time object"), val);
  m_dayOfMonth = val;
  m_flags |= (EsInternal::flagDecompChanged|EsInternal::flagDayOfYearInvalid);
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_hours() const
{
  checkDecomposed();
  return m_hour;
}
//---------------------------------------------------------------------------

void EsDateTime::set_hours(cr_ulong val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(m_year, m_month, m_dayOfMonth, val, m_min, m_sec, m_msec) )
    EsException::Throw(esT("Setting hour to %d would result in invalid date time object"), val);
  m_hour = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_minutes() const
{
  checkDecomposed();
  return m_min;
}
//---------------------------------------------------------------------------

void EsDateTime::set_minutes(cr_ulong val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(m_year, m_month, m_dayOfMonth, m_hour, val, m_sec, m_msec) )
    EsException::Throw(esT("Setting minutes to %d would result in invalid date time object"), val);
  m_min = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_seconds() const
{
  checkDecomposed();
  return m_sec;
}
//---------------------------------------------------------------------------

void EsDateTime::set_seconds(cr_ulong val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(m_year, m_month, m_dayOfMonth, m_hour, m_min, val, m_msec) )
    EsException::Throw(esT("Setting seconds to %d would result in invalid date time object"), val);
  m_sec = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

ulong EsDateTime::get_milliseconds() const
{
  checkDecomposed();
  return m_msec;
}
//---------------------------------------------------------------------------

void EsDateTime::set_milliseconds(cr_ulong val)
{
  if( (m_flags & EsInternal::flagInitialized) && !dtIsValid(m_year, m_month, m_dayOfMonth, m_hour, m_min, m_sec, val) )
    EsException::Throw(esT("Setting milliseconds to %d would result in invalid date time object"), val);
  m_msec = val;
  m_flags |= EsInternal::flagDecompChanged;
  m_flags &= ~EsInternal::flagNativeChanged;
}
//---------------------------------------------------------------------------

long EsDateTime::cmp(const EsDateTime* other) const
{
  ES_ASSERT(other);
  checkComposed();
  other->checkComposed();
  return (m_dt < other->m_dt) ? -1 : ((m_dt == other->m_dt) ? 0 : 1);
}
//---------------------------------------------------------------------------

EsVariant EsDateTime::compare(cr_EsVariant otherDt) const
{
  if( otherDt.isObject() && !otherDt.isEmpty() )
  {
    EsDateTime* dt = dateTimePtrFromObj(otherDt.asExistingObject());
    if( dt )
      return cmp(dt);
  }

  EsException::Throw(esT("Could not compare EsDateTime object with incompatible value"));
  // pacify compilers
  return 0;
}
//---------------------------------------------------------------------------

EsVariant  EsDateTime::add(cr_EsVariant spanOrLlong) const
{
  checkComposed();
  if( spanOrLlong.isObject() && !spanOrLlong.isEmpty() )
  {
    EsTimeSpan* dts = timeSpanPtrFromObj(spanOrLlong.asExistingObject());
    if(dts)
      return create(m_dt + static_cast<esDT>(*dts));
  }
  else
    return create(m_dt + spanOrLlong.asLLong());

  EsException::Throw(esT("Could not add incompatible value and EsDateTime object"));
  // pacify compilers
  return EsVariant::null();
}
//---------------------------------------------------------------------------

EsVariant  EsDateTime::subtract(cr_EsVariant spanOrDtOrLlong) const
{
  checkComposed();
  if( spanOrDtOrLlong.isObject() && !spanOrDtOrLlong.isEmpty() )
  {
    // try span
    EsTimeSpan* dts = timeSpanPtrFromObj(spanOrDtOrLlong.asExistingObject());
    if(dts)
      return create(m_dt - static_cast<esDT>(*dts));
    else
    {
      // try date time - should return span object then
      EsDateTime* dt = dateTimePtrFromObj(spanOrDtOrLlong.asExistingObject());
      if( dt )
        return EsTimeSpan::create(m_dt - static_cast<esDT>(*dt));
    }
  }
  else
    return create(m_dt - spanOrDtOrLlong.asLLong());

  EsException::Throw(esT("Could not subtract incompatible value from EsDateTime object"));
  // pacify compilers
  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsDateTime::streamWrite(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);
  checkComposed();
  stream->valueWrite( m_dt );
}
//---------------------------------------------------------------------------

void EsDateTime::streamRead(cr_EsBaseIntfPtr p)
{
  EsStreamIntf::Ptr stream = p;
  ES_ASSERT(stream);
  m_dt = stream->valueRead().asLLong();
  m_flags |= EsInternal::flagNativeChanged;
  m_flags &= ~EsInternal::flagDecompChanged;
}
//---------------------------------------------------------------------------

EsDateTime& EsDateTime::operator= (esDT src)
{
  if(m_dt != src)
  {
    m_dt = src;
    m_flags |= EsInternal::flagNativeChanged;
    m_flags &= ~EsInternal::flagDecompChanged;
  }
  return *this;
}
//---------------------------------------------------------------------------

bool EsDateTime::operator== (const EsDateTime& other) const
{
  return 0 == cmp(&other);
}
//---------------------------------------------------------------------------

bool EsDateTime::operator< (const EsDateTime& other) const
{
  return -1 == cmp(&other);
}
//---------------------------------------------------------------------------

const EsDateTime& EsDateTime::operator+= (const EsDateTime& other)
{
  checkComposed();
  m_dt += static_cast<esDT>(other);
  return *this;
}
//---------------------------------------------------------------------------

const EsDateTime& EsDateTime::operator+= (llong other)
{
  checkComposed();
  m_dt += other;
  return *this;
}
//---------------------------------------------------------------------------

const EsDateTime& EsDateTime::operator-= (const EsDateTime& other)
{
  checkComposed();
  m_dt -= static_cast<esDT>(other);
  return *this;
}
//---------------------------------------------------------------------------

const EsDateTime& EsDateTime::operator-= (llong other)
{
  checkComposed();
  m_dt -= other;
  return *this;
}
//---------------------------------------------------------------------------

EsDateTime operator+ (const EsDateTime& _1, const EsTimeSpan& _2)
{
  _1.checkComposed();
  return EsDateTime(_1.m_dt+static_cast<esDT>(_2));
}
//---------------------------------------------------------------------------

EsDateTime operator+ (const EsDateTime& _1, llong _2)
{
  _1.checkComposed();
  return EsDateTime(_1.m_dt+_2);
}
//---------------------------------------------------------------------------

EsTimeSpan operator- (const EsDateTime& _1, const EsDateTime& _2)
{
  _1.checkComposed();
  _2.checkComposed();
  return EsTimeSpan( _1.m_dt-_2.m_dt );
}
//---------------------------------------------------------------------------

EsDateTime operator- (const EsDateTime& _1, const EsTimeSpan& _2)
{
  _1.checkComposed();
  return EsDateTime(_1.m_dt-static_cast<esDT>(_2));
}
//---------------------------------------------------------------------------

EsDateTime operator- (const EsDateTime& _1, llong _2)
{
  _1.checkComposed();
  return EsDateTime(_1.m_dt-_2);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsString EsDateTime::asString(cr_EsString fmt) const
{
  return asString(
    EsLocale::locale(),
    fmt
  );
}
//---------------------------------------------------------------------------

EsString EsDateTime::asString(const EsString& loc, cr_EsString fmt) const
{
  return asString(
    EsLocale::locale(loc),
    fmt
  );
}
//---------------------------------------------------------------------------

EsString EsDateTime::asString(const std::locale& loc, const EsString& fmt) const
{
  if( fmt.empty() || !isOk() )
    return EsString::null();

  std::vector<EsString::value_type> buff(fmt.size()+128);

  if( strftime(loc, &buff[0], buff.size(), fmt.c_str()) )
    return EsString(&buff[0]);

  return EsString::null();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Low-level date time string formatter && helpers
//
// Following is seriously rewritten code by Eric R. Smith and Michal Jaegermann.
// Few modifications added for seamless I18N support
//

#define BIG_BUFF_LEN 80

//---------------------------------------------------------------------------

class EsUniversalFmtStringParser
{
public:
  enum {
    None,
    Hour24,
    Hour12,
    Minute,
    Second,
    AmPm,
    Day,
    Month,
    Year,
    Era,
    SingleQuote
  };

public:
  EsUniversalFmtStringParser(const EsString& fmtUniversal) :
  m_fmtUniversal(fmtUniversal),
  m_state(None),
  m_fldCnt(0)
  {}

  EsString strftimeFmtStringGet();

protected:
  void stateSetAndProcessInput(int newState, EsString::value_type chIn, EsString& out);

protected:
  EsString m_fmtUniversal;
  int m_state;
  size_t m_fldCnt;

private:
  EsUniversalFmtStringParser() ES_REMOVEDECL;
  EsUniversalFmtStringParser(const EsUniversalFmtStringParser&) ES_REMOVEDECL;
  EsUniversalFmtStringParser& operator=(const EsUniversalFmtStringParser&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

void EsUniversalFmtStringParser::stateSetAndProcessInput(
  int newState,
  EsString::value_type chIn,
  EsString& out
)
{
  if( m_state != newState )
  {
    // Completion of previous field
    switch( m_state )
    {
    case Hour24:
      if( 2 == m_fldCnt )
        out += esT("%H");
      else if( 1 == m_fldCnt )
        out += esT("%-H");
      else
        EsException::Throw(
          esT("Unsupported universal formatting 24 hour sequence encountered")
        );
      break;
    case Hour12:
      if( 2 == m_fldCnt )
        out += esT("%I");
      else if( 1 == m_fldCnt )
        out += esT("%-I");
      else
        EsException::Throw(
          esT("Unsupported universal formatting 12 hour sequence encountered")
        );
      break;
    case Minute:
      if( 2 == m_fldCnt )
        out += esT("%M");
      else if( 1 == m_fldCnt )
        out += esT("%-M");
      else
        EsException::Throw(
          esT("Unsupported universal formatting minute sequence encountered")
        );
      break;
    case Second:
      if( 2 == m_fldCnt )
        out += esT("%S");
      else if( 1 == m_fldCnt )
        out += esT("%-S");
      else
        EsException::Throw(
          esT("Unsupported universal formatting second sequence encountered")
        );
      break;
    case AmPm:
      if( 2 == m_fldCnt )
        out += esT("%p");
      else
        EsException::Throw(
          esT("Unsupported universal formatting AM PM marker sequence encountered")
        );
      break;
    case Day:
      if( 1 == m_fldCnt )
        out += esT("%-d");
      else if( 2 == m_fldCnt )
        out += esT("%d");
      else if( 3 == m_fldCnt )
        out += esT("%a");
      else if( 4 == m_fldCnt )
        out += esT("%A");
      else
        EsException::Throw(
          esT("Unsupported universal formatting day sequence encountered")
        );
      break;
    case Month:
      if( 1 == m_fldCnt )
        out += esT("%-m");
      else if( 2 == m_fldCnt )
        out += esT("%m");
      else if( 3 == m_fldCnt )
        out += esT("%b");
      else if( 4 == m_fldCnt )
        out += esT("%B");
      else
        EsException::Throw(
          esT("Unsupported universal formatting month sequence encountered")
        );
      break;
    case Year:
      if( 1 == m_fldCnt )
        out += esT("%-y");
      else if( 2 == m_fldCnt )
        out += esT("%y");
      else if( 4 == m_fldCnt || 5 == m_fldCnt )
        out += esT("%Y");
      else
        EsException::Throw(
          esT("Unsupported universal formatting year sequence encountered")
        );
      break;
    case Era:
      // TODO: Era is not supported so far
      break;
    case SingleQuote:
      // Handle single and duplicate signle quotes. Doubles - make literal single, singles - just ignore
      for(size_t cnt = 0; cnt < m_fldCnt/2; ++cnt )
        out += esT('\'');
      break;
    }

    m_state = newState;

    if( None == m_state )
    {
      if( 0 != chIn )
        out += chIn;
      m_fldCnt = 0;
    }
    else // Start of field
      m_fldCnt = 1;
  }
  else if( None == m_state ) // Plain copy
  {
    if( 0 != chIn )
      out += chIn;
  }
  else // Continuation of the current field
    ++m_fldCnt;
}
//---------------------------------------------------------------------------

EsString EsUniversalFmtStringParser::strftimeFmtStringGet()
{
  EsString result;

  size_t sze = m_fmtUniversal.size();
  result.reserve(
    sze
  );

  m_state = None;
  int newState = None;
  m_fldCnt = 0;

  EsString::value_type ch = 0;
  for(size_t idx = 0; idx < sze; ++idx)
  {
    ch = m_fmtUniversal[idx];

    switch( ch )
    {
    case esT('h'):
      newState = Hour12;
      break;
    case esT('H'):
      newState = Hour24;
      break;
    case esT('m'):
      newState = Minute;
      break;
    case esT('s'):
      newState = Second;
      break;
    case esT('t'):
      newState = AmPm;
      break;
    case esT('d'):
      newState = Day;
      break;
    case esT('M'):
      newState = Month;
      break;
    case esT('y'):
      newState = Year;
      break;
    case esT('g'):
      newState = Era;
      break;
    case esT('\''):
      newState = SingleQuote;
      break;
    default:
      newState = None;
      break;
    }

    stateSetAndProcessInput(
      newState,
      ch,
      result
    );
  }

  // Finalize input processing
  stateSetAndProcessInput(
    None,
    0,
    result
  );

  return result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsString EsDateTime::universalFmtToStrftime(cr_EsString uniFmt)
{
  EsUniversalFmtStringParser parser(uniFmt);

  return parser.strftimeFmtStringGet();
}
//---------------------------------------------------------------------------

// First weekday is the number of the first day in week in terms EsDateTime
// (1=mon, ... 7=sun)
//
static ulong weeknumberCalc(const EsDateTime& dt, ulong firstWeekday)
{
  ES_ASSERT(1 <= firstWeekday);
  ES_ASSERT(7 >= firstWeekday);

  ulong doy = dt.get_dayOfYear();
  ulong dow = dt.get_dayOfWeek();
  EsDateTime _1stJanSameYear = dt;
  _1stJanSameYear.set_dayOfMonth(1);
  _1stJanSameYear.set_month(1);

  ulong dow1stJan = _1stJanSameYear.get_dayOfWeek();
  ulong num = (doy+6)/7; // mathematical weeks since 1st Jan

  // Make week number corrections, depending on the firstWeekday
  if( dow1stJan != firstWeekday )
    ++num;

  return num;
}
//---------------------------------------------------------------------------

static EsString _LC_Mth_name(
  const std::locale& loc,
  int m
)
{
  switch(m)
  {
  case 1:
    return _("January");
  case 2:
    return _("February");
  case 3:
    return _("March");
  case 4:
    return _("April");
  case 5:
    return _("May");
  case 6:
    return _("June");
  case 7:
    return _("July");
  case 8:
    return _("August");
  case 9:
    return _("September");
  case 10:
    return _("October");
  case 11:
    return _("November");
  case 12:
    return _("December");
  }

  // should never be here
  ES_FAIL;
  return EsString::null();
}
//---------------------------------------------------------------------------

static EsString _LC_ShortMth_name(
  const std::locale& loc,
  int m
)
{
  switch(m)
  {
  case 1:
    return _("Jan");
  case 2:
    return _("Feb");
  case 3:
    return _("Mar");
  case 4:
    return _("Apr");
  case 5:
    return _("May");
  case 6:
    return _("Jun");
  case 7:
    return _("Jul");
  case 8:
    return _("Aug");
  case 9:
    return _("Sep");
  case 10:
    return _("Oct");
  case 11:
    return _("Nov");
  case 12:
    return _("Dec");
  }

  // should never be here
  ES_FAIL;
  return EsString::null();
}
//---------------------------------------------------------------------------

static EsString _LC_Day_name(
  const std::locale& loc,
  int d
)
{
  switch( d )
  {
  case 1:
    return _("Monday");
  case 2:
    return _("Tuesday");
  case 3:
    return _("Wednesday");
  case 4:
    return _("Thursday");
  case 5:
    return _("Friday");
  case 6:
    return _("Saturday");
  case 7:
    return _("Sunday");
  }

  // should never be here
  ES_FAIL;
  return EsString::null();
}
//---------------------------------------------------------------------------

static EsString _LC_ShortDay_name(
  const std::locale& loc,
  int d
)
{
  switch( d )
  {
  case 1:
    return _("Mon");
  case 2:
    return _("Tue");
  case 3:
    return _("Wed");
  case 4:
    return _("Thu");
  case 5:
    return _("Fri");
  case 6:
    return _("Sat");
  case 7:
    return _("Sun");
  }

  // should never be here
  ES_FAIL;
  return EsString::null();
}
//---------------------------------------------------------------------------

static EsString _LC_AmPm_string(
  const std::locale& loc,
  int hr
)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  return (hr < 12) ?
    EsLocale::timeAmGet(loc) :
    EsLocale::timePmGet(loc);
#else
  return (hr < 12) ?
    _("AM") :
    _("PM");
#endif
}
//---------------------------------------------------------------------------

static EsString _LC_longTimeFmt(
  const std::locale& loc
)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsUniversalFmtStringParser parser(
    EsLocale::timeFmtGet(loc)
  );

  return parser.strftimeFmtStringGet();
#else
  EsString::value_type ts = EsLocale::timeSeparatorGet(loc);
  EsString fmt = esT("%H");
  fmt += ts;
  fmt += esT("%M");
  fmt += ts;
  fmt += esT("%S");

  return fmt;
#endif
}
//---------------------------------------------------------------------------

static EsString _LC_12hrTimeFmt(
  const std::locale& loc
)
{
  EsString::value_type ts = EsLocale::timeSeparatorGet(loc);
  EsString fmt = esT("%I");
  fmt += ts;
  fmt += esT("%M");
  fmt += ts;
  fmt += esT("%S ");
  fmt += esT("%p");

  return fmt;
}
//---------------------------------------------------------------------------

static EsString _LC_shortTimeFmt(
  const std::locale& loc
)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsUniversalFmtStringParser parser(
    EsLocale::timeFmtShortGet(loc)
  );

  return parser.strftimeFmtStringGet();
#else
  EsString::value_type ts = EsLocale::timeSeparatorGet(loc);
  EsString fmt = esT("%H");
  fmt += ts;
  fmt += esT("%M");

  return fmt;
#endif
}
//---------------------------------------------------------------------------

static EsString _LC_longDateFmt(
  const std::locale& loc
)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsUniversalFmtStringParser parser(
    EsLocale::dateFmtGet(loc)
  );

  return parser.strftimeFmtStringGet();
#else
  EsString::value_type ds = EsLocale::dateSeparatorGet(loc);
  EsString fmt = esT("%m");
  fmt += ds;
  fmt += esT("%d");
  fmt += ds;
  fmt += esT("%Y");

  return fmt;
#endif
}
//---------------------------------------------------------------------------

static EsString _LC_shortDateFmt(
  const std::locale& loc
)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsUniversalFmtStringParser parser(
    EsLocale::dateFmtShortGet(loc)
  );

  return parser.strftimeFmtStringGet();
#else
  EsString::value_type ds = EsLocale::dateSeparatorGet(loc);
  EsString fmt = esT("%m");
  fmt += ds;
  fmt += esT("%d");
  fmt += ds;
  fmt += esT("%y");

  return fmt;
#endif
}
//---------------------------------------------------------------------------

static EsString _LC_longDateTimeFmt(
  const std::locale& loc
)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsUniversalFmtStringParser parser(
    EsLocale::dateTimeFmtGet(loc)
  );

  return parser.strftimeFmtStringGet();
#else
  return esT("%a %b %d %Y %X");
#endif
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

size_t EsDateTime::strftime(const std::locale& loc, EsString::value_type* str, size_t maxsize, EsString::const_pointer fmt) const
{
  checkComposed();
  checkDecomposed();

  size_t num = 0;
  long len;
  int n;

  EsString::value_type q;
  EsString::value_type buf[BIG_BUFF_LEN];
  EsString::value_type modifier;

  EsString::const_pointer putstr;
  EsString tmpStr;

  while(1)
  {
    if(num >= maxsize)
      return 0;

    q = *fmt++;
    if(q == 0)
      break;

    if(q != esT('%'))
    {
      *str++ = q;
      num++;
      continue;
    }

    q = *fmt++;
    if(q == 0)
      break;

    if( esT('-') == q || esT('E') == q || esT('0') == q )
    {
      modifier = q;

      q = *fmt++;
      if(q == 0)
        break;
    }
    else
      modifier = 0;

    putstr = buf;
    switch(q)
    {
    case esT('A'): //< Locale-specific long day of week
    case esT('a'): //< Locale-specific short day of week
      if(get_dayOfWeek() < 1 || get_dayOfWeek() > 7)
        putstr = esT("?");
      else
      {
        if(esT('A') == q)
          tmpStr = _LC_Day_name(
            loc,
            get_dayOfWeek()
          );
        else
          tmpStr = _LC_ShortDay_name(
            loc,
            get_dayOfWeek()
          );

        putstr = tmpStr.c_str();
      }
      break;
    case esT('B'): //< Locale-specific long month name
    case esT('b'): //< Locale-specific short month name
    case esT('h'): //< C++11 Locale-specific short month name
      if(get_month() > 11)
        putstr = esT("?");
      else
      {
        if(esT('B') == q)
          tmpStr = _LC_Mth_name(
            loc,
            get_month()
          );
        else
          tmpStr = _LC_ShortMth_name(
            loc,
            get_month()
          );

        putstr = tmpStr.c_str();
      }
      break;
    case esT('c'): //< Locale-specific long date time
      strftime(
        loc,
        buf,
        sizeof(buf),
        _LC_longDateTimeFmt(loc).c_str()
      );
      break;
    case esT('C'): //< First two digits of the year
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_year() / 100
      );
      putstr = tmpStr.c_str();
      break;
    case esT('d'):  //< Day of month
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_dayOfMonth()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('D'): //< Locale-specific short date
      strftime(
        loc,
        buf,
        sizeof(buf),
        _LC_shortDateFmt(loc).c_str()
      );
      break;
    case esT('e'):  //< Day of month without prepending it with 0
      tmpStr = EsString::format(
        esT("%d"),
        get_dayOfMonth()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('F'):  //< ISO Date
      tmpStr = EsString::format(
        esT("%04d-%02d-%02d"),
        get_year(),
        get_month(),
        get_dayOfMonth()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('H'):  //< Hours in 24h fmt as 00 decimal
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_hours()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('I'): //< Hours in 12h fmt as 00 decimal
      n = get_hours();
      if(n == 0)
        n = 12;
      else if(n > 12)
        n -= 12;
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        n
      );
      putstr = tmpStr.c_str();
      break;
    case esT('j'):  //< Day of year as 000 decimal
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%03d"),
        get_dayOfYear()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('m'):  //< Month as 00 decimal
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_month()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('M'):  //< Minutes as 00 decimal
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_minutes()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('p'):   //< AM-PM locale-dependent string
      tmpStr = _LC_AmPm_string(
        loc,
        get_hours()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('r'):   //< Locale-specific 12 hours in locale-dependent format
      strftime(
        loc,
        buf,
        sizeof(buf),
        _LC_12hrTimeFmt(loc).c_str()
      );
      break;
    case esT('R'):   //< Shortened ISO time
      tmpStr = EsString::format(
        esT("%02d:%02d"),
        get_hours(),
        get_minutes()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('S'):   //< Seconds as 00 decimal
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_seconds()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('T'):  //< Equivalent to "%H:%M:%S" (the ISO 8601 time format)
      tmpStr = EsString::format(
        esT("%02d:%02d:%02d"),
        get_hours(),
        get_minutes(),
        get_seconds()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('U'):  //< Week of year - starting Sunday
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%u") :
          esT("%02u"),
        weeknumberCalc(
          *this,
          7
        )
      );
      putstr = tmpStr.c_str();
      break;
    case esT('W'):  //< Week of year - starting Monday
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%u") :
          esT("%02u"),
        weeknumberCalc(
          *this,
          1
        )
      );
      putstr = tmpStr.c_str();
      break;
    case esT('w'):  //< Day of week decimal
      tmpStr = EsString::format(
        esT("%d"),
        get_dayOfWeek()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('x'):  //< Locale-specific date representation
      strftime(
        loc,
        buf,
        sizeof(buf),
        _LC_longDateFmt(loc).c_str()
      );
      break;
    case esT('X'): //< Locale-specific time string
      strftime(
        loc,
        buf,
        sizeof(buf),
        _LC_longTimeFmt(loc).c_str()
      );
      break;
    case esT('y'):  //< Year as 00 decimal
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%02d"),
        get_year() % 100
      );
      putstr = tmpStr.c_str();
      break;
    case esT('Y'):  //< Full year
      tmpStr = EsString::format(
        esT("%d"),
        get_year()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('l'):  //< Custom option - milliseconds
      tmpStr = EsString::format(
        esT('-') == modifier ?
          esT("%d") :
          esT("%03d"),
        get_milliseconds()
      );
      putstr = tmpStr.c_str();
      break;
    case esT('%'):  //< Plain % coded as %%
      putstr = esT("%");
      break;
    case esT('n'):
      putstr = esT("\n");
      break;
    case esT('t'):
      putstr = esT("\t");
      break;
    default:        //< Just copy as-is
      buf[0] = q;
      buf[1] = 0;
      break;
    }

    len = static_cast<long>(
      esStrlen(putstr)
    );

    if(num + len >= maxsize)
      return 0;

    num += len;
    while(--len >= 0)
      *str++ = *putstr++;
  }
  *str = 0;
  return (size_t) num;
}
//---------------------------------------------------------------------------

