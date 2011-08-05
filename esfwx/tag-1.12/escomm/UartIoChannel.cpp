#include "stdafx.h"
#pragma hdrstop

#include "UartIoChannel.h"
#include "UartIoChannelConfigPane.h"

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(UartIoChannel, _i("RS232 direct connection channel"))
	// reflected property infos declarations
	ES_DECL_PROP_INFO(UartIoChannel, port, EsString, _i("Port name"), wxT("COM1"))
	ES_DECL_PROP_INFO_ENUM_BEGIN(UartIoChannel, baud, long, _i("Baud rate"), 9600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(2400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(4800)
		ES_DECL_PROP_INFO_ENUM_ENTRY(9600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(14400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(19200)
		ES_DECL_PROP_INFO_ENUM_ENTRY(38400)
		ES_DECL_PROP_INFO_ENUM_ENTRY(57600)
		ES_DECL_PROP_INFO_ENUM_ENTRY(115200)
		ES_DECL_PROP_INFO_ENUM_ENTRY(128000)
		ES_DECL_PROP_INFO_ENUM_ENTRY(256000)
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_ENUM_BEGIN(UartIoChannel, bits, long, _i("Byte size"), 8)
		ES_DECL_PROP_INFO_ENUM_ENTRY(4)
		ES_DECL_PROP_INFO_ENUM_ENTRY(5)
		ES_DECL_PROP_INFO_ENUM_ENTRY(6)
		ES_DECL_PROP_INFO_ENUM_ENTRY(7)
		ES_DECL_PROP_INFO_ENUM_ENTRY(8)
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_ENUM_BEGIN(UartIoChannel, parity, long, _i("Parity"), 0)
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(0, _i("None"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(1, _i("Odd"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(2, _i("Even"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(3, _i("Mark"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(4, _i("Space"))
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_ENUM_BEGIN(UartIoChannel, stopBits, long, _i("Stop bits"), 0)
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(0, _i("1 Stop Bit"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(1, _i("1.5 Stop Bits"))
		ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY(2, _i("2 Stop Bits"))
	ES_DECL_PROP_INFO_ENUM_END
	ES_DECL_PROP_INFO_RANGE(UartIoChannel, rxBuffLen, long, _i("RX buffer size"), 8192, 512, 65536)		
	ES_DECL_PROP_INFO_RANGE(UartIoChannel, txBuffLen, long, _i("TX buffer size"), 8192, 512, 65536)
	ES_DECL_PROP_INFO(UartIoChannel, resetOnRxTmo, bool, _i("Automatically reset channel on RX timeout"), false)
	// reflected interface methods
	//
	// EsReflectedClassIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsReflectedClassIntf, setConfigString, void_Call_cr_EsString_bool, _i("Set uart channel configuration string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsReflectedClassIntf, getConfigString, EsString_CallConst, _i("Get uart channel configuration string"))
	// EsIoChannelIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, open, bool_Call, _i("Open uart channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, close, void_Call, _i("Close uart channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, isOpen, bool_CallConst, _i("Return true if uart channel is open"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, putBytes, ulong_Call_cr_EsBinBuffer_ulong_EsBaseIntfPtr, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, getBytes, EsBinBuffer_Call_ulong_ulong_EsBaseIntfPtr, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, getTxTimeEstimate, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, reset, void_Call, _i("Reset channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsIoChannelIntf, getError, long_CallConst, _i("Get channel-specific error code"))
	// EsRateCtlIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsRateCtlIntf, isRateSupported, bool_CallConst_long, _i("Return true if requested channel rate is supported"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsRateCtlIntf, getSupportedRates, EsVariant_CallConst, _i("Return variant array of supported rates"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsRateCtlIntf, getDefaultRate, long_CallConst, _i("Return default channel rate value"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsRateCtlIntf, getRate, long_CallConst, _i("Return currently set channel rate"))	
	ES_DECL_REFLECTED_INTF_METHOD_INFO(UartIoChannel, EsRateCtlIntf, setRate, bool_Call_long, _i("Try to set channel rate"))	
	// reflected class methods
	ES_DECL_REFLECTED_CTOR_INFO(UartIoChannel, EsBaseIntfPtr_ClassCall, _i("UartIoChannel constructor"))
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(UartIoChannel, getMaxByteTmo, ulong_ClassCall_ulong_ulong_bool_ulong, _i("Calculate per-byte timeout estimate in ms"));
ES_DECL_CLASS_INFO_END

// -------------------------------------------------------------------------------------------------------
// communication class
//
UartIoChannel::UartIoChannel(wxEvtHandler* sink) :
m_sysError(0),
m_lineError(0),
m_com(0),
m_evtSink(0)
{
	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);

	setEvtSink(sink);
}

UartIoChannel::~UartIoChannel()
{
	close();
	setEvtSink(0);
}

// properties
EsString UartIoChannel::get_port() const
{
	wxMutexLocker lock(m_mx);
	return m_port;
}

void UartIoChannel::set_port(const EsString& val)
{
	wxMutexLocker lock(m_mx);
	if( m_port != val )
	{
		if( 0 != m_com )
			EsException::Throw(0, EsException::Generic, _("Cannot change port while channel is active."));
		m_port = val;
	}
}

long UartIoChannel::get_baud() const
{
	wxMutexLocker lock(m_mx);
	return m_baud;
}

// internal baud setter service 
bool UartIoChannel::internalSetBaud(const long& val, bool doThrow)
{
	if( ES_THIS_REFLECTED_PROP_INFO_GET(baud)->validate(val, doThrow) )
	{
		if( m_baud != val )
		{
			if( 0 != m_com )
			{
				if( doThrow )
					EsException::Throw(0, EsException::Generic, _("Cannot change baud rate while channel is active."));
				else
					return false;
			}

			m_baud = val;
		}
		return true;		
	}
	
	return false;
}

void UartIoChannel::set_baud(const long& val)
{
	wxMutexLocker lock(m_mx);
	internalSetBaud(val, true);
}

long UartIoChannel::get_bits() const
{
	wxMutexLocker lock(m_mx);
	return m_bits;
}

void UartIoChannel::set_bits(const long& val)
{
	wxMutexLocker lock(m_mx);
	if( m_bits != val && ES_THIS_REFLECTED_PROP_INFO_GET(bits)->validate(val, true) )
	{
		if( 0 != m_com )
			EsException::Throw(0, EsException::Generic, _("Cannot change data bit length while channel is active."));
		m_bits = val;
	}
}

long UartIoChannel::get_parity() const
{
	wxMutexLocker lock(m_mx);
	return m_parity;
}

void UartIoChannel::set_parity(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_parity != val && ES_THIS_REFLECTED_PROP_INFO_GET(parity)->validate(val, true) )
	{
		if( 0 != m_com )
			EsException::Throw(0, EsException::Generic, _("Cannot change parity while channel is active."));
		m_parity = val;
	}
}

long UartIoChannel::get_stopBits() const
{
	wxMutexLocker lock(m_mx);
	return m_stopBits;
}

void UartIoChannel::set_stopBits(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_stopBits != val && ES_THIS_REFLECTED_PROP_INFO_GET(stopBits)->validate(val, true) )
	{
		if( 0 != m_com )
			EsException::Throw(0, EsException::Generic, _("Cannot change stop bits while channel is active."));
		m_stopBits = val;
	}
}

long UartIoChannel::get_rxBuffLen() const
{
	wxMutexLocker lock(m_mx);
	return m_rxBuffLen;
}

void UartIoChannel::set_rxBuffLen(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_rxBuffLen != val && ES_THIS_REFLECTED_PROP_INFO_GET(rxBuffLen)->validate(val, true) )
	{
		if( 0 != m_com )
			EsException::Throw(0, EsException::Generic, _("Cannot change RX buffer length while channel is active."));
		m_rxBuffLen = val;
	}
}

long UartIoChannel::get_txBuffLen() const
{
	wxMutexLocker lock(m_mx);
	return m_txBuffLen;
}

void UartIoChannel::set_txBuffLen(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_txBuffLen != val && ES_THIS_REFLECTED_PROP_INFO_GET(txBuffLen)->validate(val, true) )
	{
		if( 0 != m_com )
			EsException::Throw(0, EsException::Generic, _("Cannot change TX buffer length while channel is active."));
		m_txBuffLen = val;
	}
}

// autoreset on RX timeout
bool UartIoChannel::get_resetOnRxTmo() const 
{ 
	wxMutexLocker lock(m_mx); 
	return m_resetOnRxTmo; 
}

void UartIoChannel::set_resetOnRxTmo(const bool& val) 
{ 
	wxMutexLocker lock(m_mx); 
	m_resetOnRxTmo = val; 
}

// decode comm line error into string
EsString UartIoChannel::decodeLineError(DWORD err)
{
	EsString result;

	if( err & CE_MODE )
		result += _("The requested mode is not supported, or the file handle is invalid.");
	if( err & CE_BREAK )
	{
		if( !result.empty() )
				result += wxT('\n');
			result += _("The hardware detected a break condition.");
	}
	if( err & CE_FRAME )
	{
		if( !result.empty() )
			result += wxT('\n');
		result += _("The hardware detected a framing error.");	
	}
	if( err & CE_IOE )
	{
		if( !result.empty() )
			result += wxT('\n');
		result += _("An I/O error occurred during communications with the device.");
	}
	if( err & CE_OVERRUN )
	{
		if( !result.empty() )
			result += wxT('\n');
		result += _("A character-buffer overrun has occurred. The next character is lost.");
	}
	if( err & CE_RXOVER )
	{
		if( !result.empty() )
			result += wxT('\n');
		result += _("An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.");
	}
	if( err & CE_RXPARITY )
	{
		if( !result.empty() )
			result += wxT('\n');
		result += _("The hardware detected a parity error.");
	}
	if( err & CE_TXFULL )
	{
		if( !result.empty() )
			result += wxT('\n');
		result += _("The application tried to transmit a character, but the output buffer was full.");
	}	

	return result;
}

// log recently occured errors
bool UartIoChannel::checkLogLastError() const
{
	EsString err;
	if( m_sysError )
		err += wxSysErrorMsg(m_sysError);
	if( m_lineError )
	{
		if( !err.empty() )
			err += wxT('\n');
		err += decodeLineError(m_lineError);
	}

	if( !err.empty() && m_evtSink )
	{
		wxQueueEvent( m_evtSink, new IoChannelEvent(IoChannelEvent::TypeError, err)	);

		return true;
	}

	return false;
}

// rough max estimate of ms per 1.5 bytes at specified baud
ulong UartIoChannel::getMaxByteTmo(DWORD baud, DWORD bits, bool parity, DWORD stop)
{
	ulong result = (1500 * (bits + stop + 1 + (parity ? 0 : 1))) / baud;

	return result ? result : 1;
}

void UartIoChannel::save(wxConfigBase& storage) const
{
	// save channel settings
	storage.Write( getClassInfo().getName(), getConfigString().c_str() );
}

void UartIoChannel::load(wxConfigBase& storage)
{
	// save current storage path
	wxString configString;
	storage.Read( getClassInfo().getName(), &configString, wxEmptyString );
	setConfigString( configString, true );
}

// reset error & copmm state
void UartIoChannel::resetError()
{
	if( 0 != m_com )
	{
		ClearCommError(m_com, &m_lineError, 0);
		m_sysError = m_lineError = 0;
	}
}

// open port using current configuration
bool UartIoChannel::internalOpen()
{
	bool result = false;
	m_sysError = m_lineError = 0;
	m_com = CreateFile( m_port.c_str(), 
		GENERIC_READ|GENERIC_WRITE|SYNCHRONIZE,
		0, // exclusive - do not share
		0, // do not inherit handle
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL/* | FILE_FLAG_OVERLAPPED*/,
		0);
	
	if( m_com == INVALID_HANDLE_VALUE )
		m_sysError = wxSysErrorCode();
	else	
	{
		DCB dcb;
		memset(&dcb, 0, sizeof(DCB));
		dcb.DCBlength = sizeof(DCB);
		dcb.BaudRate = m_baud;
		dcb.fBinary = TRUE;
		dcb.fParity = m_parity != 0; 
		dcb.ByteSize = m_bits;
		dcb.Parity = m_parity;  
		dcb.StopBits = m_stopBits;

		resetError();

		if( SetCommState(m_com, &dcb) )
		{
			if( SetupComm(m_com, m_rxBuffLen, m_txBuffLen ) )
			{
				COMMTIMEOUTS tms;
				if( GetCommTimeouts(m_com, &tms) )
				{
					tms.ReadTotalTimeoutConstant = 0;
					tms.ReadTotalTimeoutMultiplier = 0;
					tms.ReadIntervalTimeout = MAXDWORD;

					if( SetCommTimeouts(m_com, &tms) && EscapeCommFunction(m_com, SETDTR) )
					{
						result = true;
						internalReset();
						if( m_evtSink )
							wxQueueEvent( m_evtSink,
								new IoChannelEvent(IoChannelEvent::TypeOpen, 
									EsString::format(_("Port '%s' opened. Configured for %d baud, %d data bits, %s, %s."),
										m_port.c_str(), m_baud, m_bits, 
										ES_THIS_REFLECTED_PROP_INFO_GET(parity)->getValueLabel(m_parity).c_str(),	
										ES_THIS_REFLECTED_PROP_INFO_GET(stopBits)->getValueLabel(m_stopBits).c_str()) )
							);
					}
					else
						m_sysError = wxSysErrorCode();
				}
				else
					m_sysError = wxSysErrorCode();
			}
			else
				m_sysError = wxSysErrorCode();
		}
		else
			m_sysError = wxSysErrorCode();
	}

	if( m_sysError )
	{
		CloseHandle(m_com);
		m_com = 0;

		checkLogLastError();
	}

	return result;
}

void UartIoChannel::internalClose()
{
	if( 0 != m_com )
	{
		internalReset();
		EscapeCommFunction(m_com, CLRDTR);
		CloseHandle(m_com);
		m_com = 0;

		if( m_evtSink )
			wxQueueEvent( m_evtSink,
				new IoChannelEvent(IoChannelEvent::TypeClose, 
					EsString::format(_("Port '%s' closed."), 
					m_port.c_str()) )
			);
	}
}

// port manipulation
ES_IMPL_INTF_METHOD(bool, UartIoChannel::open)()
{
	wxMutexLocker lock(m_mx);
	if( 0 != m_com )
		return true;
	else
		return internalOpen();
}

ES_IMPL_INTF_METHOD(void, UartIoChannel::close)()
{
	wxMutexLocker lock(m_mx);
	internalClose();
}

ES_IMPL_INTF_METHOD(bool, UartIoChannel::isOpen)() const
{
	wxMutexLocker lock(m_mx);
	return 0 != m_com;
}

// byte io
//
// internal byte io
DWORD UartIoChannel::internalPutBytes(const BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk)
{
	bool timeout = false;
	const BYTE* pos = data;
	const BYTE* end = data+len;

	if( m_com != 0 )
	{
		resetError();
		while( !timeout && 
					!checkLogLastError() && 
					!chnlIsBreaking(brk) && 
					pos < end )
		{
			DWORD result = 0;
			// wxThread::Yield();
			if( !WriteFile(m_com, data, len, &result, 0) )
				m_sysError = wxSysErrorCode();

			ClearCommError(m_com, &m_lineError, 0);
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

DWORD UartIoChannel::internalGetBytes(BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk)
{
	bool timeout = false;
	DWORD localTmo = tmo;
	BYTE* pos = data;
	BYTE* end = data+len;

	if( 0 != m_com && len )
	{
		resetError();
		while( !timeout && 
					!checkLogLastError() && 
					pos < end &&
					!chnlIsBreaking(brk) )
		{
			DWORD toRead = std::min(queryUnreadBytes(), static_cast<DWORD>(end-pos));
			if( toRead )
			{
				localTmo = tmo; // reset local timeout
				DWORD result = 0;
				m_sysError = 0;
				
				if( !ReadFile(m_com, pos, toRead, &result, 0) )
					m_sysError = wxSysErrorCode();

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
					{
						CancelIo(m_com);
						PurgeComm(m_com, PURGE_RXABORT|PURGE_RXCLEAR);
					}
				}
			}
		}
	}

	return pos-data;
}

ES_IMPL_INTF_METHOD(DWORD, UartIoChannel::putBytes)(const BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker)
{
	wxMutexLocker lock(m_mx);
	EsCommChannelBreakImpl brk(breaker);
	return internalPutBytes(data, len, tmo, brk.getCommChannelBreakPtr());
}

// query unread bytes
DWORD UartIoChannel::queryUnreadBytes()
{
	COMSTAT stat;
	memset(&stat, 0, sizeof(COMSTAT));

	if( !ClearCommError(m_com, &m_lineError, &stat) )
		m_sysError = wxSysErrorCode();

	return stat.cbInQue;
}

ES_IMPL_INTF_METHOD(DWORD, UartIoChannel::getBytes)(BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker)
{
	wxMutexLocker lock(m_mx);
	EsCommChannelBreakImpl brk(breaker);
	return internalGetBytes(data, len, tmo, brk.getCommChannelBreakPtr());
}

ES_IMPL_INTF_METHOD(DWORD, UartIoChannel::putBytes)(cr_EsBinBuffer data, ulong tmo, EsBaseIntfPtr breaker)
{
	if( !data.empty() )
		return putBytes(&data[0], data.size(), tmo, breaker);
	
	return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, UartIoChannel::getBytes)(ulong len, ulong tmo, EsBaseIntfPtr breaker)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = getBytes(&result[0], len, tmo, breaker);
		result.erase(result.begin()+len, result.end());
	}
	
	return result;
}

ES_IMPL_INTF_METHOD(ulong, UartIoChannel::getTxTimeEstimate)(ulong len) const
{
	return UartIoChannel::getMaxByteTmo(
		get_baud(), get_bits(), 0 != get_parity(), get_stopBits()) * len;
}

// internal reset method
void UartIoChannel::internalReset()
{
	if( 0 != m_com )
	{
		CancelIo(m_com);
		PurgeComm(m_com, PURGE_RXABORT|PURGE_RXCLEAR|
											PURGE_TXABORT|PURGE_TXCLEAR);
		resetError();
	}
}

ES_IMPL_INTF_METHOD(void, UartIoChannel::reset)()
{
	wxMutexLocker lock(m_mx);
	internalReset();
}

ES_IMPL_INTF_METHOD(long, UartIoChannel::getError)() const
{
	wxMutexLocker lock(m_mx);	
	return m_lineError;
}

ES_IMPL_INTF_METHOD(void, UartIoChannel::setEvtSink)(wxEvtHandler* sink)
{
	wxMutexLocker lock(m_mx);
	m_evtSink = sink;
}

// rate change support
ES_IMPL_INTF_METHOD(bool, UartIoChannel::isRateSupported)(long rate) const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud)->validate(rate);
}

ES_IMPL_INTF_METHOD(EsVariant, UartIoChannel::getSupportedRates)() const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud)->getAllValues();
}

ES_IMPL_INTF_METHOD(long, UartIoChannel::getDefaultRate)() const
{
	return ES_THIS_REFLECTED_PROP_INFO_GET(baud)->getDefault().asLong();
}

ES_IMPL_INTF_METHOD(long, UartIoChannel::getRate)() const
{
	return get_baud();
}

ES_IMPL_INTF_METHOD(bool, UartIoChannel::setRate)(long rate)
{
	bool result = false;
	wxMutexLocker lock(m_mx);
	if( m_baud != rate )
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

// convert UartIoChannel to comm channel interface 
//
BOOL UartIoChannel::cLock(busHANDLE h, DWORD tmo)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
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

void UartIoChannel::cUnlock(busHANDLE h)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.Unlock();
		wxASSERT(wxMUTEX_NO_ERROR == err);
	}
	catch(...)
	{}
}

BOOL UartIoChannel::cIsConnected(busHANDLE h)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	return 0 != chnl->m_com;
}

BOOL UartIoChannel::cConnect(busHANDLE h)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		return chnl->internalOpen();
	}
	catch(...)
	{}
	
	return FALSE;
}

void UartIoChannel::cDisconnect(busHANDLE h)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		chnl->internalClose();
	}
	catch(...)
	{}
}	

DWORD UartIoChannel::cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk)
{
	UartIoChannel* chnl = (UartIoChannel*)h;	
	try
	{
		return chnl->internalPutBytes(data, count, 1000, brk);
	}
	catch(...)
	{}
	
	return 0;
}

DWORD UartIoChannel::cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk)
{
	UartIoChannel* chnl = (UartIoChannel*)h;	
	try
	{
		return chnl->internalGetBytes(data, count, timeout, brk);
	}
	catch(...)
	{}
	
	return 0;
}

void UartIoChannel::cResetIo(busHANDLE h, DWORD WXUNUSED(timeout))
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		chnl->internalReset();
	}
	catch(...)
	{}
}

BOOL UartIoChannel::cIsRateSupported(busHANDLE h, DWORD rate)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		return chnl->isRateSupported(rate);
	}
	catch(...)
	{}
	
	return FALSE;
}

DWORD UartIoChannel::cGetRate(busHANDLE h)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		return chnl->m_baud;
	}
	catch(...)
	{}
	
	return 0;
}

BOOL UartIoChannel::cSetRate(busHANDLE h, DWORD rate)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		return chnl->internalSetBaud(rate, false);
	}
	catch(...)
	{}
	
	return FALSE;
}	

DWORD UartIoChannel::cSendEstimateGet(busHANDLE h, DWORD len)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
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

BOOL UartIoChannel::cWaitTxEmpty(busHANDLE h, CommChannelBreak* brk)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		BOOL breaking = chnlIsBreaking( brk );
		COMSTAT stat;
		BOOL ok = TRUE;
		memset(&stat, 0, sizeof(COMSTAT));

		while( !breaking &&
						ok &&
						0 != stat.cbOutQue )
		{
			wxThread::Sleep(4);
			ok = ClearCommError(chnl->m_com, &chnl->m_lineError, &stat);
			breaking = chnlIsBreaking( brk );
		}
		
		return !breaking && ok;
	}
	catch(...)
	{}
	
	return FALSE;
}

int UartIoChannel::cGetError(busHANDLE h)
{
	UartIoChannel* chnl = (UartIoChannel*)h;
	try
	{
		return static_cast<int>(chnl->m_lineError);
	}
	catch(...)
	{}
	
	return 0;
}	
	
// initialize C CommChannel from existing EsIoChannelIntf
ES_IMPL_INTF_METHOD(void, UartIoChannel::initCommChannel)(CommChannel* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_UART;
	chnl->lock = UartIoChannel::cLock;
	chnl->unlock = UartIoChannel::cUnlock;
	chnl->isConnected = UartIoChannel::cIsConnected;
	chnl->connect = UartIoChannel::cConnect;
	chnl->disconnect = UartIoChannel::cDisconnect;
	// set rate support
	chnl->isRateSupported = UartIoChannel::cIsRateSupported;
	chnl->getRate = UartIoChannel::cGetRate;
	chnl->setRate = UartIoChannel::cSetRate;
	chnl->sendTimeEstimateGet = UartIoChannel::cSendEstimateGet;
	// data io
	chnl->putBytes = UartIoChannel::cPutBytes;
	chnl->getBytes = UartIoChannel::cGetBytes;
	chnl->resetIo = UartIoChannel::cResetIo;
	chnl->waitTxEmpty = UartIoChannel::cWaitTxEmpty;
	// misc
	chnl->getError = UartIoChannel::cGetError;
}
