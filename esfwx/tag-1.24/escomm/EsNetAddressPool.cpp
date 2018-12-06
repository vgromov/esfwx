#include "escommpch.h"
#pragma hdrstop

#include "EsNetAddressPool.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// Address pool reflected implementation
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsNetAddressPool, NO_CLASS_DESCR)
  ES_DECL_PROP_INFO_RO(EsNetAddressPool, empty, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsNetAddressPool, count, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, initialize, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, peek, ulong_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, peekAt, ulong_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, acquire, ulong_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, acquireAt, ulong_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, lease, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsNetAddressPool, EsNetAddressPoolIntf, release, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsNetAddressPool, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsNetAddressPool::EsNetAddressPool()
{
  initialize();
}

void EsNetAddressPool::initialize()
{
  m_pool.clear();
  for(ulong addr = 0; addr < 255; ++addr)
    m_pool.insert(addr);
}

void EsNetAddressPool::poolIdxCheck(ulong idx) const
{
  if( idx >= m_pool.size() )
    EsException::Throw(esT("Address pool index %d is out of bounds"), idx);
}

ulong EsNetAddressPool::peekAt(ulong idx) const
{
  poolIdxCheck(idx);
  PoolT::const_iterator cit = m_pool.begin();
  std::advance(cit, idx);
  return (*cit);
}

ulong EsNetAddressPool::peek() const
{
  return peekAt(0);
}

ulong EsNetAddressPool::acquire()
{
  return acquireAt(0);
}

ulong EsNetAddressPool::acquireAt(ulong idx)
{
  ulong result = peekAt(idx);
  lease(result);
  return result;
}

void EsNetAddressPool::lease(ulong addr)
{
  m_pool.erase(addr);
}

void EsNetAddressPool::release(ulong addr)
{
  m_pool.insert(addr);
}

bool EsNetAddressPool::get_empty() const
{
  return isEmpty();
}

ulong EsNetAddressPool::get_count() const
{
  return availableAddressesCountGet();
}

EsBaseIntfPtr EsNetAddressPool::NEW()
{
  std::unique_ptr<EsNetAddressPool> tmp( new EsNetAddressPool );
  ES_ASSERT(tmp);
  tmp->m_dynamic = true;

  return tmp.release()->asBaseIntfPtrDirect();
}

