#ifndef _socket_server_io_channel_h_
#define _socket_server_io_channel_h_

// server endpoint of the socket io channel
//
class EKOSF_COMM_CLASS SocketServerIoChannel :	public EsBaseIntf,
																								public EsReflectedClassIntf,
																								public EsIoChannelIntf
{
protected:
	SocketServerIoChannel(wxEvtHandler* sink = 0);

public:
	// reflection declaration
	ES_DECL_REFLECTED_CLASS_BASE(SocketServerIoChannel)

	// interface support map
	//
	ES_INTF_MAP_BEGIN( SocketServerIoChannel )
		ES_INTF_SUPPORTS( SocketServerIoChannel, EsReflectedClassIntf )
		ES_INTF_SUPPORTS( SocketServerIoChannel, EsIoChannelIntf )
	ES_INTF_MAP_END
	{
		// destroy implementation
		delete this;
	}

	// socket channel configuraiton properties
	//
	ES_DECL_PROPERTY_STD(port, long)
	ES_DECL_PROPERTY_STD(operationTimeout, long)	

	// class services
	//
	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr) 
	{ 
		SocketServerIoChannel* chnl = new SocketServerIoChannel(); 
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
	// internal types
	//
	// incoming client connection listener
	class EKOSF_COMM_CLASS Listener : public EsThread
	{
	public:
		Listener(SocketServerIoChannel& chnl);

	protected:
		virtual long worker(EsThread::Worker& thread);
		virtual void onEnterWorker();
		virtual void onExitWorker();
		
		// internal socket state checkers
		void internalCheckAcceptClient();
		void internalCheckLostClient();

	protected:
		SocketServerIoChannel& m_owner;
		// server socket
		wxSocketServer* m_srv;
		// connected client. this channel only supports one client
		SocketIoAdapter m_adapter;
		// socket MT guard
		mutable wxMutex m_mxSocket;

		friend class SocketServerIoChannel;
	};
	
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
	// MT guard
	mutable wxMutex m_mx;
	// io event sink
	wxEvtHandler* m_sink;
	// incoming connection listener
	Listener m_listener;
};

#endif // _socket_server_io_channel_h_
