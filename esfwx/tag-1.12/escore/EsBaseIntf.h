#ifndef _base_intf_h_
#define _base_intf_h_

// interface method calling convention macros
//
#define ES_DECL_INTF_METHOD(Retval, MethodName) \
	virtual Retval MethodName

#define ES_IMPL_INTF_METHOD(Retval, MethodName) \
	Retval MethodName

// interface unique ID structure	
struct EsIID 
{
	// data members
	int id0, id1, id2, id3;
	// constructors
	inline EsIID() : id0(0), id1(0), id2(0), id3(0) {}
	inline EsIID(const EsIID& other) : id0(other.id0), id1(other.id1), id2(other.id2), id3(other.id3) {}
	inline EsIID(int _0, int _1, int _2, int _3) : id0(_0), id1(_1), id2(_2), id3(_3) {}
	// templated EsIID generator
	template <typename IntfT>
	static inline EsIID fromIntf()
	{
		return EsIID(IntfT::uid0, IntfT::uid1, IntfT::uid2, IntfT::uid3);
	}
	// check EsIID for emptyness
	inline bool empty() const
	{
		return id0 == 0 && id1 == 0 && id2 == 0 && id3 == 0;
	}
	// EsIID equality check
	inline bool operator== (const EsIID& other) const 
	{
		return id0 == other.id0 && id1 == other.id1 && id2 == other.id2 && id3 == other.id3;
	}
};
	
// template for smart pointer to interface declaration
//
template<typename IntfT>
class EsIntfPtr
{
public:
	typedef IntfT Intf;
	
public:
	inline EsIntfPtr(IntfT* intf = 0, bool inc = false, bool own = true) : 
	m_intf(intf), 
	m_own(own) 
	{
		wxCriticalSectionLocker lock(m_cs);
		if( inc && m_intf )
			m_intf->incRef();
	}
	inline ~EsIntfPtr() 
	{ 
		wxCriticalSectionLocker lock(m_cs);
		clean(); 
	}
	inline void reset(IntfT* intf = 0, bool own = true) 
	{ 
		wxCriticalSectionLocker lock(m_cs);
		internal_reset(intf, own);
	}
	// peek contained interface pointer
	inline IntfT* get() const
	{
		wxCriticalSectionLocker lock(m_cs);
		return m_intf;
	}
	// return contained interface id, or empty id if contained pointer is 0
	inline EsIID getID() const
	{
		wxCriticalSectionLocker lock(m_cs);
		if( m_intf )
			return EsIID( IntfT::uid0, IntfT::uid1, IntfT::uid2, IntfT::uid3 );
		
		return EsIID;
	}
	// detach interface pointer, lose ownership of it, do not refcount any more
	inline IntfT* detach()
	{  
		wxCriticalSectionLocker lock(m_cs);
		IntfT* result = m_intf;
		m_intf = 0;
		return result;
	}
	inline IntfT* operator->() const 
	{ 
		wxCriticalSectionLocker lock(m_cs);
		wxASSERT(m_intf); 
		return m_intf; 
	}
	inline operator bool() const 
	{ 
		wxCriticalSectionLocker lock(m_cs);
		return 0 != m_intf; 
	}
	// ref-counted copy from other instance of the same interface pointer type
	inline EsIntfPtr(const EsIntfPtr<IntfT>& src) :
	m_own(false),
	m_intf(0)
	{
		wxCriticalSectionLocker locksrc(src.m_cs);
		m_intf = src.m_intf;
		m_own = src.m_own;
		if( m_own && m_intf )
			m_intf->incRef();
	}
	// ref-counted copy from other instance of other interface pointer type
	template < typename OtherIntfT >
	inline EsIntfPtr(const EsIntfPtr<OtherIntfT>& src) :
	m_own(false),
	m_intf(0)
	{
		EsIntfPtr<IntfT> tmp = src.request<IntfT>();
		m_intf = tmp.m_intf;
		m_own = tmp.m_own;
		if( m_own && m_intf )
			m_intf->incRef();
	}
	// ref-counted assign from other instance of the same interface pointer type
	inline EsIntfPtr<IntfT>& operator= (const EsIntfPtr<IntfT>& src)
	{
		if( this != &src ) // if we're not assigning ouselves to ourselves
		{
			wxCriticalSectionLocker locksrc(src.m_cs);
			wxCriticalSectionLocker lock(m_cs);
			internal_reset(src.m_intf, src.m_own);

			if( m_own && m_intf )
				m_intf->incRef();
		}
			
		return *this;	
	}
	// ref-counted assign from other instance of the other interface pointer type
	template < typename OtherIntfT >
	inline EsIntfPtr<IntfT>& operator= (const EsIntfPtr<OtherIntfT>& src)
	{ 
		EsIntfPtr<IntfT> tmp = src.request<IntfT>();
		wxCriticalSectionLocker lock(m_cs);
		internal_reset(tmp.m_intf, tmp.m_own);
		if( m_own && m_intf )
			m_intf->incRef();
			
		return *this;
	}
	// request and return other interface smartptr type
	template < typename OtherIntfT >
	inline EsIntfPtr< OtherIntfT > request() const
	{
		wxCriticalSectionLocker lock(m_cs);
		return internal_request<OtherIntfT>();
	}
	// EsIntfPtr templated equality check operator
	//
	template < typename OtherIntfPtrT >
	inline bool operator== (const OtherIntfPtrT& other) const
	{
		EsBaseIntf::Ptr p2 = other.request<EsBaseIntf>();
		EsBaseIntf::Ptr p1 = request<EsBaseIntf>();

		return	p1 && 
						p2 && 
						p1.get() == p2.get();
	}
	// EsIntfPtr templated inequality check operator
	//
	template < typename OtherIntfPtrT >
	inline bool operator!= (const OtherIntfPtrT& other) const
	{
		return ! this->operator== (other);
	}
	
private:
	template < typename OtherIntfT >
	inline EsIntfPtr< OtherIntfT > internal_request() const
	{
		EsIntfPtr< OtherIntfT > result;
		if( m_intf )
			result.reset( reinterpret_cast< OtherIntfT* >(m_intf->requestIntf( EsIID::fromIntf< OtherIntfT >() )) );
		return result;		
	}
	
	inline void internal_reset(IntfT* intf, bool own) 
	{ 
		clean();
		m_intf = intf;
		m_own = own;
	}
		
	inline void clean()
	{
		if(m_intf && m_own) 
		{ 
			m_intf->decRef(); 
			m_intf = 0; 
		} 
	}

protected:
	bool m_own;
	IntfT* m_intf;
	mutable wxCriticalSection m_cs;
};

// interface declaration helper macro
//
// UID0-3 four 4-byte parts of iterface's GUID
#define ES_DECL_INTF_BEGIN1( UID0, UID1, UID2, UID3, InterfaceName ) \
class ES_ABSTRACT InterfaceName \
{ \
public: \
	enum { uid0 = 0x ## UID0, uid1 = 0x ## UID1, uid2 = 0x ## UID2, uid3 = 0x ## UID3 }; \
	typedef EsIntfPtr<InterfaceName> Ptr;
	
// UID0-3 four 4-byte parts of iterface's GUID
#define ES_DECL_INTF_BEGIN2( UID0, UID1, UID2, UID3, InterfaceName1, InterfaceName2 ) \
class ES_ABSTRACT InterfaceName1 : public InterfaceName2 \
{ \
public: \
	enum { uid0 = 0x ## UID0, uid1 = 0x ## UID1, uid2 = 0x ## UID2, uid3 = 0x ## UID3 }; \
	typedef EsIntfPtr<InterfaceName1> Ptr;
	
#define ES_DECL_INTF_END };

// base interface definition. this class must be the base class
// for any interface used throughout the framework, to allow
// kina COM-QueryInterface feature impementation. NB! interface implementation
// used here is not refcounted, so beware.
//
ES_DECL_INTF_BEGIN1( 61233F3D, 2A86471d, B73DAE05, 51A4E46C, EsBaseIntf )
	// analog to COM's query interface method. if requested interface is not supported,
	// return NULL pointer, otherwise, return properly cast requested interface pointer,
	// wrapped in void pointer
	ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid ) const = 0;
	// basic EKOSF RTTI support
	ES_DECL_INTF_METHOD(EsString, getClassName)() const = 0;
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const = 0;
	ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const = 0;
	// refcount support
	ES_DECL_INTF_METHOD(long, incRef)() = 0;
	ES_DECL_INTF_METHOD(void, decRef)() = 0;
	// special deletion method
	ES_DECL_INTF_METHOD(void, destroy)() = 0;
ES_DECL_INTF_END

// thread-safe refcounted implementation helper
//
struct EsRefCounted
{
	EsRefCounted() : m_cnt(1) 
	{
	}
	
	~EsRefCounted()
	{
	}
	
	inline long inc() 
	{ 
		return ++m_cnt;
	}

	inline long dec() 
	{ 
		return --m_cnt;
	}

	long m_cnt;
};

// helper macros for interface map AKA requestIntf dispatcher implementation
//
// standard, dynamically created interface implementor
//
#define ES_INTF_MAP_BEGIN( ClassName ) \
protected: \
	EsRefCounted m_rc; \
public: \
	virtual	~ClassName(); \
	static inline EsString getClassNameStatic() { return wxT( #ClassName ); } \
	ES_DECL_INTF_METHOD(EsString, getClassName)() const { return wxT( #ClassName ); } \
	ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const { return 0 == name.compare( wxT( #ClassName ) ); } \
	ES_DECL_INTF_METHOD(long, incRef)() { return m_rc.inc(); } \
	ES_DECL_INTF_METHOD(void, decRef)() { if( 1 > m_rc.dec() ) destroy(); } \
	ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid ) const { \
	if( EsIID::fromIntf< EsBaseIntf >() == iid ) { \
		EsBaseIntf* intf = static_cast< EsBaseIntf* >( const_cast< ClassName *>(this) ); \
		wxASSERT(intf); intf->incRef(); \
		return intf; }

#define ES_INTF_SUPPORTS( ClassName, intfTypeName ) \
	else if( EsIID::fromIntf< intfTypeName >() == iid ) { \
		intfTypeName* intf = dynamic_cast< intfTypeName* >( const_cast< ClassName *>(this) ); \
		wxASSERT(intf); intf->incRef(); \
		return intf; }

#define ES_INTF_MAP_END \
	return 0; } \
	ES_DECL_INTF_METHOD(void, destroy)() 

// non-refcount implementation, for externally managed implementor objects
//
#define ES_NON_RC_INTF_MAP_BEGIN( ClassName ) \
public: \
	virtual	~ClassName(); \
	static inline EsString getClassNameStatic() { return wxT( #ClassName ); } \
	ES_DECL_INTF_METHOD(EsString, getClassName)() const { return wxT( #ClassName ); } \
	ES_DECL_INTF_METHOD(bool, is)(const EsString& name) const { return 0 == name.compare( wxT( #ClassName ) ); } \
	ES_DECL_INTF_METHOD(long, incRef)() { return 0; } \
	ES_DECL_INTF_METHOD(void, decRef)() {} \
	ES_DECL_INTF_METHOD(void*, requestIntf)( const EsIID& iid ) const { \
	if( EsIID::fromIntf< EsBaseIntf >() == iid ) { \
		EsBaseIntf* intf = static_cast< EsBaseIntf* >( const_cast< ClassName *>(this) ); \
		wxASSERT(intf); intf->incRef(); \
		return intf; }

#define ES_NON_RC_INTF_MAP_END \
	return 0; } \
	ES_DECL_INTF_METHOD(void, destroy)()
	
// special case - embedded interfaced object
// not only it does not use refcounting, but also it's destroy method does nothing
//
#define ES_EMBEDDED_INTF_MAP_BEGIN( ClassName ) \
	ES_NON_RC_INTF_MAP_BEGIN( ClassName )	

#define ES_EMBEDDED_INTF_MAP_END \
	ES_NON_RC_INTF_MAP_END \
	{}

#endif // _base_intf_h_