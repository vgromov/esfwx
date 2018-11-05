#ifndef _es_net_address_pool_h_
#define _es_net_address_pool_h_

/// Address pool interface specification
///
ES_DECL_INTF_BEGIN2( 36974FAC, CF5D4F69, 85D2EFD8, 8E6EBA15, EsNetAddressPoolIntf, EsBaseIntf )

  /// Initialize pool, effectively resetting it
	ES_DECL_INTF_METHOD(void, initialize)() = 0;
	/// Peek address at the beginning of the pool
	ES_DECL_INTF_METHOD(ulong, peek)() const = 0;
	/// Peek address in pool at specified position
	ES_DECL_INTF_METHOD(ulong, peekAt)(ulong idx) const = 0;
	/// Acquire address from the beginning of the pool
	ES_DECL_INTF_METHOD(ulong, acquire)() = 0;
	/// Acquire address from the specific pool position
	ES_DECL_INTF_METHOD(ulong, acquireAt)(ulong idx) = 0;
	/// Lease specified address
	ES_DECL_INTF_METHOD(void, lease)(ulong addr) = 0;
	/// Release previously acquired address
	ES_DECL_INTF_METHOD(void, release)(ulong addr) = 0;
  /// Return true if pool is empty (no more addresses available)
  ES_DECL_INTF_METHOD(bool, isEmpty)() const = 0;
  /// Return count of free addresses in pool
	ES_DECL_INTF_METHOD(ulong, availableAddressesCountGet)() const = 0;

ES_DECL_INTF_END

/// Reflected address pool support class
///
class ESCOMM_CLASS ES_INTF_IMPL2(EsNetAddressPool, EsNetAddressPoolIntf, EsReflectedClassIntf)

public:
	EsNetAddressPool();

	ES_DECL_REFLECTED_CLASS_BASE(EsNetAddressPool)
	ES_DECL_ATTR_HANDLING_STD

	/// EsNetAddressPoolIntf services reflection
	///
	/// (Re-) initialize address pool
	ES_DECL_REFLECTED_INTF_METHOD0(void, initialize);
	/// Peek address at the beginning of the pool
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, peek);
	/// Peek address in pool at specified position
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, peekAt, ulong);
	/// Acquire address from the beginning of the pool
	ES_DECL_REFLECTED_INTF_METHOD0(ulong, acquire);
	/// Acquire address from the specific pool position
	ES_DECL_REFLECTED_INTF_METHOD1(ulong, acquireAt, ulong);
	/// Lease specified address
	ES_DECL_REFLECTED_INTF_METHOD1(void, lease, ulong);
	/// Release previously acquired address
	ES_DECL_REFLECTED_INTF_METHOD1(void, release, ulong);

  /// Non-reflected EsNetAddressPoolIntf services implementation
  ///
  /// Return true if pool is empty (no more addresses available)
  ES_DECL_INTF_METHOD(bool, isEmpty)() const { return m_pool.empty(); }

  /// Return count of free addresses in pool
	ES_DECL_INTF_METHOD(ulong, availableAddressesCountGet)() const { return m_pool.size(); }

	/// Reflected properties
	///
	ES_DECL_PROPERTY_RO(empty, bool)
	ES_DECL_PROPERTY_RO(count, ulong)

	/// Reflected ctor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

protected:
	void poolIdxCheck(ulong idx) const;

protected:
	typedef std::set<ulong> PoolT;

	PoolT m_pool;
};

#endif // _es_net_address_pool_h_