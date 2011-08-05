#include <stdafx.h>
#pragma hdrstop

#include <algorithm>
#include "EkonnectIoChannel.h"
#include "EkonnectIoChannelConfigPane.h"

// driver singleton
FtdiDriver EkonnectIoChannel::s_drv;
 
// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EkonnectIoChannel, _i("EKONNECT adapter direct connection channel"))
	// reflected property infos declarations
	ES_DECL_PROP_INFO(EkonnectIoChannel, device, EsString, _i("Device name"), wxT("60000-00000-00"))
	ES_DECL_PROP_INFO_ENUM_BEGIN(EkonnectIoChannel, baud, long, _i("Baud rate"), FtdiDriver::FT_BAUD_9600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_300)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_1200)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_2400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_4800)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_9600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_14400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_19200)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_38400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_57600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_115200)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_230400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_460800)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BAUD_921600)
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_ENUM_BEGIN(EkonnectIoChannel, bits, long, _i("Byte size"), FtdiDriver::FT_BITS_8)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BITS_5)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BITS_6)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BITS_7)
		ES_DECL_PROP_INFO_ENUM_ENTRY(FtdiDriver::FT_BITS_8)
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_ENUM_BEGIN(EkonnectIoChannel, parity, long, _i("Parity"), FtdiDriver::FT_PARITY_NONE)
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_PARITY_NONE,	_i("None"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_PARITY_ODD,		_i("Odd"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_PARITY_EVEN,	_i("Even"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_PARITY_MARK,	_i("Mark"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_PARITY_SPACE, _i("Space"))
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_ENUM_BEGIN(EkonnectIoChannel, stopBits, long, _i("Stop bits"), FtdiDriver::FT_STOP_BITS_1)
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_STOP_BITS_1,	_i("1 Stop Bit"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_STOP_BITS_1_5,_i("1.5 Stop Bits"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(FtdiDriver::FT_STOP_BITS_2,	_i("2 Stop Bits"))
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_RANGE(EkonnectIoChannel, rxTmo, long, _i("RX timeout in ms"), FtdiDriver::FT_DEFAULT_RX_TIMEOUT, 1, 65536)		
	ES_DECL_PROP_INFO_RANGE(EkonnectIoChannel, txTmo, long, _i("TX timeout in ms"), FtdiDriver::FT_DEFAULT_TX_TIMEOUT, 1, 65536)
	ES_DECL_PROP_INFO_RANGE(EkonnectIoChannel, rxBuffLen, long, _i("RX buffer size"), FtdiDriver::FT_DEFAULT_RX_BUFFER, 64, 65536)		
	ES_DECL_PROP_INFO_RANGE(EkonnectIoChannel, txBuffLen, long, _i("TX buffer size"), FtdiDriver::FT_DEFAULT_TX_BUFFER, 64, 65536)
	ES_DECL_PROP_INFO(EkonnectIoChannel, resetOnRxTmo, bool, _i("Automatically reset channel on RX timeout"), false)
	ES_DECL_PROP_INFO(EkonnectIoChannel, useStandardSocket, bool, _i("Use standard RS232 socket"), false)
	ES_DECL_PROP_INFO(EkonnectIoChannel, supplyAuxPower, bool, _i("Supply power for connected device"), true)
	ES_DECL_PROP_INFO(EkonnectIoChannel, programmingMode, bool, _i("Request device programming mode"), false)
	ES_DECL_PROP_INFO_RANGE(EkonnectIoChannel, rebootDelay, long, _i("Delay after device reboot, ms"), 1000, 250, 60000)
	// reflected interface methods
	//
	// EsReflectedClassIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsReflectedClassIntf, setConfigString, void_Call_cr_EsString_bool, _i("Set EKONNECT device channel configuration string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsReflectedClassIntf, getConfigString, EsString_CallConst, _i("Get EKONNECT device channel configuration string"))
	// EsIoChannelIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, open, bool_Call, _i("Open EKONNECT device channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, close, void_Call, _i("Close EKONNECT device channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, isOpen, bool_CallConst, _i("Return true if EKONNECT device channel is open"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, putBytes, ulong_Call_cr_EsBinBuffer_ulong_EsBaseIntfPtr, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, getBytes, EsBinBuffer_Call_ulong_ulong_EsBaseIntfPtr, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, getTxTimeEstimate, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, reset, void_Call, _i("Reset EKONNECT device channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsIoChannelIntf, getError, long_CallConst, _i("Get EKONNECT device channel-specific error code"))
	// BootCtlIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, BootCtlIntf, bootNormalMode, bool_Call, _i("Try to (re)boot connected device in normal mode"))
	ES_DECL_REFLECTED_METHOD_INFO(EkonnectIoChannel, bootProgrammingModeBlocking, bootProgrammingMode, bool_Call, _i("Try to (re)boot connected device in programming mode"))
	// EsRateCtlIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsRateCtlIntf, isRateSupported, bool_CallConst_long, _i("Return true if requested channel rate is supported"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsRateCtlIntf, getSupportedRates, EsVariant_CallConst, _i("Return variant array of supported rates"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsRateCtlIntf, getDefaultRate, long_CallConst, _i("Return default channel rate value"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsRateCtlIntf, getRate, long_CallConst, _i("Return currently set channel rate"))	
	ES_DECL_REFLECTED_INTF_METHOD_INFO(EkonnectIoChannel, EsRateCtlIntf, setRate, bool_Call_long, _i("Try to set channel rate"))	
	// reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(EkonnectIoChannel, EsBaseIntfPtr_ClassCall, _i("EkonnectIoChannel constructor"))
ES_DECL_CLASS_INFO_END

// -------------------------------------------------------------------------------------------------------
// ekobox communication class
//
EkonnectIoChannel::EkonnectIoChannel(wxEvtHandler* sink) :
m_stat(FtdiDriver::FT_OK),
m_io(0),
m_evtSink(0)
{
	s_drv.load();
	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);

	setEvtSink(sink);
}

EkonnectIoChannel::~EkonnectIoChannel()
{
	close();
	setEvtSink(0);
	s_drv.unload();
}

// properties
EsString EkonnectIoChannel::get_device() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_device;
}

void EkonnectIoChannel::set_device(const EsString& val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_device != val )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change device while channel is active."));
		m_device = val;
	}
}

long EkonnectIoChannel::get_baud() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_baud;
}

// internal baud setter service 
bool EkonnectIoChannel::internalSetBaud(const long& val, bool doThrow)
{
	if( m_baud != val )
	{
		if( ES_THIS_REFLECTED_PROP_INFO_GET(baud)->validate(val, doThrow) )
		{
			m_baud = val;
			
			if( m_io )
			{
				m_stat = s_drv.ftSetBaudRate(m_io, m_baud);
				if( checkLogStatus() )
					return true;
				else if( doThrow )
					FtdiDriver::checkThrowStatus(m_stat);				
			}
			else
				return true;
		}
	}
	else
		return true;
	
	return false;
}

void EkonnectIoChannel::set_baud(const long& val)
{
	wxMutexLocker lock(m_mx); 
	
	internalSetBaud(val, true);
}

long EkonnectIoChannel::get_bits() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_bits;
}

void EkonnectIoChannel::set_bits(const long& val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_bits != val && ES_THIS_REFLECTED_PROP_INFO_GET(bits)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change data bit length while channel is active."));
		m_bits = val;
	}
}

long EkonnectIoChannel::get_parity() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_parity;
}

void EkonnectIoChannel::set_parity(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_parity != val && ES_THIS_REFLECTED_PROP_INFO_GET(parity)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change parity while channel is active."));
		m_parity = val;
	}
}

long EkonnectIoChannel::get_stopBits() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_stopBits;
}

void EkonnectIoChannel::set_stopBits(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_stopBits != val && ES_THIS_REFLECTED_PROP_INFO_GET(stopBits)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change stop bits while channel is active."));
		m_stopBits = val;
	}
}
long EkonnectIoChannel::get_rxTmo() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_rxTmo;
}

void EkonnectIoChannel::set_rxTmo(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_rxTmo != val && ES_THIS_REFLECTED_PROP_INFO_GET(rxTmo)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change RX timeout while channel is active."));
		m_rxTmo = val;
	}
}

long EkonnectIoChannel::get_txTmo() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_txTmo;
}

void EkonnectIoChannel::set_txTmo(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_txTmo != val && ES_THIS_REFLECTED_PROP_INFO_GET(txTmo)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change TX timeout while channel is active."));
		m_txTmo = val;
	}
}

long EkonnectIoChannel::get_rxBuffLen() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_rxBuffLen;
}

void EkonnectIoChannel::set_rxBuffLen(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_rxBuffLen != val && ES_THIS_REFLECTED_PROP_INFO_GET(rxBuffLen)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change RX buffer length while channel is active."));
		m_rxBuffLen = val;
	}
}

long EkonnectIoChannel::get_txBuffLen() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_txBuffLen;
}

void EkonnectIoChannel::set_txBuffLen(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_txBuffLen != val && ES_THIS_REFLECTED_PROP_INFO_GET(txBuffLen)->validate(val, true) )
	{
		if( 0 != m_io )
			EsException::Throw(0, EsException::Generic, _("Cannot change TX buffer length while channel is active."));
		m_txBuffLen = val;
	}
}

// autoreset on RX timeout
bool EkonnectIoChannel::get_resetOnRxTmo() const 
{ 
	wxMutexLocker lock(m_mx); 
	 
	return m_resetOnRxTmo; 
}

void EkonnectIoChannel::set_resetOnRxTmo(const bool& val) 
{ 
	wxMutexLocker lock(m_mx); 
	 
	m_resetOnRxTmo = val; 
}

// communication socket selector
bool EkonnectIoChannel::get_useStandardSocket() const 
{ 
	wxMutexLocker lock(m_mx); 
	 
	return m_useStandardSocket; 
}

void EkonnectIoChannel::set_useStandardSocket(const bool& val) 
{ 
	wxMutexLocker lock(m_mx); 
	 
	m_useStandardSocket = val;
	if( m_io )
		setCbusBits(m_useStandardSocket, m_supplyAuxPower, m_programmingMode);
}

// auxiliary power supply control
bool EkonnectIoChannel::get_supplyAuxPower() const 
{ 
	wxMutexLocker lock(m_mx); 
	 
	return m_supplyAuxPower; 
}

void EkonnectIoChannel::set_supplyAuxPower(const bool& val) 
{ 
	wxMutexLocker lock(m_mx); 
	 
	m_supplyAuxPower = val;
	if( m_io )
		setCbusBits(m_useStandardSocket, m_supplyAuxPower, m_programmingMode);
}

// programming mode selection
bool EkonnectIoChannel::get_programmingMode() const 
{ 
	wxMutexLocker lock(m_mx); 
	 
	return m_programmingMode; 
}

void EkonnectIoChannel::set_programmingMode(const bool& val) 
{ 
	wxMutexLocker lock(m_mx); 
	 
	m_programmingMode = val;
	if( m_io )
		setCbusBits(m_useStandardSocket, m_supplyAuxPower, m_programmingMode);
}

long EkonnectIoChannel::get_rebootDelay() const
{
	wxMutexLocker lock(m_mx); 
	
	return m_rebootDelay;
}

void EkonnectIoChannel::set_rebootDelay(const long &val)
{
	wxMutexLocker lock(m_mx); 
	
	if( m_rebootDelay != val && ES_THIS_REFLECTED_PROP_INFO_GET(rebootDelay)->validate(val, true) )
		m_rebootDelay = val;
}

// internal CBUS pins setup
bool EkonnectIoChannel::setCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode)
{
	if( 0 != m_io )
	{
		BYTE ctlMask = 0xE0;
		if( useStdSocket )
			ctlMask |= 0x04; // CBUS2 = standard socket (1) | Ekosfera socket (0)
		if( !supplyAuxPower )
			ctlMask |= 0x02; // CBUS1 = head_on (0)
		if( !programmingMode )
			ctlMask |= 0x08; // CBUS3 = progMode (0)
		m_stat = s_drv.ftSetBitMode(m_io, ctlMask, FtdiDriver::FT_BITMODE_CBUS);
		return checkLogStatus();
	}
	
	return false;
}

bool EkonnectIoChannel::interlockedSetCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode)
{
	wxMutexLocker lock(m_mx); 
	
	return setCbusBits(useStdSocket, supplyAuxPower, programmingMode);
}

// check for & log ftdi device status from the last call
bool EkonnectIoChannel::checkLogStatus() const
{
	if( m_stat != FtdiDriver::FT_OK )
	{
		if( m_evtSink )
			wxQueueEvent( m_evtSink,
				new IoChannelEvent(IoChannelEvent::TypeError, FtdiDriver::getStatusString(m_stat))
			);

		return false;
	}
	
	return true;
}

void EkonnectIoChannel::save(wxConfigBase& storage) const
{
	// save channel settings
	storage.Write( getClassInfo().getName(), getConfigString().c_str() );
}

void EkonnectIoChannel::load(wxConfigBase& storage)
{
	// save current storage path
	wxString configString;
	storage.Read( getClassInfo().getName(), &configString, wxEmptyString );
	setConfigString( configString, true );
}

// open port using current configuration
bool EkonnectIoChannel::internalOpen()
{
	bool result = false;
	m_stat = s_drv.ftOpenEx( reinterpret_cast<PVOID>(
		const_cast<char*>( wxString(m_device.c_str()).ToAscii().data() )), 
		FtdiDriver::FT_OPEN_BY_SERIAL_NUMBER,
		&m_io);
	
	if( checkLogStatus() )
	{
		m_stat = s_drv.ftSetBaudRate(m_io, m_baud);
		if( checkLogStatus() )
		{
			m_stat = s_drv.ftSetDataCharacteristics(m_io, m_bits, m_stopBits, m_parity);
			if( checkLogStatus() )
			{
				m_stat = s_drv.ftSetTimeouts(m_io, m_rxTmo, m_txTmo);
				if( checkLogStatus() )
				{
					m_stat = s_drv.ftSetUSBParameters(m_io, m_rxBuffLen, m_txBuffLen);
					if( checkLogStatus() && setCbusBits(m_useStandardSocket, m_supplyAuxPower, m_programmingMode) && internalReset() )
					{
						result = true;
						if( m_evtSink )
							wxQueueEvent( m_evtSink,
								new IoChannelEvent(IoChannelEvent::TypeOpen, 
									EsString::format(_("EKONNECT device '%s' opened. Configured for %d baud, %d data bits, %s, %s."),
									m_device.c_str(), m_baud, m_bits, 
									ES_THIS_REFLECTED_PROP_INFO_GET(parity)->getValueLabel(m_parity).c_str(),	
									ES_THIS_REFLECTED_PROP_INFO_GET(stopBits)->getValueLabel(m_stopBits).c_str()) )
							);
					}
				}
			}
		}
	}

	if( FtdiDriver::FT_OK != m_stat )
	{
		if( m_io )
			m_stat = s_drv.ftClose(m_io);
		
		checkLogStatus();
		m_io = 0;
		FtdiDriver::checkThrowStatus(m_stat);
	}

	return result;
}

void EkonnectIoChannel::internalClose()
{
	if( 0 != m_io )
	{
		internalReset();
		m_stat = s_drv.ftClose(m_io);
		m_io = 0;		
		if( checkLogStatus() )
		{
			if( m_evtSink )
				wxQueueEvent( m_evtSink,
					new IoChannelEvent(IoChannelEvent::TypeClose, 
						EsString::format(_("Device '%s' closed."), m_device.c_str()) )
				);
		}
		else
			FtdiDriver::checkThrowStatus(m_stat);
	}
}

// port manipulation
ES_IMPL_INTF_METHOD(bool, EkonnectIoChannel::open)()
{
	wxMutexLocker lock(m_mx); 
	
	if( 0 != m_io )
		return true;
	else
		return internalOpen();
}

ES_IMPL_INTF_METHOD(void, EkonnectIoChannel::close)()
{
	wxMutexLocker lock(m_mx); 
	
	internalClose();
}

ES_IMPL_INTF_METHOD(bool, EkonnectIoChannel::isOpen)() const
{
	wxMutexLocker lock(m_mx); 
	
	return 0 != m_io;
}

// byte io
DWORD EkonnectIoChannel::internalPutBytes(const BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk)
{
	bool timeout = false;
	const BYTE* pos = data;
	const BYTE* end = data+len;

	if( m_io != 0 )
	{
		m_stat = FtdiDriver::FT_OK;
		while( !timeout && 
						checkLogStatus() && 
						!chnlIsBreaking(brk) && 
						pos < end )
		{
			DWORD result = 0;
			m_stat = s_drv.ftWrite(m_io, reinterpret_cast<LPVOID>(const_cast<BYTE*>(data)), len, &result);
			if( !checkLogStatus() )
				FtdiDriver::checkThrowStatus(m_stat);

			// send "bytes sent" event
			if( m_evtSink && result )
				wxQueueEvent( m_evtSink,
					new IoChannelEvent(IoChannelEvent::TypeTX, 
						EsString::format(_("%d bytes sent."), result), 
						pos, std::min(end, pos+result) )
				);
			
			// move to the next buffer position
			pos += result;
		}
	}

	return pos-data; 
}

ES_IMPL_INTF_METHOD(DWORD, EkonnectIoChannel::putBytes)(const BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker)
{
	wxMutexLocker lock(m_mx); 
	EsCommChannelBreakImpl brk(breaker);
	return internalPutBytes(data, len, tmo, brk.getCommChannelBreakPtr());	
}

// query unread bytes
DWORD EkonnectIoChannel::queryUnreadBytes()
{
	DWORD result = 0;

	m_stat = s_drv.ftGetQueueStatus(m_io, &result);
	if( !checkLogStatus() )
		FtdiDriver::checkThrowStatus(m_stat);

	return result;
}

DWORD EkonnectIoChannel::internalGetBytes(BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk /*= 0*/)
{
	bool timeout = false;
	DWORD localTmo = tmo;
	BYTE* pos = data;
	BYTE* end = data+len;

	if( 0 != m_io && len )
	{
		m_stat = FtdiDriver::FT_OK;
		while( !timeout && 
						checkLogStatus() && 
						pos < end &&
						!chnlIsBreaking(brk) )
		{
			DWORD toRead = std::min(queryUnreadBytes(), static_cast<DWORD>(end-pos));
			if( toRead )
			{
				localTmo = tmo; // reset local timeout
				DWORD result = 0;

				m_stat = s_drv.ftRead(m_io, pos, toRead, &result);
				if( !checkLogStatus() )
					FtdiDriver::checkThrowStatus(m_stat);

				// send "bytes received" event
				if( m_evtSink && result )
					wxQueueEvent( m_evtSink,
						new IoChannelEvent(IoChannelEvent::TypeRX, 
							EsString::format(_("%d bytes received."), result), 
							pos, std::min(end, pos+result) )
					);
				
				// move to the next buffer position
				pos += result;
			}
			else // enter bytes polling 
			{
				if( tmo )
					wxThread::Sleep(SLEEP_GRANULARITY);
					
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

ES_IMPL_INTF_METHOD(DWORD, EkonnectIoChannel::getBytes)(BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker)
{
	wxMutexLocker lock(m_mx); 
	EsCommChannelBreakImpl brk(breaker);
	return internalGetBytes(data, len, tmo, brk.getCommChannelBreakPtr());	
}

ES_IMPL_INTF_METHOD(ulong, EkonnectIoChannel::putBytes)(const EsBinBuffer& data, ulong tmo, EsBaseIntfPtr breaker)
{
	if( !data.empty() )
		return putBytes(&data[0], data.size(), tmo, breaker);
	
	return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, EkonnectIoChannel::getBytes)(ulong len, ulong tmo, EsBaseIntfPtr breaker)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = getBytes(&result[0], len, tmo, breaker);
		result.erase(result.begin()+len, result.end());
	}
	
	return result;
}

ES_IMPL_INTF_METHOD(ulong, EkonnectIoChannel::getTxTimeEstimate)(ulong len) const
{
	return UartIoChannel::getMaxByteTmo(
		get_baud(), get_bits(), 0 != get_parity(), get_stopBits()) * len;
}

// internal reset method
bool EkonnectIoChannel::internalReset()
{
	if( 0 != m_io )
	{
		m_stat = s_drv.ftPurge(m_io, FtdiDriver::FT_PURGE_RX|FtdiDriver::FT_PURGE_TX);
		return checkLogStatus();
	}
	
	return false;
}

ES_IMPL_INTF_METHOD(void, EkonnectIoChannel::reset)()
{
	wxMutexLocker lock(m_mx); 
	
	internalReset();
	FtdiDriver::checkThrowStatus(m_stat);
}

ES_IMPL_INTF_METHOD(long, EkonnectIoChannel::getError)() const
{
	wxMutexLocker lock(m_mx); 
		
	return m_stat;
}

ES_IMPL_INTF_METHOD(void, EkonnectIoChannel::setEvtSink)(wxEvtHandler* sink)
{
	wxMutexLocker lock(m_mx); 
	
	m_evtSink = sink;
}

// applicable device filtering functor
struct isNotApplicableDevice
{
	isNotApplicableDevice()
	{}

	isNotApplicableDevice(const isNotApplicableDevice& src)
	{}	
	
	inline bool operator() (const FtdiDriver::FT_DEVICE_LIST_INFO_NODE& node) const
	{
		if( !m_regexSerial.IsValid() )
		{
			m_regexSerial.Compile( m_regexSerialPattern, wxRE_ADVANCED|wxRE_NOSUB );
			wxASSERT( m_regexSerial.IsValid() );
		}
		
		if( !m_regexDescr.IsValid() )
		{
			m_regexDescr.Compile( m_regexDescrPattern, wxRE_ADVANCED|wxRE_NOSUB );
			wxASSERT( m_regexDescr.IsValid() );
		}
		
		return FtdiDriver::FT_DEVICE_232R != node.getType() ||
			!m_regexSerial.Matches( node.getSerialNumberStr().c_str() ) ||
			!m_regexDescr.Matches( node.getDescriptionStr().c_str() );
	}
	
	static const wxChar* m_regexSerialPattern;
	mutable wxRegEx m_regexSerial;
	static const wxChar* m_regexDescrPattern;
	mutable wxRegEx m_regexDescr;
};

const wxChar* isNotApplicableDevice::m_regexSerialPattern = wxT("6000(0|1)-[0-9]{5}-[0-9]{2}");
const wxChar*	isNotApplicableDevice::m_regexDescrPattern = wxT("EKONNECT[0-9]?");

// get list of applicable io devices found
FtdiDriver::DeviceList EkonnectIoChannel::getDeviceList()
{
	FtdiDriver::DeviceList result;
	
	// silently catch all exceptions
	try
	{
		result = s_drv.getDeviceList();
	}
	catch(...)
	{
	}
	
	// filter only devices with our hardware id & serial mask
	FtdiDriver::DeviceList::iterator new_end = 
		std::remove_if(result.begin(), result.end(), isNotApplicableDevice() );
	result.erase(new_end, result.end());
		
	return result;
}

// rate change support
ES_IMPL_INTF_METHOD(bool, EkonnectIoChannel::isRateSupported)(long rate) const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud)->validate(rate);
}

ES_IMPL_INTF_METHOD(EsVariant, EkonnectIoChannel::getSupportedRates)() const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud)->getAllValues();
}

ES_IMPL_INTF_METHOD(long, EkonnectIoChannel::getDefaultRate)() const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud)->getDefault().asLong();
}

ES_IMPL_INTF_METHOD(long, EkonnectIoChannel::getRate)() const
{
	return get_baud();
}

ES_IMPL_INTF_METHOD(bool, EkonnectIoChannel::setRate)(long rate)
{
	wxMutexLocker lock(m_mx); 
	
	return internalSetBaud(rate, false);
}

// convert EkonnectIoChannel to comm channel interface 
//
BOOL EkonnectIoChannel::cLock(busHANDLE h, DWORD tmo)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.LockTimeout(tmo);
		wxASSERT( wxMUTEX_DEAD_LOCK != err );
		return wxMUTEX_NO_ERROR == err;
	}
	catch(...)
	{}
	
	return FALSE;
}

void EkonnectIoChannel::cUnlock(busHANDLE h)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.Unlock();
		wxASSERT(wxMUTEX_NO_ERROR == err);
	}
	catch(...)
	{}
}

BOOL EkonnectIoChannel::cIsConnected(busHANDLE h)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	return 0 != chnl->m_io;
}

BOOL EkonnectIoChannel::cConnect(busHANDLE h)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		return chnl->internalOpen();
	}
	catch(...)
	{}
	
	return FALSE;
}

void EkonnectIoChannel::cDisconnect(busHANDLE h)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		chnl->internalClose();
	}
	catch(...)
	{}
}	

DWORD EkonnectIoChannel::cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;	
	try
	{
		return chnl->internalPutBytes(data, count, 1000, brk);
	}
	catch(...)
	{}
	
	return 0;
}

DWORD EkonnectIoChannel::cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;	
	try
	{
		return chnl->internalGetBytes(data, count, timeout, brk);
	}
	catch(...)
	{}
	
	return 0;
}

void EkonnectIoChannel::cResetIo(busHANDLE h, DWORD WXUNUSED(timeout))
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		chnl->internalReset();
	}
	catch(...)
	{}
}

BOOL EkonnectIoChannel::cWaitTxEmpty(busHANDLE h, CommChannelBreak* brk)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		BOOL breaking = chnlIsBreaking( brk );
		DWORD rx, tx, evt; 
		FtdiDriver::FT_STATUS stat = s_drv.ftGetStatus(chnl->m_io, &rx, &tx, &evt); 

		while( !breaking &&
						FtdiDriver::FT_OK == stat &&
						0 != tx )
		{
			wxThread::Sleep(4);
			breaking = chnlIsBreaking( brk );
		}
		
		return !breaking && FtdiDriver::FT_OK == stat;
	}
	catch(...)
	{}
	
	return FALSE;
}

BOOL EkonnectIoChannel::cIsRateSupported(busHANDLE h, DWORD rate)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		return chnl->isRateSupported(rate);
	}
	catch(...)
	{}
	
	return FALSE;
}

DWORD EkonnectIoChannel::cGetRate(busHANDLE h)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		return chnl->m_baud;
	}
	catch(...)
	{}
	
	return 0;
}

BOOL EkonnectIoChannel::cSetRate(busHANDLE h, DWORD rate)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		return chnl->internalSetBaud(rate, false);
	}
	catch(...)
	{}
	
	return FALSE;
}	

DWORD EkonnectIoChannel::cSendEstimateGet(busHANDLE h, DWORD len)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		return (len * 2000 * (chnl->m_bits + 
			((FtdiDriver::FT_STOP_BITS_1 == chnl->m_stopBits) ? 1 : 2) + 
			((FtdiDriver::FT_PARITY_NONE == chnl->m_parity) ? 0 : 1) + 1)) / chnl->m_baud;
	}
	catch(...)
	{}
	
	return 0;
}

int EkonnectIoChannel::cGetError(busHANDLE h)
{
	EkonnectIoChannel* chnl = (EkonnectIoChannel*)h;
	try
	{
		return static_cast<int>(chnl->m_stat);
	}
	catch(...)
	{}
	
	return 0;
}		

// initialize C CommChannel from existing EsIoChannelIntf
ES_IMPL_INTF_METHOD(void, EkonnectIoChannel::initCommChannel)(CommChannel* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_UART;
	chnl->lock = EkonnectIoChannel::cLock;
	chnl->unlock = EkonnectIoChannel::cUnlock;
	chnl->isConnected = EkonnectIoChannel::cIsConnected;
	chnl->connect = EkonnectIoChannel::cConnect;
	chnl->disconnect = EkonnectIoChannel::cDisconnect;
	// set rate support
	chnl->isRateSupported = EkonnectIoChannel::cIsRateSupported;
	chnl->getRate = EkonnectIoChannel::cGetRate;
	chnl->setRate = EkonnectIoChannel::cSetRate;	
	chnl->sendTimeEstimateGet = EkonnectIoChannel::cSendEstimateGet;
	// data io
	chnl->putBytes = EkonnectIoChannel::cPutBytes;
	chnl->getBytes = EkonnectIoChannel::cGetBytes;
	chnl->resetIo = EkonnectIoChannel::cResetIo;
	chnl->waitTxEmpty = EkonnectIoChannel::cWaitTxEmpty;
	// misc 
	chnl->getError = EkonnectIoChannel::cGetError;
}

// IoBootCtlChannelIntf interface
// 
// (re)boot device in normal operations node (as configured by channel properties)
ES_IMPL_INTF_METHOD(bool, EkonnectIoChannel::bootNormalMode)()
{
	bool result = false;
	wxMutexLocker lock(m_mxBoot);
	m_stat = FtdiDriver::FT_OK;
	if( interlockedSetCbusBits(m_useStandardSocket, false, true) )
	{
		wxThread::Sleep(2000);
		if( interlockedSetCbusBits( m_useStandardSocket, false, m_programmingMode ) )
		{
			wxThread::Sleep(10);
			result = interlockedSetCbusBits(m_useStandardSocket, m_supplyAuxPower, m_programmingMode );
		}
	}
	FtdiDriver::checkThrowStatus(m_stat);
	return result;
}

// (re)boot device in programming mode
ES_IMPL_INTF_METHOD(bool, EkonnectIoChannel::bootProgrammingMode)(EsThread::Worker* thread /*= 0*/)
{
	bool result = false;
	wxMutexLocker lock(m_mxBoot);
	m_stat = FtdiDriver::FT_OK;	
	if( interlockedSetCbusBits(m_useStandardSocket, false, true) )
	{
		long delay = m_rebootDelay;
		while( delay > 0 )
		{
			if( !thread )
				wxThread::Sleep(1000);
			else if( thread->checkForStop(0) )
			{
				if( m_evtSink )
					EsProgressMonitor::notifyPulse(m_evtSink, 0, _("Cancelling boot mode...") );
				
				interlockedSetCbusBits(m_useStandardSocket, true, true);
				goto __exit;			
			}
			else
				thread->Sleep(1000);
				
			if( m_evtSink )
				EsProgressMonitor::notifyPulse(m_evtSink, 0, EsString::format(_("Entering boot mode, %d s remaining..."), delay/1000) );
				
			delay -= 1000;
		}
		
		result = interlockedSetCbusBits(m_useStandardSocket, true, true);
	}

__exit:	
	FtdiDriver::checkThrowStatus(m_stat);
	return result;
}

bool EkonnectIoChannel::bootProgrammingModeBlocking()
{
	return bootProgrammingMode();
}
