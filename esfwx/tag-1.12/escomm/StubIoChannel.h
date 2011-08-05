#ifndef _stub_io_channel_h_
#define _stub_io_channel_h_

// stub io channel class. does nothing
//
class EKOSF_COMM_CLASS StubIoChannel :	public EsBaseIntf,
																				public EsIoChannelIntf,
																				public EsReflectedClassIntf
{
protected:
	StubIoChannel(wxEvtHandler* sink = 0);

public:
	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( StubIoChannel )

	// interface support map
	//
	ES_INTF_MAP_BEGIN( StubIoChannel )
		ES_INTF_SUPPORTS( StubIoChannel, EsIoChannelIntf )
		ES_INTF_SUPPORTS( StubIoChannel, EsReflectedClassIntf )		
	ES_INTF_MAP_END
	{
		// destroy implementation
		delete this;
	}
	
	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr) 
	{ 
		StubIoChannel* chnl = new StubIoChannel(); 
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

protected:
	// internal services
	bool internalOpen();
	void internalClose();
	bool internalIsOpen() const;
	DWORD internalPutBytes(const BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk);
	DWORD internalGetBytes(BYTE* data, DWORD len, DWORD tmo, CommChannelBreak* brk);
	void internalReset();
	long internalGetError() const;

	// wrappers for c channel interface
	static BOOL cLock(busHANDLE h, DWORD tmo);
	static void cUnlock(busHANDLE h);
	static BOOL cIsConnected(busHANDLE h);
	static BOOL cConnect(busHANDLE h);
	static void cDisconnect(busHANDLE h);
	static DWORD cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk);
	static DWORD cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk);
	static void cResetIo(busHANDLE h, DWORD timeout);
	static BOOL cWaitTxEmpty(busHANDLE h, CommChannelBreak* brk);
	static int cGetError(busHANDLE h);

protected:
	// data members
	//
	wxEvtHandler* m_evtSink;
	bool m_open;
	// multithreading guard
	mutable wxMutex m_mx;
};

#endif // _stub_io_channel_h_
