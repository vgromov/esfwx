#include "escorepch.h"
#pragma hdrstop

#include "EsString.h"
#include <locale>
#include <iomanip>
#include <sstream>
#include <algorithm>

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

typedef std::basic_istringstream<EsString::value_type> EsInputStringStream;
typedef std::basic_ostringstream<EsString::value_type> EsOutputStringStream;
//---------------------------------------------------------------------------

// special values
//
const EsString& EsString::null() ES_NOTHROW
{
  static const EsString sc_nullString;
  return sc_nullString;
}
//---------------------------------------------------------------------------

const EsString& EsString::tab() ES_NOTHROW
{
  static const EsString sc_tab = esT("\t");
  return sc_tab;
}
//---------------------------------------------------------------------------

const EsString& EsString::asterisk() ES_NOTHROW
{
  static const EsString sc_asterisk = esT("*");
  return sc_asterisk;
}
//---------------------------------------------------------------------------

const EsString& EsString::trimSymbolsDef() ES_NOTHROW
{
  static const EsString sc_trimDef = esT(" \t");
  return sc_trimDef;
}
//---------------------------------------------------------------------------

const EsString& EsString::space() ES_NOTHROW
{
  static const EsString sc_space = esT(" ");
  return sc_space;
}
//---------------------------------------------------------------------------

const EsString::Array& EsString::nullArray() ES_NOTHROW
{
  static const EsString::Array sc_nullStringArray;
  return sc_nullStringArray;
}
//---------------------------------------------------------------------------

const EsByteString& EsString::defEncoding() ES_NOTHROW
{
  static const EsByteString sc_defEncoding =
#if defined(ES_UNICODE)
#  if defined(ES_USE_WCHAR)
#   if 2 == ES_WCHAR_SIZE
#     if ES_ENDIAN == ES_LITTLE_ENDIAN
        "UTF-16LE";
#     elif ES_ENDIAN == ES_BIG_ENDIAN
        "UTF-16BE";
#     else
#       error Unsupported OS 16-bit wide string endianness detected.
#     endif
#   elif 4 == ES_WCHAR_SIZE
#     if ES_ENDIAN == ES_LITTLE_ENDIAN
        "UTF-32LE";
#     elif ES_ENDIAN == ES_BIG_ENDIAN
        "UTF-32BE";
#     else
#       error Unsupported OS 32-bit wide string endianness detected.
#     endif
#   endif
#  else
    "UTF-8";
#  endif
#else
  "CP1251";
#endif

  return sc_defEncoding;
}
//---------------------------------------------------------------------------

EsString::~EsString() ES_NOTHROW
{}
//---------------------------------------------------------------------------

// string hash value access with on-demand hash recalculation
esU64 EsString::hashGet() const ES_NOTHROW
{
  if( m_hashInvalid )
  {
    m_hash.reset();
    m_hash.update(*this);
    m_hashInvalid = false;
  }

  return m_hash.get();
}
//---------------------------------------------------------------------------

// misc
EsString EsString::substr(size_t offs, size_t cnt /*= npos*/) const
{
  EsString result( m_str.substr(offs, (cnt == npos) ? EsBasicStringT::npos : cnt) );
  return result;
}
//---------------------------------------------------------------------------

void EsString::swap(EsString& other) ES_NOTHROW
{
  ES_ASSERT(this != &other);
  m_str.swap(other.m_str);

  EsHashFNV_1a_64 tmpHash = m_hash;
  m_hash = other.m_hash;
  other.m_hash = tmpHash;

  bool tmpInvalid = m_hashInvalid;
  m_hashInvalid = other.m_hashInvalid;
  other.m_hashInvalid = tmpInvalid;
}
//---------------------------------------------------------------------------

void EsString::move(EsString& other) ES_NOTHROW
{
  ES_ASSERT(this != &other);
  m_str.swap(other.m_str);
  m_hash = other.m_hash;
  m_hashInvalid = other.m_hashInvalid;
}
//---------------------------------------------------------------------------

// search
size_t EsString::find(value_type ch, size_t offs/* = 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find(ch, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find(const_pointer str, size_t offs/* = 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find(const_pointer str, size_t offs, size_t cnt) const
{
  EsBasicStringT::size_type pos = m_str.find(str, offs, cnt);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find(const EsString& str, size_t offs/* = 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_of(value_type ch, size_t offs/* = 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find_first_of(ch, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_of(const_pointer str, size_t offs/* = 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find_first_of(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_of(const EsString& str, size_t offs/* = 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find_first_of(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_of(value_type ch, size_t end/* = EsString::npos*/) const
{
  EsBasicStringT::size_type pos = m_str.find_last_of(ch, end);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_of(const_pointer str, size_t end/* = EsString::npos*/) const
{
  EsBasicStringT::size_type pos = m_str.find_last_of(str, end);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_of(const EsString& str, size_t end/* = EsString::npos*/) const
{
  EsBasicStringT::size_type pos = m_str.find_last_of(str, end);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_not_of(value_type ch, size_t offs /*= 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find_first_not_of(ch, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_not_of(const_pointer str, size_t offs /*= 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find_first_not_of(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_not_of(const_pointer str, size_t offs, size_t cnt) const
{
  EsBasicStringT::size_type pos = m_str.find_first_not_of(str, offs, cnt);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_first_not_of(const EsString& str, size_t offs /*= 0*/) const
{
  EsBasicStringT::size_type pos = m_str.find_first_not_of(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_not_of(value_type ch, size_t offs /*= EsString::npos*/) const
{
  EsBasicStringT::size_type pos = m_str.find_last_not_of(ch, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_not_of(const_pointer str, size_t offs /*= EsString::npos*/) const
{
  EsBasicStringT::size_type pos = m_str.find_last_not_of(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_not_of(const_pointer str, size_t offs, size_t cnt) const
{
  EsBasicStringT::size_type pos = m_str.find_last_not_of(str, offs, cnt);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

size_t EsString::find_last_not_of(const EsString& str, size_t offs /*= EsString::npos*/) const
{
  EsBasicStringT::size_type pos = m_str.find_last_not_of(str, offs);
  return (pos == EsBasicStringT::npos) ? npos : pos;
}
//---------------------------------------------------------------------------

// comparison
long EsString::compare( EsString::const_pointer src, bool ignoreCase /*= false*/,
  const std::locale& loc /*= EsLocale::locale()*/ ) const
{
  return EsString::scompare(c_str(), src, ignoreCase, loc);
}
//---------------------------------------------------------------------------

long EsString::compare( const EsBasicStringT& src, bool ignoreCase /*= false*/,
  const std::locale& loc /*= EsLocale::locale()*/ ) const
{
  EsString str(src);
  return EsString::scompare(*this, str, ignoreCase, loc);
}
//---------------------------------------------------------------------------

long EsString::compare( const EsString& src, bool ignoreCase /*= false*/,
  const std::locale& loc /*= EsLocale::locale()*/ ) const
{
  return EsString::scompare(*this, src, ignoreCase, loc);
}
//---------------------------------------------------------------------------

// try to automatically find radix. radix 16 is specified by 0x 0X string prefix
// radix 8 - by 0 prefix. all other cases are considered radix 10
//
static inline int defineRadix(EsString::const_iterator pos, EsString::const_iterator end, const std::locale& loc) ES_NOTHROW
{
  int radix = 10;
  // first, skip spaces, signes until prefix is found
  while( pos != end && std::isspace(*pos, loc) )
    ++pos; // skip prefix space
  if( pos != end && (*pos == esT('+') || *pos == esT('-')) )
    ++pos; // skip sign
  if(pos != end && *pos++ == esT('0'))
  {
    if( pos != end )
    {
      if( (*pos == esT('x') || *pos == esT('X')) )
        radix = 16;
      else
        radix = 8;
    }
  }

  return radix;
}
//---------------------------------------------------------------------------

template <typename ResultT>
static ResultT _toInt(EsString::const_iterator pos, EsString::const_iterator end, EsString::const_iterator& endPos,
  int radix, const std::locale& loc, bool forceAll)
{
  if( 0 == radix )
    radix = defineRadix(pos, end, loc);
  // check if value range is ok for radix
  EsNumericCheck::checkRangeInteger(2, 16, radix, esT("Radix"));

  EsString str(pos, end);
  EsInputStringStream strinp(str);
  strinp.imbue(loc);
  ResultT result = static_cast<ResultT>(0);
  strinp >> std::setbase(radix) >> result;
  if( strinp.fail() || (forceAll && !strinp.eof()) )
  {
    EsByteString bs( typeid( ResultT ).name() );
    EsException::Throw(
      _("Could not convert string '%s' to %s"),
      str,
      EsString::fromUtf8(bs)
    );
  }
  if( !strinp.eof() )
    endPos = pos + static_cast<EsString::const_iterator::difference_type>(strinp.tellg());
  else
    endPos = end;

  return result;
}
//---------------------------------------------------------------------------

template <typename ArgT>
static EsString _fromInt(ArgT val, int radix, const std::locale& loc, const EsString& errMsg)
{
  EsNumericCheck::checkRangeInteger(2, 16, radix, esT("Radix"));
  EsOutputStringStream strout;
  strout.imbue(loc);
  strout << std::setbase(radix) << val;
  if( strout.fail() )
    EsException::Throw(errMsg.c_str());

  return strout.str();
}
//---------------------------------------------------------------------------

// todo: optimize translation lookup for error messages. so far, disable localization for them
// standard string conversion functions mappings
EsString EsString::fromLong(long l, int radix /*= 10*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  return _fromInt<long>(l, radix, loc, esT("Could not convert integer to string"));
}
//---------------------------------------------------------------------------

long EsString::toLong(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString::const_iterator endPos;
  return _toInt<long>(str.begin(), str.end(), endPos, radix, loc, true);
}
//---------------------------------------------------------------------------

EsString EsString::fromULong(unsigned long u, int radix /*= 10*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  return _fromInt<unsigned long>(u, radix, loc, esT("Could not convert unsigned integer to string"));
}
//---------------------------------------------------------------------------

unsigned long EsString::toULong(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString::const_iterator endPos;
  return _toInt<unsigned long>(str.begin(), str.end(), endPos, radix, loc, true);
}
//---------------------------------------------------------------------------

EsString EsString::fromInt64(esI64 i64, int radix /*= 10*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  return _fromInt<esI64>(i64, radix, loc, esT("Could not convert 64 bit integer to string"));
}
//---------------------------------------------------------------------------

esI64 EsString::toInt64(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString::const_iterator endPos;
  return _toInt<esI64>(str.begin(), str.end(), endPos, radix, loc, true);
}
//---------------------------------------------------------------------------

EsString EsString::fromUInt64(esU64 u64, int radix /*= 10*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  return _fromInt<esU64>(u64, radix, loc, esT("Could not convert unsigned 64 bit integer to string"));
}
//---------------------------------------------------------------------------

esU64 EsString::toUInt64(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString::const_iterator endPos;
  return _toInt<esU64>(str.begin(), str.end(), endPos, radix, loc, true);
}
//---------------------------------------------------------------------------

EsString EsString::fromDouble(double d, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsOutputStringStream strout;
  strout.imbue(loc);
  strout << d;

  return strout.str();
}
//---------------------------------------------------------------------------

double EsString::toDouble(const EsString& str, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsInputStringStream strinp(str);
  strinp.imbue(loc);
  double result = 0.;
  strinp >> result;
  if( strinp.fail() )
    EsException::Throw(
      _("Could not convert string '%s' to floating point value"),
      str
    );

  return result;
}
//---------------------------------------------------------------------------

// String pseudo - variadic formatting
//
// variant-based string formatter. fmt parameter must contain string variant
// with printf-compatible formatting control codes.
// args should be collection of variants with values to format.
// returned is variant containing resulting string. exceptions may be thrown if
// arguments count|types do not match the formatting field count|field types
enum {
  fmtField = 1,
  fmtPfx,
  fmtWidth,
  fmtPrecision,
  fmtTypeMod,
  fmtType,
  fmtPercent
};
//---------------------------------------------------------------------------

static inline bool typeIsInt( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('d') || type == esT('i');
}
//---------------------------------------------------------------------------

static inline bool typeIsUint( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('o') || type == esT('u') ||
    type == esT('x') || type == esT('X');
}
//---------------------------------------------------------------------------

static inline bool typeIsHex( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('x') || type == esT('X');
}
//---------------------------------------------------------------------------

static inline bool typeIsFloat( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('a') || type == esT('A') ||
    type == esT('e') || type == esT('E') ||
    type == esT('g') || type == esT('G') || type == esT('f') || type == esT('F');
}
//---------------------------------------------------------------------------

static inline bool typeIsScientificFloat( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('e') || type == esT('E');
}
//---------------------------------------------------------------------------

static inline bool typeIsFixedFloat( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('f') || type == esT('F');
}
//---------------------------------------------------------------------------

static inline bool typeIsHexFloat( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('a') || type == esT('A');
}
//---------------------------------------------------------------------------

static inline bool typeIsUppercase( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('X') ||
         type == esT('A') ||
         type == esT('E') ||
         type == esT('G') ||
         type == esT('F');
}
//---------------------------------------------------------------------------

static inline bool typeIsChar( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('c');
}
//---------------------------------------------------------------------------

static inline bool typeIsString( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('s');
}
//---------------------------------------------------------------------------

static inline bool typeIsPointer( EsString::value_type type ) ES_NOTHROW
{
  return type == esT('p');
}
//---------------------------------------------------------------------------

static inline bool typeModIsLL(const EsString& tmod) ES_NOTHROW
{
  return
    esT("L") == tmod ||
    esT("ll") == tmod ||
    esT("I64") == tmod;
}
//---------------------------------------------------------------------------

static inline bool typeModIsL(const EsString& tmod) ES_NOTHROW
{
  return
    esT("l") == tmod ||
    esT("I32") == tmod;
}
//---------------------------------------------------------------------------

static inline bool typeModIsShort(const EsString& tmod) ES_NOTHROW
{
  return esT("h") == tmod;
}
//---------------------------------------------------------------------------

static inline bool typeModIsChar(const EsString& tmod) ES_NOTHROW
{
  return esT("hh") == tmod;
}
//---------------------------------------------------------------------------

static inline void printUint(EsOutputStringStream& oss, const EsString& fldMod, const EsVariant& val)
{
  if( typeModIsLL(fldMod) )
    oss << val.asULLong();
  else
  {
    ulong u = val.asULong();
    if( typeModIsShort(fldMod) )
      oss << (unsigned short)u;
    else if( typeModIsChar(fldMod) )
      oss << (unsigned char)u;
    else
      oss << u;
  }
}
//---------------------------------------------------------------------------

static inline const EsString& throwUnknownTypeSpecifier( EsString::value_type type )
{
  EsException::Throw(esT("Unsupported format type specifier: '%c'"), type);
  return EsString::null();
}
//---------------------------------------------------------------------------

// wildcard width|precision formatting helpers
static EsVariant extractWidthOrPrecision(const EsRegEx& re, int what, size_t& argIdx, const EsVariant& args)
{
  ES_ASSERT(fmtWidth == what || fmtPrecision == what);
  const EsString& match = re.matchGet(what);
  if( !match.empty() )
  {
    // check if width is *, so we have to extract argument and convert it to integer
    if( esT('*') == match[0] )
      return args.itemGet(argIdx++);
    else
      return EsString::toULong(match);
  }

  return EsVariant::null();
}
//---------------------------------------------------------------------------

static inline bool isPositional(const EsString& pfx)
{
  return !pfx.empty() && esT('$') == pfx[pfx.size()-1]; //< Positional specifier
}
//---------------------------------------------------------------------------

static size_t argPositionGet(const EsString& pfx, ulong maxPos)
{
  EsString::const_iterator endPos;
  ulong pos = _toInt<unsigned long>(pfx.begin(), pfx.end()-1, endPos, 10, EsLocale::locale(), false);
  EsNumericCheck::checkRangeUInteger(1, maxPos, pos, esT("EsString::format positional specifier"));

  return pos-1;
}
//---------------------------------------------------------------------------

EsString EsString::formatV(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& args)
{
  EsRegEx re(
    esT("(%([-+0 #]|[0-9]+\\$)?([0-9]+|\\*)?(?:\\.([0-9]+|\\*))?")
    esT("((?:[hljztL]{1,2})|(?:I32)|(?:I64))?")
    esT("([cdiopuxXaAeEgGfs]))|(%%)")
  );

  ES_ASSERT(re.isOk());

  if( !fmt )
    return EsString::null();

  EsString result;

  if( args.isEmpty() )
    return fmt;

  // find format fields in format control string
  EsString::const_pointer beg = fmt;
  EsString::const_pointer pos = beg;
  EsString::const_pointer end = pos+esStrlen(fmt);
  size_t argIdx = 0;
  re.set_text(fmt);
  while( re.get_matches() )
  {
    // optional width and precision placeholders for * case
    size_t matchStart, matchLen;
    // format the entire field, append formatted string to result and skip further
    EsString::const_pointer tmpPos = pos;
    if(
      re.matchGet(
        matchStart,
        matchLen,
        fmtField
      ) &&
      matchLen
    )
    {
      if( pos < beg+matchStart )
        // append string prefix to the result, moving position after the field end
        result.append(pos, beg+matchStart);

      pos = beg+matchStart;
      EsString fmtFld(pos, pos+matchLen);
      pos += matchLen;

      // Extract optional formatting prefix
      const EsString& pfx = re.matchGet(fmtPfx);

      // Extract optional width and precision placeholders for * case
      const EsVariant& width = extractWidthOrPrecision(re, fmtWidth, argIdx, args);
      const EsVariant& precision = extractWidthOrPrecision(re, fmtPrecision, argIdx, args);

      // Extract optional type modifier
      const EsString& fldMod = re.matchGet(fmtTypeMod);

      // extract field type
      bool ok = re.matchGet(
        matchStart,
        matchLen,
        fmtType
      );

      ES_ASSERT(ok);
      ES_ASSERT(matchLen);
      EsString::value_type type = *(beg+matchStart);

      size_t argIdxLocal = argIdx;
      if( isPositional(pfx) )
        argIdxLocal = argPositionGet(
          pfx,
          args.countGet()
        );
      else
        argIdxLocal = argIdx++;

      const EsVariant& val = args.itemGet(argIdxLocal);

      // Format field, using optional prefix|width|precision specifiers
      EsOutputStringStream oss;
      oss.imbue(loc);
      std::ios_base::fmtflags flags = static_cast<std::ios_base::fmtflags>(0);

      if( !width.isEmpty() )
        oss.width( width.asULLong(loc) );

      if( !precision.isEmpty() )
      {
        ulong prec = precision.asULong(loc);

        if( typeIsFloat(type) )
          oss.precision( prec );

        if( 0 < prec )
        {
          if( typeIsFloat(type) )
            flags |= std::ios::showpoint;
          else if( typeIsInt(type) || typeIsUint(type) || typeIsString(type) )
            oss.width(prec);
        }
      }

      if( !pfx.empty() && !isPositional(pfx) )
      {
        if(esT('-') == pfx[0])
          flags |= std::ios::left;
        else
        {
          flags |= std::ios::right;
          if(esT('+') == pfx[0] )
            flags |= std::ios::showpos;
          else if(esT('#') == pfx[0])
            flags |= std::ios::showbase;
          else if(esT(' ') == pfx[0])
            oss.fill(esT(' '));
          else if(esT('0') == pfx[0])
            oss.fill(esT('0'));
        }
      }

      if( typeIsScientificFloat(type) )
        flags |= std::ios::scientific;
      else if( typeIsFixedFloat(type) )
        flags |= std::ios::fixed;
#ifndef __GLIBCXX__
      // hexfloat is currently not supported in gnu libstdc++ used in Android
      else if( typeIsHexFloat(type) )
        flags |= std::ios::hexfloat;
#endif
      else if( typeIsHex(type) || typeIsPointer(type) )
        flags |= std::ios::hex;

      if( typeIsUppercase(type) )
        flags |= std::ios::uppercase;

      oss.flags(flags);
      if( typeIsInt(type) )
      {
        if( typeModIsLL(fldMod) )
          oss << val.asLLong();
        else
        {
          long l = val.asLong();
          if( typeModIsShort(fldMod) )
            oss << (short)l;
          else if( typeModIsChar(fldMod) )
            oss << (signed char)l;
          else
            oss << l;
        }
      }
      else if( typeIsUint(type) )
        printUint(oss, fldMod, val);
      else if( typeIsFloat(type) )
        oss << val.asDouble();
      else if( typeIsString(type) )
      {
        const EsString& str = val.asString();
        if( !str.empty() )
        {
          if( !precision.isEmpty() )
          {
            const EsString& sub = str.substr(0, precision.asULong());
            oss.write(sub.c_str(), sub.size());
          }
          else
            oss.write(str.c_str(), str.size());
        }
        else
          oss << esT("(null)");
      }
      else if( typeIsChar(type) )
      {
        if( typeModIsL(fldMod) )
          oss << val.asLong();
        else
          oss.put( val.asChar() );
      }
      else if( typeIsPointer(type) )
      {
        if( val.isPointer() )
        {
          void* p = val.asPointer();
          if( sizeof(void*) == sizeof(ulong) )
            oss << (ulong)p;
          else if( sizeof(void*) == sizeof(ullong) )
            oss << (ullong)p;
          else
            EsException::Throw(_("Unsupported pointer size"));
        }
        else
          printUint(oss, fldMod, val);
      }
      else
        throwUnknownTypeSpecifier(type);

      result += oss.str();
    }
    else if( re.matchGet(matchStart, matchLen, fmtPercent) &&
             matchLen )
    {
      // append string prefix to the result
      if( pos < beg+matchStart )
      {
        result.append(pos, beg+matchStart);
        pos = beg+matchStart;
      }

      result += esT('%');
      pos += matchLen;
    }

    re.set_offset( pos-beg );
  }
  // add the rest of the unmatched string to the result and return
  if( pos < end )
    result.append(pos, end);

  return result;
}
//---------------------------------------------------------------------------

// Pseudo-variadic formatting with explicit locale
EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt)
{
  return formatV(loc, fmt, EsVariant::null());
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0)
{
  const EsVariant::Array& args = {
    arg0
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1)
{
  const EsVariant::Array& args = {
    arg0,
    arg1
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8,
    arg9
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8,
    arg9,
    arg10
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8,
    arg9,
    arg10,
    arg11
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8,
    arg9,
    arg10,
    arg11,
    arg12
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12, const EsVariant& arg13)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8,
    arg9,
    arg10,
    arg11,
    arg12,
    arg13
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12, const EsVariant& arg13, const EsVariant& arg14)
{
  const EsVariant::Array& args = {
    arg0,
    arg1,
    arg2,
    arg3,
    arg4,
    arg5,
    arg6,
    arg7,
    arg8,
    arg9,
    arg10,
    arg11,
    arg12,
    arg13,
    arg14
  };

  return formatV(loc, fmt, args);
}
//---------------------------------------------------------------------------

// convert string to|from escaped forms
//
static inline void checkCharRange(int c)
{
  ES_COMPILE_TIME_ASSERT(std::numeric_limits<char>::min() < 0, charMustBeOfSignedType); // Watch that the char is a signed type

  if ( c < std::numeric_limits<char>::min() || c > std::numeric_limits<unsigned char>::max() ) // Check -127 .. 255
    EsException::Throw(
      _("Wide character with code 0x%X encountered in place where only eight-bit characters allowed"),
      static_cast<unsigned>(c)
    );
}
//---------------------------------------------------------------------------

static inline void appendHexEscape(EsString& res, EsString::value_type c)
{
  EsString::value_type str[4];
  str[0] = esT('\\');
  str[1] = esT('x');
  str[2] = static_cast<EsString::value_type>(EsString::binLoNibbleToHex(unsigned((unsigned char)c) >> 4));
  str[3] = static_cast<EsString::value_type>(EsString::binLoNibbleToHex(unsigned((unsigned char)c) & 0x0F));
  res.append(str, 4);
}
//---------------------------------------------------------------------------

static inline void appendEscape(EsString& res, EsString::value_type c)
{
  EsString::value_type str[ 2 ];
  str[0] = esT('\\');
  str[1] = c;
  res.append(str, 2);
}
//---------------------------------------------------------------------------

static EsString::value_type escapeToChar(EsString::const_iterator cit,
  EsString::const_iterator end, EsString::const_iterator& endPos, const std::locale& loc)
{
  ES_ASSERT(cit != end);
  EsString::value_type c = *cit++;
  long xoChar; // accumulator for the value of octal or hexadecimal number
  if( cit < end && c == esT('\\') )
  {
    c = *cit++;
    switch( c )
    {
    case esT('a'): case esT('A'): c = esT('\a'); break;
    case esT('b'): case esT('B'): c = esT('\b'); break;
    case esT('f'): case esT('F'): c = esT('\f'); break;
    case esT('n'): case esT('N'): c = esT('\n'); break;
    case esT('r'): case esT('R'): c = esT('\r'); break;
    case esT('t'): case esT('T'): c = esT('\t'); break;
    case esT('v'): case esT('V'): c = esT('\v'); break;
    case esT('\\')               : c = esT('\\'); break;
    case esT('\'')               : c = esT('\''); break;
    case esT('\"')               : c = esT('\"'); break;
    case esT('x'): case esT('X'):
      if( cit < end && EsString::isHexChar(*cit) )
      {
        esU8 nibble;
        EsString::hexToBinNibble(static_cast<esU8>(*cit++), nibble);
        xoChar = nibble;
        if( cit < end && EsString::isHexChar(*cit) )
        {
          xoChar <<= 4;
          EsString::hexToBinNibble(static_cast<esU8>(*cit++), nibble);
          xoChar |= nibble;
        }
        endPos = cit;
        return static_cast<EsString::value_type>(xoChar);
      }
      break;
    default:
      if( c >= esT('0') && c <= esT('7') )
      {
        xoChar = 0;
        do
        {
          xoChar <<= 3;
          xoChar |= c - esT('0');
          c = *cit++;

        }  while( cit < end && c >= esT('0') && c <= esT('7') );

        endPos = cit-1;
        return static_cast<EsString::value_type>(xoChar);
      }
      else
      {
        EsException::Throw(
          (std::isprint(c, loc) ? _("Unknown string escape sequence with character '%c'")
            : _("Unknown string escape sequence with character code 0x%X")), c);
      }
    }
  }

  endPos = cit;
  return c;
}
//---------------------------------------------------------------------------

static EsString::value_type xmlEscapeToChar(EsString::const_iterator pos,
  EsString::const_iterator end, EsString::const_iterator& endPos, const std::locale& loc)
{
  struct XMLEscape
  {
    int m_size;
    EsString::value_type m_esc[6];
    EsString::value_type m_repr;
  };

  static const XMLEscape s_table[] = {
    {3, esT("lt;"),   esT('<')},
    {3, esT("gt;"),   esT('>')},
    {4, esT("amp;"),  esT('&')},
    {5, esT("quot;"), esT('"')},
    {5, esT("apos;"), esT('\'')}
  };

  EsString::const_iterator cit = pos;
  if( cit != end && *cit++ == esT('&') )
  {
    if( cit != end && *cit++ == esT('#') )
    {
      if( cit != end )
      {
        int result;
        if( *cit == esT('x') || *cit == esT('X') )
          result = _toInt<int>(cit, end, endPos, 16, loc, false);
        else
          result = _toInt<int>(cit, end, endPos, 10, loc, false);

        if( *endPos++ == esT(';') )
        {
          checkCharRange(result);
          return static_cast<EsString::value_type>(result);
        }
      }
    }
    else
    {
      const XMLEscape* it = s_table;
      const XMLEscape* itEnd = it + ES_CONST_ARRAY_COUNT(s_table);
      for ( ; it != itEnd; ++it )
      {
        if( std::lexicographical_compare(cit, end, it->m_esc, it->m_esc+it->m_size) )
        {
          endPos = cit + it->m_size;
          return it->m_repr;
        }
      }
    }
  }

  EsString err(pos, pos + esMin(end-pos, static_cast<EsString::const_iterator::difference_type>(16)));
  size_t semicolon = err.find( esT(';') );
  if( semicolon != EsString::npos )
    err[semicolon+1] = esT('\0');

  EsException::Throw(
    _("Unknown XML sequence '%s'"),
    err
  );

  return esT('\0'); // pacify compiler in debug mode
}
//---------------------------------------------------------------------------

EsString EsString::toString(const EsString& src, int masks)
{
  EsString result;
  if( (masks & StrQuote) != 0 )
  {
    result += esT('\"');
    result.reserve(src.size() + 2);
  }
  else
    result.reserve(src.size());

  EsString::const_iterator it = src.begin();
  EsString::const_iterator itEnd = src.end();

  if( (masks & (StrKeepSideBlanks | StrNoCEscape)) == 0 )
  {
    for( ; it < itEnd && (*it == esT('\x20')); ++it )            // handle leading blanks
      result.append(esT("\\x20"), 4);
    for( ; it < itEnd && *(itEnd - 1) == esT('\x20'); --itEnd )  // handle trailing blanks
      ;
  }

  for( ; it < itEnd; ++it )
  {
    EsString::value_type c = *it;
    switch ( c )
    {
    case esT('<'):
      if ( (masks & StrXML) != 0 )
      {
        if ( (masks & StrNoCEscape) != 0 )
          result.append(esT("&lt;"), 4);
        else
          appendHexEscape(result, c);
      }
      else
        result += c;
      break;
    case esT('>'):
      if( (masks & StrXML) != 0 )
      {
        if ( (masks & StrNoCEscape) != 0 )
          result.append(esT("&gt;"), 4);
        else
          appendHexEscape(result, c);
      }
      else
        result += c;
      break;
    case esT('&'):
      if( (masks & StrXML) != 0 )
      {
        if ( (masks & StrNoCEscape) != 0 )
          result.append(esT("&amp;"), 5);
        else
          appendHexEscape(result, c);
      }
      else
        result += c;
      break;
    case esT('"'):
      if( (masks & StrXML) != 0 )
      {
        if ( (masks & StrNoCEscape) != 0 )
          result.append(esT("&quot;"), 6);
        else
          appendHexEscape(result, c);
      }
      else if( (masks & StrNoCEscape) == 0 )
        appendEscape(result, c);
      else
        result += c;
      break;
    case esT('\\'):
      if( (masks & StrNoCEscape) != 0 )
        result += c;
      else
        appendEscape(result, c);
      break;
    default:
      if( (static_cast<EsString::value_type>(c) < 0x20 // We know what we do, do not use is** function here!
#ifndef ES_UNICODE
        || ((masks & StrI18n) == 0 && static_cast<long>(c) >= 0x7Fu)
#endif
        ) )
      {
#ifdef ES_UNICODE
        checkCharRange(c);
#endif

        if( (masks & StrXML) != 0 )
        {
          if( (masks & StrNoCEscape) != 0 )
            result.append( format(esT("&#%u;"), static_cast<unsigned long>(
              static_cast<long>(c) )) );
          else
            appendHexEscape(result, c);
        }
        else if( (masks & StrNoCEscape) == 0 )
        {
          // try to add known C escape sequences as text rather than hex
          if( 0x0A == c )
            appendEscape(result, esT('n'));
          else if( 0x0D == c )
            appendEscape(result, esT('r'));
          else if( 0x09 == c )
            appendEscape(result, esT('t'));
          else
            appendHexEscape(result, c);
        }
        else
          result += c;
      }
      else
        result += c;
    }
  }

  if ( (masks & (StrKeepSideBlanks | StrNoCEscape)) == 0 ) // Take care of trailing blanks
  {
    ES_ASSERT(it == itEnd); // oh sure
    for ( itEnd = src.end(); it < itEnd; ++it )
      result.append(esT("\\x20"), 4);
  }

  if ( (masks & StrQuote) != 0 )
    result += esT('\"');

  return result;
}
//---------------------------------------------------------------------------

EsString EsString::fromString(const EsString& src, int masks, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString result;
  size_t size = src.size();

  if( size > 0 )
  {
    EsString::const_iterator pos = src.begin();
    EsString::const_iterator end = src.end();
    if( (masks & StrQuote) != 0 && size >= 2 )
    {
      EsString::value_type first = *pos;
      EsString::value_type last = *(end-1);
      if ( (first == esT('\"') && last == esT('\"')) || // starts and ends with quotes -- consider this is enough evidence this is what we need
        (first == esT('\'') && last == esT('\'')) )
      {
        ++pos;
        --end;
        size -= 2;
      }
    }

    result.reserve(size); // at least that number of symbols...
    while( pos < end )
    {
      EsString::value_type c = *pos;
      if ( c == esT('\\') && (masks & StrNoCEscape) == 0 )
        result += escapeToChar(pos, end, pos, loc);
      else if ( c == esT('&') && (masks & StrXML) != 0 )
        result += xmlEscapeToChar(pos, end, pos, loc);
      else
      {
        result += c;
        ++pos;
      }
    }
  }
  return result;
}
//---------------------------------------------------------------------------

// create string from array of strings, delimited by delimiter,
// each string is optionally enclosed in specific chars, if left and right enclosures are set
EsString EsString::fromStringArray(const EsString::Array& as,
                                    const EsString& delimiter /*= esT(' ')*/,
                                    const EsString& leftEnclosure /*= EsString::null()*/,
                                    const EsString& rightEnclosure /*= EsString::null()*/ ) ES_NOTHROW
{
  EsString result;

  for(size_t idx = 0; idx < as.size(); ++idx)
  {
    if( idx > 0 )
      result += delimiter;

    result += as[idx];
  }

  return leftEnclosure + result + rightEnclosure;
}
//---------------------------------------------------------------------------

// return true if ch belongs to hes chars
bool EsString::isHexChar( int ch ) ES_NOTHROW
{
  return (ch >= esT('0') && ch <= esT('9')) ||
    (ch >= esT('A') && ch <= esT('F')) ||
    (ch >= esT('a') && ch <= esT('f'));
}
//---------------------------------------------------------------------------

// conver hex char to the binary nibble
bool EsString::hexToBinNibble( int ch, esU8& nibble ) ES_NOTHROW
{
  if(ch >= esT('0') && ch <= esT('9'))
    nibble = static_cast<esU8>(ch-esT('0'));
  else if(ch >= esT('A') && ch <= esT('F'))
    nibble = static_cast<esU8>(ch-esT('A')+10);
  else if(ch >= esT('a') && ch <= esT('f'))
    nibble = static_cast<esU8>(ch-esT('a')+10);
  else
    return false;

  return true;
}
//---------------------------------------------------------------------------

size_t EsString::hexToBin( EsString::const_pointer hex, size_t hexLen, EsBinBuffer::pointer bin, size_t binLen ) ES_NOTHROW
{
  return hexToBin<EsString::value_type>(
    hex,
    hexLen,
    bin,
    binLen
  );
}
//---------------------------------------------------------------------------

EsBinBuffer EsString::hexToBin( const EsString& hex )
{
  EsBinBuffer result;

  if( !hex.empty() )
  {
    result.resize( hex.size()/2 );

    hexToBin(
      hex.c_str(),
      hex.size(),
      &result[0],
      result.size()
    );
  }

  return result;
}
//---------------------------------------------------------------------------

// valid hex chars
static const char c_hexChars[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// convert low bin nibble to hex char
int EsString::binLoNibbleToHex( esU8 lowNibble ) ES_NOTHROW
{
  return c_hexChars[lowNibble & 0x0F];
}
//---------------------------------------------------------------------------

// return how many bin bytes were converted into hex representation
size_t EsString::binToHex( EsBinBuffer::const_pointer bin, size_t binLen, EsString& hex )
{
  // check buffers && lengths. hex buffer length must be at least double of bin buffer len
  if( bin && binLen > 0 )
  {
    hex.clear();
    hex.reserve(binLen*2);
    EsBinBuffer::const_pointer binEnd = bin+binLen;
    EsBinBuffer::const_pointer binBeg = bin;
    while(bin < binEnd)
    {
      hex.append( 1, static_cast<ES_CHAR>(binLoNibbleToHex((*bin) >> 4)) );
      hex.append( 1, static_cast<ES_CHAR>(binLoNibbleToHex(*bin++)) );
    }

    return bin-binBeg;
  }

  return 0;
}
//---------------------------------------------------------------------------

EsString EsString::binToHex( const EsBinBuffer& bin )
{
  EsString result;
  if( !bin.empty() )
    binToHex(&bin[0], bin.size(), result);
  return result;
}
//---------------------------------------------------------------------------

EsString EsString::trimLeft(const EsString& str, const EsString& symbols/* = esT(" \t")*/)
{
  size_t pos = str.find_first_not_of(symbols);
  if( EsString::npos == pos )
    return EsString::null();
  else
    return str.substr(pos);
}
//---------------------------------------------------------------------------

EsString EsString::trimRight(const EsString& str, const EsString& symbols/* = esT(" \t")*/)
{
  size_t pos = str.find_last_not_of(symbols);
  if( EsString::npos == pos )
    return EsString::null();
  else
    return str.substr(0, pos+1);
}
//---------------------------------------------------------------------------

// convert any character to an string that is printable, possibly a
// C-like escape character.
//
// If the character is blank, it is represented as \x20.
EsString EsString::charToEscapedString(int c)
{
  EsString::value_type str[4];
  if( c == esT('\'') || c == esT('\\') )
  {
    str[0] = esT('\\');
    str[1] = c;
    return EsString(str, 2);
  }

  if( c > 0x20 && c < 0x7F ) // We know what we do, do not use is** function here! Also note \x20 is nonprintable
    return EsString(1, c);

  checkCharRange(c);
  str[0] = esT('\\');
  str[1] = esT('x');
  str[2] = static_cast<EsString::value_type>(binLoNibbleToHex(unsigned((unsigned char)c) >> 4u));
  str[3] = static_cast<EsString::value_type>(binLoNibbleToHex(unsigned((unsigned char)c) & 0xFu));
  return EsString(str, 4);
}
//---------------------------------------------------------------------------

EsString EsString::wrap(const EsString& str, unsigned indentCount, unsigned lineWidth)
{
  // adjust parameters
  //
  if ( lineWidth < minimumLineWidth )
    lineWidth = minimumLineWidth;
  if ( lineWidth < indentCount + minimumLineWidth )
    indentCount = lineWidth - minimumLineWidth;

  const size_t strSize = str.size();
  const size_t aproximateResultLineSize  = lineWidth - indentCount;
  ES_ASSERT(aproximateResultLineSize >= minimumLineWidth);
  if( strSize < aproximateResultLineSize && str.find(esT('\n')) == EsString::npos ) // optimized case
    return str;

  EsString result;
  EsString indent;
  indent.reserve(indentCount + 1);
  indent += esT('\n');
  indent.append(indentCount, esT(' '));
  const size_t estimatedResultSize = strSize + indent.size() * ((strSize / aproximateResultLineSize) + 1);
  result.reserve(estimatedResultSize);

  EsString::const_iterator it = str.begin(); // running iterator
  EsString::const_iterator itEnd = str.end();
  EsString::const_iterator itPrevLine = it;  // iterator that points to the previous line's end
  EsString::const_iterator itPrevBlank = it; // iterator that points to the previous blank
  for( ; ; ++it )
  {
    if( it != itEnd )
    {
      EsString::value_type c = *it;
      switch( c )
      {
      case esT(' '): // do not use isblank here, only handle spaces
        {
          size_t runningLineSize = it - itPrevLine;
          int remainingSize = int(aproximateResultLineSize) - int(runningLineSize);
          if( remainingSize > 0 )
          {
            itPrevBlank = it;
            continue;
          }

          // ready to think about line break
          if( itPrevLine != itPrevBlank ) // if there was a blank in the line then break at that blank
            it = itPrevBlank;
          break; // break into line wrapper code
        }
      case esT('\n'): // recovery for the case there is a newline in the input
        break; // break into line wrapper code
      default: // We do not handle tabs, and they shall not (normally) appear in a line
        continue; // continue scanning
      }
    }

    // Line wrapper code is here. Need to add itPrevLine .. it
    //
    if( itPrevLine < it )
      result.append(itPrevLine, it);
    if( it != itEnd )
    {
      result += indent;
      ++it;
      itPrevLine = it;
      itPrevBlank = it;
    }
    else
      break;
  }
  return result;
}
//---------------------------------------------------------------------------

long EsString::scompare(EsString::const_pointer s1, EsString::const_pointer s2, bool ignoreCase)
{
  // if both strings are empty, or are the same classes, return equality flag
  if( (0 == s1 && 0 == s2) || s1 == s2 )
    return EsString::cmpEqual;
  else if(0 == s1)
    return EsString::cmpLess;
  else if(0 == s2)
    return EsString::cmpGreater;
  else
  {
    int result = 0;
    if(ignoreCase)
      result = esStricmp(s1, s2);
    else
      result = esStrcmp(s1, s2);

    return ( result < 0 ) ? EsString::cmpLess :
      ((0 == result) ? EsString::cmpEqual : EsString::cmpGreater);
  }
}
//---------------------------------------------------------------------------

long EsString::scompare(const EsString& s1, const EsString& s2, bool ignoreCase, const std::locale& loc /*= EsLocale::locale()*/)
{
  if( ignoreCase )
    return scompare(toUpper(s1, loc), toUpper(s2, loc), false);

  // if both strings are empty, return equality flag
  if( s1.empty() && s2.empty() )
    return cmpEqual;
  else if(s1.empty())
    return cmpLess;
  else if(s2.empty())
    return cmpGreater;
  else
  {
    // try fast hash-to-hash comparison first, if hash is valid
    if(s1.isValidHash() && s2.isValidHash() && s1.hashGet() == s2.hashGet())
      return cmpEqual;

    return EsString::scompare(s1.c_str(), s2.c_str(), ignoreCase);
  }
}
//---------------------------------------------------------------------------

// lower|uppercase conversion
EsString::value_type EsString::toLower(EsString::value_type c, const std::locale& loc /*= EsLocale::locale()*/)
{
  return std::tolower(c, loc);
}
//---------------------------------------------------------------------------

struct _toLower
{
  _toLower(const std::locale& loc) : m_loc(loc) {}

  inline void operator()(EsString::value_type& ch) const ES_NOTHROW
  {
    ch = std::tolower(ch, m_loc);
  }

  const std::locale& m_loc;
};
//---------------------------------------------------------------------------

EsString EsString::toLower(const EsString& str, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString result(str);
  std::for_each(result.begin(), result.end(), _toLower(loc) );

  return result;
}
//---------------------------------------------------------------------------

EsString::value_type EsString::toUpper(EsString::value_type c, const std::locale& loc /*= EsLocale::locale()*/)
{
  return std::toupper(c, loc);
}
//---------------------------------------------------------------------------

struct _toUpper
{
  _toUpper(const std::locale& loc) : m_loc(loc) {}

  inline void operator()(EsString::value_type& ch) const ES_NOTHROW
  {
    ch = std::toupper(ch, m_loc);
  }

  const std::locale& m_loc;
};
//---------------------------------------------------------------------------

EsString EsString::toUpper(const EsString& str, const std::locale& loc /*= EsLocale::locale()*/)
{
  EsString result(str);
  std::for_each(result.begin(), result.end(), _toUpper(loc));

  return result;
}
//---------------------------------------------------------------------------

EsString EsString::booleanStringGetI18n(bool val)
{
  return val ? _("true") : _("false");
}
//---------------------------------------------------------------------------

// byte string convertions
//
// ascii representation. just force string elements to esU8
EsByteString EsString::toAscii(const EsString& src)
{
  EsByteString result(src.size(), 0);
  for(size_t idx = 0; idx < src.size(); ++idx)
    result[idx] = static_cast<esU8>(src[idx]);
  return result;
}
//---------------------------------------------------------------------------

// just widen byte string chars
EsString EsString::fromAscii(EsByteString::const_pointer src, EsByteString::size_type cnt)
{
  if( !src )
    return EsString::null();

  size_t len = (cnt == EsByteString::npos) ?
    esBstrnlen(src) :
    static_cast<size_t>(cnt);

  EsString result(len, 0);

  for(size_t idx = 0; idx < len; ++idx)
    result[idx] = static_cast<EsString::value_type>( src[idx] );

  return result;
}
//---------------------------------------------------------------------------

EsString EsString::fromAscii(const EsByteString& src)
{
  size_t cnt = src.size();
  EsString result(cnt, 0);

  EsByteString::const_pointer sp = src.data();
  for(size_t idx = 0; idx < cnt; ++idx )
    result[idx] = static_cast<EsString::value_type>( sp[idx] );

  return result;
}
//---------------------------------------------------------------------------

#if defined(ES_USE_WCHAR)
static EsStringConverter::Ptr strToUtf8convGet()
{
  static EsStringConverter::Ptr s_conv = EsStringConverter::convGet(
    "UTF-8",
    EsString::defEncoding()
  );

  return s_conv;
}
#endif
//---------------------------------------------------------------------------

// utf-8 representation
EsByteString EsString::toUtf8(const EsString& src)
{
#if defined(ES_USE_WCHAR)
  return strToUtf8convGet()->wToC(src);
#else
  // we are UTF-8 byte string by design
  return src;
#endif
}
//---------------------------------------------------------------------------

#if defined(ES_USE_WCHAR)
static EsStringConverter::Ptr utf8toStrConvGet()
{
  static EsStringConverter::Ptr s_conv = EsStringConverter::convGet(
    EsString::defEncoding(),
    "UTF-8"
  );

  return s_conv;
}
#endif
//---------------------------------------------------------------------------

EsString EsString::fromUtf8(const EsByteString& src)
{
#if defined(ES_USE_WCHAR)
  return utf8toStrConvGet()->cToW(src);
#else
  // we are UTF-8 byte string by design
  return src;
#endif
}
//---------------------------------------------------------------------------

static EsStringConverter::Ptr strToCp1251convGet()
{
  static EsStringConverter::Ptr s_conv = EsStringConverter::convGet(
    "CP1251//IGNORE",
    EsString::defEncoding()
  );

  return s_conv;
}
//---------------------------------------------------------------------------

// CP1251 representation
EsByteString EsString::toCp1251(const EsString& src)
{
#if defined(ES_USE_WCHAR)
  return strToCp1251convGet()->wToC(src);
#else
  return strToCp1251convGet()->cToC(src);
#endif
}
//---------------------------------------------------------------------------

static EsStringConverter::Ptr cp1251toStrConvGet()
{
  static EsStringConverter::Ptr s_conv = EsStringConverter::convGet(
    EsString::defEncoding(),
    "CP1251"
  );

  return s_conv;
}
//---------------------------------------------------------------------------

EsString EsString::fromCp1251(const EsByteString& src)
{
#if defined(ES_USE_WCHAR)
  return cp1251toStrConvGet()->cToW(src);
#else
  return cp1251toStrConvGet()->cToC(src);
#endif
}
//---------------------------------------------------------------------------

// universal byte conversion
EsByteString EsString::toByteString(const EsString& src, int conversion)
{
  switch(conversion)
  {
  case EsString::ASCII:
    return toAscii(src);
  case EsString::CP1251:
    return toCp1251(src);
  case EsString::UTF8:
    return toUtf8(src);
  }

  EsException::Throw(esT("Unknown byte string conversion type '%d' requested"), conversion);
  return nullByteString(); // pacify compilers
}
//---------------------------------------------------------------------------

EsString EsString::fromByteString(const EsByteString& src, int conversion)
{
  switch(conversion)
  {
  case EsString::ASCII:
    return fromAscii(src);
  case EsString::CP1251:
    return fromCp1251(src);
  case EsString::UTF8:
    return fromUtf8(src);
  }

  EsException::Throw(esT("Unknown byte string conversion type '%d' requested"), conversion);
  return EsString::null(); // pacify compilers
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_MAC || \
    ES_OS == ES_OS_IOS
# include "EsString.mac.cxx"
#elif ES_OS == ES_OS_ANDROID
# include "EsString.android.cxx"
#endif
//---------------------------------------------------------------------------


