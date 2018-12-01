#ifndef _es_vector_h_
#define _es_vector_h_

/// std::vector replacement template for plain C++ types, which do not
/// require constructor calls.
/// Its implementation targets performance issues of far more
/// generic and powerful std::vecrtor class
///
template <typename T>
class EsVector
{
public:
  typedef T* pointer;
  typedef T& reference;
  typedef const T* const_pointer;
  typedef const T& const_reference;
  typedef T value_type;
  typedef size_t size_type;
  typedef pointer iterator;
  typedef const_pointer const_iterator;

  // Consts
  static const size_type npos = static_cast< size_type >(-1);

private:
  void internalReserve( typename EsVector<T>::size_type capacity ) ES_NOTHROW;
  void internalAppend( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt );
  void internalAppend( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val );
  template < typename InputIteratorT >
  void internalAppend( InputIteratorT start, InputIteratorT end );
  void tidy() ES_NOTHROW;
  void emptyCheck() const;
  void positionCheck(typename EsVector<T>::size_type pos) const;
  static typename EsVector<T>::size_type capacityCalc(typename EsVector<T>::size_type capacity);
  void relocate(typename EsVector<T>::size_type startFrom, typename EsVector<T>::size_type startTo, typename EsVector<T>::size_type cnt) ES_NOTHROW;
  void internalMove( EsVector<T>& other ) ES_NOTHROW;

public:
  EsVector();
  EsVector( const EsVector<T>& src );
  EsVector( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt );
  EsVector( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val );
  template < typename InputIteratorT >
  EsVector( InputIteratorT start, InputIteratorT end );

  ~EsVector() { tidy(); }

  // Vector is using local storage
  bool usesLocalStorage() const ES_NOTHROW { return m_ptr == m_sptr; }

  // std container interface
  bool empty() const ES_NOTHROW { return 0 == m_size; }
  // assign other sequence's contents to us
  void assign( const EsVector<T>& src ) ES_NOTHROW;
  void assign( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt ) ES_NOTHROW;
  void assign( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val );
  template < typename InputIteratorT >
  void assign( InputIteratorT start, InputIteratorT end ) ES_NOTHROW;
  // append sequence to existing one
  void append( const EsVector<T>& src );
  void append( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt );
  void append( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val );
  template < typename InputIteratorT >
  void append( InputIteratorT start, InputIteratorT end );
  // insert other sequence in this one
  void insert( typename EsVector<T>::size_type at_pos, const EsVector<T>& src );
  void insert( typename EsVector<T>::size_type at_pos, typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt );
  void insert( typename EsVector<T>::size_type at_pos, typename EsVector<T>::const_reference val );
  void insert( typename EsVector<T>::size_type at_pos, typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val );
  void insert( typename EsVector<T>::iterator pos, const EsVector<T>& src ) { insert( pos-begin(), src ); }
  void insert( typename EsVector<T>::iterator pos, typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt ) { insert(pos-begin(), src, cnt); }
  typename EsVector<T>::iterator insert( typename EsVector<T>::iterator pos, typename EsVector<T>::const_reference val ) { insert(pos-begin(), val); return pos; }
  void insert( typename EsVector<T>::iterator pos, typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val ) { insert(pos-begin(), cnt, val); }
  template < typename InputIteratorT >
  void insert( typename EsVector<T>::size_type at_pos, InputIteratorT start, InputIteratorT end );
  template < typename InputIteratorT >
  void insert( typename EsVector<T>::iterator pos, InputIteratorT start, InputIteratorT end ) { insert(pos-begin(), start, end); }
  // misc services
  void swap( EsVector<T>& other ) ES_NOTHROW;
  void move( EsVector<T>& other ) ES_NOTHROW;
  typename EsVector<T>::size_type size() const ES_NOTHROW { return m_size; }
  typename EsVector<T>::size_type capacity() const ES_NOTHROW { return m_capacity; }
  static typename EsVector<T>::size_type max_size() ES_NOTHROW { return static_cast<size_t>(-1)/sizeof(typename EsVector<T>::value_type); }
  // container sizing
  void reserve( typename EsVector<T>::size_type capacity ) ES_NOTHROW;
  void resize( typename EsVector<T>::size_type newSize ) ES_NOTHROW;
  void resize( typename EsVector<T>::size_type newSize, typename EsVector<T>::const_reference val ) ES_NOTHROW;
  void clear() ES_NOTHROW;
  // elements erasure
  typename EsVector<T>::iterator erase(typename EsVector<T>::iterator pos);
  typename EsVector<T>::iterator erase(typename EsVector<T>::iterator first, typename EsVector<T>::iterator last);
  // members access
  typename EsVector<T>::reference at(typename EsVector<T>::size_type idx);
  typename EsVector<T>::const_reference at(typename EsVector<T>::size_type idx) const;
  typename EsVector<T>::reference front();
  typename EsVector<T>::const_reference front() const;
  typename EsVector<T>::reference back();
  typename EsVector<T>::const_reference back() const;
  // push|pop interface
  void push_back(typename EsVector<T>::const_reference val) ES_NOTHROW;
  typename EsVector<T>::value_type pop_back();
  // iterator-like access
  typename EsVector<T>::iterator begin() ES_NOTHROW;
  typename EsVector<T>::const_iterator begin() const ES_NOTHROW;
  typename EsVector<T>::iterator end() ES_NOTHROW;
  typename EsVector<T>::const_iterator end() const ES_NOTHROW;
  // extensions
  typename EsVector<T>::size_type find(typename EsVector<T>::const_reference val) const ES_NOTHROW;
  typename EsVector<T>::size_type find_last(typename EsVector<T>::const_reference val) const ES_NOTHROW;

  // Direct access
  typename EsVector<T>::pointer data() ES_NOTHROW { return m_ptr; }
  typename EsVector<T>::const_pointer data() const ES_NOTHROW { return m_ptr; }

  // operators
  //
  // indexed access
  typename EsVector<T>::reference operator[](typename EsVector<T>::size_type idx) { return at(idx); }
  typename EsVector<T>::const_reference operator[](typename EsVector<T>::size_type idx) const { return at(idx); }
  // comparison
  bool operator== (const EsVector<T>& other) const ES_NOTHROW;
  bool operator!= (const EsVector<T>& other) const ES_NOTHROW;
  bool operator< (const EsVector<T>& other) const ES_NOTHROW;
  bool operator<= (const EsVector<T>& other) const ES_NOTHROW;
  bool operator> (const EsVector<T>& other) const ES_NOTHROW;
  bool operator>= (const EsVector<T>& other) const ES_NOTHROW;
  // assignment
  EsVector<T>& operator=(const EsVector<T>& other);
  // summation
  void operator+= (const EsVector<T>& other);
  EsVector<T> operator+ (const EsVector<T>& other) const;

#ifdef ES_MODERN_CPP
  /// Move constructor
  EsVector(EsVector<T>&& src) ES_NOTHROW;

  /// Move assignment
  EsVector<T>& operator=(EsVector<T>&& other) ES_NOTHROW;
#endif

  /// Semi-private service to fixup pointer to local storage.
  /// Must be used only after vector object is moved|copied directly
  /// by memcpy calls. Must not be used in any other cases.
  /// Should not be used directly by programmer.
  ///
  inline void localStorageFixup() ES_NOTHROW { m_ptr = m_sptr; }

private:
  enum { minCapacity = 32 };

private:
  size_type m_capacity;
  size_type m_size;
  pointer m_ptr;
  value_type m_sptr[minCapacity];
};

/// Specialization (s)
///

/// EsVector specialization for esU8
typedef EsVector<esU8> EsVector_esU8;

/// esU8 specialization declaration for EsVector<esU8>::resize(newSize, val)
template <>
inline void EsVector<esU8>::resize( EsVector<esU8>::size_type newSize, EsVector<esU8>::const_reference val ) ES_NOTHROW;

#include "EsVector.hxx"

#endif // _es_vector_h_
