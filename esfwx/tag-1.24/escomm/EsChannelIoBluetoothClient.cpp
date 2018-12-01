#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoBluetoothClient.h"
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoBluetoothClient, _i("Bluetooth client connection channel"))
  // reflected property infos declarations
  //
  ES_DECL_PROP_INFO_PERSISTENT(           EsChannelIoBluetoothClient, deviceAddress, EsString, _i("Bluetooth device address"), EsBluetooth::anyAddr(), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsChannelIoBluetoothClient, deviceName, EsString, _i("Bluetooth device name"), NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(           EsChannelIoBluetoothClient, serviceClass, EsString, _i("Bluetooth service class"), EsUtilities::GUIDtoStr(GUID_NULL), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsChannelIoBluetoothClient, serviceName, EsString, _i("Bluetooth service name"), NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(                   EsChannelIoBluetoothClient, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(                   EsChannelIoBluetoothClient, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // EsChannelIoIntf
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, open, bool_Call, _i("Open Bluetooth channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, close, void_Call, _i("Close Bluetooth channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true if Bluetooth channel is open"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, reset, void_Call, _i("Reset channel IO buffers"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, errorGet, long_CallConst, _i("Get channel-specific error code"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsChannelIoBluetoothClient, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get channel-specific error string"))
  // Reflected class methods
  //
  ES_DECL_REFLECTED_CTOR_INFO(            EsChannelIoBluetoothClient, EsBaseIntfPtr_ClassCall, _i("EsChannelIoBluetoothClient constructor"))
  // Class attributes
  //
  ES_DECL_REFLECTED_CLASS_ATTR(p2p)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
EsChannelIoBluetoothClient::Client::Client(EsChannelIoBluetoothClient& owner) :
m_owner(owner)
{}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothClient::Client::onSendFarewell(EsSocket::Impl& io)
{
  ES_DEBUG_TRACE( esT("EsChannelIoBluetoothClient::Client::onSendFarewell") );
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothClient::Client::onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len)
{
  ES_DEBUG_TRACE( esT("EsChannelIoBluetoothClient::Client::onServerFarewellReceived") );
}
//---------------------------------------------------------------------------

// --------------------------------------------------------------------------
EsChannelIoBluetoothClient::EsChannelIoBluetoothClient() :
m_bt(*this),
m_addr(EsSocketAddrType::Bluetooth)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}

EsBaseIntfPtr EsChannelIoBluetoothClient::NEW()
{
  std::unique_ptr<EsChannelIoBluetoothClient> chnl( new EsChannelIoBluetoothClient() );
  ES_ASSERT(chnl.get());

  chnl->m_dynamic = true;
  return chnl.release()->asBaseIntfPtrDirect();
}

EsChannelIoBluetoothClient::~EsChannelIoBluetoothClient()
{
  close();
}

void EsChannelIoBluetoothClient::set_breaker(const EsBaseIntfPtr& breaker)
{
  return breakerSet( breaker );
}

void EsChannelIoBluetoothClient::set_monitor(const EsBaseIntfPtr& monitor)
{
  monitorSet(monitor);
}

EsString EsChannelIoBluetoothClient::get_deviceAddress() const
{
  EsMutexLocker lock(m_mx);
  return m_addr.rawGet();
}

void EsChannelIoBluetoothClient::set_deviceAddress(const EsString& val)
{
  EsMutexLocker lock(m_mx);
  m_addr.rawSet(val);
}

EsString EsChannelIoBluetoothClient::get_deviceName() const
{
  EsMutexLocker lock(m_mx);
  return m_addr.nameGet();
}

void EsChannelIoBluetoothClient::set_deviceName(const EsString& val)
{
  EsMutexLocker lock(m_mx);
  m_addr.nameSet(val);
}

EsString EsChannelIoBluetoothClient::get_serviceClass() const
{
  EsMutexLocker lock(m_mx);
  return m_addr.serviceGet();
}

void EsChannelIoBluetoothClient::set_serviceClass(const EsString& val)
{
  EsMutexLocker lock(m_mx);
  m_addr.serviceSet(val);
}

EsString EsChannelIoBluetoothClient::get_serviceName() const
{
  EsMutexLocker lock(m_mx);
  EsBluetoothServiceUUID uuid(m_addr.serviceGet());
  return uuid.nameGet();
}

void EsChannelIoBluetoothClient::set_serviceName(const EsString& val)
{
  ulong uuid = EsBluetoothServiceUUID::uuidFindByName(val);
  if( uuid )
  {
    EsBluetoothServiceUUID svc(uuid);

    EsMutexLocker lock(m_mx);
    m_addr.serviceSet( svc.asString() );
  }
}

// log recently occured errors
bool EsChannelIoBluetoothClient::checkLogLastError() const
{
  const EsString& err = EsSockets::errorStringGet( m_bt.errorGet() );
  if( !err.empty() )
  {
    ES_DUAL_CALL2_NR( m_monitor, EsChannelIoMonitorIntf, channelError, internalIdGet(), err );

    return true;
  }

  return false;
}

// io breaker access
ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoBluetoothClient::breakerGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_breaker;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoBluetoothClient::breakerSet)(const EsBaseIntf::Ptr& breaker)
{
  EsMutexLocker lock(m_mx);
  m_breaker = breaker;
}

EsString EsChannelIoBluetoothClient::internalIdGet() const
{
  return EsString::format(
    esT("%s-%s"), 
    typeNameGet(), 
    m_addr.rawGet()
  );
}

// channel unique idGet string
ES_IMPL_INTF_METHOD(EsString, EsChannelIoBluetoothClient::idGet)() const
{
  EsMutexLocker lock(m_mx);
  return internalIdGet();
}

// open port using current configuration
bool EsChannelIoBluetoothClient::internalOpen()
{
  bool result = m_bt.open(m_addr, EsSocketType::Bluetooth);
  if(result)
    ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelOpened, internalIdGet());

  return result;
}

void EsChannelIoBluetoothClient::internalClose()
{
  internalReset();
  m_bt.close(false); //< do not close socket gracefully
  ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelClosed, internalIdGet());
}

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoBluetoothClient::monitorGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_monitor;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoBluetoothClient::monitorSet)(const EsBaseIntf::Ptr& monitor)
{
  EsMutexLocker lock(m_mx);
  m_monitor = monitor;
}

// port manipulation
ES_IMPL_INTF_METHOD(bool, EsChannelIoBluetoothClient::open)()
{
  EsMutexLocker lock(m_mx);
  if( m_bt.isActive() )
    return true;
  else
    return internalOpen();
}

ES_IMPL_INTF_METHOD(void, EsChannelIoBluetoothClient::close)()
{
  EsMutexLocker lock(m_mx);
  if( m_bt.isActive() )
    internalClose();
}

ES_IMPL_INTF_METHOD(bool, EsChannelIoBluetoothClient::isOpen)() const
{
  EsMutexLocker lock(m_mx);
  return m_bt.isActive();
}

// byte io
//
// internal byte io
esU32 EsChannelIoBluetoothClient::internalPutBytes(const esU8* data, esU32 len, esU32 tmo)
{
  esU32 sent = m_bt.send(data, len, tmo, true);

  if( sent )
    ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesSent, internalIdGet(), EsBinBuffer(data, data+sent));

  return sent;
}

esU32 EsChannelIoBluetoothClient::internalGetBytes(esU8* data, esU32 len, esU32 tmo)
{
  esU32 read = m_bt.receive(data, len, tmo, true);

  if( read )
    ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesReceived, internalIdGet(), EsBinBuffer(data, data+read));

  return read;
}

ES_IMPL_INTF_METHOD(esU32, EsChannelIoBluetoothClient::bytesPut)(const esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  return internalPutBytes(data, len, tmo);
}

// query unread bytes
esU32 EsChannelIoBluetoothClient::queryUnreadBytes()
{
  return m_bt.incomingSizeGet(0);
}

ES_IMPL_INTF_METHOD(esU32, EsChannelIoBluetoothClient::bytesGet)(esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  return internalGetBytes(data, len, tmo);
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoBluetoothClient::bytesPut)(cr_EsBinBuffer data, ulong tmo)
{
  if( !data.empty() )
    return bytesPut(&data[0], data.size(), tmo);

  return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, EsChannelIoBluetoothClient::bytesGet)(ulong len, ulong tmo)
{
  EsBinBuffer result(len);
  if( len )
  {
    len = bytesGet(&result[0], len, tmo);
    result.resize(len);
  }

  return result;
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoBluetoothClient::txTimeEstimateGet)(ulong len) const
{
  return 100;
}

// internal reset method
void EsChannelIoBluetoothClient::internalReset()
{
  if( m_bt.isActive() )
  {
    m_bt.close(false); //< do not close socket gracefully
    m_bt.open(m_addr);
  }
}

ES_IMPL_INTF_METHOD(void, EsChannelIoBluetoothClient::reset)()
{
  EsMutexLocker lock(m_mx);
  internalReset();
}

ES_IMPL_INTF_METHOD(long, EsChannelIoBluetoothClient::errorGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_bt.errorGet();
}

ES_IMPL_INTF_METHOD(EsString, EsChannelIoBluetoothClient::errorStringGet)() const
{
  EsMutexLocker lock(m_mx);
  return EsSockets::errorStringGet(m_bt.errorGet());
}

// breaking condition check
bool EsChannelIoBluetoothClient::internalIsBreaking() const
{
  EsVariant result = false;
  ES_DUAL_CALL0(result, m_breaker, EsBreakIntf, isBreaking);
  return result.asBool();
}

// convert EsChannelIoBluetoothClient to comm channel interface
//
esBL EsChannelIoBluetoothClient::cLock(EseChannelIo* p, esU32 tmo)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
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

void EsChannelIoBluetoothClient::cUnlock(EseChannelIo* p)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.unlock();
    ES_ASSERT(EsMutex::resultOk == result);
  }
  catch(...)
  {}
}

esBL EsChannelIoBluetoothClient::cIsConnected(EseChannelIo* p)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  return chnl->isOpen();
}

esBL EsChannelIoBluetoothClient::cConnect(EseChannelIo* p)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    return chnl->internalOpen();
  }
  catch(...)
  {}

  return FALSE;
}

void EsChannelIoBluetoothClient::cDisconnect(EseChannelIo* p)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    chnl->internalClose();
  }
  catch(...)
  {}
}

esU32 EsChannelIoBluetoothClient::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    return chnl->internalPutBytes(data, count, 1000);
  }
  catch(...)
  {}

  return 0;
}

esU32 EsChannelIoBluetoothClient::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    return chnl->internalGetBytes(data, count, timeout);
  }
  catch(...)
  {}

  return 0;
}

void EsChannelIoBluetoothClient::cResetIo(EseChannelIo* p)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    chnl->internalReset();
  }
  catch(...)
  {}
}

esU32 EsChannelIoBluetoothClient::cSendEstimateGet(EseChannelIo* p, esU32 len)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    return chnl->txTimeEstimateGet(len);
  }
  catch(...)
  {}

  return 0;
}

int EsChannelIoBluetoothClient::cGetError(EseChannelIo* p)
{
  EsChannelIoBluetoothClient* chnl = (EsChannelIoBluetoothClient*)p->m_bus;
  try
  {
    return static_cast<int>(chnl->errorGet());
  }
  catch(...)
  {}

  return 0;
}

// initialize C EseChannelIo from existing EsChannelIoIntf
ES_IMPL_INTF_METHOD(void, EsChannelIoBluetoothClient::commChannelInit)(EseChannelIo* chnl)
{
  chnlInit(chnl, this);
  chnl->m_type = CHNL_BLUETOOTH;
  chnl->lock = EsChannelIoBluetoothClient::cLock;
  chnl->unlock = EsChannelIoBluetoothClient::cUnlock;
  chnl->isConnected = EsChannelIoBluetoothClient::cIsConnected;
  chnl->connect = EsChannelIoBluetoothClient::cConnect;
  chnl->disconnect = EsChannelIoBluetoothClient::cDisconnect;
  // data io
  chnl->bytesPut = EsChannelIoBluetoothClient::cPutBytes;
  chnl->bytesGet = EsChannelIoBluetoothClient::cGetBytes;
  chnl->resetIo = EsChannelIoBluetoothClient::cResetIo;
  // misc
  chnl->errorGet = EsChannelIoBluetoothClient::cGetError;
  chnl->sendTimeEstimateGet = EsChannelIoBluetoothClient::cSendEstimateGet;
}

#endif //ES_COMM_USE_CHANNEL_BLUETOOTH
