#ifndef _es_string_converter_h_
#define _es_string_converter_h_

/// Versatile string encoding converter class,
/// internally uses tweaked iconv implementation.
///
class ESCORE_CLASS EsStringConverter
{
protected:
  typedef EsVector<char> BuffT;

public:
  // hconv_t preallocated placeholder
  struct HConvHolder
  {
    void* data[28];
    mbstate_t dummy2;
  };

  typedef std::shared_ptr<EsStringConverter> Ptr;

protected:
  EsStringConverter(const EsByteString& toCode, const EsByteString& fromCode);

public:
  /// Requested encodings names access
  inline const EsByteString& toCodeGet() const ES_NOTHROW { return m_toCode; }
  inline const EsByteString& fromCodeGet() const ES_NOTHROW { return m_fromCode; }

  /// Access internal to and from conversion IDs
  ulong toIdGet() const ES_NOTHROW;
  ulong fromIdGet() const ES_NOTHROW;

  /// Output bom size access
  inline ulong toBomSizeGet() const ES_NOTHROW { return m_bomSize; }

  /// Conversion stuff
  EsByteString cToC(const EsByteString& src);

  EsByteString w16toC(const EsWideString16& src);
  EsByteString w32toC(const EsWideString32& src);
  EsByteString wToC(const EsWideString& src);

  EsWideString16 cToW16(const EsByteString& src);
  EsWideString32 cToW32(const EsByteString& src);
  EsWideString cToW(const EsByteString& src);

  EsWideString16 w16ToW16(const EsWideString16& src);
  EsWideString32 w16ToW32(const EsWideString16& src);
  EsWideString16 w32ToW16(const EsWideString32& src);
  EsWideString32 w32ToW32(const EsWideString32& src);
  EsWideString wToW(const EsWideString& src);

  /// Return list of all supported encodings
  static const EsStringIndexedMap& supportedEncodingsGet() ES_NOTHROW;

  /// Return an empty buffer
  static const BuffT& null() ES_NOTHROW;

  /// Registered converters registry
  static EsStringConverter::Ptr convGet(const EsByteString& to, const EsByteString& from);

  /// Generic converters
  ///
  template <typename inW_T, typename outW_T >
  outW_T inW_TtoOut_W_T(const inW_T& src)
  {
    if( src.empty() )
      return nullStringT<outW_T>();

    ES_ASSERT( charSizeGetFromCode(m_fromCode) == charSizeGetFromCode(m_toCode) );

    BuffT in;
    bool invert = endiannessGetFromCode(m_fromCode) != endiannessGetFromCode(m_toCode);
    W_TtoBuff<inW_T>(
      src,
      in,
      invert
    );
    const BuffT& out = convert(in);

    outW_T result;
    buffToW_T(
      out,
      result,
      invert
    );

    return result;
  }

  template <typename W_T>
  EsByteString W_TtoC(const W_T& src)
  {
    if( src.empty() )
      return nullByteString();

    BuffT in;
    W_TtoBuff<W_T>(
      src,
      in,
      ES_ENDIAN != endiannessGetFromCode(m_fromCode)
    );

    const BuffT& out = convert(in);
    return EsByteString(
      out.begin(),
      out.end()
    );
  }

  template <typename W_T>
  W_T cToW_T(const EsByteString& src)
  {
    if( src.empty() )
      return nullStringT<W_T>();

    BuffT in(src.begin(), src.end());
    const BuffT& out = convert(in);

    W_T result;
    buffToW_T<W_T>(
      out,
      result,
      ES_ENDIAN != endiannessGetFromCode(m_toCode)
    );

    return result;
  }

protected:
  // Internal services
  //
  size_t bomSizeGet() ES_NOTHROW;
  size_t convert(HConvHolder& locH, const char** inbuf, size_t* inbytesleft, char** outbuf, size_t* outbytesleft) ES_NOTHROW;
  BuffT convert(const BuffT& in);
  void checkError() const;

  static void buffToW(const EsStringConverter::BuffT& src, EsWideString& dest, bool inverse);
  static void buffToW16(const BuffT& src, EsWideString16& dest, bool inverse);
  static void buffToW32(const BuffT& src, EsWideString32& dest, bool inverse);

  static void wToBuff(const EsWideString& src, EsStringConverter::BuffT& dest, bool inverse);
  static void w16ToBuff(const EsWideString16& dest, BuffT& src, bool inverse);
  static void w32ToBuff(const EsWideString32& dest, BuffT& src, bool inverse);

  static inline size_t pureCodeEndGet(const EsByteString& code) ES_NOTHROW;

  /// Return minimum char size for specified encoding
  static size_t charSizeGetFromCode(const EsByteString& code) ES_NOTHROW;

  /// Return endianness from enconding name, in sence of ES_ENDIAN values
  /// ES_ENDIAN_UNDEFINED is returned for single-byte codings
  ///
  static int endiannessGetFromCode(const EsByteString& code) ES_NOTHROW;

  // Internal Templated services
  //

  template <typename W_T>
  static void buffToW_T(const EsStringConverter::BuffT& src, W_T& dest, bool inverse)
  {
    ES_ASSERT(0 == src.size() % sizeof(typename W_T::value_type));

    dest.resize( src.size() / sizeof(typename W_T::value_type));
    BuffT::const_pointer cp = &src[0];
    BuffT::const_pointer end = cp+src.size();
    size_t idx = 0;
    while(cp < end)
    {
      if( sizeof(typename W_T::value_type) > 1 )
      {
        typename W_T::value_type v = 0;

        esU8* vp = inverse ?
          (esU8*)&v + sizeof(typename W_T::value_type) -1 :
          (esU8*)&v;
        esU8* vpe = inverse ?
          (esU8*)&v -1 :
          vp + sizeof(typename W_T::value_type);

        while(vp != vpe)
        {
          *vp = *cp++;
          if( inverse )
            --vp;
          else
            ++vp;
        }

        dest[idx++] = v;
      }
      else
        dest[idx++] = *cp++;
    }
  }

  template <typename W_T>
  static void W_TtoBuff(const W_T& src, EsStringConverter::BuffT& dest, bool inverse)
  {
    ES_ASSERT( sizeof( typename W_T::value_type ) > 1 );

    dest.resize( src.size()*sizeof(typename W_T::value_type) );
    BuffT::pointer p = &dest[0];
    for(size_t idx = 0; idx < src.size(); ++idx)
    {
      typename W_T::value_type v = src[idx];

      const esU8* vp = inverse ?
        (const esU8*)&v + sizeof(typename W_T::value_type) -1 :
        (const esU8*)&v;
      const esU8* vpe = inverse ?
        (const esU8*)&v -1 :
        vp + sizeof(typename W_T::value_type);

      while(vp != vpe)
      {
        *p++ = *vp;
        if( inverse )
          --vp;
        else
          ++vp;
      }
    }
  }

private:
  HConvHolder m_hConv;
  size_t m_bomSize;
  EsByteString m_toCode;
  EsByteString m_fromCode;

private:
  // prohibited functionality
  EsStringConverter() ES_REMOVEDECL;
  EsStringConverter(const EsStringConverter&) ES_REMOVEDECL;
  const EsStringConverter& operator=(const EsStringConverter&) ES_REMOVEDECL;
};

#endif // _es_string_converter_h_
