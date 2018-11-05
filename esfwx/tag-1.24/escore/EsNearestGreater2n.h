#ifndef _es_nearest_greater_2_n_h_
#define _es_nearest_greater_2_n_h_

/// Templated calculator for nearest integer greater than (or equal to) the specified,
/// whose value is the 2^n
///

#include <limits>

template <typename T>
class EsNearestGreater2n;

template <typename T, size_t sze>
class EsNearestGreater2nImpl
{
private:
  static inline T get(T in);

  EsNearestGreater2nImpl() ES_REMOVEDECL;
  EsNearestGreater2nImpl(const EsNearestGreater2nImpl&) ES_REMOVEDECL;
  const EsNearestGreater2nImpl& operator=(const EsNearestGreater2nImpl&) ES_REMOVEDECL;

  friend class EsNearestGreater2n<T>;
};

template <typename T>
class EsNearestGreater2nImpl<T, 2>
{
private:
  static inline T get(T in) ES_NOTHROW
  {
    --in;

    in |= in >> 1;
    in |= in >> 2;
    in |= in >> 4;
    in |= in >> 8;

    return ++in;
  }

  EsNearestGreater2nImpl() ES_REMOVEDECL;
  EsNearestGreater2nImpl(const EsNearestGreater2nImpl&) ES_REMOVEDECL;
  const EsNearestGreater2nImpl& operator=(const EsNearestGreater2nImpl&) ES_REMOVEDECL;

  friend class EsNearestGreater2n<T>;
};

template <typename T>
class EsNearestGreater2nImpl<T, 4>
{
private:
  static inline T get(T in) ES_NOTHROW
  {
    --in;

    in |= in >> 1;
    in |= in >> 2;
    in |= in >> 4;
    in |= in >> 8;
    in |= in >> 16;

    return ++in;
  }

  EsNearestGreater2nImpl() ES_REMOVEDECL;
  EsNearestGreater2nImpl(const EsNearestGreater2nImpl&) ES_REMOVEDECL;
  const EsNearestGreater2nImpl& operator=(const EsNearestGreater2nImpl&) ES_REMOVEDECL;

  friend class EsNearestGreater2n<T>;
};

template <typename T>
class EsNearestGreater2nImpl<T, 8>
{
private:
  static inline T get(T in) ES_NOTHROW
  {
    --in;

    in |= in >> 1;
    in |= in >> 2;
    in |= in >> 4;
    in |= in >> 8;
    in |= in >> 16;
    in |= in >> 32;

    return ++in;
  }

  EsNearestGreater2nImpl() ES_REMOVEDECL;
  EsNearestGreater2nImpl(const EsNearestGreater2nImpl&) ES_REMOVEDECL;
  const EsNearestGreater2nImpl& operator=(const EsNearestGreater2nImpl&) ES_REMOVEDECL;

  friend class EsNearestGreater2n<T>;
};

template <typename T>
class EsNearestGreater2n
{
  ES_COMPILE_TIME_ASSERT( std::numeric_limits<T>::is_integer, typeMustBeInteger );
  ES_COMPILE_TIME_ASSERT( !std::numeric_limits<T>::is_signed, typeMustBeUnsigned );

private:
  typedef EsNearestGreater2nImpl<T, sizeof(T)> ImplT;

public:
  static inline T get(T in) ES_NOTHROW { return ImplT::get(in); }

private:
  EsNearestGreater2n() ES_REMOVEDECL;
  EsNearestGreater2n(const EsNearestGreater2n&) ES_REMOVEDECL;
  const EsNearestGreater2n& operator=(const EsNearestGreater2n&) ES_REMOVEDECL;
};

#endif // _es_nearest_greater_2_n_h_
