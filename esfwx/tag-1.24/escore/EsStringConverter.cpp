#include "escorepch.h"
#pragma hdrstop

#include "EsStringConverter.h"
#include <iterator>

#pragma warning(push)

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#  pragma warn -8012
#endif

typedef EsStringConverter::HConvHolder iconv_allocation_t;

#include <cwchar>
using std::mbrtowc;

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# if !defined(ES_CLANG)
using std::wcrtomb;
# else
/*
 * This code was written by Rich Felker in 2010; no copyright is claimed.
 * This code is in the public domain. Attribution is appreciated but
 * unnecessary.
 */

__weak extern "C"
size_t wcrtomb(char* s, wchar_t wc, mbstate_t* st)
{
  if (!s) return 1;
  if ((unsigned)wc < 0x80) {
    *s = wc;
    return 1;
  } else if ((unsigned)wc < 0x800) {
    *s++ = 0xc0 | (wc>>6);
    *s = 0x80 | (wc&0x3f);
    return 2;
  } else if ((unsigned)wc < 0xd800 || (unsigned)wc-0xe000 < 0x2000) {
    *s++ = 0xe0 | (wc>>12);
    *s++ = 0x80 | ((wc>>6)&0x3f);
    *s = 0x80 | (wc&0x3f);
    return 3;
  } else if ((unsigned)wc-0x10000 < 0x100000) {
    *s++ = 0xf0 | (wc>>18);
    *s++ = 0x80 | ((wc>>12)&0x3f);
    *s++ = 0x80 | ((wc>>6)&0x3f);
    *s = 0x80 | (wc&0x3f);
    return 4;
  }
  errno = EILSEQ;
  return -1;
}
# endif
#endif

#define NOMINMAX
#include "iconv/iconv.c"
#include "iconv/localcharset.c"

#pragma warning(pop)

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

static const size_t iconv_failed = static_cast<size_t>(-1);
static const size_t convBuffSize = 4096;
//---------------------------------------------------------------------------

const EsStringConverter::BuffT& EsStringConverter::null() ES_NOTHROW
{
  static const BuffT sc_null;

  return sc_null;
}
//---------------------------------------------------------------------------

// Registered converters lookup table
typedef std::map<ullong, EsStringConverter::Ptr> ConvPtrMapT;

EsStringConverter::Ptr EsStringConverter::convGet(const EsByteString& to, const EsByteString& from)
{
  static ConvPtrMapT s_convs;

  const EsStringIndexedMap& encodings = supportedEncodingsGet();

  size_t pcEnd = pureCodeEndGet(to);
  size_t idxTo = encodings.itemFind( to, pcEnd );

  if( EsStringIndexedMap::npos != idxTo )
  {
    pcEnd = pureCodeEndGet(from);
    size_t idxFrom = encodings.itemFind( from, pcEnd );

    if( EsStringIndexedMap::npos != idxFrom )
    {
      ullong key = encodings.valueGet(idxTo).asULLong() << 32;
      key |= encodings.valueGet(idxFrom).asULong();

      ConvPtrMapT::iterator it = s_convs.find( key );

      if( s_convs.end() == it )
      {
        Ptr convp;
        convp.reset( new EsStringConverter(to, from) );

        s_convs[ key ] = convp;
        return convp;
      }

      ES_ASSERT( (*it).second );
      return (*it).second;
    }
  }

  EsException::Throw(
    esT("Unsupported string converter requested: from '%s', to '%s'"),
    EsString::fromAscii(from),
    EsString::fromAscii(to)
  );

  return Ptr();
}
//---------------------------------------------------------------------------

EsStringConverter::EsStringConverter(const EsByteString& toCode, const EsByteString& fromCode) :
m_toCode( toCode ),
m_fromCode( fromCode )
{
  int result = es_iconv_open_into(
    m_toCode.c_str(),
    m_fromCode.c_str(),
    &m_hConv
  );

  if( 0 != result )
    EsException::Throw(
      esT("Could not create conversion from '%s' to '%s'"),
      EsString::fromAscii(fromCode),
      EsString::fromAscii(toCode)
    );

  m_bomSize = bomSizeGet();
}
//---------------------------------------------------------------------------

ulong EsStringConverter::toIdGet() const ES_NOTHROW
{
  const conv_t ct = (const conv_t)&m_hConv;
  return ct->oindex;
}
//---------------------------------------------------------------------------

ulong EsStringConverter::fromIdGet() const ES_NOTHROW
{
  const conv_t ct = (const conv_t)&m_hConv;
  return ct->iindex;
}
//---------------------------------------------------------------------------

// internal services
//
size_t EsStringConverter::bomSizeGet() ES_NOTHROW
{
  // Most (but not all) iconv implementations automatically insert a BOM
  // at the beginning of text converted to UTF-8, UTF-16 and UTF-32, but
  // we usually don't want this, as some of the string using code
  // assumes there is no BOM (as the exact encoding is known externally)
  // As such, when doing conversions we will strip the BOM if it exists,
  // then manually add it when writing files
  char buff[8];
  const char* src = "";
  char *dst = buff;
  size_t srcLen = 1;
  size_t dstLen = 8;

  HConvHolder locH = m_hConv; //< Thread-safeness
  size_t res = es_iconv(
    &locH,
    &src,
    &srcLen,
    &dst,
    &dstLen
  );

  size_t size = 0;
  for (src = buff; src < dst; ++src)
  {
    if (*src)
      ++size;
  }

  if(size)
  {
    // If there is a BOM, it will always be at least as big as the NUL
    size = esMax(
      size,
      (8-dstLen)/2
    );
  }

  return size;
}
//---------------------------------------------------------------------------

size_t EsStringConverter::convert(HConvHolder& locH, const char** inbuf,
  size_t* inbytesleft, char** outbuf, size_t* outbytesleft) ES_NOTHROW
{
  // If this encoding has a forced BOM (i.e. it's UTF-16 or UTF-32 without
  // a specified byte order), skip over it
  if(m_bomSize > 0 && inbytesleft && *inbytesleft)
  {
    // libiconv marks the bom as written after writing the first
    // character after the bom rather than when it writes the bom, so
    // convert at least one extra character
    char bom[8];
    char *dst = bom;
    size_t dstSize = esMin(static_cast<size_t>(8), m_bomSize + *outbytesleft);
    const char *src = *inbuf;
    size_t srcSize = *inbytesleft;

    es_iconv(&locH, &src, &srcSize, &dst, &dstSize);
  }

  return es_iconv(&locH, inbuf, inbytesleft, outbuf, outbytesleft);
}
//---------------------------------------------------------------------------

EsStringConverter::BuffT EsStringConverter::convert(const EsStringConverter::BuffT& in)
{
  size_t srcLeft = in.size();

  if( !srcLeft )
    return null();

  BuffT result;
  size_t resultSze = 0;
  const char* srcP = in.data();
  BuffT::value_type tmp[convBuffSize];
  HConvHolder locH = m_hConv;

  while(0 < srcLeft)
  {
    char* tmpP = tmp;
    size_t tmpLeft = convBuffSize;
    size_t cresult = convert(locH, &srcP, &srcLeft, &tmpP, &tmpLeft);
    if( cresult == iconv_failed )
      checkError();

    size_t addSze = convBuffSize-tmpLeft;
    size_t newSize = resultSze + addSze;
    result.resize( newSize );

    BuffT::pointer p = result.data() + resultSze;
    tmpP = tmp;
    while(addSze--)
      *p++ = *tmpP++;

    resultSze = newSize;
  }

  return result;
}
//---------------------------------------------------------------------------

void EsStringConverter::checkError() const
{
  switch(errno)
  {
  case EINVAL:
    EsException::Throw(
      esT("One or more characters in the input string were not valid ones in the given input encoding '%s'"),
      EsString::fromAscii(m_fromCode)
    );
    break;
  case EILSEQ:
    EsException::Throw(
      esT("One or more characters could not be converted to the selected target encoding '%s'"),
      EsString::fromAscii(m_toCode)
    );
    break;
  case E2BIG:
    // ignore error - we always have enough space in output buffer
    break;
  default:
    EsException::Throw(
      esT("An unknown '%s' to '%s' string conversion failure occured"),
      EsString::fromAscii(m_fromCode),
      EsString::fromAscii(m_toCode)
    );
  }
}
//---------------------------------------------------------------------------

void EsStringConverter::buffToW16(const EsStringConverter::BuffT& src, EsWideString16& dest, bool inverse)
{
  buffToW_T<EsWideString16>(
    src,
    dest,
    inverse
  );
}
//---------------------------------------------------------------------------

void EsStringConverter::buffToW32(const EsStringConverter::BuffT& src, EsWideString32& dest, bool inverse)
{
  buffToW_T<EsWideString32>(
    src,
    dest,
    inverse
  );
}
//---------------------------------------------------------------------------

void EsStringConverter::buffToW(const EsStringConverter::BuffT& src, EsWideString& dest, bool inverse)
{
  buffToW_T<EsWideString>(
    src,
    dest,
    inverse
  );
}
//---------------------------------------------------------------------------

void EsStringConverter::w16ToBuff(const EsWideString16& src, EsStringConverter::BuffT& dest, bool inverse)
{
  W_TtoBuff<EsWideString16>(
    src,
    dest,
    inverse
  );
}
//---------------------------------------------------------------------------

void EsStringConverter::w32ToBuff(const EsWideString32& src, EsStringConverter::BuffT& dest, bool inverse)
{
  W_TtoBuff<EsWideString32>(
    src,
    dest,
    inverse
  );
}
//---------------------------------------------------------------------------

void EsStringConverter::wToBuff(const EsWideString& src, EsStringConverter::BuffT& dest, bool inverse)
{
  W_TtoBuff<EsWideString>(
    src,
    dest,
    inverse
  );
}
//---------------------------------------------------------------------------

EsByteString EsStringConverter::cToC(const EsByteString& src)
{
  if( src.empty() )
    return nullByteString();

  BuffT in(src.begin(), src.end());
  const BuffT& out = convert(in);

  return EsByteString(out.begin(), out.end());
}
//---------------------------------------------------------------------------

size_t EsStringConverter::pureCodeEndGet(const EsByteString& code) ES_NOTHROW
{
  // extract pure code, to exclude anything, but code
  // there may also be addendum //TRANSLIT//IGNORE
  EsByteString::size_type pos = code.find("//");

  return (EsByteString::npos == pos) ?
    static_cast<size_t>(code.size()) :
    static_cast<size_t>(pos);
}

//---------------------------------------------------------------------------

EsByteString EsStringConverter::wToC(const EsWideString& src)
{
  return W_TtoC<EsWideString>(src);
}
//---------------------------------------------------------------------------

EsWideString EsStringConverter::cToW(const EsByteString& src)
{
  return cToW_T<EsWideString>(src);
}
//---------------------------------------------------------------------------

EsByteString EsStringConverter::w16toC(const EsWideString16& src)
{
  return W_TtoC<EsWideString16>(src);
}
//---------------------------------------------------------------------------

EsByteString EsStringConverter::w32toC(const EsWideString32& src)
{
  return W_TtoC<EsWideString32>(src);
}
//---------------------------------------------------------------------------

EsWideString16 EsStringConverter::cToW16(const EsByteString& src)
{
  return cToW_T<EsWideString16>(src);
}
//---------------------------------------------------------------------------

EsWideString32 EsStringConverter::cToW32(const EsByteString& src)
{
  return cToW_T<EsWideString32>(src);
}
//---------------------------------------------------------------------------

EsWideString EsStringConverter::wToW(const EsWideString& src)
{
  return inW_TtoOut_W_T<EsWideString, EsWideString>(src);
}
//---------------------------------------------------------------------------

EsWideString16 EsStringConverter::w16ToW16(const EsWideString16& src)
{
  return inW_TtoOut_W_T<EsWideString16, EsWideString16>(src);
}
//---------------------------------------------------------------------------

EsWideString32 EsStringConverter::w16ToW32(const EsWideString16& src)
{
  return inW_TtoOut_W_T<EsWideString16, EsWideString32>(src);
}
//---------------------------------------------------------------------------

EsWideString16 EsStringConverter::w32ToW16(const EsWideString32& src)
{
  return inW_TtoOut_W_T<EsWideString32, EsWideString16>(src);
}
//---------------------------------------------------------------------------

EsWideString32 EsStringConverter::w32ToW32(const EsWideString32& src)
{
  return inW_TtoOut_W_T<EsWideString32, EsWideString32>(src);
}
//---------------------------------------------------------------------------

// return list of all supported encodings
//
static int encLister(unsigned int namescnt, const char* const* names, unsigned int ei, void* data)
{
  EsStringIndexedMap* list = reinterpret_cast<EsStringIndexedMap*>(data);
  ES_ASSERT(list);

  for(size_t idx = 0; idx < namescnt; ++idx)
  {
    list->itemAdd(
      EsString::fromAscii(
        names[idx]
      ),
      static_cast<ulong>(ei),
      false
    );
  }

  return 0;
}
//---------------------------------------------------------------------------

const EsStringIndexedMap& EsStringConverter::supportedEncodingsGet() ES_NOTHROW
{
  static EsStringIndexedMap s_result;

  if( s_result.isEmpty() )
    es_iconvlist(
      &encLister,
      &s_result
    );

  return s_result;
}
//---------------------------------------------------------------------------

size_t EsStringConverter::charSizeGetFromCode(const EsByteString& code) ES_NOTHROW
{
  // get encoding index from code
  EsByteString::const_pointer codePtr = code.c_str();
  const struct alias* ap = aliases_lookup(
    codePtr,
    pureCodeEndGet(code)
  );

  if( 0 == ap )
    ap = aliases2_lookup(codePtr);
  ES_ASSERT(ap);

  ulong idx = ap->encoding_index;
  switch(idx)
  {
  // two-byte encodings
  case ei_ucs2:
  case ei_ucs2be:
  case ei_ucs2internal:
  case ei_ucs2le:
  case ei_ucs2swapped:
  case ei_utf16:
  case ei_utf16be:
  case ei_utf16le:
    return 2;
  // four-byte encodings
  case ei_ucs4:
  case ei_ucs4be:
  case ei_ucs4internal:
  case ei_ucs4le:
  case ei_ucs4swapped:
  case ei_utf32:
  case ei_utf32be:
  case ei_utf32le:
    return 4;
  // wchar_t type-dependent
  case ei_local_wchar_t:
    return sizeof(wchar_t);
  default:
    return 1;
  }
}
//---------------------------------------------------------------------------

// Return endianness from enconding name, in sence of ES_ENDIAN values
// ES_ENDIAN_UNDEFINED is returned for single-byte codings
int EsStringConverter::endiannessGetFromCode(const EsByteString& code) ES_NOTHROW
{
  size_t pureCodeEnd = pureCodeEndGet(code);
  EsByteString::const_pointer cp = code.c_str();

  // Get encoding index from code
  const struct alias* ap = aliases_lookup(
    cp,
    pureCodeEnd
  );
  if( 0 == ap )
    ap = aliases2_lookup(
      cp
    );
  ES_ASSERT(ap);

  ulong idx = ap->encoding_index;
  switch(idx)
  {
  case ei_ucs2:
  case ei_ucs2be:
  case ei_ucs4:
  case ei_ucs4be:
  case ei_utf16:
  case ei_utf16be:
  case ei_utf32:
  case ei_utf32be:
    return ES_BIG_ENDIAN;
  case ei_ucs2le:
  case ei_utf16le:
  case ei_ucs4le:
  case ei_utf32le:
    return ES_LITTLE_ENDIAN;
  // system-default endianness
  case ei_ucs2internal:
  case ei_ucs4internal:
    return ES_ENDIAN;
  // reversed system-default endianness
  case ei_ucs2swapped:
  case ei_ucs4swapped:
    ES_ASSERT(ES_ENDIAN != ES_PDP_ENDIAN);
    return (ES_ENDIAN == ES_BIG_ENDIAN) ? ES_LITTLE_ENDIAN : ES_BIG_ENDIAN;
  // wchar_t type-dependent
  case ei_local_wchar_t:
    return (1 < sizeof(wchar_t)) ? ES_ENDIAN : ES_ENDIAN_UNDEFINED;
  // there is no endianness for single-byte encodings
  default:
    return ES_ENDIAN_UNDEFINED;
  }
}
//---------------------------------------------------------------------------
