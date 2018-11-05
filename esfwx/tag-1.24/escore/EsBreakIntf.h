#ifndef _es_break_intf_h_
#define _es_break_intf_h_

// operation break check abstraction interface
//
ES_DECL_INTF_BEGIN2( 377F1490, 6CC9485b, 85D53F47, BE758A56, EsBreakIntf, EsBaseIntf )
	// return true if break is signalled
	ES_DECL_INTF_METHOD(bool, isBreaking)() const = 0;
	// return true if break is signalled during ms wait
	ES_DECL_INTF_METHOD(bool, isBreaking)(ulong ms) const = 0;
ES_DECL_INTF_END

// standard embedded threaded io breaker interface implementation
#define ES_THREADED_IO_BREAK_DECL_STD( HostClass ) \
private: \
	class ES_INTF_IMPL1_NON_RC(EsBreakIntfImpl, EsBreakIntf) \
	public: \
	EsBreakIntfImpl( const HostClass& host ) ES_NOTHROW : m_host(host) {} \
	ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW { return classNameGet(); } \
	ES_DECL_INTF_METHOD(bool, isBreaking)() const { return const_cast<HostClass&>(m_host).checkForStopping(0); } \
	ES_DECL_INTF_METHOD(bool, isBreaking)(ulong ms) const { return const_cast<HostClass&>(m_host).checkForStopping(ms); } \
	protected: \
	const HostClass& m_host; }; \
friend class EsBreakIntfImpl; \
EsBreakIntfImpl m_breaker;

#endif
