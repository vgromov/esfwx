#include "stdafx.h"
#pragma hdrstop

#include "EsString.h"
#include <locale>
#include <iomanip>
#include <sstream>
#include <algorithm>

typedef std::basic_istringstream<EsString::value_type> EsInputStringStream;
typedef std::basic_ostringstream<EsString::value_type> EsOutputStringStream;

// special values
//
const EsString EsString::s_nullString;
const EsString EsString::s_trueString = wxT("true"); 
const EsString EsString::s_falseString = wxT("false"); 	

const EsString& EsString::s_null = EsString::s_nullString;
const EsString& EsString::s_true = EsString::s_trueString;
const EsString& EsString::s_false = EsString::s_falseString;

// misc
EsString EsString::substr(size_t offs, int cnt /*= npos*/) const
{
	EsString result( m_str.substr(offs, (cnt == npos) ? EsBasicStringT::npos : cnt) );
	return result;
}

// search
int EsString::find(value_type ch, size_t offs/* = 0*/) const
{
	EsBasicStringT::size_type pos = m_str.find(ch, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;
}

int EsString::find(const_pointer str, size_t offs/* = 0*/) const
{
	EsBasicStringT::size_type pos = m_str.find(str, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;
}

int EsString::find(const_pointer str, size_t offs, size_t cnt) const
{
	EsBasicStringT::size_type pos = m_str.find(str, offs, cnt);
	return (pos == EsBasicStringT::npos) ? npos : pos;
}

int EsString::find(const EsString& str, size_t offs/* = 0*/) const
{
	EsBasicStringT::size_type pos = m_str.find(str, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;
}

int EsString::find_first_not_of(value_type ch, size_t offs /*= 0*/) const
{
	EsBasicStringT::size_type pos = m_str.find_first_not_of(ch, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_first_not_of(const_pointer str, size_t offs /*= 0*/) const
{
	EsBasicStringT::size_type pos = m_str.find_first_not_of(str, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_first_not_of(const_pointer str, size_t offs, size_t cnt) const
{
	EsBasicStringT::size_type pos = m_str.find_first_not_of(str, offs, cnt);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_first_not_of(const EsString& str, size_t offs /*= 0*/) const
{
	EsBasicStringT::size_type pos = m_str.find_first_not_of(str, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_last_not_of(value_type ch, size_t offs /*= npos*/) const
{
	EsBasicStringT::size_type pos = m_str.find_last_not_of(ch, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_last_not_of(const_pointer str, size_t offs /*= npos*/) const
{
	EsBasicStringT::size_type pos = m_str.find_last_not_of(str, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_last_not_of(const_pointer str, size_t offs, size_t cnt) const
{
	EsBasicStringT::size_type pos = m_str.find_last_not_of(str, offs, cnt);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

int EsString::find_last_not_of(const EsString& str, size_t offs /*= npos*/) const
{
	EsBasicStringT::size_type pos = m_str.find_last_not_of(str, offs);
	return (pos == EsBasicStringT::npos) ? npos : pos;	
}

// comparison
long EsString::compare( EsString::const_pointer src, bool ignoreCase /*= false*/, const std::locale& loc /*= std::locale()*/ ) const 
{ 
	EsString str(src);
	return EsString::compare(*this, str, ignoreCase, loc); 
}

long EsString::compare( const EsBasicStringT& src, bool ignoreCase /*= false*/, const std::locale& loc /*= std::locale()*/ ) const
{
	EsString str(src);
	return EsString::compare(*this, str, ignoreCase, loc); 
}

long EsString::compare( const EsString& src, bool ignoreCase /*= false*/, const std::locale& loc /*= std::locale()*/ ) const
{
	return EsString::compare(*this, src, ignoreCase, loc); 
}

// charcode conversions
//
// get ascii representation
const std::string EsString::toAscii() const
{
	wxString tmp = *this;
	wxScopedCharBuffer cb = tmp.ToAscii();
	return std::string( cb.data(), cb.length() );
}

// get utf-8 representation
const std::string EsString::toUtf8() const
{
	wxString tmp = *this;
	wxScopedCharBuffer cb = tmp.ToUTF8();
	return std::string( cb.data(), cb.length() );
}

// static services
//

// try to automatically find radix. radix 16 is specified by 0x 0X string prefix
// radix 8 - by 0 prefix. all other cases are considered radix 10
//
static int defineRadix(EsString::const_iterator pos, EsString::const_iterator end, const std::locale& loc)
{
	int radix = 10;
	// first, skip spaces, signes until prefix is found
  while( pos != end && std::isspace(*pos, loc) ) 
		++pos; // skip prefix space
  if( pos != end && (*pos == wxT('+') || *pos == wxT('-')) )
		++pos; // skip sign
  if(pos != end && *pos++ == wxT('0'))
  {
		if( pos != end )
		{
			if( (*pos == wxT('x') || *pos == wxT('X')) ) 
				radix = 16;
			else
				radix = 8;
		}
	}
  
  return radix;
}

template <typename ResultT>
static ResultT _toInt(EsString::const_iterator pos, EsString::const_iterator end, EsString::const_iterator& endPos, 
	int radix, const std::locale& loc, bool forceAll)
{
	if( 0 == radix ) 
		radix = defineRadix(pos, end, loc);
	// check if value range is ok for radix
	EsNumericCheck::checkRangeInteger(2, 16, radix, wxT("Radix"));		
	
	EsString str(pos, end);
	EsInputStringStream strinp(str);
	strinp.imbue(loc);
	ResultT result = static_cast<ResultT>(0);
	strinp >> std::setbase(radix) >> result;
	if( strinp.fail() || (forceAll && !strinp.eof()) )
	{
		wxString typeStr = typeid( ResultT ).name();
		EsException::Throw(0, EsException::Generic, 
			_("Could not convert string '%s' to %s"), str.c_str(), typeStr.t_str());
	}
	endPos = pos + strinp.tellg();
		
	return result;	
}

template <typename ArgT>
static EsString _fromInt(ArgT val, int radix, const std::locale& loc, const EsString& errMsg)
{
	EsNumericCheck::checkRangeInteger(2, 16, radix, wxT("Radix"));
	EsOutputStringStream strout;
	strout.imbue(loc);
	strout << std::setbase(radix) << val;
	if( strout.fail() )
		EsException::Throw(0, EsException::Generic, errMsg.c_str());

	return strout.str();
}

// standard string conversion functions mappings
EsString EsString::fromLong(long l, int radix /*= 10*/, const std::locale& loc /*= std::locale()*/)
{	
	return _fromInt(l, radix, loc, _("Could not convert integer to string"));
}

long EsString::toLong(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= std::locale()*/)
{
	EsString::const_iterator endPos;
	return _toInt<long>(str.begin(), str.end(), endPos, radix, loc, true);
}

EsString EsString::fromUlong(unsigned long u, int radix /*= 10*/, const std::locale& loc /*= std::locale()*/)
{
	return _fromInt(u, radix, loc, _("Could not convert unsigned integer to string"));
}

unsigned long EsString::toUlong(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= std::locale()*/)
{
	EsString::const_iterator endPos;
	return _toInt<unsigned long>(str.begin(), str.end(), endPos, radix, loc, true);
}

EsString EsString::fromInt64(INT64 i64, int radix /*= 10*/, const std::locale& loc /*= std::locale()*/)
{
	return _fromInt(i64, radix, loc, _("Could not convert 64 bit integer to string"));
}

INT64 EsString::toInt64(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= std::locale()*/)
{
	EsString::const_iterator endPos;
	return _toInt<INT64>(str.begin(), str.end(), endPos, radix, loc, true);
}

EsString EsString::fromUint64(UINT64 u64, int radix /*= 10*/, const std::locale& loc /*= std::locale()*/)
{
	return _fromInt(u64, radix, loc, _("Could not convert unsigned 64 bit integer to string"));
}

UINT64 EsString::toUint64(const EsString& str, int radix /*= 0*/, const std::locale& loc /*= std::locale()*/)
{
	EsString::const_iterator endPos;
	return _toInt<UINT64>(str.begin(), str.end(), endPos, radix, loc, true);
}

EsString EsString::fromDouble(double d, const std::locale& loc /*= std::locale()*/)
{
	EsOutputStringStream strout;
	strout.imbue(loc);
	strout << d;
	
	return strout.str();
}

double EsString::toDouble(const EsString& str, const std::locale& loc /*= std::locale()*/)
{
	EsInputStringStream strinp(str);
	strinp.imbue(loc);
	double result = 0.;
	strinp >> result;
	if( strinp.fail() )
		EsException::Throw(0, EsException::Generic, _("Could not convert string '%s' to floating point value"), str.c_str());
	
	return result;
}

// string formatting
EsString EsString::formatV(const std::locale& loc, EsString::const_pointer fmt, va_list argptr)
{
	_locale_t cLoc = _create_locale(LC_ALL, loc.name().c_str());
	int cnt = _vsctprintf_l(fmt, cLoc, argptr);
	EsString result(cnt, 0);
	_vsntprintf_l(result.data(), cnt, fmt, cLoc, argptr);
	_free_locale(cLoc);
			
	return result;
}

EsString EsString::format(const std::locale& loc, EsString::const_pointer fmt, ...)
{
	va_list argptr;
  va_start(argptr, fmt);
	return formatV(loc, fmt, argptr);
}

// generic string formatting. uses global C locale in formatting routine call
//
EsString EsString::formatV(EsString::const_pointer fmt, va_list argptr)
{
	int cnt = _vsctprintf(fmt, argptr);
	EsString result(cnt, 0);
	_vsntprintf(result.data(), cnt, fmt, argptr);
			
	return result;
}

EsString EsString::format(EsString::const_pointer fmt, ...)
{
	va_list argptr;
  va_start(argptr, fmt);
	return formatV(fmt, argptr);
}

// convert string to|from escaped forms
//
static inline void checkCharRange(int c)
{ 
	wxCOMPILE_TIME_ASSERT(CHAR_MIN < 0, charMustBeOfSignedType); // Watch that the char is a signed type
	if ( c < CHAR_MIN || c > UCHAR_MAX ) // Check -127 .. 255
		EsException::Throw(0, EsException::Generic, 
			_("Wide character with code 0x%X encountered in place where only eight-bit characters allowed"), 
			static_cast<unsigned>(c));
}

static inline void appendHexEscape(EsString& res, EsString::value_type c)
{
	EsString::value_type str[4];
	str[0] = wxT('\\');
	str[1] = wxT('x');
	str[2] = static_cast<EsString::value_type>(EsString::binLoNibbleToHex(unsigned((unsigned char)c) >> 4));
	str[3] = static_cast<EsString::value_type>(EsString::binLoNibbleToHex(unsigned((unsigned char)c) & 0x0F));
	res.append(str, 4);
}

static inline void appendEscape(EsString& res, EsString::value_type c)
{
	EsString::value_type str[ 2 ];
	str[0] = wxT('\\');
	str[1] = c;
	res.append(str, 2);
}

static EsString::value_type escapeToChar(EsString::const_iterator cit, 
	EsString::const_iterator end, EsString::const_iterator& endPos, const std::locale& loc)
{
	wxASSERT(cit != end);
	EsString::value_type c = *cit++;
	long xoChar; // accumulator for the value of octal or hexadecimal number
	if( c == wxT('\\') && cit != end )
	{
		c = *cit++;
		switch( c )
		{
		case wxT('a'): case wxT('A'): c = wxT('\a'); break;
		case wxT('b'): case wxT('B'): c = wxT('\b'); break;
		case wxT('f'): case wxT('F'): c = wxT('\f'); break;
		case wxT('n'): case wxT('N'): c = wxT('\n'); break;
		case wxT('r'): case wxT('R'): c = wxT('\r'); break;
		case wxT('t'): case wxT('T'): c = wxT('\t'); break;
		case wxT('v'): case wxT('V'): c = wxT('\v'); break;
		case wxT('\\')               : c = wxT('\\'); break;
		case wxT('\'')               : c = wxT('\''); break;
		case wxT('\"')               : c = wxT('\"'); break;
		case wxT('x'): case wxT('X'):
			if( cit != end && EsString::isHexChar(*cit) )
			{
				BYTE nibble;
				EsString::hexToBinNibble(static_cast<BYTE>(*cit++), nibble);
				xoChar = nibble;
				if( cit != end && EsString::isHexChar(*cit) )
				{
					xoChar <<= 4;
					EsString::hexToBinNibble(static_cast<BYTE>(*cit++), nibble);
					xoChar |= nibble;
				}
				endPos = cit;
				return static_cast<EsString::value_type>(xoChar);
			}
			break;
		default:
			if( c >= wxT('0') && c <= wxT('7') )
			{
				xoChar = 0;
				do
				{
					xoChar <<= 3;
					xoChar |= c - wxT('0');
					if( cit == end )
						break;
					c = *cit++;
				} while( c >= wxT('0') && c <= wxT('7') );
				endPos = cit;
				return static_cast<EsString::value_type>(xoChar);
			}
			else
			{
				EsException::Throw(0, EsException::Generic,
					(std::isprint(c, loc) ? _("Unknown string escape sequence with character '%c'")
						: _("Unknown string escape sequence with character code 0x%X")), c);
			}
		}
	}

	endPos = cit;
	return c;
}

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
		{3, wxT("lt;"),   wxT('<')},
		{3, wxT("gt;"),   wxT('>')},
		{4, wxT("amp;"),  wxT('&')},
		{5, wxT("quot;"), wxT('"')},
		{5, wxT("apos;"), wxT('\'')} 
	};

	EsString::const_iterator cit = pos;
	if( cit != end && *cit++ == wxT('&') )
	{
		if( cit != end && *cit++ == wxT('#') )
		{
			if( cit != end )
			{
				int result;
				if( *cit == wxT('x') || *cit == wxT('X') )
					result = _toInt<int>(cit, end, endPos, 16, loc, false);
				else
					result = _toInt<int>(cit, end, endPos, 10, loc, false);
				
				if( *endPos++ == wxT(';') )
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

	EsString err(pos, pos + std::min(end-pos, 16));
	int semicolon = err.find( wxT(';') );
	if( semicolon != EsString::npos )
		err[semicolon+1] = wxT('\0');
	EsException::Throw(0, EsException::Generic, _("Unknown XML sequence '%s'"), err.c_str());
	return wxT('\0'); // pacify compiler in debug mode
}

EsString EsString::toString(const EsString& src, int masks)
{
	EsString result;
	if ( (masks & StrQuote) != 0 )
	{
		result += wxT('\"');
		result.reserve(src.size() + 2);
	}
	else
		result.reserve(src.size());

	EsString::const_iterator it = src.begin();
	EsString::const_iterator itEnd = src.end();

	if ( (masks & (StrKeepSideBlanks | StrNoCEscape)) == 0 )
	{
		for ( ; it < itEnd && (*it == wxT('\x20')); ++it )            // handle leading blanks
			result.append(wxT("\\x20"), 4);
		for ( ; it < itEnd && *(itEnd - 1) == wxT('\x20'); --itEnd )  // handle trailing blanks
			;
	}

	for ( ; it < itEnd; ++it )
	{
		EsString::value_type c = *it;
		switch ( c )
		{
		case wxT('<'):
			if ( (masks & StrXML) != 0 )
			{
				if ( (masks & StrNoCEscape) != 0 )
					result.append(wxT("&lt;"), 4);
				else
					appendHexEscape(result, c);
			}
			else
				result += c;
			break;
		case wxT('>'):
			if ( (masks & StrXML) != 0 )
			{
				if ( (masks & StrNoCEscape) != 0 )
					result.append(wxT("&gt;"), 4);
				else
					appendHexEscape(result, c);
			}
			else
				result += c;
			break;
		case wxT('&'):
			if ( (masks & StrXML) != 0 )
			{
				if ( (masks & StrNoCEscape) != 0 )
					result.append(wxT("&amp;"), 5);
				else
					appendHexEscape(result, c);
			}
			else
				result += c;
			break;
		case wxT('"'):
			if ( (masks & StrXML) != 0 )
			{
				if ( (masks & StrNoCEscape) != 0 )
					result.append(wxT("&quot;"), 6);
				else
					appendHexEscape(result, c);
			}
			else if ( (masks & StrNoCEscape) == 0 )
				appendEscape(result, c);
			else
				result += c;
			break;
		case wxT('\\'):
			if ( (masks & StrNoCEscape) != 0 )
				result += c;
			else
				appendEscape(result, c);
			break;
		default:
			if ( (static_cast<wxUChar>(c) < 0x20 // We know what we do, do not use is** function here!
#if !UNICODE
				|| ((masks & StrI18n) == 0 && static_cast<wxUChar>(c) >= 0x7Fu)
#endif
				)
				)
			{
#if UNICODE
				checkCharRange(c);
#endif

				if ( (masks & StrXML) != 0 )
				{
					if ( (masks & StrNoCEscape) != 0 )
						result.append( format(wxT("&#%u;"), static_cast<unsigned long>(static_cast<wxUChar>(c))) );
					else
						appendHexEscape(result, c);
				}
				else if ( (masks & StrNoCEscape) == 0 )
					appendHexEscape(result, c);
				else
					result += c;
			}
			else
				result += c;
		}
	}

	if ( (masks & (StrKeepSideBlanks | StrNoCEscape)) == 0 ) // Take care of trailing blanks
	{
		wxASSERT(it == itEnd); // oh sure
		for ( itEnd = src.end(); it < itEnd; ++it )
			result.append(wxT("\\x20"), 4);
	}

	if ( (masks & StrQuote) != 0 )
		result += wxT('\"');

	return result;
}

EsString EsString::fromString(const EsString& src, int masks, const std::locale& loc /*= std::locale()*/)
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
			if ( (first == wxT('\"') && last == wxT('\"')) || // starts and ends with quotes -- consider this is enough evidence this is what we need
				(first == wxT('\'') && last == wxT('\'')) )
			{
				++pos;
				--end;
				size -= 2;
			}
		}

		result.reserve(size); // at least that number of symbols...
		while( pos != end )
		{
			EsString::value_type c = *pos;
			if ( c == wxT('\\') && (masks & StrNoCEscape) == 0 )
				result += escapeToChar(pos, end, pos, loc);
			else if ( c == wxT('&') && (masks & StrXML) != 0 )
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

// create string from array of strings, delimited by delimiter, 
// each string is optionally enclosed in specific chars, if left and right enclosures are set
EsString EsString::fromStringArray(const EsString::Array& as, 
																		const EsString& delimiter /*= wxT(' ')*/, 
																		const EsString& leftEnclosure /*= EsString::s_null*/, 
																		const EsString& rightEnclosure /*= EsString::s_null*/ )
{
	EsString result;

	for(size_t idx = 0; idx < as.size(); ++idx)
	{
		if( !result.empty() )
			result += delimiter;

		result += leftEnclosure + as[idx] + rightEnclosure;
	}

	return result;
}


// return true if ch belongs to hes chars
bool EsString::isHexChar( int ch )
{
	return (ch >= wxT('0') && ch <= wxT('9')) ||
		(ch >= wxT('A') && ch <= wxT('F')) ||
		(ch >= wxT('a') && ch <= wxT('f'));
}

// conver hex char to the binary nibble
bool EsString::hexToBinNibble( int ch, BYTE& nibble )
{
	if(ch >= wxT('0') && ch <= wxT('9'))
		nibble = static_cast<BYTE>(ch-wxT('0'));
	else if(ch >= wxT('A') && ch <= wxT('F'))
		nibble = static_cast<BYTE>(ch-wxT('A')+10);
	else if(ch >= wxT('a') && ch <= wxT('f'))
		nibble = static_cast<BYTE>(ch-wxT('a')+10);
	else
		return false;	

	return true;
}

// convert hex string to binary buffer and back
// 
// return how many hex chars were converted to bin representation
size_t EsString::hexToBin( EsString::const_pointer hex, size_t hexLen, EsBinBuffer::pointer bin, size_t binLen )
{
	// check buffers && lengths. hex buffer length must be greater than 0 and even
	// tune-up actual count of data which we may extract from hex buffer and place into binary buffer
	hexLen = std::min(hexLen, binLen*2);
	if( hex && bin && hexLen > 0 && 0 == hexLen%2 )
	{
		EsString::const_pointer hexEnd = hex+hexLen;
		EsString::const_pointer hexBeg = hex;
		while(hex < hexEnd)
		{
			EsBinBuffer::value_type nibbleLo;
			EsBinBuffer::value_type nibbleHi;
			if( hexToBinNibble(*hex++, nibbleHi) && hexToBinNibble(*hex++, nibbleLo) )
				*bin++ = (nibbleHi << 4) | nibbleLo;
			else
				break;
		}

		return hex-hexBeg;
	}

	return 0;
}

EsBinBuffer EsString::hexToBin( const EsString& hex )
{
	EsBinBuffer result;
	
	if( !hex.empty() )
	{
		result.resize( hex.size()/2 );
		hexToBin( hex.c_str(), hex.size(), &result[0], result.size() );
	}
	
	return result;
}

// valid hex chars
static const char c_hexChars[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// convert low bin nibble to hex char
int EsString::binLoNibbleToHex( BYTE lowNibble )
{
	return c_hexChars[lowNibble & 0x0F];
}

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
			hex.append( 1, static_cast<wxChar>(binLoNibbleToHex((*bin) >> 4)) );
			hex.append( 1, static_cast<wxChar>(binLoNibbleToHex(*bin++)) );
		}

		return bin-binBeg;
	}

	return 0;	
}

EsString EsString::binToHex( const EsBinBuffer& bin )
{
	EsString result;
	if( !bin.empty() )
		binToHex(&bin[0], bin.size(), result);
	return result;
}

// convert string to the filename-safe form, i.e. replace all non safe symbols like /\:*?<>|
// with specified replacement symbol, '_' by-default
EsString EsString::makeSafeFileName(const EsString& src, const EsString& symbols, EsString::value_type replacement)
{
	wxRegEx regex( EsString(wxT("([/\\:*?<>|\"")) + symbols + wxT("])"), wxRE_ADVANCED );
	wxASSERT(regex.IsValid());
	
	wxString result = src.c_str();
	regex.Replace( &result, replacement );
	
	return result;
}

EsString EsString::trimLeft(const EsString& str, const EsString& symbols/* = wxT(" \t")*/)
{
	int pos = str.find_first_not_of(symbols);
	if( EsString::npos == pos )
		return EsString::s_null;
	else
		return str.substr(pos);
}

EsString EsString::trimRight(const EsString& str, const EsString& symbols/* = wxT(" \t")*/)
{
	int pos = str.find_last_not_of(symbols);
	if( EsString::npos == pos )
		return EsString::s_null;
	else
		return str.substr(0, pos+1);
}

// convert any character to an string that is printable, possibly a
// C-like escape character.
//
// If the character is blank, it is represented as \x20.
EsString EsString::charToEscapedString(int c)
{
	EsString::value_type str[4];
	if( c == wxT('\'') || c == wxT('\\') )
	{
		str[0] = wxT('\\');
		str[1] = c;
		return EsString(str, 2);
	}
	
	if( c > 0x20 && c < 0x7F ) // We know what we do, do not use is** function here! Also note \x20 is nonprintable
		return EsString(1, c);

	checkCharRange(c);
	str[0] = wxT('\\');
	str[1] = wxT('x');
	str[2] = static_cast<EsString::value_type>(binLoNibbleToHex(unsigned((unsigned char)c) >> 4u));
	str[3] = static_cast<EsString::value_type>(binLoNibbleToHex(unsigned((unsigned char)c) & 0xFu));
	return EsString(str, 4);
}

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
	wxASSERT(aproximateResultLineSize >= minimumLineWidth);
	if( strSize < aproximateResultLineSize && str.find(wxT('\n')) == EsString::npos ) // optimized case
		return str;

	EsString result;
	EsString indent;
	indent.reserve(indentCount + 1);
	indent += wxT('\n');
	indent.append(indentCount, wxT(' '));
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
			case wxT(' '): // do not use isblank here, only handle spaces
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
			case wxT('\n'): // recovery for the case there is a newline in the input
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

long EsString::compare(const EsString& s1, const EsString& s2, bool ignoreCase, const std::locale& loc /*= std::locale()*/)
{
	if( ignoreCase )
		return compare(toUpper(s1, loc), toUpper(s2, loc), false);

	EsString::const_pointer beg1 = s1.c_str();
	EsString::const_pointer end1 = beg1 + s1.size();
	EsString::const_pointer beg2 = s2.c_str();
	EsString::const_pointer end2 = beg2 + s2.size();
	long result = std::use_facet< std::collate< EsString::value_type > >(loc).compare(
		beg1, end1, beg2, end2 );
	return ((result < 0) ? cmpLess : (result > 0 ? cmpGreater : cmpEqual)); 
}

// lower|uppercase conversion
EsString::value_type EsString::toLower(EsString::value_type c, const std::locale& loc /*= std::locale()*/)
{
	return std::tolower(c, loc);
}

struct _toLower : public std::unary_function<EsString::value_type, EsString::value_type>
{
	_toLower(const std::locale& loc) : m_loc(loc) {}
	
	result_type operator()(argument_type arg) const 
	{
		return std::tolower(arg, m_loc);
	}
	
	const std::locale& m_loc;
};

EsString EsString::toLower(const EsString& str, const std::locale& loc /*= std::locale()*/)
{
	EsString result(str);	
	std::for_each(str.begin(), str.end(), _toLower(loc) );
	
	return result;
}

EsString::value_type EsString::toUpper(EsString::value_type c, const std::locale& loc /*= std::locale()*/)
{
	return std::toupper(c, loc);
}

struct _toUpper : public std::unary_function<EsString::value_type, EsString::value_type>
{
	_toUpper(const std::locale& loc) : m_loc(loc) {}
	
	result_type operator()(argument_type arg) const 
	{
		return std::toupper(arg, m_loc);
	}
	
	const std::locale& m_loc;
};

EsString EsString::toUpper(const EsString& str, const std::locale& loc /*= std::locale()*/)
{
	EsString result(str);
	std::for_each(result.begin(), result.end(), _toUpper(loc));
	
	return result;
} 

EsString EsString::fromUtf8(const char* utf8)
{
	wxString tmp(utf8, *wxConvCurrent);
	return EsString(tmp);
}