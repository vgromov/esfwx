#ifndef _es_bin_buffer_h_
#define _es_bin_buffer_h_

// Forward declarations
//
class EsVariant;

/// Binary buffer basic type
///
typedef EsVector_esU8 EsBinBufferBaseT;

/// EsBinBuffer class declaration
///
class ESCORE_CLASS EsBinBuffer
{
public:
	// Type proxies
	typedef EsBinBufferBaseT::iterator iterator;
	typedef EsBinBufferBaseT::const_iterator const_iterator;
	typedef EsBinBufferBaseT::const_pointer const_pointer;
	typedef EsBinBufferBaseT::const_reference const_reference;
	typedef EsBinBufferBaseT::pointer pointer;
	typedef EsBinBufferBaseT::reference reference;
	typedef EsBinBufferBaseT::value_type value_type;
	typedef EsBinBufferBaseT::size_type size_type;

public:
	// ctors
	//
	inline EsBinBuffer( size_type cnt = 0, const esU8& val = 0 ) :
	m_bb(cnt, val)
	{}
	
	inline EsBinBuffer( const EsBinBuffer& src ) :
	m_bb(src.m_bb)
	{}

	EsBinBuffer( const esBA& ba);
	EsBinBuffer( const EsVariant& v );

	inline EsBinBuffer( const EsBinBufferBaseT& src ) :
	m_bb(src)
	{}
	
	template <typename InputIteratorT>
	inline EsBinBuffer( InputIteratorT first, InputIteratorT last ) :
	m_bb(first, last)
	{}

	inline EsBinBuffer( const EsByteString& src )
	{
    if(!src.empty())
    	m_bb.assign(src.c_str(), src.c_str()+src.size()+1);
  }

  ~EsBinBuffer() ES_NOTHROW;

  /// EsVector delegates
  ///
  /// Return true if local storage is used
  inline bool usesLocalStorage() const ES_NOTHROW { return m_bb.usesLocalStorage(); }

  /// Semi-private service to fixup pointer to local storage.
  /// Must be used only after vector object is moved|copied directly
  /// by memcpy calls. Must not be used in any other cases.
  /// Should not be used directly by programmer.
  ///
  inline void localStorageFixup() ES_NOTHROW { m_bb.localStorageFixup(); }

	/// Services
	///
	inline void assign( size_type cnt, const esU8& val ) { m_bb.assign(cnt, val); }
	template< typename InputIteratorT >
	inline void assign( InputIteratorT first, InputIteratorT last ) { m_bb.assign<InputIteratorT>(first, last); }
	inline reference at( size_type pos ) { return m_bb.at(pos); }
	inline const_reference at( size_type pos ) const { return m_bb.at(pos); }
	inline reference back() { return m_bb.back(); } 
	inline const_reference back() const { return m_bb.back(); }
	inline const_iterator begin() const ES_NOTHROW { return m_bb.begin(); }
	inline iterator begin() ES_NOTHROW { return m_bb.begin(); }
	inline size_type capacity() const ES_NOTHROW { return m_bb.capacity(); }
	inline void clear() { m_bb.clear(); }
	inline bool empty() const ES_NOTHROW { return m_bb.empty(); }
	inline iterator end() ES_NOTHROW { return m_bb.end(); }
	inline const_iterator end() const ES_NOTHROW { return m_bb.end(); }
	inline iterator erase(iterator pos) { return m_bb.erase(pos); }
	inline iterator erase(iterator first, iterator last) { return m_bb.erase(first, last); }
	inline reference front() { return m_bb.front(); } 
	inline const_reference front() const { return m_bb.front(); }
	inline iterator insert( iterator pos, const esU8& val ) { return m_bb.insert(pos, val); }
	inline void insert(iterator pos, size_type cnt, const esU8& val) { m_bb.insert(pos, cnt, val); }
	inline void append(const esU8& val) { m_bb.insert(m_bb.end(), val); }
	inline void fill(const esU8& val) { std::fill(m_bb.begin(), m_bb.end(), val); }
	void append(const EsBinBuffer& src);
	void append(size_t size, const esU8* buff);
	template< typename InputIteratorT >
	inline void insert( iterator pos, InputIteratorT first, InputIteratorT last ) { m_bb.insert<InputIteratorT>(pos, first, last); }
	inline size_type max_size() const ES_NOTHROW { return m_bb.max_size(); }
	inline void pop_back() { m_bb.pop_back(); }
	inline void push_back(const esU8& val) { m_bb.push_back(val); }
	inline void reserve(size_type cnt) { m_bb.reserve(cnt); }
	inline void resize(size_type newSize) { m_bb.resize(newSize, 0); }
	inline void resize(size_type newSize, esU8 val) { m_bb.resize(newSize, val); }
	size_type size() const ES_NOTHROW;

  // Moving and swapping contents
  inline void move( EsBinBuffer& other ) ES_NOTHROW
  {
    ES_ASSERT(this != &other);
    m_bb.move(other.m_bb);
  }

  inline void swap( EsBinBuffer& other ) ES_NOTHROW
  {
    ES_ASSERT(this != &other);
    m_bb.swap(other.m_bb);
  }

	// Extensions
	inline size_type find(const_reference val) const { return m_bb.find(val); }
	inline size_type find_last(const_reference val) const { return m_bb.find_last(val); }
	size_type search(const EsBinBuffer& src) const;

  // Direct access
  EsBinBuffer::pointer data() ES_NOTHROW;
  EsBinBuffer::const_pointer data() const ES_NOTHROW;

	// operators
	//
	// indexed access
	reference operator[](size_type _Pos);
	const_reference operator[](size_type _Pos) const;

	// binary buffer to byte array cast
	operator esBA () const;
	// esBA assignment
	EsBinBuffer& operator= (const esBA& src);
	// EsBinBuffer assignment
	EsBinBuffer& operator= (const EsBinBuffer& src);
	// summation
	EsBinBuffer operator+ (const EsBinBuffer& src) const;
	EsBinBuffer& operator+= (const EsBinBuffer& src);
	inline EsBinBuffer& operator+= (EsBinBuffer::value_type val) { m_bb.push_back(val); return *this; }
	EsBinBuffer& operator+= (const esBA& src);
	// comparison
	inline bool operator== (const EsBinBuffer& other) const ES_NOTHROW { return m_bb == other.m_bb; }
	inline bool operator!= (const EsBinBuffer& other) const ES_NOTHROW { return m_bb != other.m_bb; }
	inline bool operator< (const EsBinBuffer& other) const ES_NOTHROW { return m_bb < other.m_bb; }
	inline bool operator<= (const EsBinBuffer& other) const ES_NOTHROW { return m_bb <= other.m_bb; }
	inline bool operator> (const EsBinBuffer& other) const ES_NOTHROW { return m_bb > other.m_bb; }
	inline bool operator>= (const EsBinBuffer& other) const ES_NOTHROW { return m_bb >= other.m_bb; }

	static const size_type npos = EsBinBufferBaseT::npos;
	static const EsBinBuffer& null() ES_NOTHROW;

#ifdef ES_MODERN_CPP
  /// Move constructor
  inline EsBinBuffer( EsBinBuffer&& src ) ES_NOTHROW
  {
    move(src);
  }

  /// Move assignment
  inline EsBinBuffer& operator=( EsBinBuffer&& other ) ES_NOTHROW
  {
    move(other);
    return *this;
  }
#endif

protected:
	// Data members
	EsBinBufferBaseT m_bb;
};
typedef std::shared_ptr<EsBinBuffer> EsBinBufferPtr;

#endif // _es_bin_buffer_h_
