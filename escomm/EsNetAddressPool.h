#ifndef _es_net_address_pool_h_
#define _es_net_address_pool_h_

#ifdef ES_COMM_USE_RPC

/// Address pool interface specification
///
ES_DECL_INTF_BEGIN( 36974FAC, CF5D4F69, 85D2EFD8, 8E6EBA15, EsNetAddressPoolIntf )

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
  ES_DECL_REFLECTED_INTF_METHOD0(void, initialize) ES_OVERRIDE;
  /// Peek address at the beginning of the pool
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, peek) ES_OVERRIDE;
  /// Peek address in pool at specified position
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, peekAt, ulong) ES_OVERRIDE;
  /// Acquire address from the beginning of the pool
  ES_DECL_REFLECTED_INTF_METHOD0(ulong, acquire) ES_OVERRIDE;
  /// Acquire address from the specific pool position
  ES_DECL_REFLECTED_INTF_METHOD1(ulong, acquireAt, ulong) ES_OVERRIDE;
  /// Lease specified address
  ES_DECL_REFLECTED_INTF_METHOD1(void, lease, ulong) ES_OVERRIDE;
  /// Release previously acquired address
  ES_DECL_REFLECTED_INTF_METHOD1(void, release, ulong) ES_OVERRIDE;

  /// Non-reflected EsNetAddressPoolIntf services implementation
  ///
  /// Return true if pool is empty (no more addresses available)
  ES_DECL_INTF_METHOD(bool, isEmpty)() const ES_OVERRIDE { return m_pool.empty(); }

  /// Return count of free addresses in pool
  ES_DECL_INTF_METHOD(ulong, availableAddressesCountGet)() const ES_OVERRIDE { return static_cast<ulong>(m_pool.size()); }

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

#endif //< ES_COMM_USE_RPC

#endif // _es_net_address_pool_h_
