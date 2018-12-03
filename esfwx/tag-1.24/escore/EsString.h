#ifndef _es_string_h_
#define _es_string_h_

// Forward decl
//
namespace EsLocale
{
  ESCORE_FUNC(const std::locale&, locale() ES_NOTHROW );
}

/// String class. Uses std::basic_string internally.
///
class ESCORE_CLASS EsString
{
public:
  /// Type proxies
  ///
  typedef EsBasicStringT::traits_type traits_type;
  typedef EsBasicStringT::value_type value_type;
  typedef EsBasicStringT::reference reference;
  typedef EsBasicStringT::const_reference const_reference;
  typedef EsBasicStringT::const_pointer const_pointer;
  typedef EsBasicStringT::pointer pointer;
  typedef EsBasicStringT::const_iterator const_iterator;
  typedef EsBasicStringT::iterator iterator;

  /// Constants
  ///
  enum {
    /// Element byte count
    value_size                = sizeof(EsString::value_type),

    /// Comparison results
    cmpLess                   = -1,
    cmpEqual                  = 0,
    cmpGreater                = 1,

    /// Wrapping
    minimumLineWidth          = 20,     ///< Minimum line width for word wrapping. A word 20 characters wide is "internationalization"
    defaultLineWidth          = 100,    ///< Default line width for word wrapping.

    /// String conversion masks
    StrNoFlags                = 0,      ///< Special 'no flags' value for convenience
    StrQuote                  = 0x01,   ///< Enclose | disenclose string in | from quotes
    StrNoCEscape              = 0x02,   ///< Do not include C escape symbols in string, pass them to output as-is
    StrXML                    = 0x04,   ///< Process input string and escape XML-aware stuff
    StrKeepSideBlanks         = 0x08,   ///< Do not trim side blank chars from string in fromString processor
    StrEscapeNonAscii         = 0x10,   ///< Do not escape non-ascii characters

    /// Byte encodings
    ASCII                     = 1,
    CP1251,
    UTF8,
  };

  enum HashInit {
    doHashInit
  };

  /// npos (find result)
  static const size_t npos = static_cast<size_t>(-1);

  /// Types
  ///
  /// Hash value type used in string
  typedef esU64 HashT;

  /// Shared string pointer
  typedef std::shared_ptr<EsString> Ptr;

  /// String array type
  typedef std::vector<EsString> Array;
  typedef std::shared_ptr<EsString::Array> ArrayPtr;

public:
  /// Constructors
  ///
  EsString() ES_NOTHROW : m_hashInvalid(true) {}
  EsString(EsString::value_type ch) ES_NOTHROW : m_str(1, ch), m_hashInvalid(true) {}
  EsString(const EsString& src) ES_NOTHROW : m_str(src.m_str), m_hash(src.m_hash), m_hashInvalid(src.m_hashInvalid) {}
  EsString(size_t cnt, EsString::value_type ch) ES_NOTHROW : m_str(cnt, ch), m_hashInvalid(true) {}
  EsString(EsString::const_pointer src) ES_NOTHROW : m_str(src), m_hashInvalid(true) {}
  EsString(EsString::const_pointer src, HashInit) ES_NOTHROW : m_str(src), m_hashInvalid(true) { hashGet(); }
  EsString(EsString::const_pointer src, size_t len) ES_NOTHROW : m_str(src, len), m_hashInvalid(true) {}
  EsString(EsString::const_pointer beg, EsString::const_pointer end) ES_NOTHROW : m_str(beg, end), m_hashInvalid(true) {}
  EsString(EsString::const_iterator beg, EsString::const_iterator end) ES_NOTHROW : m_str(beg, end), m_hashInvalid(true) {}
  template<typename IteratorT>
  EsString(IteratorT beg, IteratorT end) ES_NOTHROW : m_str(beg, end), m_hashInvalid(true) {}
  EsString(const EsBasicStringT& src) ES_NOTHROW : m_str(src.c_str()), m_hashInvalid(true) {}
  EsString(EsByteString::const_pointer src, bool unicode = false) : m_hashInvalid(true) { m_str = (unicode ? fromUtf8(src) : fromAscii(src)); }

#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (4 == ES_CHAR_SIZE)
  EsString(EsWideString16::const_pointer src, size_t len = EsString::npos, const EsByteString& conv = nullByteString());
  EsString(const EsWideString16& src, const EsByteString& conv = nullByteString());
#endif
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (2 == ES_CHAR_SIZE)
  EsString(EsWideString32::const_pointer src, size_t len = EsString::npos, const EsByteString& conv = nullByteString());
  EsString(const EsWideString32& src, const EsByteString& conv = nullByteString());
#endif

  ~EsString() ES_NOTHROW;

  /// String hash value access with on-demand hash recalculation
  esU64 hashGet() const ES_NOTHROW;
  inline bool isValidHash() const ES_NOTHROW { return !m_hashInvalid; }

  /// Compatibility services
  ///

  /// Misc
  inline bool empty() const ES_NOTHROW { return m_str.empty(); }
  inline void clear() ES_NOTHROW { m_str.clear(); m_hashInvalid = true; }
  inline size_t size() const ES_NOTHROW { return m_str.size(); }
  inline size_t length() const ES_NOTHROW { return m_str.size(); }
  inline void reserve(size_t cnt) { m_str.reserve(cnt); }
  inline void resize(size_t cnt) { m_str.resize(cnt); m_hashInvalid = true; }
  EsString substr(size_t offs, size_t cnt = npos) const;
  inline iterator erase( iterator first, iterator last) { return m_str.erase(first, last); }
  inline iterator erase( iterator it ) { return m_str.erase(it); }
  inline void erase(size_t pos, size_t count = EsString::npos) { m_str.erase(pos, count); }
  void swap(EsString& other) ES_NOTHROW;
  void move(EsString& other) ES_NOTHROW;

  /// Search
  size_t find(value_type ch, size_t offs = 0) const;
  size_t find(const_pointer str, size_t offs = 0) const;
  size_t find(const_pointer str, size_t offs, size_t cnt) const;
  size_t find(const EsString& str, size_t offs = 0) const;
  size_t find_first_of(value_type ch, size_t offs = 0) const;
  size_t find_first_of(const_pointer str, size_t offs = 0) const;
  size_t find_first_of(const EsString& str, size_t offs = 0) const;
  size_t find_last_of(value_type ch, size_t end = EsString::npos) const;
  size_t find_last_of(const_pointer str, size_t end = EsString::npos) const;
  size_t find_last_of(const EsString& str, size_t end = EsString::npos) const;
  size_t find_first_not_of(value_type ch, size_t offs = 0) const;
  size_t find_first_not_of(const_pointer str, size_t offs = 0) const;
  size_t find_first_not_of(const_pointer str, size_t offs, size_t cnt) const;
  size_t find_first_not_of(const EsString& str, size_t offs = 0) const;
  size_t find_last_not_of(value_type ch, size_t offs = EsString::npos) const;
  size_t find_last_not_of(const_pointer str, size_t offs = EsString::npos) const;
  size_t find_last_not_of(const_pointer str, size_t offs, size_t cnt) const;
  size_t find_last_not_of(const EsString& str, size_t offs = EsString::npos) const;

  // C-like access
  inline EsString::const_pointer c_str() const ES_NOTHROW { return m_str.c_str(); }
  inline EsString::const_pointer data() const ES_NOTHROW { return m_str.data(); }

  // NB! _NOT SAFE_ raw buffer writeable access.
  inline EsString::pointer data() ES_NOTHROW { return const_cast<EsString::pointer>(m_str.data()); }

  // Lexicographic comparison
  long compare( EsString::const_pointer src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale() ) const;
  long compare( const EsBasicStringT& src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale() ) const;
  long compare( const EsString& src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale() ) const;
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (4 == ES_CHAR_SIZE)
  long compare(EsWideString16::const_pointer src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale()) const;
  long compare(const EsWideString16& src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale()) const;
#endif
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (4 == ES_CHAR_SIZE)
  long compare(EsWideString32::const_pointer src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale()) const;
  long compare(const EsWideString32& src, bool ignoreCase = false, const std::locale& loc = EsLocale::locale()) const;
#endif

  // Iterative access
  inline EsString::const_iterator begin() const ES_NOTHROW { return m_str.begin(); }
  inline EsString::iterator begin() ES_NOTHROW { return m_str.begin(); }
  inline EsString::const_iterator end() const ES_NOTHROW { return m_str.end(); }
  inline EsString::iterator end() ES_NOTHROW { return m_str.end(); }

  // Append
  inline EsString& append(size_t cnt, EsString::value_type ch) { m_str.append(cnt, ch); m_hashInvalid = true; return *this; }
  inline EsString& append(EsString::const_pointer src) { if(nullptr == src) return *this; m_str.append(src); m_hashInvalid = true; return *this; }
  inline EsString& append(EsString::const_pointer src, size_t cnt) { if(nullptr == src || 0 == cnt) return *this; m_str.append(src, cnt); m_hashInvalid = true; return *this; }
  inline EsString& append(const EsString& src) { m_str.append(src.m_str); m_hashInvalid = true; return *this; }
  inline EsString& append(EsString::const_pointer beg, EsString::const_pointer end) { if(nullptr == beg || end <= beg) return *this; m_str.append(beg, end); m_hashInvalid = true; return *this; }
  inline EsString& append(EsString::const_iterator beg, EsString::const_iterator end) { m_str.append(beg, end); m_hashInvalid = true; return *this; }

  // Insert
  inline EsString& insert(size_t pos, const EsString& str) { m_str.insert(pos, str.m_str); m_hashInvalid = true; return *this; }
  inline EsString& insert(size_t pos, const EsString& str, size_t subpos, size_t sublen) { m_str.insert(pos, str.m_str, subpos, sublen); m_hashInvalid = true; return *this; }
  inline EsString& insert(size_t pos, EsString::const_pointer s) { if(nullptr == s) return *this; m_str.insert(pos, s); m_hashInvalid = true; return *this; }
  inline EsString& insert(size_t pos, EsString::const_pointer s, size_t n) { if(nullptr == s || 0 == n) return *this; m_str.insert(pos, s, n); m_hashInvalid = true; return *this; }
  inline EsString& insert(size_t pos, size_t n, EsString::value_type c) { m_str.insert(pos, n, c); m_hashInvalid = true; return *this; }
  inline void insert(EsString::iterator p, size_t n, EsString::value_type c) { m_str.insert(p, n, c); m_hashInvalid = true; }
  inline EsString::iterator insert(EsString::iterator p, char c) { m_hashInvalid = true; return m_str.insert(p, c); }
  template <typename InputIteratorT>
  inline void insert(EsString::iterator p, InputIteratorT first, InputIteratorT last) { m_str.insert(p, first, last); m_hashInvalid = true; }

  // Filling
  inline void push_back(EsString::value_type item) { m_str.push_back(item); m_hashInvalid = true; }

  // Assignment
  inline EsString& assign(const EsBasicStringT& src) { m_str = src; m_hashInvalid = true; return *this; }
  inline EsString& assign(const EsString& src) { m_str = src.m_str; m_hashInvalid = src.m_hashInvalid; m_hash = src.m_hash; return *this; }
  inline EsString& assign(EsString::const_pointer src) { if(nullptr == src) { clear(); return *this; } m_str = src; m_hashInvalid = true; return *this; }
  inline EsString& assign(EsString::const_pointer src, size_t len) { if(nullptr == src || 0 == len) { clear(); return *this; } m_str.assign(src, len); m_hashInvalid = true; return *this; }
  inline EsString& assign(EsString::const_iterator beg, EsString::const_iterator end) { m_str.assign(beg, end); m_hashInvalid = true; return *this; }
  inline EsString& assign(EsString::const_pointer beg, EsString::const_pointer end) { if(nullptr == beg || end <= beg) { clear(); return *this; } m_str.assign(beg, end); m_hashInvalid = true; return *this; }
  template<typename IteratorT>
  inline EsString& assign(IteratorT beg, IteratorT end) { m_str.assign(beg, end); m_hashInvalid = true; return *this; }
  // Indexed access
  inline EsString::const_reference at(size_t idx) const { return m_str.at(idx); }
  inline EsString::reference at(size_t idx) { m_hashInvalid = true; return m_str.at(idx); }

  // operators
  //
  // assignment
  inline EsString& operator =(const EsBasicStringT& src) { return assign(src); }
  inline EsString& operator =(const EsString& src) { return assign(src); }
  inline EsString& operator =(EsString::const_pointer src) { return assign(src); }

  // addition
  inline EsString operator +(EsString::value_type ch) const { EsString result(*this); result.append(1, ch); return result; }
  inline EsString operator +(EsString::const_pointer src) const { EsString result(*this); result.append(src); return result; }
  inline EsString operator +(const EsBasicStringT& src) const { EsString result(*this); result.append(src.c_str()); return result; }
  inline EsString operator +(const EsString& src) const { EsString result(*this); result.append(src); return result; }
  // addition with assignment
  inline EsString& operator +=(EsString::value_type ch) { return append(1, ch); }
  inline EsString& operator +=(EsString::const_pointer src) { return append(src); }
  inline EsString& operator +=(const EsBasicStringT& src) { return append(src.c_str()); }
  inline EsString& operator +=(const EsString& src) { return append(src); }
  // indexed access
  inline EsString::const_reference operator [](size_t idx) const { return at(idx); }
  inline EsString::reference operator [](size_t idx) { return at(idx); }
  // type conversion
  operator const EsBasicStringT& () const ES_NOTHROW { return m_str; }

  // static services
  //
  // standard string conversion functions mappings
  static EsString fromLong(long l, int radix = 10, const std::locale& loc = EsLocale::locale());
  static long toLong(const EsString& str, int radix = 0, const std::locale& loc = EsLocale::locale());

  static EsString fromULong(unsigned long u, int radix = 10, const std::locale& loc = EsLocale::locale());
  static unsigned long toULong(const EsString& str, int radix = 0, const std::locale& loc = EsLocale::locale());

  static EsString fromInt64(esI64 i64, int radix = 10, const std::locale& loc = EsLocale::locale());
  static esI64 toInt64(const EsString& str, int radix = 0, const std::locale& loc = EsLocale::locale());

  static EsString fromUInt64(esU64 u64, int radix = 10, const std::locale& loc = EsLocale::locale());
  static esU64 toUInt64(const EsString& str, int radix = 0, const std::locale& loc = EsLocale::locale());

  static EsString fromDouble(double d, const std::locale& loc = EsLocale::locale());
  static double toDouble(const EsString& str, const std::locale& loc = EsLocale::locale());

  // String formatting. use supplied C++ locale object in formatting routines
  //
  static EsString formatV(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& args);
  static inline EsString formatV(EsString::const_pointer fmt, const EsVariant& args)
  {
    return EsString::formatV(EsLocale::locale(), fmt, args);
  }

  // Pseudo-variadic formatting with explicit locale
  static EsString format(const std::locale& loc, EsString::const_pointer fmt);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12, const EsVariant& arg13);
  static EsString format(const std::locale& loc, EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12, const EsVariant& arg13, const EsVariant& arg14);

  // Pseudo-variadic formatting with implicit locale
  static EsString format(EsString::const_pointer fmt) { return format(EsLocale::locale(), fmt); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0) { return format(EsLocale::locale(), fmt, arg0); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1) { return format(EsLocale::locale(), fmt, arg0, arg1); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12, const EsVariant& arg13) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13); }
  static EsString format(EsString::const_pointer fmt, const EsVariant& arg0, const EsVariant& arg1, const EsVariant& arg2, const EsVariant& arg3, const EsVariant& arg4, const EsVariant& arg5, const EsVariant& arg6, const EsVariant& arg7, const EsVariant& arg8, const EsVariant& arg9, const EsVariant& arg10, const EsVariant& arg11, const EsVariant& arg12, const EsVariant& arg13, const EsVariant& arg14) { return format(EsLocale::locale(), fmt, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14); }

  // create string from array of strings, delimited by delimiter,
  // each string is optionally enclosed in specific chars, if left and right enclosures are set
  static EsString fromStringArray(const EsString::Array& as,
                           const EsString& delimiter = EsString::space(),
                           const EsString& leftEnclosure = EsString::null(),
                           const EsString& rightEnclosure = EsString::null() ) ES_NOTHROW;

  /// String-to-string conversion
  static EsString toString(const EsString& src, ulong flags = 0);
  static EsString fromString(const EsString& src, ulong flags = 0, const std::locale& loc = EsLocale::locale());

  /// Some preconfigured string conversions
  ///
  ///
  static inline EsString toXMLString(const EsString& str)
  {
    return toString(str, (StrXML|StrNoCEscape));
  }

  static inline EsString fromXMLString(const EsString& str, const std::locale& loc = EsLocale::locale())
  {
    return fromString(str, (StrXML|StrNoCEscape), loc);
  }

  /// Add C-like escapes to the string where necessary
  static inline EsString toEscapedString(const EsString& str)
  {
    return toString(str);
  }

  /// Acts as toEscapedString function plus "quotes" the result string.
  static inline EsString toQuotedEscapedString(const EsString& str)
  {
    return toString(str, StrQuote);
  }

  /// Acts as toEscapedString function plus converts XML service symbols (<, >, &, ') to escape sequences.
  static inline EsString toEscapedXmlString(const EsString& str)
  {
    return toString(str, StrXML);
  }

  /// Acts as toEscapedXmlString function plus converts XML service symbols (<, >, &, ') to escape sequences + quotes resulting string.
  static inline EsString toQuotedEscapedXmlString(const EsString& str)
  {
    return toString(str, StrQuote|StrXML);
  }

  /// Convert any character to an string that is printable, possibly a
  /// C-like escape character.
  /// If the character is blank, it is represented as \x20.
  ///
  static EsString charToEscapedString(int c);

  /// Convert the standard string that can contain C-like escape sequences to the standard
  /// string, where all escape sequences are taken literally, as characters.
  /// See function toEscapedString for backward conversion.
  ///
  static inline EsString fromEscapedString(const EsString& str, const std::locale& loc = EsLocale::locale())
  {
    return fromString(str, StrQuote, loc); // possibly remove quotes
  }

  /// Return true if ch belongs to hes chars
  static bool isHexChar( int ch ) ES_NOTHROW;

  /// Convert hex char to the binary nibble
  static bool hexToBinNibble( int ch, esU8& nibble ) ES_NOTHROW;

  /// Convert low bin nibble to hex char
  static int binLoNibbleToHex( esU8 lowNibble ) ES_NOTHROW;

  /// Convert hex string to binary buffer and back
  /// Returned is number of converted hex or binary bytes, respectively
  ///
  static size_t hexToBin( EsString::const_pointer hex, size_t hexLen, EsBinBuffer::pointer bin, size_t binLen ) ES_NOTHROW;
  static EsBinBuffer hexToBin( const EsString& hex );
  static size_t binToHex( EsBinBuffer::const_pointer bin, size_t binLen, EsString& hex );
  static EsString binToHex( const EsBinBuffer& bin );

  // trim trailing|leading symbols from string
  static EsString trimLeft(const EsString& str, const EsString& symbols = EsString::trimSymbolsDef());
  static EsString trimRight(const EsString& str, const EsString& symbols = EsString::trimSymbolsDef());
  static inline EsString trim(const EsString& str, const EsString& symbols = EsString::trimSymbolsDef())
  {
    return trimLeft( trimRight(str, symbols), symbols);
  }

  // wrap string if its length exceeds line width. wrapped part is prepended with indentCount
  static EsString wrap(const EsString& str, unsigned indentCount, unsigned lineWidth);

  // string comparison
  static long scompare(const EsString& s1, const EsString& s2, bool ignoreCase, const std::locale& loc = EsLocale::locale());
  static long scompare(EsString::const_pointer s1, EsString::const_pointer s2, bool ignoreCase = false);

  // lower|uppercase conversion
  static EsString::value_type toLower(EsString::value_type c, const std::locale& loc = EsLocale::locale());
  static EsString toLower(const EsString& str, const std::locale& loc = EsLocale::locale());
  static EsString::value_type toUpper(EsString::value_type c, const std::locale& loc = EsLocale::locale());
  static EsString toUpper(const EsString& str, const std::locale& loc = EsLocale::locale());

  /// get boolean representation
  static const EsString& booleanStringGet(bool val) ES_NOTHROW { return val ? EsStdNames::_true() : EsStdNames::_false(); }
  /// get internationalized variant of boolean string
  static EsString booleanStringGetI18n(bool val);

  // byte string convertions
  //
  // ascii representation
  static EsByteString toAscii(const EsString& src);
  static EsString fromAscii(EsByteString::const_pointer src, EsByteString::size_type cnt = EsByteString::npos);
  static EsString fromAscii(const EsByteString& src);
  // utf-8 representation
  static EsByteString toUtf8(const EsString& src);
  static EsString fromUtf8(const EsByteString& src);
  // CP1251 representation
  static EsByteString toCp1251(const EsString& src);
  static EsString fromCp1251(const EsByteString& src);
  // universal byte conversion
  static EsByteString toByteString(const EsString& src, int conversion);
  static EsString fromByteString(const EsByteString& src, int conversion);

  // special values
  static const EsString& null() ES_NOTHROW;
  static const EsString::Array& nullArray() ES_NOTHROW;
  static const EsByteString& defEncoding() ES_NOTHROW;
  static const EsString& tab() ES_NOTHROW;
  static const EsString& asterisk() ES_NOTHROW;
  static const EsString& trimSymbolsDef() ES_NOTHROW;
  static const EsString& space() ES_NOTHROW;

#ifdef ES_MODERN_CPP
  // Modern C++ stuff
  //
  // Move constructor
  EsString(EsString&& src) ES_NOTHROW : m_hashInvalid(true)
  {
    move(src);
  }
  // Move assignment
  EsString& operator =(EsString&& src) ES_NOTHROW
  {
    move(src);
    return *this;
  }
#endif

  /// Platform-specific stuff
  ///
#if ES_OS == ES_OS_MAC || \
    ES_OS == ES_OS_IOS

  static EsByteString cfStringRefToByteString(CFStringRef cfstr);
  static EsString cfStringRefToString(CFStringRef cfstr);
  static CFStringRef toCfStringRef(const EsByteString& bstr, int conversion = EsString::UTF8);
  static CFStringRef toCfStringRef(const EsString& str);

#elif ES_OS == ES_OS_ANDROID

  static EsByteString jstringToByteString(JStringPtr str);
  static EsString jstringToString(JStringPtr str);
  static JStringPtr toJstring(const EsByteString& bstr, int conversion = EsString::UTF8);
  static JStringPtr toJstring(const EsString& str);

#endif

  /// Generic hex buffer conversion to binary buffer
  template <typename HexBuffValueT>
  static size_t hexToBin(const HexBuffValueT* hex, size_t hexLen, EsBinBuffer::pointer bin, size_t binLen) ES_NOTHROW
  {
    // Ensure hexToBinNibble is capableof handling input char value
    ES_COMPILE_TIME_ASSERT(sizeof(HexBuffValueT) <= sizeof(int), _HexBuffValueT_is_less_or_equal_to_int);

    // Check buffers && lengths. hex buffer length must be greater than 0 and even
    // Tune-up actual count of data which we may extract from hex buffer and place into binary buffer
    //

    hexLen = esMin(hexLen, binLen * 2);

    if(
      hex &&
      bin &&
      hexLen > 0 &&
      0 == (hexLen % 2)
    )
    {
      const HexBuffValueT* hexEnd = hex + hexLen;
      const HexBuffValueT* hexBeg = hex;
      while(hex < hexEnd)
      {
        EsBinBuffer::value_type nibbleLo;
        EsBinBuffer::value_type nibbleHi;

        if(
           hexToBinNibble(
             *hex++,
             nibbleHi
            ) &&
            hexToBinNibble(
              *hex++,
              nibbleLo
            )
        )
          *bin++ = (nibbleHi << 4) | nibbleLo;
        else
          break;
      }

      return hex - hexBeg;
    }

    return 0;
  }

protected:
  EsBasicStringT m_str;
  mutable EsHashFNV_1a_64 m_hash;
  mutable bool m_hashInvalid;
};

/// Comparison and equality | inequality operators
inline bool operator == (const EsString& _1, const EsString& _2) ES_NOTHROW { return EsString::cmpEqual == _1.compare(_2); }
inline bool operator == (EsString::const_pointer _1, const EsString& _2) ES_NOTHROW { ES_ASSERT(_1); return EsString::cmpEqual == _2.compare(_1); }
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (2 == ES_CHAR_SIZE)
inline bool operator == (EsWideString32::const_pointer _1, const EsString& _2) ES_NOTHROW { ES_ASSERT(_1); return EsString::cmpEqual == _2.compare(_1); }
#endif
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (4 == ES_CHAR_SIZE)
inline bool operator == (EsWideString16::const_pointer _1, const EsString& _2) ES_NOTHROW { ES_ASSERT(_1); return EsString::cmpEqual == _2.compare(_1); }
#endif
inline bool operator != (const EsString& _1, const EsString& _2) ES_NOTHROW { return EsString::cmpEqual != _1.compare(_2); }
inline bool operator != (EsString::const_pointer _1, const EsString& _2) ES_NOTHROW { return !operator==(_1, _2); }
inline bool operator < (const EsString& _1, const EsString& _2) ES_NOTHROW { return EsString::cmpLess == _1.compare(_2); }
inline bool operator <= ( const EsString& _1, const EsString& _2 ) ES_NOTHROW { return _1 == _2 || _1 < _2; }
inline bool operator > (const EsString& _1, const EsString& _2) ES_NOTHROW { return EsString::cmpGreater == _1.compare(_2); }
inline bool operator >= ( const EsString& _1, const EsString& _2 ) ES_NOTHROW { return _1 == _2 || _1 > _2; }

/// 'Math' operators
inline EsString operator + (EsString::value_type _1, const EsString& _2) { EsString tmp(_1); tmp += _2; return tmp; }
inline EsString operator + (EsString::const_pointer _1, const EsString& _2) { EsString tmp(_1); tmp += _2; return tmp; }
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (2 == ES_CHAR_SIZE)
inline EsString operator + (EsWideString32::const_pointer _1, const EsString& _2) { EsString tmp(_1); tmp += _2; return tmp; }
#endif
#if (defined(ES_CHAR_IS_WCHAR_T) || defined(ES_USE_NARROW_ES_CHAR)) || (4 == ES_CHAR_SIZE)
inline EsString operator + (EsWideString16::const_pointer _1, const EsString& _2) { EsString tmp(_1); tmp += _2; return tmp; }
#endif
inline EsString::Array operator + (const EsString::Array& _1, const EsString& _2) { EsString::Array tmp(_1); tmp.push_back(_2); return tmp; }
inline EsString::Array operator + (const EsString::Array& _1, const EsString::Array& _2) { EsString::Array tmp(_1); tmp.insert(tmp.end(), _2.begin(), _2.end()); return tmp; }

// String array alias type
typedef EsString::Array  EsStringArray;

#endif // _es_string_h_
