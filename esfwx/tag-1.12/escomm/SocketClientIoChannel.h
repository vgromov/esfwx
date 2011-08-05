#ifndef _socket_client_io_channel_h_
#define _socket_client_io_channel_h_

// client endpoint of the socket io channel
//
class EKOSF_COMM_CLASS SocketClientIoChannel :	public EsBaseIntf,
																								public EsReflectedClassIntf,
																								public EsIoChannelIntf
{
protected:
	SocketClientIoChannel(wxEvtHandler* sink = 0);
	
public:
	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( SocketClientIoChannel )

	// interface support map
	//
	ES_INTF_MAP_BEGIN( SocketClientIoChannel )
		ES_INTF_SUPPORTS( SocketClientIoChannel, EsReflectedClassIntf )
		ES_INTF_SUPPORTS( SocketClientIoChannel, EsIoChannelIntf )		
	ES_INTF_MAP_END
	{
		// destroy implementation
		delete this;
	}
	
	// socket channel configuraiton properties
	//
	ES_DECL_PROPERTY_STD(target, EsString)
	ES_DECL_PROPERTY_STD(targetPort, long)
	ES_DECL_PROPERTY_STD(operationTimeout, long)
	// class services
	//
	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr) 
	{ 
		SocketClientIoChannel* chnl = new SocketClientIoChannel(); 
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
	
	// the rest of services (non-interfaced)
	//
	void save(wxConfigBase& storage) const;
	void load(wxConfigBase& storage);
	
protected:
	// wrappers for c channel interfacing
	static BOOL cLock(busHANDLE h, DWORD tmo);
	static void cUnlock(busHANDLE h);
	static BOOL cIsConnected(busHANDLE h);
	static BOOL cConnect(busHANDLE h);
	static void cDisconnect(busHANDLE h);
	static DWORD cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk);
	static DWORD cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk);
	static void cResetIo(busHANDLE h, DWORD timeout);
	static int cGetError(busHANDLE h);

protected:
	// data members
	//
	// communication event sink
	wxEvtHandler* m_evtSink;
	// multithreading guard
	mutable wxMutex m_mx;
	// socket io adapter wrapper
	SocketIoAdapter m_adapter;
};

#endif // _socket_client_io_channel_h_
