#include "escorepch.h"
#pragma hdrstop

#include "EsLocale.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#if !defined(ES_USE_NARROW_ES_CHAR) && !defined(ES_CHAR_IS_WCHAR_T)
# if !defined(ES_USE_CUSTOM_CLASSIC_LOCALE)
#   define ES_USE_CUSTOM_CLASSIC_LOCALE
# endif
#endif

//---------------------------------------------------------------------------
// Basic character classificator for custom locale(s)
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL || defined(ES_USE_CUSTOM_CLASSIC_LOCALE)
enum {
  es_BB	      = 0x80,           /* BEL, BS, etc. */
  es_CN	      = 0x40,           /* CR, FF, HT, NL, VT */
  es_DI	      = 0x20,           /* '0'-'9' */
  es_LO	      = 0x10,           /* 'a'-'z' */
  es_PU       = 0x08,           /* punctuation */
  es_SP	      = 0x04,           /* space */
  es_UP       = 0x02,           /* 'A'-'Z' */
  es_XD	      = 0x01,           /* '0'-'9', 'A'-'F', 'a'-'f' */
  es_XBB      = es_BB | es_CN,  /* blank or control */
  es_XDI      = es_DI | es_XD,	/* digit or hex digit */
  es_XLO      = es_LO | es_XD,	/* lower case or hex digit */
  es_XUP      = es_UP | es_XD	  /* upper case or hex digit */
};

static ulong esCharTypeBasicGet(ulong ch)
{
  static const uint8_t sc_Ctyptab[256] =
  {	/* indexed by [0, UCHAR_MAX] */
   0, es_BB, es_BB, es_BB, es_BB, es_BB, es_BB, es_BB,
  es_BB, es_XBB, es_XBB, es_XBB, es_XBB, es_XBB, es_BB, es_BB,
  es_BB, es_BB, es_BB, es_BB, es_BB, es_BB, es_BB, es_BB,
  es_BB, es_BB, es_BB, es_BB, es_BB, es_BB, es_BB, es_BB,
  es_SP, es_PU, es_PU, es_PU, es_PU, es_PU, es_PU, es_PU,
  es_PU, es_PU, es_PU, es_PU, es_PU, es_PU, es_PU, es_PU,
  es_XDI, es_XDI, es_XDI, es_XDI, es_XDI, es_XDI, es_XDI, es_XDI,
  es_XDI, es_XDI, es_PU, es_PU, es_PU, es_PU, es_PU, es_PU,
  es_PU, es_XUP, es_XUP, es_XUP, es_XUP, es_XUP, es_XUP, es_UP,
  es_UP, es_UP, es_UP, es_UP, es_UP, es_UP, es_UP, es_UP,
  es_UP, es_UP, es_UP, es_UP, es_UP, es_UP, es_UP, es_UP,
  es_UP, es_UP, es_UP, es_PU, es_PU, es_PU, es_PU, es_PU,
  es_PU, es_XLO, es_XLO, es_XLO, es_XLO, es_XLO, es_XLO, es_LO,
  es_LO, es_LO, es_LO, es_LO, es_LO, es_LO, es_LO, es_LO,
  es_LO, es_LO, es_LO, es_LO, es_LO, es_LO, es_LO, es_LO,
  es_LO, es_LO, es_LO, es_PU, es_PU, es_PU, es_PU, es_BB,
  };	/* rest all match nothing */

  if( ch > 255 )
    return 0;

  return static_cast<ulong>(
    sc_Ctyptab[ch]
  );
}
#endif // 0 != ES_USE_EMBEDDED_LOCALE_IMPL || defined(ES_USE_CUSTOM_CLASSIC_LOCALE)
//---------------------------------------------------------------------------

#ifdef ES_USE_CUSTOM_CLASSIC_LOCALE
using namespace std;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// ES_CHAR facets typedefs
//
typedef ctype<ES_CHAR>                    ES_CHAR_ctype;
typedef numpunct<ES_CHAR>                 ES_CHAR_numpunct;
typedef num_get<ES_CHAR>                  ES_CHAR_num_get;
typedef num_put<ES_CHAR>                  ES_CHAR_num_put;
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// ctype<ES_CHAR> facet specialization implementation
//

template <>
ctype<ES_CHAR>::~ctype()
{}
//---------------------------------------------------------------------------

template <>
bool ctype<ES_CHAR>::do_is(ctype<ES_CHAR>::mask m, ctype<ES_CHAR>::char_type c) const
{
  return esCharTypeBasicGet(c) & m;
}
//---------------------------------------------------------------------------

template <>
const ctype<ES_CHAR>::char_type* ctype<ES_CHAR>::do_is(const ctype<ES_CHAR>::char_type* low,
  const ctype<ES_CHAR>::char_type* high, ctype<ES_CHAR>::mask* vec) const
{
  for(; low != high; ++low, ++vec)
    *vec =
      static_cast<mask>(
        esCharTypeBasicGet(*low)
      );

  return low;
}
//---------------------------------------------------------------------------

template <>
const ctype<ES_CHAR>::char_type* ctype<ES_CHAR>::do_scan_is(ctype<ES_CHAR>::mask m,
  const ctype<ES_CHAR>::char_type* low, const ctype<ES_CHAR>::char_type* high) const
{
  for(; low != high; ++low)
    if( esCharTypeBasicGet(*low) & m )
      break;

  return low;
}
//---------------------------------------------------------------------------

template <>
const ctype<ES_CHAR>::char_type* ctype<ES_CHAR>::do_scan_not(ctype<ES_CHAR>::mask m,
  const ctype<ES_CHAR>::char_type* low, const ctype<ES_CHAR>::char_type* high) const
{
  for(; low != high; ++low)
    if( !(esCharTypeBasicGet(*low) & m) )
      break;

  return low;
}
//---------------------------------------------------------------------------

template <>
ctype<ES_CHAR>::char_type ctype<ES_CHAR>::do_toupper(ctype<ES_CHAR>::char_type c) const
{
  return (static_cast<unsigned>(c) < 256) ?
    std::toupper(c) :
    c;
}
//---------------------------------------------------------------------------

template <>
const ctype<ES_CHAR>::char_type* ctype<ES_CHAR>::do_toupper
    (ctype<ES_CHAR>::char_type* low, const ctype<ES_CHAR>::char_type* high) const
{
  for(; low != high; ++low)
    *low = (static_cast<unsigned>(*low) < 256) ?
      std::toupper(*low) :
      *low;

  return low;
}
//---------------------------------------------------------------------------

template <>
ctype<ES_CHAR>::char_type ctype<ES_CHAR>::do_tolower(ctype<ES_CHAR>::char_type c) const
{
  return (static_cast<unsigned>(c) < 256) ?
    std::tolower(c) :
    c;
}
//---------------------------------------------------------------------------

template <>
const ctype<ES_CHAR>::char_type* ctype<ES_CHAR>::do_tolower
    (ctype<ES_CHAR>::char_type* low, const ctype<ES_CHAR>::char_type* high) const
{
  for(; low != high; ++low)
    *low = (static_cast<unsigned>(*low) < 256) ?
      std::tolower(*low) :
      *low;

  return low;
}
//---------------------------------------------------------------------------

template <>
ctype<ES_CHAR>::char_type ctype<ES_CHAR>::do_widen(char c) const
{
  return static_cast<ctype<ES_CHAR>::char_type>(c);
}
//---------------------------------------------------------------------------

template <>
const char* ctype<ES_CHAR>::do_widen(const char* low, const char* high,
    ctype<ES_CHAR>::char_type* dest) const
{
  for(; low != high; ++low, ++dest)
    *dest = *low;

  return low;
}
//---------------------------------------------------------------------------

template <>
char ctype<ES_CHAR>::do_narrow(ctype<ES_CHAR>::char_type c, char dfault) const
{
  if( static_cast<unsigned>(c) < 256 )
    return static_cast<char>(c);

  return dfault;
}
//---------------------------------------------------------------------------

template <>
const ctype<ES_CHAR>::char_type* ctype<ES_CHAR>::do_narrow
    (const ctype<ES_CHAR>::char_type* low, const ctype<ES_CHAR>::char_type* high,
    char dfault, char* dest) const
{
  for(; low != high; ++low, ++dest)
    if( static_cast<unsigned>(*low) < 256 )
      *dest = static_cast<char>(*low);
    else
      *dest = dfault;

  return low;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// numpunct<ES_CHAR> facet specialization implementation
#if ES_OS == ES_OS_ANDROID || \
    ES_OS == ES_OS_IOS

template<>
numpunct<ES_CHAR>::~numpunct()
{
  delete _M_data;
}
//---------------------------------------------------------------------------

template<>
void numpunct<ES_CHAR>::_M_initialize_numpunct(__c_locale __cloc)
{
  const std::locale& tmp = std::locale::classic();

  _M_data = new __cache_type();

  const std::numpunct<char>& np = std::use_facet< std::numpunct<char> >(tmp);

  const std::string& tn = np.truename();
  _M_data->_M_truename_size = tn.size();
  _M_data->_M_truename = new char_type[_M_data->_M_truename_size];
  for(size_t idx = 0; idx < tn.size(); ++idx)
    ((char_type*)_M_data->_M_truename)[idx] = tn[idx];

  const std::string& fn = np.falsename();
  _M_data->_M_falsename_size = fn.size();
  _M_data->_M_falsename = new char_type[_M_data->_M_falsename_size];
  for(size_t idx = 0; idx < fn.size(); ++idx)
    ((char_type*)_M_data->_M_falsename)[idx] = fn[idx];

  _M_data->_M_decimal_point = np.decimal_point();
  _M_data->_M_thousands_sep = np.thousands_sep();

  _M_data->_M_grouping_size = np.grouping().size();
  _M_data->_M_grouping = new char[_M_data->_M_grouping_size];

  const std::string& grp = np.grouping();
  std::copy(grp.c_str(), grp.c_str()+grp.size(), (char*)_M_data->_M_grouping);
  _M_data->_M_use_grouping = (_M_data->_M_grouping_size
     && static_cast<signed char>(_M_data->_M_grouping[0]) > 0
     && (_M_data->_M_grouping[0] != __gnu_cxx::__numeric_traits<char>::__max));

  _M_data->_M_allocated = true;

  for(int idx = 0; idx < __num_base::_S_oend; ++idx)
    ((char_type*)_M_data->_M_atoms_out)[idx] = __num_base::_S_atoms_out[idx];
  for(int idx = 0; idx < __num_base::_S_iend; ++idx)
    ((char_type*)_M_data->_M_atoms_in)[idx] = __num_base::_S_atoms_in[idx];
}
//---------------------------------------------------------------------------
#endif // ES_OS == ES_OS_ANDROID || ES_OS == ES_OS_IOS

template<>
ES_CHAR numpunct<ES_CHAR>::do_decimal_point() const
#if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
 ES_NOTHROW
#endif
{	// return decimal point
  return esT('.');
}

template<>
ES_CHAR numpunct<ES_CHAR>::do_thousands_sep() const
#if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
 ES_NOTHROW
#endif
{	// return thousands separator
  return esT('\0');
}

template<>
std::string numpunct<ES_CHAR>::do_grouping() const
#if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
 ES_NOTHROW
#endif
{	// return grouping string
  return "";
}

template<>
EsBasicStringT numpunct<ES_CHAR>::do_falsename() const
#if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
 ES_NOTHROW
#endif
{	// return name for false
  return esT("false");
}

template<>
EsBasicStringT numpunct<ES_CHAR>::do_truename() const
#if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
 ES_NOTHROW
#endif
{	// return name for true
  return esT("true");
}
//---------------------------------------------------------------------------
#endif // ES_USE_CUSTOM_CLASSIC_LOCALE

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL

struct EsLocInfoNode
{
  const ES_CHAR* m_loc;
  const ES_CHAR* m_minus;
  const ES_CHAR* m_plus;
  const ES_CHAR* m_sepDec;
  const ES_CHAR* m_sepTh;
  const char* m_grp;
  const ES_CHAR* m_cur;
  int m_curFracDigits;
  int m_curFmtPos;
  int m_curFmtNeg;
  const ES_CHAR* m_moneySepDec;
  const ES_CHAR* m_moneySepTh;
  const char* m_moneyGrp;
  const ES_CHAR* m_sepLst;
  const ES_CHAR* m_am;
  const ES_CHAR* m_pm;
  const ES_CHAR* m_sepDate;
  const ES_CHAR* m_sepTime;
  const ES_CHAR* m_timeLong;
  const ES_CHAR* m_dateShort;
  const ES_CHAR* m_timeShort;
  const ES_CHAR* m_dateLong;
  const ES_CHAR* m_yearMonth;
};

#define ES_LOCINFO_BEGIN \
static const EsLocInfoNode* locInfoNodesGet() { \
static const EsLocInfoNode sc_locInfos[] = {

#define ES_LOCINFO_ENTRY(locname, locale_snativedigits, locale_snegativesign, locale_spositivesign, locale_sdecimal, locale_sthousand, locale_sgrouping, locale_scurrency, locale_icurrdigits, locale_icurrency, locale_inegcurr, locale_smondecimalsep, locale_smonthousandsep, locale_smongrouping, locale_slist, locale_s1159, locale_s2359, locale_sdate, locale_stime, locale_stimeformat, locale_sshortdate, locale_sshorttime, locale_slongdate, locale_syearmonth) \
  { esT( locname ), \
    esT( locale_snegativesign ), \
    esT( locale_spositivesign ), \
    esT( locale_sdecimal ), \
    esT( locale_sthousand ), \
    locale_sgrouping, \
    esT( locale_scurrency ), \
    locale_icurrdigits, \
    locale_icurrency, \
    locale_inegcurr, \
    esT( locale_smondecimalsep ), \
    esT( locale_smonthousandsep ), \
    locale_smongrouping, \
    esT( locale_slist ), \
    esT( locale_s1159 ), \
    esT( locale_s2359 ), \
    esT( locale_sdate ), \
    esT( locale_stime ), \
    esT( locale_stimeformat ), \
    esT( locale_sshortdate ), \
    esT( locale_sshorttime ), \
    esT( locale_slongdate ), \
    esT( locale_syearmonth ) },

#define ES_LOCINFO_END \
  { 0 } \
}; \
  return sc_locInfos; \
}

#include "EsLocale.def"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Custom locale implementations
//
class EsCtype : public std::ctype<EsString::value_type>
{
public:
  typedef std::ctype<EsString::value_type> BaseT;

public:
  EsCtype(const EsLocInfoNode& node) : m_node(node) {}

  bool do_is(BaseT::mask m, BaseT::char_type c) const
  {
    return esCharTypeBasicGet(c) & m;
  }

  const BaseT::char_type* do_is(
    const BaseT::char_type* low,
    const BaseT::char_type* high,
    BaseT::mask* vec
  ) const
  {
    for(; low != high; ++low, ++vec)
      *vec = static_cast<BaseT::mask>(
        esCharTypeBasicGet(*low)
      );

    return low;
  }

  const BaseT::char_type* do_scan_is(
    BaseT::mask m,
    const BaseT::char_type* low,
    const BaseT::char_type* high
  ) const
  {
    for(; low != high; ++low)
      if( esCharTypeBasicGet(*low) & m )
        break;

    return low;
  }

  const BaseT::char_type* do_scan_not(
    BaseT::mask m,
    const BaseT::char_type* low,
    const BaseT::char_type* high
  ) const
  {
    for(; low != high; ++low)
      if( !(esCharTypeBasicGet(*low) & m) )
        break;

    return low;
  }

  BaseT::char_type do_toupper(BaseT::char_type c) const
  {
    return (static_cast<unsigned>(c) < 256) ?
      ::toupper(static_cast<unsigned>(c)) :
      c;
  }

  const BaseT::char_type* do_toupper(
    BaseT::char_type* low,
    const BaseT::char_type* high
  ) const
  {
    for(; low != high; ++low)
      *low = (static_cast<unsigned>(*low) < 256) ?
        ::toupper(static_cast<unsigned>(*low)) :
        *low;

    return low;
  }

  BaseT::char_type do_tolower(BaseT::char_type c) const
  {
    return (static_cast<unsigned>(c) < 256) ?
      ::tolower(static_cast<unsigned>(c)) :
      c;
  }

  const BaseT::char_type* do_tolower(
    BaseT::char_type* low,
    const BaseT::char_type* high
  ) const
  {
    for(; low != high; ++low)
      *low = (static_cast<unsigned>(*low) < 256) ?
        ::tolower(static_cast<unsigned>(*low)) :
        *low;

    return low;
  }

  BaseT::char_type do_widen(char c) const
  {
    return static_cast<BaseT::char_type>(c);
  }

  const char* do_widen(
    const char* low,
    const char* high,
    BaseT::char_type* dest
  ) const
  {
    for(; low != high; ++low, ++dest)
      *dest = *low;

    return low;
  }

  char do_narrow(BaseT::char_type c, char dfault) const
  {
    if( static_cast<unsigned>(c) < 256 )
      return static_cast<char>(c);

    return dfault;
  }

  const BaseT::char_type* do_narrow(
    const BaseT::char_type* low,
    const BaseT::char_type* high,
    char dfault,
    char* dest
  ) const
  {
    for(; low != high; ++low, ++dest)
      if( static_cast<unsigned>(*low) < 256 )
        *dest = static_cast<char>(*low);
      else
        *dest = dfault;

    return low;
  }

protected:
  const EsLocInfoNode& m_node;
};
//---------------------------------------------------------------------------

class EsNumpunct : public std::numpunct<EsString::value_type>
{
public:
  typedef std::numpunct<EsString::value_type> BaseT;

public:
  EsNumpunct(const EsLocInfoNode& node) :
  m_node(node)
  {
    m_grp.assign( m_node.m_grp );
  }

  BaseT::char_type do_decimal_point() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return decimal point
    if( m_node.m_sepDec )
      return m_node.m_sepDec[0];

    return esT('.');
  }

  BaseT::char_type do_thousands_sep() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return thousands separator
    if( m_node.m_sepTh )
      return m_node.m_sepTh[0];

    return esT(',');
  }

 std::string do_grouping() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return grouping string
    return m_grp;
  }

  EsBasicStringT do_falsename() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return name for false
    static const EsBasicStringT sc_false = esT("false");

    return sc_false;
  }

  EsBasicStringT do_truename() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return name for true
    static const EsBasicStringT sc_true = esT("true");

    return sc_true;
  }

protected:
  const EsLocInfoNode& m_node;
  std::string m_grp;
};
//---------------------------------------------------------------------------

class EsMoneypunct : public std::moneypunct<EsString::value_type>
{
public:
  typedef std::moneypunct<EsString::value_type> BaseT;

public:
  EsMoneypunct(const EsLocInfoNode& node) :
  m_node(node)
  {
    m_grp.assign( m_node.m_moneyGrp );
    m_cur.assign( m_node.m_cur );
    m_ps.assign( m_node.m_plus );
    m_ns.assign( m_node.m_minus );

    // initialize positive money fmt pattern according to MSDN LOCALE_ICURRENCY values
    //0	Prefix, no separation, for example, $1.1
    //1	Suffix, no separation, for example, 1.1$
    //2	Prefix, 1-character separation, for example, $ 1.1
    //3	Suffix, 1-character separation, for example, 1.1 $
    //
    switch( m_node.m_curFmtPos )
    {
    case 1:
      m_pospatt = {
        std::money_base::sign,
        std::money_base::value,
        std::money_base::none,
        std::money_base::symbol
      };
      break;
    case 2:
      m_pospatt = {
        std::money_base::symbol,
        std::money_base::space,
        std::money_base::sign,
        std::money_base::value
      };
      break;
    case 3:
      m_pospatt = {
        std::money_base::sign,
        std::money_base::value,
        std::money_base::space,
        std::money_base::symbol
      };
      break;
    default:
      m_pospatt = {
        std::money_base::symbol,
        std::money_base::sign,
        std::money_base::none,
        std::money_base::value
      };
      break;
    }

    // initialize negative money fmt pattern according to MSDN LOCALE_INEGCURR values
    //0	Left parenthesis, monetary symbol, number, right parenthesis; for example, ($1.1)
    //1	Negative sign, monetary symbol, number; for example, -$1.1
    //2	Monetary symbol, negative sign, number; for example, $-1.1
    //3	Monetary symbol, number, negative sign; for example, $1.1-
    //4	Left parenthesis, number, monetary symbol, right parenthesis; for example, (1.1$)
    //5	Negative sign, number, monetary symbol; for example, -1.1$
    //6	Number, negative sign, monetary symbol; for example, 1.1-$
    //7	Number, monetary symbol, negative sign; for example, 1.1$-
    //8	Negative sign, number, space, monetary symbol (like #5, but with a space before the monetary symbol); for example, -1.1 $
    //9	Negative sign, monetary symbol, space, number (like #1, but with a space after the monetary symbol); for example, -$ 1.1
    //10	Number, space, monetary symbol, negative sign (like #7, but with a space before the monetary symbol); for example, 1.1 $-
    //11	Monetary symbol, space, number, negative sign (like #3, but with a space after the monetary symbol); for example, $ 1.1-
    //12	Monetary symbol, space, negative sign, number (like #2, but with a space after the monetary symbol); for example, $ -1.1
    //13	Number, negative sign, space, monetary symbol (like #6, but with a space before the monetary symbol); for example, 1.1- $
    //14	Left parenthesis, monetary symbol, space, number, right parenthesis (like #0, but with a space after the monetary symbol); for example, ($ 1.1)
    //15	Left parenthesis, number, space, monetary symbol, right parenthesis (like #4, but with a space before the monetary symbol); for example, (1.1 $)
    switch( m_node.m_curFmtNeg )
    {
	  //case 0: //<0 Left parenthesis, monetary symbol, number, right parenthesis; for example, ($1.1)
	  case 1: //<1 Negative sign, monetary symbol, number; for example, -$1.1
		  m_negpatt = {
			  std::money_base::sign,
        std::money_base::symbol,
        std::money_base::none,
			  std::money_base::value
		  };
      break;
    case 2: //<	Monetary symbol, negative sign, number; for example, $-1.1
      m_negpatt = {
        std::money_base::symbol,
        std::money_base::sign,
        std::money_base::none,
        std::money_base::value
      };
      break;
    case 3: //<3	Monetary symbol, number, negative sign; for example, $1.1-
      m_negpatt = {
        std::money_base::symbol,
        std::money_base::none,
        std::money_base::value,
        std::money_base::sign
      };
      break;
    //4	Left parenthesis, number, monetary symbol, right parenthesis; for example, (1.1$)
    case 5: //<5	Negative sign, number, monetary symbol; for example, -1.1$
      m_negpatt = {
        std::money_base::sign,
        std::money_base::value,
        std::money_base::none,
        std::money_base::symbol
      };
      break;
    case 6: //<6	Number, negative sign, monetary symbol; for example, 1.1-$
      m_negpatt = {
        std::money_base::value,
        std::money_base::sign,
        std::money_base::none,
        std::money_base::symbol
      };
      break;
    case 7: //<7	Number, monetary symbol, negative sign; for example, 1.1$-
      m_negpatt = {
        std::money_base::value,
        std::money_base::none,
        std::money_base::symbol,
        std::money_base::sign
      };
      break;
    case 8: //<8	Negative sign, number, space, monetary symbol (like #5, but with a space before the monetary symbol); for example, -1.1 $
      m_negpatt = {
        std::money_base::sign,
        std::money_base::value,
        std::money_base::space,
        std::money_base::symbol
      };
      break;
    case 9: //<9	Negative sign, monetary symbol, space, number (like #1, but with a space after the monetary symbol); for example, -$ 1.1
      m_negpatt = {
        std::money_base::sign,
        std::money_base::symbol,
        std::money_base::space,
        std::money_base::value
      };
      break;
    case 10: //<10	Number, space, monetary symbol, negative sign (like #7, but with a space before the monetary symbol); for example, 1.1 $-
      m_negpatt = {
        std::money_base::value,
        std::money_base::space,
        std::money_base::symbol,
        std::money_base::sign
      };
      break;
    case 11: //<11	Monetary symbol, space, number, negative sign (like #3, but with a space after the monetary symbol); for example, $ 1.1-
      m_negpatt = {
        std::money_base::symbol,
        std::money_base::space,
        std::money_base::value,
        std::money_base::sign
      };
      break;
    case 12: //<12	Monetary symbol, space, negative sign, number (like #2, but with a space after the monetary symbol); for example, $ -1.1
      m_negpatt = {
        std::money_base::symbol,
        std::money_base::space,
        std::money_base::sign,
        std::money_base::value
      };
      break;
    case 13: //<13	Number, negative sign, space, monetary symbol (like #6, but with a space before the monetary symbol); for example, 1.1- $
      m_negpatt = {
        std::money_base::value,
        std::money_base::sign,
        std::money_base::space,
        std::money_base::symbol
      };
      break;
    //case 14: //<14	Left parenthesis, monetary symbol, space, number, right parenthesis (like #0, but with a space after the monetary symbol); for example, ($ 1.1)
    //case 15: //<15	Left parenthesis, number, space, monetary symbol, right parenthesis (like #4, but with a space before the monetary symbol); for example, (1.1 $)
	  default:
      m_negpatt = {
        std::money_base::symbol,
        std::money_base::sign,
        std::money_base::none,
        std::money_base::value
      };
      break;
    }
  }

  BaseT::char_type do_decimal_point() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return decimal point
    if( m_node.m_moneySepDec )
      return m_node.m_moneySepDec[0];

    return esT('.');
  }

  BaseT::char_type do_thousands_sep() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return thousands separator
    if( m_node.m_moneySepTh )
      return m_node.m_moneySepTh[0];

    return esT(',');
  }

  std::string do_grouping() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return grouping string
    return m_grp;
  }

  EsBasicStringT do_curr_symbol() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return currency string
    return m_cur;
  }

  EsBasicStringT do_positive_sign() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return positive sign
    return m_ps;
  }

  EsBasicStringT do_negative_sign() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return negative sign
    return m_ns;
  }

  int do_frac_digits() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return money fractiona digits
    return m_node.m_curFracDigits;
  }

  std::money_base::pattern do_pos_format() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return positive money formatting pattern
    return m_pospatt;
  }

  std::money_base::pattern do_neg_format() const
  #if (ES_OS != ES_OS_ANDROID) && (ES_OS != ES_OS_MAC)
   ES_NOTHROW
  #endif
  {	// return negative money formatting pattern
    return m_negpatt;
  }

protected:
  const EsLocInfoNode& m_node;
  std::string m_grp;
  EsBasicStringT m_cur;
  EsBasicStringT m_ps;
  EsBasicStringT m_ns;
  std::money_base::pattern m_pospatt;
  std::money_base::pattern m_negpatt;
};
//---------------------------------------------------------------------------

class EsNumPut : public std::num_put<EsString::value_type>
{
public:
  EsNumPut(const EsLocInfoNode& node) : m_node(node) {}

protected:
  const EsLocInfoNode& m_node;
};
//---------------------------------------------------------------------------

class EsNumGet : public std::num_get<EsString::value_type>
{
public:
  EsNumGet(const EsLocInfoNode& node) : m_node(node) {}

protected:
  const EsLocInfoNode& m_node;
};
//---------------------------------------------------------------------------

#endif // 0 != ES_USE_EMBEDDED_LOCALE_IMPL

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Locale entry index mapping from locale name string
class EsLocaleInfoMap
{
public:
  typedef std::shared_ptr< std::locale > LocalePtr;
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  typedef std::map<ulong, LocalePtr> LocaleMap;
#else
  typedef std::map<EsString::HashT, LocalePtr> LocaleMap;
#endif

protected:
  EsLocaleInfoMap() {}

public:
  static EsLocaleInfoMap& instanceGet();

  const std::locale& localeGet(const EsString& loc, bool fallbackToClassic);

#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t localeNodePosFind(const EsString& loc) const;
  size_t localeNodePosFind(const std::locale& loc) const;
#endif

protected:
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsLocaleInfoMap::LocalePtr localeCreate(const EsLocInfoNode& node, ulong pos);
#else
  EsLocaleInfoMap::LocalePtr localeCreate(const EsString& loc, bool fallbackToClassic);
#endif

protected:
  mutable EsCriticalSection m_cs;
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  EsStringIndexedMap m_map;
#endif
  LocaleMap m_locMap;

private:
  EsLocaleInfoMap(const EsLocaleInfoMap&) ES_REMOVEDECL;
  EsLocaleInfoMap& operator=(const EsLocaleInfoMap&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

EsLocaleInfoMap& EsLocaleInfoMap::instanceGet()
{
  static EsLocaleInfoMap s_map;
  return s_map;
}
//---------------------------------------------------------------------------

#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
EsLocaleInfoMap::LocalePtr EsLocaleInfoMap::localeCreate(const EsLocInfoNode& node, ulong pos)
{
  ES_ASSERT(nullptr != node.m_loc);

  ES_ASSERT(m_locMap.end() == m_locMap.find(pos));

  LocalePtr loc =
    std::make_shared<std::locale>(
      std::locale(
        std::locale(
          std::locale(
            std::locale(
              EsLocale::locale(),  //< Use our global locale as a base template
              new EsCtype(node)
            ),
            new EsMoneypunct(node)
          ),
          new EsNumpunct(node)
        ),
        new EsNumPut(node)
      ),
      new EsNumGet(node)
    );

  ES_ASSERT(loc);

  m_locMap[pos] = loc;

  return loc;
}

#else

EsLocaleInfoMap::LocalePtr EsLocaleInfoMap::localeCreate(const EsString& loc, bool fallbackToClassic)
{
  ES_ASSERT(m_locMap.end() == m_locMap.find(loc.hashGet()));

  LocalePtr newloc;
  try
  {
    newloc = std::make_shared<std::locale>(
      EsString::toAscii(loc).c_str()
    );
  }
  catch(const std::exception& ex )
  {
    if( !fallbackToClassic )
    {
      const EsString& msg = EsString::fromUtf8( ex.what() );
      // Rethrow our exception, converted from ex
      //
      EsException::Throw(
        0,
        EsException::severityGeneric,
        EsException::facilityEsCommon,
        esT("Could not create '%s' cultural locale, reason: '%s'"),
        loc,
        msg.empty() ?
          esT("Unspecified reason") :
          msg.c_str()
      );
    }
  }

  if( newloc )
    m_locMap[ loc.hashGet() ] = newloc;

  return newloc;
}

#endif // ES_USE_EMBEDDED_LOCALE_IMPL
//---------------------------------------------------------------------------

#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
size_t EsLocaleInfoMap::localeNodePosFind(const EsString& loc) const
{
  EsCriticalSectionLocker lock(m_cs);

  return m_map.itemFind(loc);
}

size_t EsLocaleInfoMap::localeNodePosFind(const std::locale& loc) const
{
  EsCriticalSectionLocker lock(m_cs);

  for(auto const& item : m_locMap)
  {
    if(&loc == item.second.get())
      return item.first;
  }

  return EsStringIndexedMap::npos;
}
#endif
//---------------------------------------------------------------------------

const std::locale& EsLocaleInfoMap::localeGet(const EsString& loc, bool fallbackToClassic)
{
  if( loc.empty() ) //< Always return classic locale for an empty name
    return EsLocale::locale();

#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL

  size_t pos = localeNodePosFind(loc);

  if( EsStringIndexedMap::npos == pos )
  {
    const EsLocInfoNode* nodes = locInfoNodesGet();
    ES_ASSERT(nodes);

    // Perform slow table lookup
    pos = 0;
    while(1)
    {
      const EsLocInfoNode& node = nodes[pos];
      if( nullptr == node.m_loc ) //< End of loc infos array
        break;

      if( 0 == loc.compare(node.m_loc) )
      {
        EsCriticalSectionLocker lock(m_cs);

        m_map.itemAdd(
          loc,
          static_cast<ulong>(pos)
        );

        // Create custom locale object, using found info node
        return *localeCreate(
          node,
          pos
        ).get();
      }

      ++pos;
    }
  }
  else //< Return cached locale object
  {
    EsCriticalSectionLocker lock(m_cs);

    ulong entryIdx = m_map.valueGet(pos).asULong();
    auto cit = m_locMap.find(entryIdx);
    ES_ASSERT(cit != m_locMap.end());

    return *(*cit).second.get();
  }

  if( !fallbackToClassic )
    EsException::Throw(
      0,
      EsException::severityGeneric,
      EsException::facilityEsCommon,
      esT("Could not create '%s' cultural locale, locale is not supported"),
      loc
    );

#else

  EsCriticalSectionLocker lock(m_cs);
  auto cit = m_locMap.find( loc.hashGet() );

  if( cit != m_locMap.end() )
    return *(*cit).second.get();
  else
  {
    LocalePtr newloc = localeCreate(
      loc,
      fallbackToClassic
    );

    if( newloc )
      return *newloc.get();
  }

#endif // ES_USE_EMBEDDED_LOCALE_IMPL

  // Locale not found, fallback
  return EsLocale::locale();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

const std::locale& EsLocale::locale() ES_NOTHROW
{
  static std::locale s_loc =
#ifdef ES_USE_CUSTOM_CLASSIC_LOCALE
    std::locale(
      std::locale(
        std::locale(
          std::locale(
            std::locale::classic(),
            new ES_CHAR_ctype
          ),
          new ES_CHAR_numpunct
        ),
        new ES_CHAR_num_put
      ),
      new ES_CHAR_num_get
    );
#else
    std::locale::classic();
#endif // ES_USE_CUSTOM_CLASSIC_LOCALE

  static bool s_globalized = false;
  if( !s_globalized )
  {
    std::locale::global( s_loc );
    s_globalized = true;
  }

  return s_loc;
}
//---------------------------------------------------------------------------

const std::locale& EsLocale::locale(const EsString& loc, bool fallbackToClassic/*= false*/)
{
  EsLocale::locale(); //< Make sure default locale is created and globalized first

  return EsLocaleInfoMap::instanceGet().localeGet(
    loc,
    fallbackToClassic
  );
}
//---------------------------------------------------------------------------

#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
const EsStringArray& EsLocale::supportedLocaleNamesGet()
{
  static const EsStringArray sc_result;

  if( sc_result.empty() )
  {
    const_cast<EsStringArray&>(sc_result).reserve( 64 );

    const EsLocInfoNode* nodes = locInfoNodesGet();

    while( 1 )
    {
      const EsLocInfoNode& node = *nodes++;
      if( !node.m_loc )
        break;

      const_cast<EsStringArray&>(sc_result).push_back(
        node.m_loc
      );
    }
  }

  return sc_result;
}
#endif // ES_USE_EMBEDDED_LOCALE_IMPL
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsString::value_type EsLocale::decimalSeparatorGet(const std::locale& loc)
{
  const std::numpunct<ES_CHAR>& np = std::use_facet< std::numpunct<ES_CHAR> >( loc );
  return np.decimal_point();
}
//---------------------------------------------------------------------------

EsString::value_type EsLocale::thousandSeparatorGet(const std::locale& loc)
{
  const std::numpunct<ES_CHAR>& np = std::use_facet< std::numpunct<ES_CHAR> >( loc );
  return np.thousands_sep();
}
//---------------------------------------------------------------------------

EsString::value_type EsLocale::listSeparatorGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_sepLst )
      return node.m_sepLst[0];
  }
#endif

  return esT(';');
}
//---------------------------------------------------------------------------

EsString::value_type EsLocale::moneyDecimalSeparatorGet(const std::locale& loc)
{
  const std::moneypunct<ES_CHAR>& mp = std::use_facet< std::moneypunct<ES_CHAR> >( loc );
  return mp.decimal_point();
}
//---------------------------------------------------------------------------

EsString::value_type EsLocale::moneyThousandSeparatorGet(const std::locale& loc)
{
  const std::moneypunct<ES_CHAR>& mp = std::use_facet< std::moneypunct<ES_CHAR> >( loc );
  return mp.thousands_sep();
}
//---------------------------------------------------------------------------

EsString EsLocale::moneySymbolGet(const std::locale& loc)
{
  const std::moneypunct<ES_CHAR>& mp = std::use_facet< std::moneypunct<ES_CHAR> >( loc );
  return mp.curr_symbol();
}
//---------------------------------------------------------------------------

EsString::value_type EsLocale::timeSeparatorGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_sepTime )
      return node.m_sepTime[0];
  }
#endif

  return esT(':');
}
//---------------------------------------------------------------------------

EsString::value_type EsLocale::dateSeparatorGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_sepDate )
      return node.m_sepDate[0];
  }
#endif

  return esT('/');
}
//---------------------------------------------------------------------------

EsString EsLocale::timeAmGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_am )
      return node.m_am;
  }
#endif

  return esT("AM");
}
//---------------------------------------------------------------------------

EsString EsLocale::timePmGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_pm )
      return node.m_pm;
  }
#endif

  return esT("PM");
}
//---------------------------------------------------------------------------

EsString EsLocale::timeFmtGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_timeLong )
      return node.m_timeLong;
  }
#endif

  return esT("HH:mm:ss");
}
//---------------------------------------------------------------------------

EsString EsLocale::dateFmtGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_dateLong )
      return node.m_dateLong;
  }
#endif

  return esT("dddd, MMMM d, yyyy");
}
//---------------------------------------------------------------------------

EsString EsLocale::timeFmtShortGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_timeShort )
      return node.m_timeShort;
  }
#endif

  return esT("HH:mm");
}
//---------------------------------------------------------------------------

EsString EsLocale::dateFmtShortGet(const std::locale& loc)
{
#if 0 != ES_USE_EMBEDDED_LOCALE_IMPL
  size_t pos = EsLocaleInfoMap::instanceGet().localeNodePosFind(loc);
  if( EsStringIndexedMap::npos != pos )
  {
    const EsLocInfoNode& node = locInfoNodesGet()[pos];

    if( node.m_dateShort )
      return node.m_dateShort;
  }
#endif

  return esT("yyyy-MM-dd");
}
//---------------------------------------------------------------------------

EsString EsLocale::dateTimeFmtGet(const std::locale& loc)
{
  return dateFmtGet(loc) + esT(" ") + timeFmtGet(loc);
}
//---------------------------------------------------------------------------


