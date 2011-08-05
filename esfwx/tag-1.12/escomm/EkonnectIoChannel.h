#ifndef _ekonnect_io_channel_h_
#define _ekonnect_io_channel_h_

// direct io channel via Ekonnect device (ftdi driver)
//
class EKOSF_COMM_CLASS EkonnectIoChannel :	public EsBaseIntf,
																						public EsReflectedClassIntf,
																						public EsIoChannelIntf,
																						public EsRateCtlIntf,
																						public BootCtlIntf
{
protected:
	// internal consts
	enum { 
		SLEEP_GRANULARITY = 1,
	};

protected:
	EkonnectIoChannel(wxEvtHandler* sink = 0);

public:
	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( EkonnectIoChannel )

	// interface support map
	//
	ES_INTF_MAP_BEGIN( EkonnectIoChannel )
		ES_INTF_SUPPORTS( EkonnectIoChannel, EsReflectedClassIntf )
		ES_INTF_SUPPORTS( EkonnectIoChannel, EsIoChannelIntf )
		ES_INTF_SUPPORTS( EkonnectIoChannel, EsRateCtlIntf )
		ES_INTF_SUPPORTS( EkonnectIoChannel, BootCtlIntf )
	ES_INTF_MAP_END
	{
		// destroy implementation
		delete this;
	}
	
	// ekobox configuraiton properties
	//
	ES_DECL_PROPERTY_STD(device, EsString)
	ES_DECL_PROPERTY_STD(baud, long)
	ES_DECL_PROPERTY_STD(bits, long)
	ES_DECL_PROPERTY_STD(parity, long)
	ES_DECL_PROPERTY_STD(stopBits, long)
	ES_DECL_PROPERTY_STD(rxTmo, long)
	ES_DECL_PROPERTY_STD(txTmo, long)
	ES_DECL_PROPERTY_STD(rxBuffLen, long)
	ES_DECL_PROPERTY_STD(txBuffLen, long)
	ES_DECL_PROPERTY_STD(resetOnRxTmo, bool)
	ES_DECL_PROPERTY_STD(useStandardSocket, bool)
	ES_DECL_PROPERTY_STD(supplyAuxPower, bool)
	ES_DECL_PROPERTY_STD(programmingMode, bool)
	ES_DECL_PROPERTY_STD(rebootDelay, long)
	// class services
	//
	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr) 
	{ 
		EkonnectIoChannel* chnl = new EkonnectIoChannel(); 
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
	// BootCtlIntf interface
	// 
	// (re)boot device in normal operations node (as configured by channel properties)
	ES_DECL_REFLECTED_INTF_METHOD0(bool, bootNormalMode);
	// (re)boot device in programming mode. may be executed in thread context, i.e. breakable
	ES_DECL_INTF_METHOD(bool, bootProgrammingMode)(EsThread::Worker* thread = 0);
	ES_DECL_REFLECTED_METHOD0(bool, bootProgrammingModeBlocking);
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
	// get list of applicable io devices found
	static FtdiDriver::DeviceList getDeviceList();

protected:
	// internal helpers
	//
	// open port using current configuration
	bool internalOpen();
	// close port
	void internalClose();
	// query unread bytes
	DWORD queryUnreadBytes();
	// check for & log ftdi device status from the last call
	bool checkLogStatus() const;
	// internal reset method
	bool internalReset();
	// internal CBUS pins setup
	bool setCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode);
	// internal CBUS pins setup, locked with m_cs
	bool interlockedSetCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode);
	// internal baud setter 
	bool internalSetBaud(const long& val, bool doThrow);
	// internal byte io
	DWORD internalPutBytes(const BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk);
	DWORD internalGetBytes(BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk);
	
protected:
	// wrappers for EkonnectIoChannel method calls from C code
	static BOOL cLock(busHANDLE h, DWORD tmo);
	static void cUnlock(busHANDLE h);
	static BOOL cIsConnected(busHANDLE h);
	static BOOL cConnect(busHANDLE h);
	static void cDisconnect(busHANDLE h);
	static DWORD cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk);
	static DWORD cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk);
	static void cResetIo(busHANDLE h, DWORD timeout);
	static BOOL cWaitTxEmpty(busHANDLE, CommChannelBreak* brk);
	static BOOL cIsRateSupported(busHANDLE h, DWORD rate);
	static DWORD cGetRate(busHANDLE h);
	static BOOL cSetRate(busHANDLE h, DWORD rate);
	static DWORD cSendEstimateGet(busHANDLE h, DWORD len);
	static int cGetError(busHANDLE h);

protected:
	// data members
	//
	static FtdiDriver s_drv;
	// status of the last FTDI entry call
	int m_stat;
	// open device handle
	FtdiDriver::FT_HANDLE m_io;
	// communication event sink
	wxEvtHandler* m_evtSink;
	// multithreading guards
	mutable wxMutex m_mx;
	mutable wxMutex m_mxBoot;
};

#endif // _ekonnect_io_channel_h_
