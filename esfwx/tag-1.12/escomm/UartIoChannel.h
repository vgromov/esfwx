#ifndef _uart_io_h_
#define _uart_io_h_

// simple uart io channel class
//
class EKOSF_COMM_CLASS UartIoChannel :	public EsBaseIntf,
																				public EsIoChannelIntf,
																				public EsRateCtlIntf,
																				public EsReflectedClassIntf
{
protected:
	// internal consts
	enum { 
		SLEEP_GRANULARITY = 1,
	};

protected:
	UartIoChannel(wxEvtHandler* sink = 0);

public:
	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( UartIoChannel )

	// interface support map
	//
	ES_INTF_MAP_BEGIN( UartIoChannel )
		ES_INTF_SUPPORTS( UartIoChannel, EsIoChannelIntf )
		ES_INTF_SUPPORTS( UartIoChannel, EsRateCtlIntf )
		ES_INTF_SUPPORTS( UartIoChannel, EsReflectedClassIntf )		
	ES_INTF_MAP_END
	{
		// destroy implementation
		delete this;
	}
	
	// comm port configuraiton properties
	//
	ES_DECL_PROPERTY_STD(port, EsString)
	ES_DECL_PROPERTY_STD(baud, long)
	ES_DECL_PROPERTY_STD(bits, long)
	ES_DECL_PROPERTY_STD(parity, long)
	ES_DECL_PROPERTY_STD(stopBits, long)
	ES_DECL_PROPERTY_STD(rxBuffLen, long)
	ES_DECL_PROPERTY_STD(txBuffLen, long)
	ES_DECL_PROPERTY_STD(resetOnRxTmo, bool)

	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr) 
	{ 
		UartIoChannel* chnl = new UartIoChannel(); 
		wxASSERT(chnl); 
		return EsBaseIntf::Ptr( static_cast<EsBaseIntf*>(chnl) );
	}
	// EsIoChannelIntf interface
	//
	// state manipulation
	ES_DECL_REFLECTED_INTF_METHOD0(bool, open);
	ES_DECL_REFLECTED_INTF_METHOD0(void, close);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen);
	// byte io
	ES_DECL_INTF_METHOD(DWORD, putBytes)(const BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker);
	ES_DECL_INTF_METHOD(DWORD, getBytes)(BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker);
	ES_DECL_REFLECTED_INTF_METHOD3(ulong, putBytes, cr_EsBinBuffer, ulong, EsBaseIntfPtr);
	ES_DECL_REFLECTED_INTF_METHOD3(EsBinBuffer, getBytes, ulong, ulong, EsBaseIntfPtr);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, getTxTimeEstimate, ulong);
	// port reset & error info access
	ES_DECL_REFLECTED_INTF_METHOD0(void, reset);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, getError);
	// event sink access
	ES_DECL_INTF_METHOD(void, setEvtSink)(wxEvtHandler* sink);
	// initialize C CommChannel from existing EsIoChannelIntf
	ES_DECL_INTF_METHOD(void, initCommChannel)(CommChannel* chnl);
	// EsRateCtlIntf interface
	//
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, isRateSupported, long);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, getSupportedRates);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, getDefaultRate);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, getRate);
	ES_DECL_REFLECTED_INTF_METHOD1(bool, setRate, long);	
	// the rest of services (non-interfaced)
	//
	void save(wxConfigBase& storage) const;
	void load(wxConfigBase& storage);
	// static services
	//
	// return per-byte timeout in ms, calculated for baud, bits, bool parity, stop
	ES_DECL_REFLECTED_CLASS_METHOD4(ulong, getMaxByteTmo, ulong, ulong, bool, ulong);

protected:
	// internal helpers
	//
	// open port using current configuration
	bool internalOpen();
	// close port
	void internalClose();
	// reset error & comm state
	void resetError();
	// query unread bytes
	DWORD queryUnreadBytes();
	// check for & log last system|communication line error, if there were any
	bool checkLogLastError() const;
	// decode comm line error into string
	static EsString decodeLineError(DWORD err);
	// internal reset method
	void internalReset();
	// internal baud setter 
	bool internalSetBaud(const long& val, bool doThrow);
	// internal byte io
	DWORD internalPutBytes(const BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk);
	DWORD internalGetBytes(BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk);	
	
protected:
	// wrappers for c channel interface
	static BOOL cLock(busHANDLE h, DWORD tmo);
	static void cUnlock(busHANDLE h);
	static BOOL cIsConnected(busHANDLE h);
	static BOOL cConnect(busHANDLE h);
	static void cDisconnect(busHANDLE h);
	static DWORD cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk);
	static DWORD cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk);
	static void cResetIo(busHANDLE h, DWORD timeout);
	static BOOL cIsRateSupported(busHANDLE h, DWORD rate);
	static DWORD cGetRate(busHANDLE h);
	static BOOL cSetRate(busHANDLE h, DWORD rate);
	static DWORD cSendEstimateGet(busHANDLE h, DWORD len);
	static BOOL cWaitTxEmpty(busHANDLE h, CommChannelBreak* brk);
	static int cGetError(busHANDLE h);

protected:
	// data members
	//
	DWORD m_sysError;
	DWORD m_lineError;
	HANDLE m_com;
	wxEvtHandler* m_evtSink;
	// multithreading guard
	mutable wxMutex m_mx;
};

#endif // _uart_io_h_
