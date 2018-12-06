#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoSocketClient.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET
//-------------------------------------------------------------------------------------------------------

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoSocketClient, _i("Client socket (network) connection channel"))
  // Reflected property infos declarations
  ES_DECL_PROP_INFO_PERSISTENT(       EsChannelIoSocketClient, target, EsString, _i("Server address"), esVT("127.0.0.1"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT( EsChannelIoSocketClient, targetPort, long, _i("Server port"), 50000, 50000, 65535, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT( EsChannelIoSocketClient, operationTimeout, long, _i("Operation timeout (s)"), 5, 1, 15, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(               EsChannelIoSocketClient, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(               EsChannelIoSocketClient, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // EsChannelIoIntf
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, open, bool_Call, _i("Open client socket channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, close, void_Call, _i("Close client socket channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true if client socket channel is open and connected to server"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, reset, void_Call, _i("Reset client socket channel IO buffers"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, errorGet, long_CallConst, _i("Get client socket channel-specific error code"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO( EsChannelIoSocketClient, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get client socket channel-specific error string"))
  // Reflected class methods
  //
  ES_DECL_REFLECTED_CTOR_INFO(        EsChannelIoSocketClient, EsBaseIntfPtr_ClassCall, _i("EsChannelIoSocketClient constructor"))
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
EsChannelIoSocketClient::Client::Client(EsChannelIoSocketClient& owner) :
m_owner(owner)
{}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::Client::onSendFarewell(EsSocket::Impl& io)
{
  ES_DEBUG_TRACE( esT("EsChannelIoSocketClient::Client::onSendFarewell") );
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::Client::onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len)
{
  ES_DEBUG_TRACE( esT("EsChannelIoSocketClient::Client::onServerFarewellReceived") );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsChannelIoSocketClient::EsChannelIoSocketClient() :
m_cli(*this)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsChannelIoSocketClient::~EsChannelIoSocketClient()
{
  close();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsChannelIoSocketClient::NEW()
{
  std::unique_ptr<EsChannelIoSocketClient> chnl( new EsChannelIoSocketClient );
  ES_ASSERT(chnl);

  chnl->m_dynamic = true;

  return chnl.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsString EsChannelIoSocketClient::get_target() const
{
  return m_target;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::set_target(const EsString &val)
{
  m_target = val;
}
//---------------------------------------------------------------------------

long EsChannelIoSocketClient::get_targetPort() const
{
  return m_targetPort;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::set_targetPort(const long &val)
{
  m_targetPort = val;
}
//---------------------------------------------------------------------------

long EsChannelIoSocketClient::get_operationTimeout() const
{
  return m_operationTimeout;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::set_operationTimeout(const long &val)
{
  m_operationTimeout = val;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::set_breaker(const EsBaseIntfPtr& breaker)
{
  breakerSet( breaker );
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::set_monitor(const EsBaseIntfPtr& monitor)
{
  monitorSet( monitor );
}
//---------------------------------------------------------------------------

EsString EsChannelIoSocketClient::internalIdGet() const
{
  return EsString::format(
    esT("%s-%s:%d"),
    typeNameGet(),
    m_target,
    m_targetPort
  );
}
//---------------------------------------------------------------------------

bool EsChannelIoSocketClient::internalIsBreaking() const
{
  EsVariant result = false;
  ES_DUAL_CALL0(result, m_breaker, EsBreakIntf, isBreaking);
  return result.asBool();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString, EsChannelIoSocketClient::idGet)() const
{
  EsMutexLocker lock(m_mx);
  return internalIdGet();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoSocketClient::monitorGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_monitor;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketClient::monitorSet)(const EsBaseIntf::Ptr& monitor)
{
  EsMutexLocker lock(m_mx);
  m_monitor = monitor;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoSocketClient::open)()
{
  EsMutexLocker lock(m_mx);
  if( m_cli.isActive() )
    return true;

  m_addr.typeSet(EsSocketAddrType::IPV4);
  m_addr.rawSet(m_target);
  m_addr.portSet(m_targetPort);

  return m_cli.open(
    m_addr,
    EsSocketType::MessageOriented
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketClient::close)()
{
  EsMutexLocker lock(m_mx);
  m_cli.close();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoSocketClient::isOpen)() const
{
  EsMutexLocker lock(m_mx);
  return m_cli.isActive();
}
//---------------------------------------------------------------------------

// io breaker access
ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoSocketClient::breakerGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_breaker;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketClient::breakerSet)(const EsBaseIntf::Ptr& breaker)
{
  EsMutexLocker lock(m_mx);
  m_breaker = breaker;
}
//---------------------------------------------------------------------------

// byte io
ES_IMPL_INTF_METHOD(esU32, EsChannelIoSocketClient::bytesPut)(const esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  esU32 result = m_cli.sendTo(
    m_addr,
    data,
    len,
    tmo,
    true
  );

  ES_DEBUG_TRACE(
    esT("EsChannelIoSocketClient::bytesPut: sending %d bytes in %d ms, sent %d"),
    len,
    tmo,
    result
  );

  if(result)
    ES_DUAL_CALL2_NR(
      m_monitor,
      EsChannelIoMonitorIntf,
      channelBytesSent,
      internalIdGet(),
      EsBinBuffer(
        data,
        data+result
      )
    );

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(esU32, EsChannelIoSocketClient::bytesGet)(esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);

  const EsDateTime& start = EsDateTime::now();
  esU8* pos = data;
  esU8* end = pos + len;
  esU32 toRead = len;

  ES_DEBUG_TRACE(
    esT("EsChannelIoSocketClient::bytesGet: reading %d bytes in %d ms"),
    len,
    tmo
  );

  do
  {
    ulong chunk = m_cli.receiveFrom(
      m_addr,
      pos,
      toRead,
      tmo,
      true
    );

    pos += chunk;
    toRead -= chunk;

    const EsTimeSpan& span = EsDateTime::now() - start;

    ES_DEBUG_TRACE(
      esT("EsChannelIoSocketClient::bytesGet: read chunk of %d bytes, %d ms spent"),
      chunk,
      span.get_allMilliseconds()
    );

    if( tmo < span.get_allMilliseconds() )
      break;

  } while( pos < end );

  ptrdiff_t result = pos-data;
  if( result )
    ES_DUAL_CALL2_NR(
      m_monitor,
      EsChannelIoMonitorIntf,
      channelBytesReceived,
      internalIdGet(),
      EsBinBuffer(
        data,
        pos
      )
    );

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoSocketClient::bytesPut)(const EsBinBuffer& data, ulong tmo)
{
  if( !data.empty() )
    return bytesPut(
      data.data(),
      data.size(),
      tmo
    );

  return 0;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBinBuffer, EsChannelIoSocketClient::bytesGet)(ulong len, ulong tmo)
{
  EsBinBuffer result(len);
  if( len )
  {
    len = bytesGet(
      result.data(),
      len,
      tmo
    );

    result.resize(len);
  }

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoSocketClient::txTimeEstimateGet)(ulong len) const
{
  return 100;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoSocketClient::reset)()
{
  EsMutexLocker lock(m_mx);
  m_cli.reset();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(long, EsChannelIoSocketClient::errorGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_cli.errorGet();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString, EsChannelIoSocketClient::errorStringGet)() const
{
  EsMutexLocker lock(m_mx);
  long err = m_cli.errorGet();

  if( EsSocketError::None != static_cast<EsSocketError>(err) )
    return EsSockets::errorStringGet(err);

  return EsString::null();
}
//---------------------------------------------------------------------------

esBL EsChannelIoSocketClient::cLock(EseChannelIo* p, esU32 tmo)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.lock(tmo);
    ES_ASSERT( EsMutex::resultDeadlock != result );

    return EsMutex::resultOk == result;
  }
  catch(...)
  {}

  return FALSE;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::cUnlock(EseChannelIo* p)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.unlock();
    ES_ASSERT(EsMutex::resultOk == result);
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

esBL EsChannelIoSocketClient::cIsConnected(EseChannelIo* p)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    return chnl->m_cli.isActive();
  }
  catch(...)
  {}

  return FALSE;
}
//---------------------------------------------------------------------------

esBL EsChannelIoSocketClient::cConnect(EseChannelIo* p)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    return chnl->open();
  }
  catch(...)
  {}

  return FALSE;
}
//---------------------------------------------------------------------------

void EsChannelIoSocketClient::cDisconnect(EseChannelIo* p)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    chnl->m_cli.close();
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

esU32 EsChannelIoSocketClient::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
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

esU32 EsChannelIoSocketClient::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
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

void EsChannelIoSocketClient::cResetIo(EseChannelIo* p)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    chnl->m_cli.reset();
  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

int EsChannelIoSocketClient::cGetError(EseChannelIo* p)
{
  EsChannelIoSocketClient* chnl = (EsChannelIoSocketClient*)p->m_bus;
  try
  {
    return static_cast<int>(chnl->m_cli.errorGet());
  }
  catch(...)
  {}

  return 0;
}
//---------------------------------------------------------------------------

// initialize C EseChannelIo from existing EsChannelIoIntf
ES_IMPL_INTF_METHOD(void, EsChannelIoSocketClient::commChannelInit)(EseChannelIo* chnl)
{
  chnlInit(chnl, this);
  chnl->m_type = CHNL_UART;
  chnl->lock = cLock;
  chnl->unlock = cUnlock;
  chnl->isConnected = cIsConnected;
  chnl->connect = cConnect;
  chnl->disconnect = cDisconnect;
  // data io
  chnl->bytesPut = cPutBytes;
  chnl->bytesGet = cGetBytes;
  chnl->resetIo = cResetIo;
  // misc
  chnl->errorGet = cGetError;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_IO_SOCKET
