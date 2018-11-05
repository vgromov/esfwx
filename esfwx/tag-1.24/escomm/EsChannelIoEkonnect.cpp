#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoEkonnect.h"
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_CHANNEL_EKONNECT

#include <algorithm>

namespace EsReflection
{
  ES_DECL_ENUMERATION_INFO(EsEkonnectIoParity, esT("Ekonnect parity enumeration"))
  ES_DECL_ENUMERATION_INFO(EsEkonnectIoStopBits, esT("Ekonnect stop bits enumeration"))
}
// -------------------------------------------------------------------------------------------------------

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoEkonnect, _i("EKONNECT adapter direct connection channel"))
  // reflected property infos declarations
	ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoEkonnect, device, EsString, _i("Device name"), esVT("60000-00000-00"), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoEkonnect, baud, long, _i("Baud rate"), EsFtdiDriver::_9600, EsChannelIoEkonnect::baudsGet(), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoEkonnect, bits, long, _i("Byte size"), EsFtdiDriver::BITS_8, EsChannelIoEkonnect::bitsGet(), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoEkonnect, parity, long, _i("Parity"), static_cast<ulong>(EsEkonnectIoParity::None), ES_ENUM(EsReflection::EsEkonnectIoParity), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(EsChannelIoEkonnect, stopBits, long, _i("Stop bits"), static_cast<ulong>(EsEkonnectIoStopBits::_1), ES_ENUM(EsReflection::EsEkonnectIoStopBits), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoEkonnect, rxTmo, long, _i("RX timeout in ms"), EsFtdiDriver::DEFAULT_RX_TIMEOUT, 1, 65536, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoEkonnect, txTmo, long, _i("TX timeout in ms"), EsFtdiDriver::DEFAULT_TX_TIMEOUT, 1, 65536, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoEkonnect, rxBuffLen, long, _i("RX buffer size"), EsFtdiDriver::DEFAULT_RX_BUFFER, 64, 65536, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsChannelIoEkonnect, txBuffLen, long, _i("TX buffer size"), EsFtdiDriver::DEFAULT_TX_BUFFER, 64, 65536, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoEkonnect, resetOnRxTmo, bool, _i("Automatically reset channel on RX timeout"), false, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(EsChannelIoEkonnect, useRS232, bool, _i("Use standard RS232 socket"), false, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_WO(EsChannelIoEkonnect, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_WO(EsChannelIoEkonnect, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsChannelIoEkonnect, isEkonnect1, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)

	// Reflected interface methods
	//
	// EsChannelIoIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, open, bool_Call, _i("Open EKONNECT device channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, close, void_Call, _i("Close EKONNECT device channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true if EKONNECT device channel is open"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, idGet, EsString_CallConst, _i("Get channel id string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, reset, void_Call, _i("Reset EKONNECT device channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, errorGet, long_CallConst, _i("Get EKONNECT device channel-specific error code"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get EKONNECT device channel-specific error string"))
	// EsPowerCtlIntf
	//
	// Remove|set device power
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsPowerCtlIntf, devicePowerSet, void_Call_bool, _i("Configure auxiliary power supply via Econnect cable"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsPowerCtlIntf, devicePowerGet, bool_CallConst, NO_METHOD_DESCR)
	// Set|reset device programming mode pin
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsPowerCtlIntf, deviceProgrammingModeSet, void_Call_bool, _i("Configure Econnect cable for programming"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsPowerCtlIntf, deviceProgrammingModeGet, bool_CallConst, NO_METHOD_DESCR)
	// EsRateCtlIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsRateCtlIntf, isRateSupported, bool_CallConst_ulong, _i("Return true if requested channel rate is supported"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsRateCtlIntf, supportedRatesGet, EsVariant_CallConst, _i("Return variant array of supported rates"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsRateCtlIntf, defaultRateGet, ulong_CallConst, _i("Return default channel rate value"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsRateCtlIntf, rateGet, ulong_CallConst, _i("Return currently set channel rate"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoEkonnect, EsRateCtlIntf, rateSet, bool_Call_ulong, _i("Try to set channel rate"))
	// Reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoEkonnect, EsBaseIntfPtr_ClassCall, _i("EsChannelIoEkonnect constructor"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoEkonnect, enumerate, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsChannelIoEkonnect::EsChannelIoEkonnect() :
m_io(0),
m_programmingMode(false),
m_supplyAuxPower(true)
{
  // Try to load FTDI driver without throwing exceptions
	EsFtdiDriver::instanceGet().load(false);

	// initialize properties to their default values
	ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsChannelIoEkonnect::NEW()
{
	std::unique_ptr<EsChannelIoEkonnect> chnl( new EsChannelIoEkonnect() );
	ES_ASSERT(chnl.get());

	chnl->m_dynamic = true;

	return chnl.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

EsChannelIoEkonnect::~EsChannelIoEkonnect()
{
	close();
}
//---------------------------------------------------------------------------

const EsVariant::Array& EsChannelIoEkonnect::baudsGet()
{
  static const EsVariant::Array sc_bauds = {
    EsFtdiDriver::_300,
    EsFtdiDriver::_600,
    EsFtdiDriver::_1200,
    EsFtdiDriver::_2400,
    EsFtdiDriver::_4800,
    EsFtdiDriver::_9600,
    EsFtdiDriver::_14400,
    EsFtdiDriver::_19200,
    EsFtdiDriver::_38400,
    EsFtdiDriver::_57600,
    EsFtdiDriver::_115200,
    EsFtdiDriver::_230400,
    EsFtdiDriver::_460800,
    EsFtdiDriver::_921600
  };

  return sc_bauds;
}
//---------------------------------------------------------------------------

const EsVariant::Array& EsChannelIoEkonnect::bitsGet()
{
  static const EsVariant::Array sc_bits = {
    EsFtdiDriver::BITS_5,
    EsFtdiDriver::BITS_6,
    EsFtdiDriver::BITS_7,
    EsFtdiDriver::BITS_8
  };

  return sc_bits;
}
//---------------------------------------------------------------------------

EsString EsChannelIoEkonnect::get_device() const
{
	EsMutexLocker lock(m_mx);

	return m_device;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_device(const EsString& val)
{
	EsMutexLocker lock(m_mx);

	if( m_device != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change device while channel is active."));

		m_device = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_baud() const
{
	EsMutexLocker lock(m_mx);

	return m_baud;
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::internalSetBaud(const long& val, bool doThrow)
{
	if( m_baud != val )
	{
		m_baud = val;

		if( m_io )
			m_io->ftSetBaudRate(m_baud);
	}

	return true;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_baud(const long& val)
{
	EsMutexLocker lock(m_mx);

	internalSetBaud(val, true);
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_bits() const
{
	EsMutexLocker lock(m_mx);

	return m_bits;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_bits(const long& val)
{
	EsMutexLocker lock(m_mx);

	if( m_bits != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change data bit length while channel is active."));
		m_bits = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_parity() const
{
	EsMutexLocker lock(m_mx);

	return m_parity;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_parity(const long &val)
{
	EsMutexLocker lock(m_mx);

	if( m_parity != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change parity while channel is active."));
		m_parity = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_stopBits() const
{
	EsMutexLocker lock(m_mx);

	return m_stopBits;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_stopBits(const long &val)
{
	EsMutexLocker lock(m_mx);

	if( m_stopBits != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change stop bits while channel is active."));
		m_stopBits = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_rxTmo() const
{
	EsMutexLocker lock(m_mx);

	return m_rxTmo;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_rxTmo(const long &val)
{
	EsMutexLocker lock(m_mx);

	if( m_rxTmo != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change RX timeout while channel is active."));
		m_rxTmo = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_txTmo() const
{
	EsMutexLocker lock(m_mx);

	return m_txTmo;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_txTmo(const long &val)
{
	EsMutexLocker lock(m_mx);

	if( m_txTmo != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change TX timeout while channel is active."));
		m_txTmo = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_rxBuffLen() const
{
	EsMutexLocker lock(m_mx);

	return m_rxBuffLen;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_rxBuffLen(const long &val)
{
	EsMutexLocker lock(m_mx);

	if( m_rxBuffLen != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change RX buffer length while channel is active."));
		m_rxBuffLen = val;
	}
}
//---------------------------------------------------------------------------

long EsChannelIoEkonnect::get_txBuffLen() const
{
	EsMutexLocker lock(m_mx);

	return m_txBuffLen;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_txBuffLen(const long &val)
{
	EsMutexLocker lock(m_mx);

	if( m_txBuffLen != val )
	{
		if( m_io && m_io->isOpen() )
			EsException::Throw( _("Cannot change TX buffer length while channel is active."));
		m_txBuffLen = val;
	}
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::get_resetOnRxTmo() const
{
	EsMutexLocker lock(m_mx);

	return m_resetOnRxTmo;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_resetOnRxTmo(const bool& val)
{
	EsMutexLocker lock(m_mx);

	m_resetOnRxTmo = val;
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::get_useRS232() const
{
	EsMutexLocker lock(m_mx);

	return m_useRS232;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_useRS232(const bool& val)
{
	EsMutexLocker lock(m_mx);

	m_useRS232 = val;
	if( m_io )
		setCbusBits(
      m_useRS232,
      m_supplyAuxPower,
      m_programmingMode
    );
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::get_isEkonnect1() const
{
	EsMutexLocker lock(m_mx);

  return 0 == m_device.find(esT("60000-"));
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::devicePowerSet(bool powerOn)
{
	EsMutexLocker lock(m_mx);

	if(m_supplyAuxPower != powerOn)
	{
		m_supplyAuxPower = powerOn;
		if( m_io )
			setCbusBits(
        m_useRS232,
        m_supplyAuxPower,
        m_programmingMode
      );
	}
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::devicePowerGet() const
{
	EsMutexLocker lock(m_mx);

	return m_supplyAuxPower;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::deviceProgrammingModeSet(bool programmingOn)
{
	EsMutexLocker lock(m_mx);

	if( m_programmingMode != programmingOn )
	{
		m_programmingMode = programmingOn;
		if( m_io )
			setCbusBits(
        m_useRS232,
        m_supplyAuxPower,
        m_programmingMode
      );
	}
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::deviceProgrammingModeGet() const
{
	EsMutexLocker lock(m_mx);

	return m_programmingMode;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_breaker(const EsBaseIntfPtr& breaker)
{
	breakerSet( breaker );
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::set_monitor(const EsBaseIntfPtr& monitor)
{
	monitorSet( monitor );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoEkonnect::breakerGet)() const
{
	EsMutexLocker lock(m_mx);
	return m_breaker;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoEkonnect::breakerSet)(const EsBaseIntf::Ptr& breaker)
{
	EsMutexLocker lock(m_mx);
	m_breaker = breaker;
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::setCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode)
{
	if( m_io )
	{
		esU8 ctlMask = 0xE0;
		if( useStdSocket )
			ctlMask |= 0x04; // CBUS2 = standard socket (1) | Ekosfera socket (0)
		if( !supplyAuxPower )
			ctlMask |= 0x02; // CBUS1 = head_on (0)
		if( !programmingMode )
			ctlMask |= 0x08; // CBUS3 = progMode (0)

		m_io->ftSetBitMode(
      ctlMask,
      EsFtdiDriver::BITMODE_CBUS
    );

		return checkLogStatus();
	}

	return false;
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::interlockedSetCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode)
{
	EsMutexLocker lock(m_mx);

	return setCbusBits(useStdSocket, supplyAuxPower, programmingMode);
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::checkLogStatus() const
{
	if( m_io && m_io->statusGet() != EsFtdiDeviceIntf::FT_OK )
	{
		ES_DUAL_CALL2_NR(
      m_monitor,
      EsChannelIoMonitorIntf,
      channelError,
      internalIdGet(),
      EsFtdiDriver::statusStringGet(
        m_io->statusGet()
      )
    );

		return false;
	}

	return true;
}
//---------------------------------------------------------------------------

EsString EsChannelIoEkonnect::idStringFormat(const EsString& deviceIdStr)
{
  return EsString::format(
    esT("%s-%s"),
    EsChannelIoEkonnect::classNameGetStatic(),
    deviceIdStr
  );
}
//---------------------------------------------------------------------------

EsString EsChannelIoEkonnect::internalIdGet() const
{
	return idStringFormat(m_device);
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString, EsChannelIoEkonnect::idGet)() const
{
	EsMutexLocker lock(m_mx);
	return internalIdGet();
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::internalIsBreaking() const
{
	EsVariant result = false;
	ES_DUAL_CALL0(
    result,
    m_breaker,
    EsBreakIntf,
    isBreaking
  );

	return result.asBool();
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::internalOpen()
{
	bool result = false;

  const EsFtdiDriver::DeviceList& devs = EsFtdiDriver::instanceGet().currentDeviceListGet();
  if( devs.empty() )
    EsFtdiDriver::instanceGet().deviceListGet(); //< Current list is empty - enumerate non-busy devices

  m_io = EsFtdiDriver::instanceGet().deviceCreate(
    EsFtdiDeviceKind::Generic,
    m_device,
    EsString::null()
  );

	if( m_io &&
      m_io->open() &&
		  m_io->ftSetBaudRate( m_baud ) &&
			m_io->ftSetDataCharacteristics(
          (esU8)m_bits,
          (esU8)m_stopBits,
          (esU8)m_parity
      ) &&
			m_io->ftSetTimeouts(
        m_rxTmo,
        m_txTmo
      ) &&
			m_io->ftSetUSBParameters(
        m_rxBuffLen,
        m_txBuffLen
      ) &&
			setCbusBits(
        m_useRS232,
        m_supplyAuxPower,
        m_programmingMode
      ) &&
      internalReset()
  )
  {
    result = true;

    ES_DUAL_CALL1_NR(
      m_monitor,
      EsChannelIoMonitorIntf,
      channelOpened,
      internalIdGet()
    );
  }
  else
  {
		checkLogStatus();

    m_io.reset();
  }

	return result;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::internalClose()
{
	if( m_io )
	{
		internalReset();

    m_io.reset();

		if( checkLogStatus() )
			ES_DUAL_CALL1_NR(
        m_monitor,
        EsChannelIoMonitorIntf,
        channelClosed,
        internalIdGet()
      );
		else
			EsFtdiDriver::statusCheckThrow(
        m_io->statusGet()
      );
	}
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoEkonnect::monitorGet)() const
{
	EsMutexLocker lock(m_mx);
	return m_monitor;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoEkonnect::monitorSet)(const EsBaseIntf::Ptr& monitor)
{
	EsMutexLocker lock(m_mx);
	m_monitor = monitor;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoEkonnect::open)()
{
	EsMutexLocker lock(m_mx);

	if( 0 != m_io )
		return true;
	else
		return internalOpen();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoEkonnect::close)()
{
	EsMutexLocker lock(m_mx);

	internalClose();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoEkonnect::isOpen)() const
{
	EsMutexLocker lock(m_mx);

	return 0 != m_io;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::internalPutBytes(const esU8* data, esU32 len, esU32 tmo)
{
	bool timeout = false;
	const esU8* pos = data;
	const esU8* end = data+len;

	if( m_io )
	{
		while( !timeout &&
						checkLogStatus() &&
						!internalIsBreaking() &&
						pos < end
    )
		{
			esU32 result = 0;
			m_io->ftWrite(
        reinterpret_cast<void*>(
          const_cast<esU8*>(
            data
          )
        ),
        len,
        &result
      );

			if( !checkLogStatus() )
				EsFtdiDriver::statusCheckThrow(
          m_io->statusGet()
        );

			// send "bytes sent" event
			if( result )
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

			// move to the next buffer position
			pos += result;
		}

		if(internalIsBreaking())
			internalLineBreak();
	}

	return pos-data;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(esU32, EsChannelIoEkonnect::bytesPut)(const esU8* data, esU32 len, esU32 tmo)
{
	EsMutexLocker lock(m_mx);
	return internalPutBytes(data, len, tmo);
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::queryUnreadBytes()
{
	esU32 result = 0;

	m_io->ftGetQueueStatus(&result);

	if( !checkLogStatus() )
		EsFtdiDriver::statusCheckThrow(
      m_io->statusGet()
    );

	return result;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::internalGetBytes(esU8* data, esU32 len, esU32 tmo /*= 0*/)
{
	bool timeout = false;
	esU32 localTmo = tmo;
	esU8* pos = data;
	esU8* end = data+len;

	if( m_io && len )
	{
		while( !timeout &&
						checkLogStatus() &&
						pos < end &&
						!internalIsBreaking()
    )
		{
			esU32 toRead = std::min(
        queryUnreadBytes(),
        static_cast<esU32>(end-pos)
      );

			if( toRead )
			{
				localTmo = tmo; // reset local timeout
				esU32 result = 0;

				m_io->ftRead(
          pos,
          toRead,
          &result
        );

				if( !checkLogStatus() )
					EsFtdiDriver::statusCheckThrow(
            m_io->statusGet()
          );

				// send "bytes received" event
				if( result )
					ES_DUAL_CALL2_NR(
            m_monitor,
            EsChannelIoMonitorIntf,
            channelBytesReceived,
            internalIdGet(),
            EsBinBuffer(
              data,
              data+result
            )
          );

				// move to the next buffer position
				pos += result;
			}
			else // enter bytes polling
			{
				if( tmo )
					EsThread::sleep(SLEEP_GRANULARITY);

				if( localTmo > SLEEP_GRANULARITY )
					localTmo -= SLEEP_GRANULARITY;
				else
				{
					timeout = true;

					if( m_resetOnRxTmo )
						internalReset();
				}
			}
		}
	}

	return pos-data;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(esU32, EsChannelIoEkonnect::bytesGet)(esU8* data, esU32 len, esU32 tmo)
{
	EsMutexLocker lock(m_mx);
	return internalGetBytes(
    data,
    len,
    tmo
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoEkonnect::bytesPut)(const EsBinBuffer& data, ulong tmo)
{
	if( !data.empty() )
		return bytesPut(
      &data[0],
      data.size(),
      tmo
    );

	return 0;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBinBuffer, EsChannelIoEkonnect::bytesGet)(ulong len, ulong tmo)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = bytesGet(
      &result[0],
      len,
      tmo
    );

		result.resize(len);
	}

	return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoEkonnect::txTimeEstimateGet)(ulong len) const
{
	return EsChannelIoUart::getMaxByteTmo(
		m_baud,
    m_bits,
    0 != m_parity,
    m_stopBits
  ) * len;
}
//---------------------------------------------------------------------------

bool EsChannelIoEkonnect::internalReset()
{
	if( m_io )
	{
		m_io->ftPurge(EsFtdiDriver::PURGE_RX|EsFtdiDriver::PURGE_TX);

		return checkLogStatus();
	}

	return false;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::lineBreak()
{
	EsMutexLocker lock(m_mx);
	internalLineBreak();
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::internalLineBreak()
{
	if( m_io )
	{
		m_io->ftSetBreakOn();
		EsThread::sleep(
      txTimeEstimateGet(1)
    );

		m_io->ftSetBreakOff();
	}
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoEkonnect::reset)()
{
	EsMutexLocker lock(m_mx);

	internalReset();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(long, EsChannelIoEkonnect::errorGet)() const
{
	EsMutexLocker lock(m_mx);

  if( m_io )
	  return m_io->statusGet();

  return 0;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString, EsChannelIoEkonnect::errorStringGet)() const
{
	EsMutexLocker lock(m_mx);

	if( m_io )
		return EsFtdiDriver::statusStringGet(
      m_io->statusGet()
    );

	return EsString::null();
}
//---------------------------------------------------------------------------

class isNotApplicableDevice
{
public:
	inline bool operator() (const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node) const
	{
		regexSerialGet().set_text(node.serialNumberStrGet());
		regexDescrGet().set_text(node.descriptionStrGet());

		return EsFtdiDriver::DEVICE_232R != node.typeGet() ||
			!regexSerialGet().get_matches() ||
			!regexDescrGet().get_matches();
	}

protected:
	static EsRegEx& regexSerialGet()
  {
    static EsRegEx s_regexSerial(esT("6000(0|1)-[0-9]{5}-[0-9]{2}"),
              static_cast<ulong>(EsRegExCompileFlag::DEFAULT)|
              static_cast<ulong>(EsRegExCompileFlag::NOSUB));

    return s_regexSerial;
  }

	static EsRegEx& regexDescrGet()
  {
    static EsRegEx s_regexDescr(esT("EKONNECT[0-9]?"),
              static_cast<ulong>(EsRegExCompileFlag::DEFAULT)|
              static_cast<ulong>(EsRegExCompileFlag::NOSUB));

    return s_regexDescr;
  }

private:
	isNotApplicableDevice& operator=(const isNotApplicableDevice& src);
};
//---------------------------------------------------------------------------

EsFtdiDriver::DeviceList EsChannelIoEkonnect::getDeviceList()
{
	EsFtdiDriver::DeviceList result;

	// Silently catch all possible exceptions
	try
	{
		result = EsFtdiDriver::instanceGet().deviceListGet();
	}
	catch(...)
	{
	}

	// filter only devices with our hardware idGet & serial mask
	EsFtdiDriver::DeviceList::iterator new_end =
		std::remove_if(
      result.begin(),
      result.end(),
      isNotApplicableDevice()
    );

	result.erase(
    new_end,
    result.end()
  );

	return result;
}
//---------------------------------------------------------------------------

EsVariant EsChannelIoEkonnect::enumerate(const EsVariant& includeBusyPorts)
{
  return EsFtdiDriver::enumerate(
    includeBusyPorts,
    false
  );
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoEkonnect::isRateSupported)(ulong rate) const
{
  return baudsGet().end() != std::find_if(
    baudsGet().begin(),
    baudsGet().end(),
    [rate]( const EsVariant::Array::value_type& val ) -> bool {
      return rate == val.asULong();
    }
  );

	return false;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsVariant, EsChannelIoEkonnect::supportedRatesGet)() const
{
	return baudsGet();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoEkonnect::defaultRateGet)() const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud).defaultGet().asLong();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(ulong, EsChannelIoEkonnect::rateGet)() const
{
	return get_baud();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoEkonnect::rateSet)(ulong rate)
{
	EsMutexLocker lock(m_mx);

	return internalSetBaud(rate, false);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

esBL EsChannelIoEkonnect::cLock(EseChannelIo* p, esU32 tmo)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
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

void EsChannelIoEkonnect::cUnlock(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		EsMutex::Result result = chnl->m_mx.unlock();
		ES_ASSERT(EsMutex::resultOk == result);
	}
	catch(...)
	{}
}
//---------------------------------------------------------------------------

esBL EsChannelIoEkonnect::cIsConnected(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	return chnl->m_io;
}
//---------------------------------------------------------------------------

esBL EsChannelIoEkonnect::cConnect(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		return chnl->internalOpen();
	}
	catch(...)
	{}
	
	return FALSE;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::cDisconnect(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		chnl->internalClose();
	}
	catch(...)
	{}
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		return chnl->internalPutBytes(data, count, 1000);
	}
	catch(...)
	{}

	return 0;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		return chnl->internalGetBytes(data, count, timeout);
	}
	catch(...)
	{}

	return 0;
}
//---------------------------------------------------------------------------

void EsChannelIoEkonnect::cResetIo(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		chnl->internalReset();
	}
	catch(...)
	{}
}
//---------------------------------------------------------------------------

esBL EsChannelIoEkonnect::cWaitTxEmpty(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
  if( !chnl->m_io )
    return FALSE;

	try
	{
		esBL breaking = chnl->internalIsBreaking();
		esU32 rx, tx, evt;
		bool ok = chnl->m_io->ftGetStatus(&rx, &tx, &evt);

		while( !breaking &&
						ok &&
						0 != tx )
		{
			EsThread::sleep(4);
			breaking = chnl->internalIsBreaking();
		}

		return !breaking && ok;
	}
	catch(...)
	{}

	return FALSE;
}
//---------------------------------------------------------------------------

esBL EsChannelIoEkonnect::cIsRateSupported(EseChannelIo* p, esU32 rate)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		return chnl->isRateSupported(rate);
	}
	catch(...)
	{}

	return FALSE;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::cGetRate(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	return chnl->m_baud;
}
//---------------------------------------------------------------------------

esBL EsChannelIoEkonnect::cSetRate(EseChannelIo* p, esU32 rate)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	try
	{
		return chnl->internalSetBaud(rate, false);
	}
	catch(...)
	{}

	return FALSE;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoEkonnect::cSendEstimateGet(EseChannelIo* p, esU32 len)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
	return chnl->txTimeEstimateGet(len);
}
//---------------------------------------------------------------------------

int EsChannelIoEkonnect::cGetError(EseChannelIo* p)
{
	EsChannelIoEkonnect* chnl = (EsChannelIoEkonnect*)p->m_bus;
  if( chnl->m_io )
    return chnl->m_io->statusGet();

	return 0;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsChannelIoEkonnect::commChannelInit)(EseChannelIo* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_UART;
	chnl->lock = EsChannelIoEkonnect::cLock;
	chnl->unlock = EsChannelIoEkonnect::cUnlock;
	chnl->isConnected = EsChannelIoEkonnect::cIsConnected;
	chnl->connect = EsChannelIoEkonnect::cConnect;
	chnl->disconnect = EsChannelIoEkonnect::cDisconnect;
	// set rate support
	chnl->isRateSupported = EsChannelIoEkonnect::cIsRateSupported;
	chnl->rateGet = EsChannelIoEkonnect::cGetRate;
	chnl->rateSet = EsChannelIoEkonnect::cSetRate;
	chnl->sendTimeEstimateGet = EsChannelIoEkonnect::cSendEstimateGet;
	// data io
	chnl->bytesPut = EsChannelIoEkonnect::cPutBytes;
	chnl->bytesGet = EsChannelIoEkonnect::cGetBytes;
	chnl->resetIo = EsChannelIoEkonnect::cResetIo;
	chnl->waitTxEmpty = EsChannelIoEkonnect::cWaitTxEmpty;
	// misc
	chnl->errorGet = EsChannelIoEkonnect::cGetError;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_EKONNECT

