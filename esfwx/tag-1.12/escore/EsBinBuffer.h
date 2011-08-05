#ifndef _es_bin_buffer_h_
#define _es_bin_buffer_h_

// binary buffer basic type
typedef std::vector<BYTE> EsBinBufferBaseT;
class EKOSF_CORE_CLASS EsBinBuffer
{
public:
	// constants
	enum {
		reserveCount = 16, // by default, reserve space for n elements
	};

public:
	// type proxies
	typedef EsBinBufferBaseT::const_iterator const_iterator;
	typedef EsBinBufferBaseT::const_pointer const_pointer;
	typedef EsBinBufferBaseT::const_reference const_reference;
	typedef EsBinBufferBaseT::const_reverse_iterator const_reverse_iterator;
	typedef EsBinBufferBaseT::iterator iterator;
	typedef EsBinBufferBaseT::pointer pointer;
	typedef EsBinBufferBaseT::reference reference;
	typedef EsBinBufferBaseT::reverse_iterator reverse_iterator;
	typedef EsBinBufferBaseT::value_type value_type;
	typedef EsBinBufferBaseT::size_type size_type;

public:
	// ctors
	//
	inline EsBinBuffer( size_type cnt = 0, const BYTE& val = 0 ) :
	m_bb(cnt, val)
	{ m_bb.reserve(reserveCount); }
	
	inline EsBinBuffer( const EsBinBuffer& src ) :
	m_bb(src.m_bb)
	{}

	EsBinBuffer( const BYTEARRAY& ba);
	EsBinBuffer( const EsVariant& v );

	inline EsBinBuffer( const EsBinBufferBaseT& src ) :
	m_bb(src)
	{}
	
	template <typename InputIteratorT>
	inline EsBinBuffer( InputIteratorT first, InputIteratorT last ) :
	m_bb(first, last)
	{}
	
	// services
	//
	inline void assign( size_type cnt, const BYTE& val ) { m_bb.assign(cnt, val); }
	template< typename InputIteratorT >
	inline void assign( InputIteratorT first, InputIteratorT last ) { m_bb.assign<InputIteratorT>(first, last); }
	inline reference at( size_type pos ) { return m_bb.at(pos); }
	inline const_reference at( size_type pos ) const { return m_bb.at(pos); }
	inline reference back() { return m_bb.back(); } 
	inline const_reference back() const { return m_bb.back(); }
	inline const_iterator begin() const { return m_bb.begin(); }
	inline iterator begin() { return m_bb.begin(); }
	inline size_type capacity() const { return m_bb.capacity(); }
	inline void clear() { m_bb.clear(); }
	inline bool empty() const { return m_bb.empty(); }
	inline iterator end() { return m_bb.end(); } 
	inline const_iterator end() const { return m_bb.end(); }
	inline iterator erase(iterator pos) { return m_bb.erase(pos); }
	inline iterator erase(iterator first, iterator last) { return m_bb.erase(first, last); }
	inline reference front() { return m_bb.front(); } 
	inline const_reference front() const { return m_bb.front(); }
	inline iterator insert( iterator pos, const BYTE& val ) { return m_bb.insert(pos, val); }
	inline void insert(iterator pos, size_type cnt, const BYTE& val) { m_bb.insert(pos, cnt, val); }
	template< typename InputIteratorT >
	inline void insert( iterator pos, InputIteratorT first, InputIteratorT last ) { m_bb.insert<InputIteratorT>(pos, first, last); }
	inline size_type max_size() const { return m_bb.max_size(); }
	inline void pop_back() { m_bb.pop_back(); }
	inline void push_back(const BYTE& val) { m_bb.push_back(val); }
	inline reverse_iterator rbegin() { return m_bb.rbegin(); } 
	inline const_reverse_iterator rbegin() const { return m_bb.rbegin(); }
	inline const_reverse_iterator rend() const { return m_bb.rend(); } 
	inline reverse_iterator rend() { return m_bb.rend(); }
	inline void reserve(size_type cnt) { m_bb.reserve(cnt); }
	inline void resize(size_type newSize) { m_bb.resize(newSize); }
	inline void resize(size_type newSize, BYTE val) { m_bb.resize(newSize, val); }
	inline size_type size() const { return m_bb.size(); }
	inline void swap( EsBinBuffer& other ) { m_bb.swap(other.m_bb); }

	// operators
	//
	// indexed access
	inline reference operator[](size_type _Pos) { return m_bb[_Pos]; }
	inline const_reference operator[](size_type _Pos) const { return m_bb[_Pos]; }
	// binary buffer to byte array cast
	operator BYTEARRAY () const;
	// BYTEARRAY assignment
	EsBinBuffer& operator= (const BYTEARRAY& src);
	// EsBinBuffer assignment
	EsBinBuffer& operator= (const EsBinBuffer& src);
	// summation
	EsBinBuffer& operator+= (const EsBinBuffer& src);
	EsBinBuffer& operator+= (const BYTEARRAY& src);	
	// comparison
	inline bool operator== (const EsBinBuffer& other) const { return m_bb == other.m_bb; }
	inline bool operator!= (const EsBinBuffer& other) const { return m_bb != other.m_bb; }
	inline bool operator< (const EsBinBuffer& other) const { return m_bb < other.m_bb; }
	inline bool operator<= (const EsBinBuffer& other) const { return m_bb <= other.m_bb; }
	inline bool operator> (const EsBinBuffer& other) const { return m_bb > other.m_bb; }
	inline bool operator>= (const EsBinBuffer& other) const { return m_bb >= other.m_bb; }

	// static services
	//
	// char strings to EsBinBuffer conversion
	static EsBinBuffer fromAscii(const char* str, int strLen = -1);
	static EsBinBuffer fromAscii(const std::string& str);

private:
	// data member
	EsBinBufferBaseT m_bb;
};

#endif // _es_bin_buffer_h_
