#ifndef _es_hash_h_
#define _es_hash_h_

// forward declarations
//
class EsString;

/// FNV-1a hash support classes
///
class ESCORE_CLASS EsHashFNV_1a_32
{
public:
  EsHashFNV_1a_32();
  EsHashFNV_1a_32(const EsHashFNV_1a_32& src);
  EsHashFNV_1a_32(const EsString& str);
  EsHashFNV_1a_32(const EsByteString& bstr);
  EsHashFNV_1a_32(const EsBinBuffer& buff);
  
  template <typename IteratorT>
  EsHashFNV_1a_32(IteratorT begin, IteratorT end);

  void update(const EsString& str) ES_NOTHROW;
  void update(const EsByteString& bstr) ES_NOTHROW;
  void update(const EsBinBuffer& buff) ES_NOTHROW;

  template <typename IteratorT>
  void update(IteratorT begin, IteratorT end) ES_NOTHROW;

  void reset() ES_NOTHROW;
  esU32 get() const ES_NOTHROW { return m_hash; }

  const EsHashFNV_1a_32& operator=(const EsHashFNV_1a_32& src) ES_NOTHROW;

protected:
  esU32 m_hash;
};

class ESCORE_CLASS EsHashFNV_1a_64
{
public:
  EsHashFNV_1a_64();
  EsHashFNV_1a_64(const EsHashFNV_1a_64& src);
  EsHashFNV_1a_64(const EsString& str);
  EsHashFNV_1a_64(const EsByteString& str);
  EsHashFNV_1a_64(const EsBinBuffer& buff);

  template <typename IteratorT>
  EsHashFNV_1a_64(IteratorT begin, IteratorT end);

  void update(const EsString& str) ES_NOTHROW;
  void update(const EsByteString& bstr) ES_NOTHROW;
  void update(const EsBinBuffer& buff) ES_NOTHROW;

  template <typename IteratorT>
  void update(IteratorT begin, IteratorT end) ES_NOTHROW;
 
  void reset() ES_NOTHROW;
  esU64 get() const ES_NOTHROW { return m_hash; }

  const EsHashFNV_1a_64& operator=(const EsHashFNV_1a_64& src) ES_NOTHROW;

protected:
  esU64 m_hash;
};
//---------------------------------------------------------------------------

// FNV-1a hash implementation
//
// specializations for the FNV1(a) algorithms
template< typename FNV_T >
struct FNV_Traits
{
  typedef FNV_T T;
  static const T c_prime = 16777619;  // the prime number to initialize the generator
  static const T c_init = 2166136261;// initial value
};
//---------------------------------------------------------------------------

template<>
struct FNV_Traits<esU32>
{
  typedef esU32 T;
  static const T c_prime = 16777619;  // the prime number to initialize the generator
  static const T c_init = 2166136261;// initial value
};
//---------------------------------------------------------------------------

template<>
struct FNV_Traits<esU64>
{
  typedef esU64 T;
  static const T c_prime = IMMEDIATE_UINT64(1099511628211);  // the prime number to initialize the generator
  static const T c_init = IMMEDIATE_UINT64(14695981039346656037);// initial value
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

template < typename T, typename argT, size_t argsize >
struct fnv1a_impl;
//---------------------------------------------------------------------------

template <typename T, typename argT>
struct fnv1a_impl< T, argT, 1 >
{
  static inline T calculate(argT b, T hash) ES_NOTHROW
  {
    return ( static_cast<T>(static_cast<esU8>(b)) ^ hash ) * FNV_Traits<T>::c_prime;
  }
};
//---------------------------------------------------------------------------

template < typename T, typename argT >
struct fnv1a_impl< T, argT, 2 >
{
  static inline T calculate(argT w, T hash) ES_NOTHROW
  {
    const esU8* pb = reinterpret_cast<const esU8*>(&w);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    return fnv1a_impl< T, esU8, 1 >::calculate(*pb, hash);
  }
};
//---------------------------------------------------------------------------

template < typename T, typename argT >
struct fnv1a_impl< T, argT, 4 >
{
  static inline T calculate(argT u, T hash) ES_NOTHROW
  {
    const esU8* pb = reinterpret_cast<const esU8*>(&u);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    return fnv1a_impl< T, esU8, 1 >::calculate(*pb, hash);
  }
};
//---------------------------------------------------------------------------

template < typename T, typename argT >
struct fnv1a_impl< T, argT, 8 >
{
  static inline T calculate(argT uu, T hash) ES_NOTHROW
  {
    const esU8* pb = reinterpret_cast<const esU8*>(&uu);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    hash = fnv1a_impl< T, esU8, 1 >::calculate(*pb++, hash);
    return fnv1a_impl< T, esU8, 1 >::calculate(*pb, hash);
  }
};
//---------------------------------------------------------------------------

template < typename T, typename argT >
inline T fnv1a(argT v, T hash) ES_NOTHROW
{
  return fnv1a_impl<T, argT, sizeof(argT)>::calculate(v, hash);
};
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

template <typename IteratorT>
EsHashFNV_1a_32::EsHashFNV_1a_32(IteratorT begin, IteratorT end)
{
  reset();
  update(begin, end);
}
//--------------------------------------------------------------------------------

template <typename IteratorT>
void EsHashFNV_1a_32::update(IteratorT begin, IteratorT end) ES_NOTHROW
{
  for(auto cit = begin; cit != end; ++cit)
    m_hash = fnv1a<esU32>(
      (*cit),
      m_hash
    );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

template <typename IteratorT>
EsHashFNV_1a_64::EsHashFNV_1a_64(IteratorT begin, IteratorT end)
{
  reset();
  update(begin, end);
}
//--------------------------------------------------------------------------------

template <typename IteratorT>
void EsHashFNV_1a_64::update(IteratorT begin, IteratorT end) ES_NOTHROW
{
  for(auto cit = begin; cit != end; ++cit)
    m_hash = fnv1a<esU64>(
      (*cit),
      m_hash
    );
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

#endif // _es_hash_h_
