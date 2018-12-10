#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoSocketServer.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoSocketServer, _i("Server socket (network) connection channel"))
  // reflected property infos declarations
  ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoSocketServer, port, long, _i("Server port"), 50000, 50000, 65535, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoSocketServer, operationTimeout, long, _i("Operation timeout (s)"), 5, 1, 15, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(EsChannelIoSocketServer, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(EsChannelIoSocketServer, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // reflected interface methods
  //
  // EsChannelIoIntf
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, open, bool_Call, _i("Open server socket channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, close, void_Call, _i("Close server socket channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true, if server socket channel is listening"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, reset, void_Call, _i("Reset server socket channel IO buffers"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, errorGet, long_CallConst, _i("Get server socket channel-specific error code"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoSocketServer, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get server socket channel-specific error string"))      
  // reflected class methods
  //
  ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoSocketServer, EsBaseIntfPtr_ClassCall, _i("EsChannelIoSocketServer constructor"))
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsChannelIoSocketServer::Server::Server(EsChannelIoSocketServer& owner) :
m_owner(owner)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientCannotBeAdded(const EsSocketAddr& addr) const
{
  m_owner.onClientCannotBeAdded(addr);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientAdding(const EsSocketAddr& addr)
{
  m_owner.onClientAdding(addr);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientAdded(const EsSocketAddr& addr)
{
  m_owner.onClientAdded(addr);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientRemoving(const EsSocketAddr& addr)
{
  m_owner.onClientRemoving(addr);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientRemoved(const EsSocketAddr& addr)
{
  m_owner.onClientRemoved(addr);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientDisconnected(const EsSocketAddr& addr)
{
  m_owner.onClientDisconnected(
    addr
  );
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io)
{
  m_owner.onClientClosing(
    addr,
    noFarewell,
    io
  );
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientClosed(const EsSocketAddr& addr)
{
  m_owner.onClientClosed(addr);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::Server::onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& response)
{
  m_owner.onClientDataReceived(
    addr,
    data,
    len,
    response
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

// server channel implementation
EsChannelIoSocketServer::EsChannelIoSocketServer() :
m_srv(*this),
m_sin(0, 1),
m_expectedLen(0)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
  m_srv.listenerMaxIncomingConnSet(1);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsChannelIoSocketServer::NEW()
{
  std::unique_ptr<EsChannelIoSocketServer> chnl( new EsChannelIoSocketServer );
  ES_ASSERT(chnl);

  chnl->m_dynamic = true;

  return chnl.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsChannelIoSocketServer::~EsChannelIoSocketServer()
{
  m_srv.close();
}
//---------------------------------------------------------------------------

long EsChannelIoSocketServer::get_port() const
{
  EsMutexLocker lock(m_mx);
  return m_port;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::set_port(const long &val)
{
  EsMutexLocker lock(m_mx);

  if( ES_THIS_REFLECTED_PROP_INFO_GET(port).validate(val, true) )
    m_port = val;
}
//---------------------------------------------------------------------------

long EsChannelIoSocketServer::get_operationTimeout() const
{
  return m_srv.handlerTtlGet()/1000;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::set_operationTimeout(const long &val)
{
  m_srv.handlerTtlSet(val * 1000);
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::set_breaker(const EsBaseIntfPtr& breaker)
{
  breakerSet( breaker );
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::set_monitor(const EsBaseIntfPtr& monitor)
{
  monitorSet( monitor );
}
//---------------------------------------------------------------------------

EsString EsChannelIoSocketServer::internalIdGet() const
{
  return EsString::format(
    esT("%s-%d"),
    typeNameGet(),
    m_port
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString, EsChannelIoSocketServer::idGet)() const
{
  EsMutexLocker lock(m_mx);
  return internalIdGet();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoSocketServer::monitorGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_monitor;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketServer::monitorSet)(const EsBaseIntf::Ptr& monitor)
{
  EsMutexLocker lock(m_mx);
  m_monitor = monitor;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientCannotBeAdded(const EsSocketAddr& addr) const
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientAdding(const EsSocketAddr& addr)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientAdded(const EsSocketAddr& addr)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientRemoving(const EsSocketAddr& addr)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientRemoved(const EsSocketAddr& addr)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientDisconnected(const EsSocketAddr& addr)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io)
{
  // TODO: Send bye sequence?
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientClosed(const EsSocketAddr& addr)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& ES_UNUSED(response))
{
  ulong inlen = 0;
  {
    EsCriticalSectionLocker lock(m_csin);

    ES_DEBUG_TRACE(
      esT("EsChannelIoSocketServer::onClientDataReceived got buffer of length: %d"),
      len
    );

    m_in.append(
      len,
      data
    );

    inlen = static_cast<ulong>(m_in.size());
  }

  if( inlen == m_expectedLen )
    m_sin.post();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoSocketServer::open)()
{
  if( m_srv.isActive() )
    return true;

  EsSocketAddr addr(
    EsSocketAddrType::IPV4,
    EsString::null(),
    m_port
  );

  m_sin.tryWait();
  return m_srv.open(
    addr,
    EsSocketType::MessageOriented
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketServer::close)()
{
  m_srv.close();
  m_in.clear();
  m_sin.tryWait();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoSocketServer::isOpen)() const
{
  return m_srv.isActive();
}
//---------------------------------------------------------------------------

// io breaker access
ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoSocketServer::breakerGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_breaker;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketServer::breakerSet)(const EsBaseIntf::Ptr& breaker)
{
  EsMutexLocker lock(m_mx);
  m_breaker = breaker;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(esU32, EsChannelIoSocketServer::bytesPut)(const esU8* data, esU32 len, esU32 tmo)
{
  m_expectedLen = 0xFFFFFFFF;

  esU32 result = m_srv.send(
    data,
    len,
    tmo,
    true
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoSocketServer::bytesPut, sending: %d, sent: %d, in %d ms"),
    len,
    result,
    tmo
  );

  if( result )
    ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesSent, internalIdGet(), EsBinBuffer(data, data+result));

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoSocketServer::bytesPut)(const EsBinBuffer& data, ulong tmo)
{
  if( !data.empty() )
    return bytesPut(
      &data[0], 
      static_cast<esU32>(data.size()),
      tmo
    );

  return 0;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(esU32, EsChannelIoSocketServer::bytesGet)(esU8* data, esU32 len, esU32 tmo)
{
  m_expectedLen = len;
  if( EsSemaphore::resultOk != m_sin.wait(tmo) )
    return 0;

  EsCriticalSectionLocker lock(m_csin);

  esU32 result = std::min(
    static_cast<esU32>(m_in.size()),
    len
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoSocketServer::bytesGet, reading: %d, read: %d, in %d ms"),
    len,
    result,
    tmo
  );

  if( result )
  {
    memcpy(
      data,
      m_in.data(),
      result
    );

    m_in.erase(
      m_in.begin(),
      m_in.begin()+result
    );

    ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesReceived, internalIdGet(), EsBinBuffer(data, data+result));
  }

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBinBuffer, EsChannelIoSocketServer::bytesGet)(ulong len, ulong tmo)
{
  EsBinBuffer result(len);
  if( len )
  {
    len = bytesGet(&result[0], len, tmo);
    result.resize(len);
  }
  
  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoSocketServer::txTimeEstimateGet)(ulong len) const
{
  return 100;  
}
//---------------------------------------------------------------------------

// port reset & error info access
ES_IMPL_INTF_METHOD(void, EsChannelIoSocketServer::reset)()
{
  EsCriticalSectionLocker lock(m_csin);
  m_in.clear();
  m_sin.tryWait();
  m_srv.reset();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(long, EsChannelIoSocketServer::errorGet)() const
{
  return m_srv.errorGet();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString, EsChannelIoSocketServer::errorStringGet)() const
{
  long err = m_srv.errorGet();

  if( EsSocketError::None != static_cast<EsSocketError>(err) )
    return EsSockets::errorStringGet(err);

  return EsString::null();
}
//---------------------------------------------------------------------------

bool EsChannelIoSocketServer::internalIsBreaking() const
{
  EsVariant result = false;
  ES_DUAL_CALL0(result, m_breaker, EsBreakIntf, isBreaking);
  return result.asBool();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// wrappers for c channel interfacing
//
esBL EsChannelIoSocketServer::cLock(EseChannelIo* p, esU32 tmo)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.lock(tmo);
    return EsMutex::resultOk == result;
  }
  catch(...)
  {}
  
  return FALSE;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::cUnlock(EseChannelIo* p)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.unlock();
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

esBL EsChannelIoSocketServer::cIsConnected(EseChannelIo* p)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    return chnl->isOpen();
  }
  catch(...)
  {}

  return FALSE;
}
//---------------------------------------------------------------------------

esBL EsChannelIoSocketServer::cConnect(EseChannelIo* p)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    return chnl->open();
  }
  catch(...)
  {}

  return FALSE;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::cDisconnect(EseChannelIo* p)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    chnl->close();
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

esU32 EsChannelIoSocketServer::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    return chnl->bytesPut(
      data,
      count,
      chnl->txTimeEstimateGet(
        count
      )
    );
  }
  catch(...)
  {}

  return 0;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoSocketServer::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    return chnl->bytesGet(
      data,
      count,
      timeout
    );
  }
  catch(...)
  {}

  return 0;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketServer::cResetIo(EseChannelIo* p)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    chnl->reset();
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

int EsChannelIoSocketServer::cGetError(EseChannelIo* p)
{
  EsChannelIoSocketServer* chnl = (EsChannelIoSocketServer*)p->m_bus;
  try
  {
    return chnl->errorGet();
  }
  catch(...)
  {}

  return 0;
}
//---------------------------------------------------------------------------

// initialize C EseChannelIo from existing EsChannelIoIntf
ES_IMPL_INTF_METHOD(void, EsChannelIoSocketServer::commChannelInit)(EseChannelIo* chnl)
{
  chnlInit(chnl, this);
  chnl->m_type = CHNL_UART;
  chnl->lock = EsChannelIoSocketServer::cLock;
  chnl->unlock = EsChannelIoSocketServer::cUnlock;
  chnl->isConnected = EsChannelIoSocketServer::cIsConnected;
  chnl->connect = EsChannelIoSocketServer::cConnect;
  chnl->disconnect = EsChannelIoSocketServer::cDisconnect;
  // data io
  chnl->bytesPut = EsChannelIoSocketServer::cPutBytes;
  chnl->bytesGet = EsChannelIoSocketServer::cGetBytes;
  chnl->resetIo = EsChannelIoSocketServer::cResetIo;
  // misc
  chnl->errorGet = EsChannelIoSocketServer::cGetError;
}
//---------------------------------------------------------------------------

#endif  // ES_COMM_USE_CHANNEL_IO_SOCKET

