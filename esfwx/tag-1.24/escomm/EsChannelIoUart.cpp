#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoUart.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#if defined(ES_COMM_USE_CHANNEL_UART) || defined(ES_COMM_USE_CHANNEL_EKONNECT)

// rough max estimate of ms per 1.5 bytes at specified baud
ulong EsChannelIoUart::getMaxByteTmo(ulong baud, ulong bits, bool parity, ulong stop)
{
	ulong result = (1500 * (bits + stop + 1 + (parity ? 0 : 1))) / baud;
	return result ? result : 1;
}

#endif // defined(ES_COMM_USE_CHANNEL_UART) || defined(ES_COMM_USE_CHANNEL_EKONNECT)

#ifdef ES_COMM_USE_CHANNEL_UART

namespace EsReflection
{
  ES_DECL_ENUMERATION_INFO(EsUartIoParity, esT("UART parity enumeration"))
  ES_DECL_ENUMERATION_INFO(EsUartIoStopBits, esT("UART stop bits enumeration"))
}

// -------------------------------------------------------------------------------------------------------
// Reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoUart, _i("UART direct connection channel"))
	// reflected property infos declarations
	//
	ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoUart, port, EsString, _i("Port name"), esVT(""), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoUart, baud, long, _i("Baud rate"), 9600, EsChannelIoUart::baudsGet(), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoUart, bits, long, _i("Byte size"), 8, EsChannelIoUart::bitsGet(), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoUart, parity, long, _i("Parity"), static_cast<ulong>(EsUartIoParity::None), ES_ENUM(EsReflection::EsUartIoParity), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoUart, stopBits, long, _i("Stop bits"), static_cast<ulong>(EsUartIoStopBits::_1), ES_ENUM(EsReflection::EsUartIoStopBits), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoUart, rxBuffLen, long, _i("RX buffer size"), 8192, 512, 65536, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoUart, txBuffLen, long, _i("TX buffer size"), 8192, 512, 65536, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoUart, resetOnRxTmo, bool, _i("Automatically reset channel on RX timeout"), false, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_WO(EsChannelIoUart, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_WO(EsChannelIoUart, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	// EsChannelIoIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, open, bool_Call, _i("Open uart channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, close, void_Call, _i("Close uart channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true if uart channel is open"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, idGet, EsString_CallConst, _i("Get channel id string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, reset, void_Call, _i("Reset channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, errorGet, long_CallConst, _i("Get channel-specific error code"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get channel-specific error string"))
	// EsRateCtlIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsRateCtlIntf, isRateSupported, bool_CallConst_ulong, _i("Return true if requested channel rate is supported"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsRateCtlIntf, supportedRatesGet, EsVariant_CallConst, _i("Return variant array of supported rates"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsRateCtlIntf, defaultRateGet, ulong_CallConst, _i("Return default channel rate value"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsRateCtlIntf, rateGet, ulong_CallConst, _i("Return currently set channel rate"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoUart, EsRateCtlIntf, rateSet, bool_Call_ulong, _i("Try to set channel rate"))
	// Reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoUart, EsBaseIntfPtr_ClassCall, _i("EsChannelIoUart constructor"))
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoUart, getMaxByteTmo, ulong_ClassCall_ulong_ulong_bool_ulong, _i("Calculate per-byte timeout estimate in ms"));
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoUart, enumerate, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

//---------------------------------------------------------------------------
// UART generic implementation methods
//
EsBaseIntfPtr EsChannelIoUart::NEW()
{
	std::unique_ptr<EsChannelIoUart> chnl( new EsChannelIoUart() );
	ES_ASSERT(chnl.get());
 	chnl->m_dynamic = true;
	return chnl.release()->asBaseIntfPtrDirect();
}

EsChannelIoUart::~EsChannelIoUart()
{
	close();
}

const EsVariant& EsChannelIoUart::bitsGet()
{
  static EsVariant sc_byteSizes =
    EsVariant(EsVariant::VAR_VARIANT_COLLECTION)
		.addToVariantCollection(4)
		.addToVariantCollection(5)
		.addToVariantCollection(6)
		.addToVariantCollection(7)
		.addToVariantCollection(8);

  return sc_byteSizes;
}

// properties
EsString EsChannelIoUart::get_port() const
{
	EsMutexLocker lock(m_mx);
	return m_port;
}

long EsChannelIoUart::get_baud() const
{
	EsMutexLocker lock(m_mx);
	return m_baud;
}

void EsChannelIoUart::set_baud(const long& val)
{
	EsMutexLocker lock(m_mx);
	internalSetBaud(val, true);
}

long EsChannelIoUart::get_bits() const
{
	EsMutexLocker lock(m_mx);
	return m_bits;
}

long EsChannelIoUart::get_parity() const
{
	EsMutexLocker lock(m_mx);
	return m_parity;
}

long EsChannelIoUart::get_stopBits() const
{
	EsMutexLocker lock(m_mx);
	return m_stopBits;
}

long EsChannelIoUart::get_rxBuffLen() const
{
	EsMutexLocker lock(m_mx);
	return m_rxBuffLen;
}

long EsChannelIoUart::get_txBuffLen() const
{
	EsMutexLocker lock(m_mx);
	return m_txBuffLen;
}

// autoreset on RX timeout
bool EsChannelIoUart::get_resetOnRxTmo() const
{
	EsMutexLocker lock(m_mx);
	return m_resetOnRxTmo;
}

void EsChannelIoUart::set_resetOnRxTmo(const bool& val)
{
	EsMutexLocker lock(m_mx);
	m_resetOnRxTmo = val;
}

void EsChannelIoUart::set_breaker(const EsBaseIntfPtr& breaker)
{
	return breakerSet( breaker );
}

void EsChannelIoUart::set_monitor(const EsBaseIntfPtr& monitor)
{
	monitorSet(monitor);
}

// log recently occured errors
bool EsChannelIoUart::checkLogLastError() const
{
	EsString err;
	if( m_sysError )
		err = EsUtilities::osErrorStringGet(m_sysError);
	if( m_lineError )
	{
		if( !err.empty() )
			err += esT('\n');
		err += decodeLineError(m_lineError);
	}

	if( !err.empty() )
	{
		ES_DUAL_CALL2_NR( m_monitor, EsChannelIoMonitorIntf, channelError, internalIdGet(), err );

		return true;
	}

	return false;
}

// io breaker access
ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoUart::breakerGet)() const
{
	EsMutexLocker lock(m_mx);
	return m_breaker;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoUart::breakerSet)(const EsBaseIntf::Ptr& breaker)
{
	EsMutexLocker lock(m_mx);
	m_breaker = breaker;
}
//---------------------------------------------------------------------------

EsString EsChannelIoUart::idStringFormat(const EsString& portStr)
{
  return EsString::format(
    esT("%s-%s"),
    EsChannelIoUart::classNameGetStatic(),
    portStr
  );
}
//---------------------------------------------------------------------------

EsString EsChannelIoUart::internalIdGet() const
{
	return idStringFormat(m_port);
}
//---------------------------------------------------------------------------

// channel unique idGet string
ES_IMPL_INTF_METHOD(EsString, EsChannelIoUart::idGet)() const
{
	EsMutexLocker lock(m_mx);
	return internalIdGet();
}

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoUart::monitorGet)() const
{
	EsMutexLocker lock(m_mx);
	return m_monitor;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoUart::monitorSet)(const EsBaseIntf::Ptr& monitor)
{
	EsMutexLocker lock(m_mx);
	m_monitor = monitor;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoUart::close)()
{
	EsMutexLocker lock(m_mx);
	internalClose();
}

ES_IMPL_INTF_METHOD(esU32, EsChannelIoUart::bytesPut)(const esU8* data, esU32 len, esU32 tmo)
{
	EsMutexLocker lock(m_mx);
	return internalPutBytes(data, len, tmo);
}

ES_IMPL_INTF_METHOD(esU32, EsChannelIoUart::bytesGet)(esU8* data, esU32 len, esU32 tmo)
{
	EsMutexLocker lock(m_mx);
	return internalGetBytes(data, len, tmo);
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoUart::bytesPut)(cr_EsBinBuffer data, ulong tmo)
{
	if( !data.empty() )
		return bytesPut(&data[0], data.size(), tmo);

	return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, EsChannelIoUart::bytesGet)(ulong len, ulong tmo)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = bytesGet(&result[0], len, tmo);
		result.resize(len);
	}

	return result;
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoUart::txTimeEstimateGet)(ulong len) const
{
	return EsChannelIoUart::getMaxByteTmo(m_baud, m_bits, 0 != m_parity, m_stopBits) * len;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoUart::reset)()
{
	EsMutexLocker lock(m_mx);
	internalReset();
}

ES_IMPL_INTF_METHOD(long, EsChannelIoUart::errorGet)() const
{
	EsMutexLocker lock(m_mx);
	return m_lineError;
}

ES_IMPL_INTF_METHOD(EsString, EsChannelIoUart::errorStringGet)() const
{
	EsMutexLocker lock(m_mx);
	return decodeLineError(m_lineError);
}

// rate change support
ES_IMPL_INTF_METHOD(bool, EsChannelIoUart::isRateSupported)(ulong rate) const
{
	return baudsGet().has(rate);
}

ES_IMPL_INTF_METHOD(EsVariant, EsChannelIoUart::supportedRatesGet)() const
{
	return baudsGet();
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoUart::defaultRateGet)() const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud).defaultGet().asLong();
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoUart::rateGet)() const
{
	return get_baud();
}

ES_IMPL_INTF_METHOD(bool, EsChannelIoUart::rateSet)(ulong rate)
{
	bool result = false;
	EsMutexLocker lock(m_mx);
	if( (ulong)m_baud != rate )
	{
		bool needReopen = 0 != m_com;
		if( needReopen )
			internalClose();

		result = internalSetBaud(rate, false);

		if( needReopen )
			internalOpen();
	}
	else
		result = true;

	return result;
}

// breaking condition check
bool EsChannelIoUart::internalIsBreaking() const
{
	EsVariant result = false;
	ES_DUAL_CALL0(result, m_breaker, EsBreakIntf, isBreaking);
	return result.asBool();
}

EsVariant EsChannelIoUart::enumerate(const EsVariant& includeBusyPorts, const EsVariant& usbOnly)
{
  EsUartEnumerator enu;
  enu.enumerate( includeBusyPorts.asBool() );

  const EsUartInfos& infos = enu.resultGet();
  EsVariant::Array result;
  result.reserve( infos.size() );

  for(size_t idx = 0; idx < infos.size(); ++idx)
  {
    EsUartInfo::Ptr info = infos[idx];
    if( info && !info->isEmpty() )
    {
      if( usbOnly.isEmpty() ||
        !usbOnly.asBool() ||
        (usbOnly.asBool() && info->isUSBdevice())
      )
      {
        EsVariant item(EsVariant::VAR_VARIANT_COLLECTION);

        item.addToVariantCollection(info->devicePathGet())
          .addToVariantCollection(info->nameGet())
          .addToVariantCollection(info->friendlyNameGet())
          .addToVariantCollection(info->instancePathGet())
          .addToVariantCollection(info->isUSBdevice())
          .addToVariantCollection(info->isModem());

        result.push_back(item);
      }
    }
  }

  return result;
}

// convert EsChannelIoUart to comm channel interface
//
esBL EsChannelIoUart::cLock(EseChannelIo* p, esU32 tmo)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
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

void EsChannelIoUart::cUnlock(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		EsMutex::Result result = chnl->m_mx.unlock();
		ES_ASSERT(EsMutex::resultOk == result);
	}
	catch(...)
	{}
}

esBL EsChannelIoUart::cIsConnected(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	return 0 != chnl->m_com;
}

esBL EsChannelIoUart::cConnect(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return chnl->internalOpen();
	}
	catch(...)
	{}

	return FALSE;
}

void EsChannelIoUart::cDisconnect(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		chnl->internalClose();
	}
	catch(...)
	{}
}

esU32 EsChannelIoUart::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return chnl->internalPutBytes(data, count, 1000);
	}
	catch(...)
	{}

	return 0;
}

esU32 EsChannelIoUart::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return chnl->internalGetBytes(data, count, timeout);
	}
	catch(...)
	{}

	return 0;
}

void EsChannelIoUart::cResetIo(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		chnl->internalReset();
	}
	catch(...)
	{}
}

esBL EsChannelIoUart::cIsRateSupported(EseChannelIo* p, esU32 rate)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return chnl->isRateSupported(rate);
	}
	catch(...)
	{}

	return FALSE;
}

esU32 EsChannelIoUart::cGetRate(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return chnl->m_baud;
	}
	catch(...)
	{}

	return 0;
}

esBL EsChannelIoUart::cSetRate(EseChannelIo* p, esU32 rate)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return chnl->internalSetBaud(rate, false);
	}
	catch(...)
	{}

	return FALSE;
}

esU32 EsChannelIoUart::cSendEstimateGet(EseChannelIo* p, esU32 len)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return (len * 1500 * (chnl->m_bits +
			((0 == chnl->m_stopBits) ? 1 : 2) +
			((0 == chnl->m_parity) ? 0 : 1) + 1)) / chnl->m_baud;
	}
	catch(...)
	{}

	return 0;
}

esBL EsChannelIoUart::cWaitTxEmpty(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		esBL breaking = chnl->internalIsBreaking();

		while( !breaking &&
						0 != chnl->queryUnsentBytes() )
		{
			EsThread::sleep(4);
			breaking = chnl->internalIsBreaking();
		}

		return !breaking;
	}
	catch(...)
	{}

	return FALSE;
}

int EsChannelIoUart::cGetError(EseChannelIo* p)
{
	EsChannelIoUart* chnl = (EsChannelIoUart*)p->m_bus;
	try
	{
		return static_cast<int>(chnl->m_lineError);
	}
	catch(...)
	{}

	return 0;
}

// initialize C EseChannelIo from existing EsChannelIoIntf
ES_IMPL_INTF_METHOD(void, EsChannelIoUart::commChannelInit)(EseChannelIo* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_UART;
	chnl->lock = EsChannelIoUart::cLock;
	chnl->unlock = EsChannelIoUart::cUnlock;
	chnl->isConnected = EsChannelIoUart::cIsConnected;
	chnl->connect = EsChannelIoUart::cConnect;
	chnl->disconnect = EsChannelIoUart::cDisconnect;
	// set rate support
	chnl->isRateSupported = EsChannelIoUart::cIsRateSupported;
	chnl->rateGet = EsChannelIoUart::cGetRate;
	chnl->rateSet = EsChannelIoUart::cSetRate;
	chnl->sendTimeEstimateGet = EsChannelIoUart::cSendEstimateGet;
	// data io
	chnl->bytesPut = EsChannelIoUart::cPutBytes;
	chnl->bytesGet = EsChannelIoUart::cGetBytes;
	chnl->resetIo = EsChannelIoUart::cResetIo;
	chnl->waitTxEmpty = EsChannelIoUart::cWaitTxEmpty;
	// misc
	chnl->errorGet = EsChannelIoUart::cGetError;
}

#endif // ES_COMM_USE_CHANNEL_UART

#if defined(ES_COMM_USE_CHANNEL_UART) || defined(ES_COMM_USE_UART)
//---------------------------------------------------------------------------

EsUartInfo::Ptr EsUartEnumerator::uartInfoFindByDevPath(const EsString& val) const
{
	for( size_t idx = 0; idx < m_infos.size(); ++idx )
	{
    EsUartInfo::Ptr info = m_infos[idx];
    ES_ASSERT(info);

		if( info->devicePathGet() == val )
			return info;
	}

	return EsUartInfo::Ptr();
}
//---------------------------------------------------------------------------

EsUartInfo::Ptr EsUartEnumerator::uartInfoFindByName(const EsString& val) const
{
	for( size_t idx = 0; idx < m_infos.size(); ++idx )
	{
    EsUartInfo::Ptr info = m_infos[idx];
    ES_ASSERT(info);

		if( info->nameGet() == val )
			return info;
	}

	return EsUartInfo::Ptr();
}
//---------------------------------------------------------------------------

EsUartInfo::Ptr EsUartEnumerator::uartInfoFindByFriendlyName(const EsString& val) const
{
	for( size_t idx = 0; idx < m_infos.size(); ++idx )
	{
    EsUartInfo::Ptr info = m_infos[idx];
    ES_ASSERT(info);

		if( info->friendlyNameGet() == val )
			return info;
	}

	return EsUartInfo::Ptr();
}
//---------------------------------------------------------------------------

// Platform - dependent UART implementation
//
#if ES_OS == ES_OS_WINDOWS
# include "EsChannelIoUart.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsChannelIoUart.posix.cxx"
# if ES_OS == ES_OS_MAC || \
     ES_OS == ES_OS_IOS
#   include "EsChannelIoUart.mac.cxx"
# elif (ES_OS == ES_OS_LINUX) || (ES_OS == ES_OS_ANDROID)
#   include "EsChannelIoUart.linux.cxx"
# endif
#else
# error UART is not implemented on this platform!
#endif

//---------------------------------------------------------------------------

#endif // defined(ES_COMM_USE_CHANNEL_UART) || defined(ES_COMM_USE_UART)

