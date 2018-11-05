#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoBluetoothLE.h"
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoBluetoothLE, _i("Bluetooth LE byte streaming service channel"))
	// Reflected property infos declarations
	//
	ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoBluetoothLE, deviceAddress, EsString, _i("Bluetooth LE device address"), EsBluetooth::anyAddr(), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoBluetoothLE, subscribeToNotifications, bool, _i("Subscribe to pending notifications"), false, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(EsChannelIoBluetoothLE, deviceName, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_WO(EsChannelIoBluetoothLE, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_WO(EsChannelIoBluetoothLE, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	// EsChannelIoIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, open, bool_Call, _i("Open Bluetooth LE channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, close, void_Call, _i("Close Bluetooth LE channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true if Bluetooth LE channel is open"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, errorGet, long_CallConst, _i("Get channel-specific error code"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoBluetoothLE, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get channel-specific error string"))
	// Reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoBluetoothLE, EsBaseIntfPtr_ClassCall, _i("EsChannelIoBluetoothLE constructor"))
  // Class attributes
  //
  ES_DECL_REFLECTED_CLASS_ATTR(p2p)
ES_DECL_CLASS_INFO_END
// -------------------------------------------------------------------------------------------------------

EsChannelIoBluetoothLE::EsChannelIoBluetoothLE() :
m_addr( EsBluetooth::anyAddr() ),
m_result( EsBluetoothLEresult::gattSuccess ),
m_inTxBatch(false)
{
  EsBluetoothLE::initialize();

	// initialize properties to their default values
	ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsChannelIoBluetoothLE::NEW()
{
	std::unique_ptr<EsChannelIoBluetoothLE> chnl( new EsChannelIoBluetoothLE() );
	ES_ASSERT(chnl.get());
 	chnl->m_dynamic = true;
	return chnl.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsChannelIoBluetoothLE::~EsChannelIoBluetoothLE()
{
	close();
  EsBluetoothLE::uninitialize();
}
//---------------------------------------------------------------------------

const GUID& EsChannelIoBluetoothLE::byteSvcUUID()
{
  static const GUID sc_byteSvcUUID =
    ES_MAKE_UUID(0x04000000, 0x10AE, 0x4C36, 0x9A, 0x27, 0x96, 0xBC, 0xD4, 0xC7, 0xF8, 0x35);

  return sc_byteSvcUUID;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::init()
{
  ES_ASSERT( EsBluetoothLE::initialized() );

  static const GUID sc_pendingBytesUUID =
    ES_MAKE_UUID(0x04000100, 0x10AE, 0x4C36, 0x9A, 0x27, 0x96, 0xBC, 0xD4, 0xC7, 0xF8, 0x35);
  static const GUID sc_bytesIoUUID =
    ES_MAKE_UUID(0x04000200, 0x10AE, 0x4C36, 0x9A, 0x27, 0x96, 0xBC, 0xD4, 0xC7, 0xF8, 0x35);

  if( m_addr == EsBluetooth::anyAddr() )
    return;

  EsBluetoothRadioEnumerator enu;
  enu.execute();
  const EsBluetoothRadios& radios = enu.resultGet();
  for( size_t idx = 0; idx < radios.size(); ++idx )
  {
    EsBluetoothRadio::Ptr radio = radios[idx];
    if( radio->isEnabled() )
    {
      EsBluetoothLEdeviceEnumerator leEnum(radio);
      leEnum.execute();
      const EsBluetoothLEdevices& devs = leEnum.resultGet();

      for(size_t didx = 0; didx < devs.size(); ++didx)
      {
        EsBluetoothLEdevice::Ptr dev = devs[didx];
        if( m_addr == dev->addrGet() )
        {
          m_dev = dev;
          m_dev->discover();

          EsBluetoothLEservice::Ptr svc = m_dev->serviceFind(
            byteSvcUUID()
          );
          if( svc )
          {
            m_chxPendingBytes = svc->characteristicFind(sc_pendingBytesUUID);
            m_chxIo = svc->characteristicFind(sc_bytesIoUUID);

            if( m_chxIo && m_chxPendingBytes )
            {
              m_name = m_dev->nameGet();
              m_radio = radio;
              return;
            }
          }
        }
      }
    }
  }
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::uninit()
{
  if( m_chxPendingBytes )
  {
    if( m_chxPendingBytes->isSubscribed() )
      m_chxPendingBytes->subscribe(false);

    m_chxPendingBytes.reset();
  }
  m_chxIo.reset();

  if( m_dev && m_dev->isConnected() )
  {
    m_dev->disconnect();
    ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelClosed, internalIdGet());
  }

  m_dev.reset();
  m_radio.reset();

  m_inTxBatch = false;
  m_cacheRx.clear();
  m_cacheTx.clear();
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::set_deviceAddress(const EsString& addr)
{
  EsMutexLocker lock(m_mx);

  if( m_dev && addr == m_dev->addrGet() )
    return;

  if( internalIsConnected() )
    EsException::Throw(_("Could not change device address while device connection is active"));

  uninit();
  m_name.clear();
  m_addr = addr;
  init();
}
//---------------------------------------------------------------------------

EsString EsChannelIoBluetoothLE::get_deviceAddress() const
{
  EsMutexLocker lock(m_mx);
  return m_addr;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::set_subscribeToNotifications(const bool& on)
{
  EsMutexLocker lock(m_mx);

  if( m_dev && m_subscribeToNotifications == on )
    return;

  m_subscribeToNotifications = on;
  if( internalIsConnected() )
    m_chxPendingBytes->subscribe(m_subscribeToNotifications);
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::get_subscribeToNotifications() const
{
  EsMutexLocker lock(m_mx);
  return m_subscribeToNotifications;
}
//---------------------------------------------------------------------------

EsString EsChannelIoBluetoothLE::get_deviceName() const
{
  EsMutexLocker lock(m_mx);
  return m_name;
}
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsChannelIoBluetoothLE::monitorGet() const
{
  EsMutexLocker lock(m_mx);
  return m_monitor;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::monitorSet(const EsBaseIntf::Ptr& monitor)
{
  EsMutexLocker lock(m_mx);
  m_monitor = monitor;
}
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsChannelIoBluetoothLE::breakerGet() const
{
  EsMutexLocker lock(m_mx);
  return m_breaker;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::breakerSet(const EsBaseIntf::Ptr& breaker)
{
  EsMutexLocker lock(m_mx);
  m_breaker = breaker;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::set_monitor(const EsBaseIntfPtr& monitor)
{
  monitorSet(monitor);
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::set_breaker(const EsBaseIntfPtr& breaker)
{
  breakerSet(breaker);
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::internalIsOpen() const
{
  return m_dev &&
    m_chxPendingBytes &&
    m_chxIo;
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::internalIsConnected() const
{
  return internalIsOpen() &&
    m_dev->isConnected();
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::isOpen() const
{
  EsMutexLocker lock(m_mx);
  return internalIsOpen();
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::internalConnect()
{
  ES_ASSERT(m_dev);
  ES_ASSERT(m_chxPendingBytes);
  ES_ASSERT(m_chxIo);

  m_result = EsBluetoothLEresult::gattSuccess;

  // If we can successfully read from pending - we are connected,
  // subscribe to the pending chx then
  m_result = m_chxPendingBytes->read();
  if( EsBluetoothLEresult::gattSuccess == m_result )
  {
    m_result = m_chxPendingBytes->subscribe(m_subscribeToNotifications);

    if( EsBluetoothLEresult::gattSuccess == m_result )
    {
      if(m_monitor)
  		  ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelOpened, internalIdGet());

      EsBluetoothLEdescriptor::Ptr dcr = m_chxPendingBytes->descriptorFind(
        EsBluetoothLEdescriptor::uuidGetByKind(
          EsBluetoothLEdescriptor::kindClientConfiguration
        )
      );
      if( dcr )
        m_result = dcr->read();
    }
  }
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::internalReconnect()
{
  if( !internalIsConnected() )
  {
    uninit();
    init();
    internalConnect();
  }

  return internalIsConnected();
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::open()
{
  EsMutexLocker lock(m_mx);
  uninit();
  init();

  return internalIsOpen();
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::close()
{
  EsMutexLocker lock(m_mx);
  uninit();
}
//---------------------------------------------------------------------------

EsString EsChannelIoBluetoothLE::internalIdGet() const
{
	return EsString::format(
    esT("%s-%s"), 
    typeNameGet(), 
    m_addr
  );
}
//---------------------------------------------------------------------------

EsString EsChannelIoBluetoothLE::idGet() const
{
  EsMutexLocker lock(m_mx);
  return internalIdGet();
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::internalIsBreaking() const
{
	EsVariant result = false;
	ES_DUAL_CALL0(result, m_breaker, EsBreakIntf, isBreaking);
	return result.asBool();
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::checkLogLastError() const
{
	if( EsBluetoothLEresult::gattSuccess != m_result )
		ES_DUAL_CALL2_NR( m_monitor, EsChannelIoMonitorIntf, channelError, internalIdGet(), EsBluetoothLE::resultCodeStringGet(m_result) );
}
//---------------------------------------------------------------------------

esU32 EsChannelIoBluetoothLE::queryUnreadBytes()
{
  esU32 result = 0;

  if( m_chxPendingBytes )
  {
    if( internalReconnect() &&
        !m_chxPendingBytes->isSubscribed()
    )
      m_chxPendingBytes->read();

    const EsBinBuffer& val = m_chxPendingBytes->valueGet();
    if( !val.empty() )
      memcpy(&result, &val[0], esMin(sizeof(result), val.size()));
  }

  return result;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoBluetoothLE::internalBytesGet(esU8* dest, esU32 toRead, esU32 tmo)
{
  m_result = EsBluetoothLEresult::gattSuccess;
  esU8* pos = dest;

  if( !tmo )
    tmo = 1;

  // Try to reconnect just in case
  if( toRead &&
    internalReconnect()
  )
  {
    while( toRead && tmo && !internalIsBreaking() )
    {
      m_result = EsBluetoothLEresult::gattSuccess;

      // If cache have not enough data to serve our read request,
      // read from device. Otherwise, get data from cache instead.
      //
      while( m_cacheRx.size() < toRead && tmo && !internalIsBreaking() )
      {
        if( queryUnreadBytes() )
        {
          const EsDateTime& beg = EsDateTime::now();

          m_result = m_chxIo->read();
          if( EsBluetoothLEresult::gattSuccess == m_result )
          {
            m_cacheRx += m_chxIo->valueGet();

            const EsTimeSpan& span = EsDateTime::now()-beg;
            ullong ms = span.get_allMilliseconds();
            if(ms > std::numeric_limits<esU32>::max() )
              ms = std::numeric_limits<esU32>::max();

            if( tmo > static_cast<esU32>(ms) )
              tmo -= static_cast<esU32>(ms);
            else
              tmo = 0;
          }
          else
            break;
        }
        else if( tmo > 5 )
        {
          EsThread::sleep(5);
          tmo -= 5;
        }
        else
          tmo = 0;
      }

      if( EsBluetoothLEresult::gattSuccess == m_result )
      {
        esU32 total = m_cacheRx.size();
        esU32 len = esMin(toRead, total);

        if( len )
        {
          memcpy(pos, &m_cacheRx[0], len);
          toRead -= len;
          pos += len;

          ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesReceived, internalIdGet(), EsBinBuffer(&m_cacheRx[0], &m_cacheRx[0]+len));

          // Shift cache data by erasing the received part from cache
          m_cacheRx.erase(
            m_cacheRx.begin(),
            m_cacheRx.begin()+len
          );
        }
      }
      else
      {
        checkLogLastError();
        uninit();
        break;
      }
    }
  }

  return pos-dest;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoBluetoothLE::txCacheSend()
{
  m_result = EsBluetoothLEresult::gattSuccess;

  if( m_cacheTx.empty() )
    return 0;

  esU32 toWrite = m_cacheTx.size();
  const esU8* src = &m_cacheTx[0];
  const esU8* pos = src;

  esU32 tmo = txTimeEstimateGet(toWrite);
  if( !tmo )
    tmo = 1;

  // Try to reconnect just in case
  if( toWrite &&
    internalReconnect()
  )
  {
    while( toWrite && tmo && !internalIsBreaking() )
    {
      esU32 len = esMin(toWrite, static_cast<esU32>(20));

      EsBinBuffer bb(pos, pos+len);
      m_chxIo->valueSet(bb);

      const EsDateTime& beg = EsDateTime::now();
      m_result = m_chxIo->write();
      if( m_result == EsBluetoothLEresult::gattSuccess )
      {
        const EsTimeSpan& span = EsDateTime::now()-beg;

        toWrite -= len;
        pos += len;

     		ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesSent, internalIdGet(), bb);
        ullong ms = span.get_allMilliseconds();
        if (ms > std::numeric_limits<esU32>::max())
          ms = std::numeric_limits<esU32>::max();

        if( tmo > static_cast<esU32>(ms) )
          tmo -= static_cast<esU32>(ms);
        else
          tmo = 0;
      }
      else
      {
        checkLogLastError();
        uninit();
        break;
      }
    }
  }

  return pos-src;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoBluetoothLE::internalBytesPut(const esU8* src, esU32 toWrite, esU32 ES_UNUSED(tmo))
{
  m_cacheTx.append(toWrite, src);

  if( m_inTxBatch )
    return toWrite;
  else
    return txCacheSend();
}
//---------------------------------------------------------------------------

esU32 EsChannelIoBluetoothLE::bytesPut(const esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  return internalBytesPut(data, len, tmo);
}
//---------------------------------------------------------------------------

esU32 EsChannelIoBluetoothLE::bytesGet(esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  return internalBytesGet(data, len, tmo);
}
//---------------------------------------------------------------------------

ulong EsChannelIoBluetoothLE::bytesPut(cr_EsBinBuffer bb, ulong tmo)
{
  if( !bb.empty() )
    return bytesPut(&bb[0], bb.size(), tmo);

  return 0;
}
//---------------------------------------------------------------------------

EsBinBuffer EsChannelIoBluetoothLE::bytesGet(ulong len, ulong tmo)
{
  EsBinBuffer result(len);

  esU32 read = bytesGet(&result[0], len, tmo);
  result.resize(read);

  return result;
}
//---------------------------------------------------------------------------

ulong EsChannelIoBluetoothLE::txTimeEstimateGet(ulong len) const
{
  // Assume an application baud rate of approx 10 kbps (1 byte = 1 ms)
  // Double the required time interval, to take remote SOC processing latency
  // into account. Add about 250 ms for potential connection interval latency
  return 250 + 2*len;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::internalReset()
{
  if( internalIsConnected() )
    internalReconnect();
  else
  {
    m_inTxBatch = false;
    m_cacheRx.clear();
    m_cacheTx.clear();
  }
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::reset()
{
  EsMutexLocker lock(m_mx);
  internalReset();
}
//---------------------------------------------------------------------------

long EsChannelIoBluetoothLE::errorGet() const
{
  return static_cast<long>(m_result);
}
//---------------------------------------------------------------------------

EsString EsChannelIoBluetoothLE::errorStringGet() const
{
  return EsBluetoothLE::resultCodeStringGet(m_result);
}
//---------------------------------------------------------------------------

bool EsChannelIoBluetoothLE::txBatchBegin()
{
  m_cacheTx.clear();
  m_inTxBatch = true;

  return true;
}
//---------------------------------------------------------------------------

void EsChannelIoBluetoothLE::txBatchEnd(bool ok)
{
  if( m_inTxBatch )
  {
    if( ok )
      txCacheSend();
    else
      m_cacheTx.clear();

    m_inTxBatch = false;
  }
}
//---------------------------------------------------------------------------

// Convert EsChannelIoBluetoothLE to comm channel interface
//
esBL EsChannelIoBluetoothLE::cLock(EseChannelIo* p, esU32 tmo)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
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

void EsChannelIoBluetoothLE::cUnlock(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		EsMutex::Result result = chnl->m_mx.unlock();
		ES_ASSERT(EsMutex::resultOk == result);
	}
	catch(...)
	{}
}

esBL EsChannelIoBluetoothLE::cIsConnected(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	return chnl->isOpen();
}

esBL EsChannelIoBluetoothLE::cConnect(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
    chnl->internalConnect();
		return chnl->internalIsConnected();
	}
	catch(...)
	{}

	return FALSE;
}

void EsChannelIoBluetoothLE::cDisconnect(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		chnl->uninit();
	}
	catch(...)
	{}
}

esU32 EsChannelIoBluetoothLE::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		return chnl->internalBytesPut(data, count, 0);
	}
	catch(...)
	{}

	return 0;
}

esU32 EsChannelIoBluetoothLE::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		return chnl->internalBytesGet(data, count, timeout);
	}
	catch(...)
	{}

	return 0;
}

esBL EsChannelIoBluetoothLE::cTxBatchBegin(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		return chnl->txBatchBegin() ? TRUE : FALSE;
	}
	catch(...)
	{}

  return FALSE;
}

void EsChannelIoBluetoothLE::cTxBatchEnd(EseChannelIo* p, esBL ok)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		chnl->txBatchEnd( TRUE == ok );
	}
	catch(...)
	{}
}

void EsChannelIoBluetoothLE::cResetIo(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		chnl->internalReset();
	}
	catch(...)
	{}
}

esU32 EsChannelIoBluetoothLE::cSendEstimateGet(EseChannelIo* p, esU32 len)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		return chnl->txTimeEstimateGet(len);
	}
	catch(...)
	{}

  return 0;
}

int EsChannelIoBluetoothLE::cGetError(EseChannelIo* p)
{
	EsChannelIoBluetoothLE* chnl = (EsChannelIoBluetoothLE*)p->m_bus;
	try
	{
		return static_cast<int>(chnl->errorGet());
	}
	catch(...)
	{}

	return 0;
}

// initialize C EseChannelIo from existing EsChannelIoIntf
ES_IMPL_INTF_METHOD(void, EsChannelIoBluetoothLE::commChannelInit)(EseChannelIo* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_BLUETOOTH;
	chnl->lock = EsChannelIoBluetoothLE::cLock;
	chnl->unlock = EsChannelIoBluetoothLE::cUnlock;
	chnl->isConnected = EsChannelIoBluetoothLE::cIsConnected;
	chnl->connect = EsChannelIoBluetoothLE::cConnect;
	chnl->disconnect = EsChannelIoBluetoothLE::cDisconnect;
	// data io
	chnl->bytesPut = EsChannelIoBluetoothLE::cPutBytes;
	chnl->bytesGet = EsChannelIoBluetoothLE::cGetBytes;
	chnl->resetIo = EsChannelIoBluetoothLE::cResetIo;
  // TX batch
  chnl->txBatchBegin = EsChannelIoBluetoothLE::cTxBatchBegin;
  chnl->txBatchEnd = EsChannelIoBluetoothLE::cTxBatchEnd;
	// misc
	chnl->errorGet = EsChannelIoBluetoothLE::cGetError;
  chnl->sendTimeEstimateGet = EsChannelIoBluetoothLE::cSendEstimateGet;
}
#endif // ES_COMM_USE_CHANNEL_BLUETOOTH_LE

