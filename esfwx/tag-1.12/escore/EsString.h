#ifndef _es_string_h_
#define _es_string_h_

// string class. uses from std::basic_string internally.
//
typedef std::basic_string<wxChar> EsBasicStringT;
class EKOSF_CORE_CLASS EsString
{
public:
	// constants
	//
	enum {
		// comparison results
		cmpLess = -1,
		cmpEqual = 0,
		cmpGreater = 1,
		// npos (find result)
		npos = -1,
		// wrapping
		minimumLineWidth = 20,		// Minimum line width for word wrapping. A word 20 characters wide is "internationalization"
		defaultLineWidth = 100,		// Default line width for word wrapping.
		// string conversion masks
		StrQuote = 0x01,
		StrNoCEscape = 0x02,
		StrXML = 0x04,
		StrKeepSideBlanks = 0x08,
		StrI18n = 0x10,
	};
	
	// type proxies
	//
	typedef EsBasicStringT::value_type value_type;
	typedef EsBasicStringT::reference reference;
	typedef EsBasicStringT::const_reference const_reference;	
	typedef EsBasicStringT::const_pointer const_pointer;
	typedef EsBasicStringT::pointer pointer;
	typedef EsBasicStringT::const_iterator const_iterator;
	typedef EsBasicStringT::iterator iterator;	
	
	// types
	//
	// string array type
	typedef std::vector<EsString> Array;	
	
private:
	// special value, internal member
	static const EsString s_nullString;	
	static const EsString s_trueString; 
	static const EsString s_falseString; 	
	
public:
	// constructors
	//
	EsString() {}
	EsString(const EsString& src) : m_str(src.m_str) {}
	EsString(size_t cnt, EsString::value_type ch) : m_str(cnt, ch) {}	
	EsString(EsString::const_pointer src) : m_str(src) {}	
	EsString(EsString::const_pointer src, size_t len) : m_str(src, len) {}
	EsString(EsString::const_pointer beg, EsString::const_pointer end) : m_str(beg, end) {}
	EsString(EsString::const_iterator beg, EsString::const_iterator end) : m_str(beg, end) {}
	template<typename IteratorT>
  EsString(IteratorT beg, IteratorT end) : m_str(beg, end) {}
	EsString(const EsBasicStringT& src) : m_str(src.c_str()) {}
	EsString(const wxString& src) : m_str(src.wc_str()) {}
	
	// compatibility services
	//
	// misc
	bool empty() const { return m_str.empty(); }
	void clear() { m_str.clear(); }
	size_t size() const { return m_str.size(); }
	size_t length() const { return m_str.size(); }
	void reserve(size_t cnt) { m_str.reserve(cnt); }
	EsString substr(size_t offs, int cnt = npos) const;
	// search
	int find(value_type ch, size_t offs = 0) const;
	int find(const_pointer str, size_t offs = 0) const;
	int find(const_pointer str, size_t offs, size_t cnt) const;
	int find(const EsString& str, size_t offs = 0) const;
	int find_first_not_of(value_type ch, size_t offs = 0) const;
	int find_first_not_of(const_pointer str, size_t offs = 0) const;
	int find_first_not_of(const_pointer str, size_t offs, size_t cnt) const;
	int find_first_not_of(const EsString& str, size_t offs = 0) const;
	int find_last_not_of(value_type ch, size_t offs = npos) const;
	int find_last_not_of(const_pointer str, size_t offs = npos) const;
	int find_last_not_of(const_pointer str, size_t offs, size_t cnt) const;
	int find_last_not_of(const EsString& str, size_t offs = npos) const;	
	// C-like access
	EsString::const_pointer c_str() const { return m_str.c_str(); }
	// lexicographic comparison
	long compare( EsString::const_pointer src, bool ignoreCase = false, const std::locale& loc = std::locale() ) const;
	long compare( const EsBasicStringT& src, bool ignoreCase = false, const std::locale& loc = std::locale() ) const;
	long compare( const EsString& src, bool ignoreCase = false, const std::locale& loc = std::locale() ) const;
	// iterative access
	EsString::const_iterator begin() const { return m_str.begin(); }
	EsString::iterator begin() { return m_str.begin(); }	
	EsString::const_iterator end() const { return m_str.end(); }	
	EsString::iterator end() { return m_str.end(); }
	// append
	EsString& append(size_t cnt, EsString::value_type ch) { m_str.append(cnt, ch); return *this; }
	EsString& append(EsString::const_pointer src) { m_str.append(src); return *this; }
	EsString& append(EsString::const_pointer src, size_t cnt) { m_str.append(src, cnt); return *this; }
	EsString& append(const EsString& src) { m_str.append(src.m_str); return *this; }
	EsString& append(EsString::const_pointer beg, EsString::const_pointer end) { m_str.append(beg, end); return *this; }		
	EsString& append(EsString::const_iterator beg, EsString::const_iterator end) { m_str.append(beg, end); return *this; }	
	// assignment
	EsString& assign(const EsBasicStringT& src) { m_str = src; return *this; }
	EsString& assign(const EsString& src) { m_str = src.m_str; return *this; }
	EsString& assign(const wxString& src) { m_str = src.wc_str(); return *this; }
	EsString& assign(EsString::const_pointer src) { m_str = src; return *this; }
	EsString& assign(EsString::const_pointer src, size_t len) { m_str.assign(src, len); return *this; }	
	EsString& assign(EsString::const_iterator beg, EsString::const_iterator end) { m_str.assign(beg, end); return *this; }
	EsString& assign(EsString::const_pointer beg, EsString::const_pointer end) { m_str.assign(beg, end); return *this; }	
	// indexed access
	EsString::const_reference at(size_t idx) const { return m_str.at(idx); }
	EsString::reference at(size_t idx) { return m_str.at(idx); }	
	
	// charcode conversions
	//
	// get ascii representation
	const std::string toAscii() const;
	// get utf-8 representation
	const std::string toUtf8() const;
		
	// operators
	//
	// assignment
	EsString& operator =(const EsBasicStringT& src) { return assign(src); }
	EsString& operator =(const EsString& src) { return assign(src); }
	EsString& operator =(const wxString& src) { return assign(src); }
	EsString& operator =(EsString::const_pointer src) { return assign(src); }
	// addition
	EsString operator +(EsString::value_type ch) const { EsString result(*this); result.append(1, ch); return result; }
	EsString operator +(EsString::const_pointer src) const { EsString result(*this); result.append(src); return result; }	
	EsString operator +(const EsBasicStringT& src) const { EsString result(*this); result.append(src.c_str()); return result; }
	EsString operator +(const EsString& src) const { EsString result(*this); result.append(src); return result; }
	EsString operator +(const wxString& src) const { EsString result(*this); result.append(src.wc_str()); return result; }	
	// addition with assignment
	EsString& operator +=(EsString::value_type ch) { return append(1, ch); }
	EsString& operator +=(EsString::const_pointer src) { return append(src); }	
	EsString& operator +=(const EsBasicStringT& src) { return append(src.c_str()); }
	EsString& operator +=(const EsString& src) { return append(src); }
	EsString& operator +=(const wxString& src) { return append(src.wc_str()); }
	// indexed access
	EsString::const_reference operator [](size_t idx) const { return at(idx); }
	EsString::reference operator [](size_t idx) { return at(idx); }
	// type conversion
	operator wxString () const { return wxString( c_str() ); }
	operator const EsBasicStringT& () const { return m_str; }
	
	// static services
	//
	
	// standard string conversion functions mappings
	static EsString fromLong(long l, int radix = 10, const std::locale& loc = std::locale());
	static long toLong(const EsString& str, int radix = 0, const std::locale& loc = std::locale());
	
	static EsString fromUlong(unsigned long u, int radix = 10, const std::locale& loc = std::locale());
	static unsigned long toUlong(const EsString& str, int radix = 0, const std::locale& loc = std::locale());
	
	static EsString fromInt64(INT64 i64, int radix = 10, const std::locale& loc = std::locale());
	static INT64 toInt64(const EsString& str, int radix = 0, const std::locale& loc = std::locale());
	
	static EsString fromUint64(UINT64 u64, int radix = 10, const std::locale& loc = std::locale());
	static UINT64 toUint64(const EsString& str, int radix = 0, const std::locale& loc = std::locale());

	static EsString fromDouble(double d, const std::locale& loc = std::locale());
	static double toDouble(const EsString& str, const std::locale& loc = std::locale());
	
	// string formatting. use supplied C++ locale object in formatting routines
	//
	static EsString formatV(const std::locale& loc, EsString::const_pointer fmt, va_list argptr);
	static EsString format(const std::locale& loc, EsString::const_pointer fmt, ...);
	
	// generic string formatting. uses global C locale in formatting routine call
	//
	static EsString formatV(EsString::const_pointer fmt, va_list argptr);
	static EsString format(EsString::const_pointer fmt, ...);

	// create string from array of strings, delimited by delimiter, 
	// each string is optionally enclosed in specific chars, if left and right enclosures are set
	static EsString fromStringArray(const EsString::Array& as, 
													 const EsString& delimiter = wxT(" "), 
													 const EsString& leftEnclosure = s_null, 
													 const EsString& rightEnclosure = s_null );
	
	// string-to-string conversion
	static EsString toString(const EsString& src, int masks = 0);
	static EsString fromString(const EsString& src, int masks = 0, const std::locale& loc = std::locale());
	
	// some preconfigured string conversions
	static inline EsString toXMLString(const EsString& str)
	{
		return toString(str, (StrXML|StrNoCEscape|StrI18n));
	}

	static inline EsString fromXMLString(const EsString& str, const std::locale& loc = std::locale())
	{
		return fromString(str, (StrXML|StrNoCEscape|StrI18n), loc);
	}

	// add C-like escapes to the string where necessary
	static inline EsString toEscapedString(const EsString& str)
	{
		return toString(str, StrI18n);
	}
	
	// acts as toEscapedString function plus "quotes" the result string.
	static inline EsString toQuotedEscapedString(const EsString& str)
	{
		return toString(str, StrQuote|StrI18n);
	}

	// acts as toEscapedString function plus converts XML service symbols (<, >, &, ') to escape sequences.
	static inline EsString toEscapedXmlString(const EsString& str)
	{
		return toString(str, StrXML|StrI18n);
	}

	// acts as toEscapedXmlString function plus converts XML service symbols (<, >, &, ') to escape sequences + quotes resulting string.
	static inline EsString toQuotedEscapedXmlString(const EsString& str)
	{
		return toString(str, StrQuote|StrXML|StrI18n);
	}

	// convert any character to an string that is printable, possibly a
	// C-like escape character.
	//
	// If the character is blank, it is represented as \x20.
	static EsString charToEscapedString(int c);

	// Convert the standard string that can contain C-like escape sequences to the standard
	// string, where all escape sequences are taken literally, as characters.
	// See function toEscapedString for backward conversion.
	static inline EsString fromEscapedString(const EsString& str, const std::locale& loc = std::locale())
	{
	  return fromString(str, StrQuote, loc); // possibly remove quotes
	}
	
	// return true if ch belongs to hes chars
	static bool isHexChar( int ch );
	// convert hex char to the binary nibble
	static bool hexToBinNibble( int ch, BYTE& nibble );
	// convert low bin nibble to hex char
	static int binLoNibbleToHex( BYTE lowNibble );
	// convert hex string to binary buffer and back
	// returned is number of converted hex or binary bytes, respectively
	static size_t hexToBin( EsString::const_pointer hex, size_t hexLen, EsBinBuffer::pointer bin, size_t binLen );
	static EsBinBuffer hexToBin( const EsString& hex );
	static size_t binToHex( EsBinBuffer::const_pointer bin, size_t binLen, EsString& hex );
	static EsString binToHex( const EsBinBuffer& bin );

	// convert string to the filename-safe form, i.e. replace all non safe symbols like /\:*?<>|
	// with specified replacement symbol, '_' by-default. If additional symbols need to be replaced in file name,
	// they should be specified in symbols parameter
	//
	static EsString makeSafeFileName(const EsString& src, const EsString& symbols = EsString::s_null, EsString::value_type replacement = wxT('_'));
	
	// trim trailing|leading symbols from string
	static EsString trimLeft(const EsString& str, const EsString& symbols = wxT(" \t"));
	static EsString trimRight(const EsString& str, const EsString& symbols = wxT(" \t"));
	static inline EsString trim(const EsString& str, const EsString& symbols = wxT(" \t")) 
	{ 
		return trimLeft( trimRight(str, symbols), symbols); 
	}

	// wrap string if its length exceeds line width. wrapped part is prepended with indentCount
	static EsString wrap(const EsString& str, unsigned indentCount, unsigned lineWidth);
	
	// string comparison
	static long compare(const EsString& s1, const EsString& s2, bool ignoreCase, const std::locale& loc = std::locale());
	
	// lower|uppercase conversion
	static EsString::value_type toLower(EsString::value_type c, const std::locale& loc = std::locale());
	static EsString toLower(const EsString& str, const std::locale& loc = std::locale());
	static EsString::value_type toUpper(EsString::value_type c, const std::locale& loc = std::locale());
	static EsString toUpper(const EsString& str, const std::locale& loc = std::locale()); 
	
	// get boolean representation
	static const EsString& getBooleanStr(bool val) { return val ? EsString::s_true : EsString::s_false; }
	
	// static char convertion
	//
	static EsString fromUtf8(const char* utf8);
	
	// special values
	static const EsString& s_null;	
	static const EsString& s_true; 
	static const EsString& s_false; 	
	
protected:
	// internal raw buffer access
	EsString::pointer data() { return const_cast<EsString::pointer>(m_str.data()); }
	
protected:
	EsBasicStringT m_str;	
};

// comparison
inline bool operator== (const EsString& _1, const EsString& _2) { return EsString::cmpEqual == _1.compare(_2); }
inline bool operator!= (const EsString& _1, const EsString& _2) { return EsString::cmpEqual != _1.compare(_2); }

#endif // _es_string_h_
