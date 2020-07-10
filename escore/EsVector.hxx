// EsVector implementation include file

template <typename T>
EsVector<T>::EsVector() :
m_capacity(minCapacity),
m_size(0),
m_ptr(m_sptr)
{}
//---------------------------------------------------------------------------

template <typename T>
EsVector<T>::EsVector( const EsVector<T>& other ) :
m_capacity(minCapacity),
m_size(0),
m_ptr(m_sptr)
{
  ES_ASSERT(this != &other);
  assign(other);
}
//---------------------------------------------------------------------------

template <typename T>
EsVector<T>::EsVector( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt ) :
m_capacity(minCapacity),
m_size(0),
m_ptr(m_sptr)
{
  assign(src, cnt);
}
//---------------------------------------------------------------------------

template <typename T>
EsVector<T>::EsVector( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val ) :
m_capacity(minCapacity),
m_size(0),
m_ptr(m_sptr)
{
  assign(cnt, val);
}
//---------------------------------------------------------------------------

template <typename T>
template < typename InputIteratorT >
EsVector<T>::EsVector( InputIteratorT start, InputIteratorT end ) :
m_capacity(minCapacity),
m_size(0),
m_ptr(m_sptr)
{
  assign<InputIteratorT>(start, end);
}
//---------------------------------------------------------------------------

// std container interface
// assign other sequence's contents to us
template <typename T>
void EsVector<T>::assign( const EsVector<T>& other ) ES_NOTHROW
{
  ES_ASSERT(this != &other);
  assign((const_pointer)other.m_ptr, other.m_size);
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::assign( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt ) ES_NOTHROW
{
  internalReserve(cnt);
  if( cnt )
    memcpy(m_ptr, src, sizeof(value_type)*cnt);
  m_size = cnt;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::assign( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val )
{
  internalReserve(cnt);
  if( cnt )
    std::fill_n(((pointer)m_ptr), cnt, val);
  m_size = cnt;
}
//---------------------------------------------------------------------------

template < typename T >
template < typename InputIteratorT >
void EsVector<T>::assign( InputIteratorT start, InputIteratorT end ) ES_NOTHROW
{
  size_type cnt = end-start;
  internalReserve(cnt);
  InputIteratorT pos = start;
  for(size_type idx = 0; idx < cnt; ++idx )
  {
    ((pointer)m_ptr)[idx] = (*pos);
    ++pos;
  }
  m_size = cnt;
}
//---------------------------------------------------------------------------

// append sequence to existing one
template <typename T>
void EsVector<T>::append( const EsVector<T>& src )
{
  ES_ASSERT(this != &src);
  internalAppend((const_pointer)src.m_ptr, src.m_size);
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::internalAppend( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt )
{
  size_type newSize = m_size + cnt;
  internalReserve(newSize);
  pointer pos = ((pointer)m_ptr)+m_size;
  if( cnt )
    memcpy(pos, src, sizeof(value_type)*cnt);
  m_size = newSize;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::append( typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt )
{
  internalAppend(src, cnt);
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::internalAppend( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val )
{
  size_type newSize = m_size + cnt;
  internalReserve(newSize);
  pointer pos = ((pointer)m_ptr)+m_size;
  if( cnt )
    std::fill_n(pos, cnt, val);
  m_size = newSize;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::append( typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val )
{
  internalAppend(cnt, val);
}
//---------------------------------------------------------------------------

template <typename T>
template < typename InputIteratorT >
void EsVector<T>::internalAppend( InputIteratorT start, InputIteratorT end )
{
  size_type cnt = end-start;
  size_type newSize = m_size + cnt;
  internalReserve(newSize);
  InputIteratorT pos = start;
  for(size_type idx = m_size; idx < newSize; ++idx )
  {
    ((pointer)m_ptr)[idx] = (*pos);
    ++pos;
  }
  m_size = newSize;
}
//---------------------------------------------------------------------------

template <typename T>
template < typename InputIteratorT >
void EsVector<T>::append( InputIteratorT start, InputIteratorT end )
{
  internalAppend<InputIteratorT>(start, end);
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::positionCheck(typename EsVector<T>::size_type pos) const
{
  if( pos >= m_size )
    EsExceptionThrow(
      esT("Vector position %d is out of bounds [0, %d["),
      pos,
      m_size
    );
}
//---------------------------------------------------------------------------

// insert other sequence in this one
template <typename T>
void EsVector<T>::insert( typename EsVector<T>::size_type at_pos, const EsVector<T>& src )
{
  insert(at_pos, src.m_ptr, src.m_size);
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::relocate(typename EsVector<T>::size_type startFrom, typename EsVector<T>::size_type startTo, typename EsVector<T>::size_type cnt) ES_NOTHROW
{
  while(cnt)
  {
    ((pointer)m_ptr)[startTo--] = ((pointer)m_ptr)[startFrom--];
    --cnt;
  }
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::insert( typename EsVector<T>::size_type at_pos, typename EsVector<T>::const_pointer src, typename EsVector<T>::size_type cnt )
{
  if( m_size == at_pos )
    internalAppend(src, cnt);
  else
  {
    positionCheck(at_pos);
    size_type newSize = m_size + cnt;
    internalReserve(newSize);
    if( cnt )
    {
      relocate(m_size-1, newSize-1, m_size-at_pos);
      memcpy(m_ptr+at_pos, src, sizeof(value_type)*cnt);
    }
    m_size = newSize;
  }
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::insert( typename EsVector<T>::size_type at_pos, typename EsVector<T>::size_type cnt, typename EsVector<T>::const_reference val )
{
  if( m_size == at_pos )
    internalAppend(cnt, val);
  else
  {
    positionCheck(at_pos);
    size_type newSize = m_size + cnt;
    internalReserve(newSize);
    if( cnt )
    {
      relocate(m_size-1, newSize-1, m_size-at_pos);
      pointer pos = ((pointer)m_ptr)+at_pos;
      while(cnt)
      {
        *pos++ = val;
        --cnt;
      }
    }
    m_size = newSize;
  }
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::insert( typename EsVector<T>::size_type at_pos, typename EsVector<T>::const_reference val )
{
  insert(at_pos, 1, val);
}
//---------------------------------------------------------------------------

template <typename T>
template < typename InputIteratorT >
void EsVector<T>::insert( typename EsVector<T>::size_type at_pos, InputIteratorT start, InputIteratorT end )
{
  if( m_size == at_pos )
    internalAppend(start, end);
  else
  {
    positionCheck(at_pos);
    if( start < end )
    {
      size_type cnt = end-start;
      size_type newSize = m_size + cnt;
      internalReserve(newSize);
      relocate(m_size-1, newSize-1, m_size-at_pos);

      InputIteratorT srcPos = start;
      pointer pos = ((pointer)m_ptr)+at_pos;
      while( srcPos != end )
        *pos++ = (*srcPos++);

      m_size = newSize;
    }
  }
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::internalMove( EsVector<T>& other ) ES_NOTHROW
{
  if( other.usesLocalStorage() )
  {
    // Just move other local storage to us
    memcpy(m_sptr, other.m_sptr, other.m_size*sizeof(value_type));
  }
  else // Otherwise, just get hold of other's pointer
  {
    m_ptr = other.m_ptr;
    other.m_ptr = other.m_sptr;
  }

  // Use other's capacity and size
  m_capacity = other.m_capacity;
  m_size = other.m_size;

  // Reset other's capacity and size
  other.m_capacity = minCapacity;
  other.m_size = 0;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::move( EsVector<T>& other ) ES_NOTHROW
{
  ES_ASSERT(this != &other);
  tidy();
  internalMove(other);
}
//---------------------------------------------------------------------------

// misc services
template <typename T>
void EsVector<T>::swap( EsVector<T>& other ) ES_NOTHROW
{
  ES_ASSERT(this != &other);
  size_type capacity = m_capacity;
  size_type size = m_size;

  m_capacity = other.m_capacity;
  m_size = other.m_size;
  other.m_capacity = capacity;
  other.m_size = size;

  if( usesLocalStorage() && other.usesLocalStorage() )
  {
    // Exchange local storages, leave ours and others m_ptr(s) intact
    // (both pointing to self local storage)
    value_type tmp[minCapacity];
    memcpy(tmp, other.m_sptr, m_size*sizeof(value_type));
    memcpy(other.m_sptr, m_sptr, size*sizeof(value_type));
    memcpy(m_sptr, tmp, m_size*sizeof(value_type));
  }
  else if( usesLocalStorage() &&
          !other.usesLocalStorage() )
  {
    // Move our local storage to other
    memcpy(other.m_sptr, m_sptr, size*sizeof(value_type));
    // Now other is using local storage
    pointer ptr = (pointer)other.m_ptr;
    other.localStorageFixup();
    // And we are not
    m_ptr = ptr;
  }
  else if( !usesLocalStorage() &&
            other.usesLocalStorage() )
  {
    // Move other local storage to us
    memcpy(m_sptr, other.m_sptr, m_size*sizeof(value_type));
    // Now we are using local storage
    pointer ptr = (pointer)m_ptr;
    localStorageFixup();
    // And other is not
    other.m_ptr = ptr;
  }
  else // Nobody uses local storage, just exchange pointers
  {
    pointer ptr = (pointer)m_ptr;
    m_ptr = other.m_ptr;
    other.m_ptr = ptr;
  }
}
//---------------------------------------------------------------------------

// container sizing
template <typename T>
void EsVector<T>::internalReserve(typename EsVector<T>::size_type capacity) ES_NOTHROW
{
  if( capacity < minCapacity )
    capacity = minCapacity;

  if( m_capacity < capacity )
  {
    capacity = EsNearestGreater2n<size_type>::get(capacity);
    if( capacity >= max_size() )
      ES_FAIL;

    bool doCopySptr = false;
    if( m_ptr == m_sptr ) // before the first realloc!
    {
      doCopySptr = true;
      m_ptr = 0;
    }
    pointer newptr = reinterpret_cast<pointer>(
      realloc(
        m_ptr, 
        sizeof(value_type)*capacity
      )
    );
    if(!newptr)
      return;

    m_ptr = newptr;
    // copy valuable contents of the m_sptr once
    if( doCopySptr && m_size )
      memcpy(
        m_ptr, 
        m_sptr, 
        sizeof(value_type)*m_size
      );

    m_capacity = capacity;
  }
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::reserve( typename EsVector<T>::size_type capacity ) ES_NOTHROW
{
  internalReserve(capacity);
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::resize( typename EsVector<T>::size_type newSize ) ES_NOTHROW
{
  if( m_size < newSize )
    internalReserve(newSize);
  m_size = newSize;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::resize( typename EsVector<T>::size_type newSize, typename EsVector<T>::const_reference val ) ES_NOTHROW
{
  if( m_size < newSize )
  {
    internalReserve(newSize);

    pointer pos = ((pointer)m_ptr)+m_size;
    pointer end = ((pointer)m_ptr)+newSize;
    while( pos < end )
      *pos++ = val;
  }
  m_size = newSize;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::clear() ES_NOTHROW
{
  m_size = 0;
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::tidy() ES_NOTHROW
{
  if( m_ptr != m_sptr )
  {
    if( m_ptr )
      free(m_ptr);

    m_ptr = m_sptr;
  }
  m_capacity = minCapacity;
  m_size = 0;
}
//---------------------------------------------------------------------------

// members access
template <typename T>
typename EsVector<T>::reference EsVector<T>::at(typename EsVector<T>::size_type idx)
{
  positionCheck(idx);
  return ((pointer)m_ptr)[idx];
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::const_reference EsVector<T>::at(typename EsVector<T>::size_type idx) const
{
  positionCheck(idx);
  return ((pointer)m_ptr)[idx];
}
//---------------------------------------------------------------------------

template <typename T>
void EsVector<T>::emptyCheck() const
{
  if( !m_size )
    EsExceptionThrow(esT("Invalid operation on an empty vector"));
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::reference EsVector<T>::front()
{
  emptyCheck();
  return ((pointer)m_ptr)[0];
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::const_reference EsVector<T>::front() const
{
  emptyCheck();
  return ((pointer)m_ptr)[0];
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::reference EsVector<T>::back()
{
  emptyCheck();
  return ((pointer)m_ptr)[m_size-1];
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::const_reference EsVector<T>::back() const
{
  emptyCheck();
  return ((pointer)m_ptr)[m_size-1];
}
//---------------------------------------------------------------------------

// push|pop interface
template <typename T>
void EsVector<T>::push_back(typename EsVector<T>::const_reference val) ES_NOTHROW
{
  internalReserve(m_size+1);
  ((pointer)m_ptr)[m_size++] = val;
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::value_type EsVector<T>::pop_back()
{
  emptyCheck();
  --m_size;
  return ((pointer)m_ptr)[m_size];
}
//---------------------------------------------------------------------------

// iterator-like access
template <typename T>
typename EsVector<T>::iterator EsVector<T>::begin() ES_NOTHROW
{
  if( m_size )
    return (pointer)m_ptr;

  return 0;
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::const_iterator EsVector<T>::begin() const ES_NOTHROW
{
  if( m_size )
    return (const_pointer)m_ptr;

  return 0;
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::iterator EsVector<T>::end() ES_NOTHROW
{
  if( m_size )
    return ((pointer)m_ptr)+m_size;

  return 0;
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::const_iterator EsVector<T>::end() const ES_NOTHROW
{
  if( m_size )
    return ((const_pointer)m_ptr)+m_size;

  return 0;
}
//---------------------------------------------------------------------------

// elements erasure
template <typename T>
typename EsVector<T>::iterator EsVector<T>::erase(typename EsVector<T>::iterator pos)
{
  return erase(pos, pos+1);
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::iterator EsVector<T>::erase(typename EsVector<T>::iterator first, typename EsVector<T>::iterator last)
{
  positionCheck(first-(pointer)m_ptr);
  iterator end = ((pointer)m_ptr)+m_size;
  ES_ASSERT(first < last);
  ES_ASSERT(last <= end);

  iterator to = first;
  iterator from = last;
  iterator until = end;

  iterator result = first;
  size_type rest = 0;
  if( from < until )
  {
    rest = until-from;
    while(from < until)
      (*to++) = (*from++);
  }
  m_size = first-(pointer)m_ptr+rest;
  if( !m_size )
    result = (pointer)m_ptr;
  return result;
}
//---------------------------------------------------------------------------

template <typename T>
bool EsVector<T>::operator== (const EsVector<T>& other) const ES_NOTHROW
{
  if( m_size == other.m_size )
  {
    if( m_size )
      return 0 == memcmp(m_ptr, other.m_ptr, sizeof(value_type)*m_size);

    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

template <typename T>
bool EsVector<T>::operator!= (const EsVector<T>& other) const ES_NOTHROW
{
  return !this->operator==(other);
}
//---------------------------------------------------------------------------

template <typename T>
bool EsVector<T>::operator< (const EsVector<T>& other) const ES_NOTHROW
{
  if( m_size < other.m_size )
    return true;
  else if( m_size == other.m_size )
  {
    if( m_size )
      return 0 > memcmp(
        m_ptr, 
        other.m_ptr, 
        sizeof(value_type)*m_size
      );
  }

  return false;
}
//---------------------------------------------------------------------------

template <typename T>
bool EsVector<T>::operator<= (const EsVector<T>& other) const ES_NOTHROW
{
  return this->operator< (other) ||
    this->operator== (other);
}
//---------------------------------------------------------------------------

template <typename T>
bool EsVector<T>::operator> (const EsVector<T>& other) const ES_NOTHROW
{
  return !this->operator<= (other);
}
//---------------------------------------------------------------------------

template <typename T>
bool EsVector<T>::operator>= (const EsVector<T>& other) const ES_NOTHROW
{
  return this->operator< (other);
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::size_type EsVector<T>::find(typename EsVector<T>::const_reference val) const ES_NOTHROW
{
  for(size_type idx = 0; idx < m_size; ++idx)
    if( val == ((pointer)m_ptr)[idx] )
      return idx;

  return npos;
}
//---------------------------------------------------------------------------

template <typename T>
typename EsVector<T>::size_type EsVector<T>::find_last(typename EsVector<T>::const_reference val) const ES_NOTHROW
{
  size_type idx = m_size;
  while(idx)
    if( val == ((pointer)m_ptr)[--idx] )
      return idx;

  return npos;
}
//---------------------------------------------------------------------------

// assignment
template <typename T>
EsVector<T>& EsVector<T>::operator=(const EsVector<T>& other)
{
  assign(other);
  return *this;
}
//---------------------------------------------------------------------------

// summation
template <typename T>
void EsVector<T>::operator+= (const EsVector<T>& other)
{
  append(other);
}
//---------------------------------------------------------------------------

template <typename T>
EsVector<T> EsVector<T>::operator+ (const EsVector<T>& other) const
{
  EsVector<T> tmp( *this );
  tmp += other;

  return tmp;
}
//---------------------------------------------------------------------------

#ifdef ES_MODERN_CPP
template <typename T>
EsVector<T>::EsVector(EsVector<T>&& other) ES_NOTHROW :
m_capacity(minCapacity),
m_size(0),
m_ptr(m_sptr)
{
  ES_ASSERT(this != &other);
  internalMove(other);
}
//---------------------------------------------------------------------------

template <typename T>
EsVector<T>& EsVector<T>::operator=(EsVector<T>&& other) ES_NOTHROW
{
  move(other);
  return *this;
}
#endif
//---------------------------------------------------------------------------

// EsVector specialization(s) implementation
template <>
inline void EsVector<esU8>::resize( EsVector<esU8>::size_type newSize, EsVector<esU8>::const_reference val ) ES_NOTHROW
{
  if( m_size < newSize )
  {
    internalReserve(newSize);
    memset(
      reinterpret_cast<pointer>(m_ptr)+m_size,
      val,
      newSize-m_size
    );
  }
  m_size = newSize;
}
//---------------------------------------------------------------------------
