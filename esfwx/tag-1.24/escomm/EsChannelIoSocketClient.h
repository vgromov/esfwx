#ifndef _socket_client_io_channel_h_
#define _socket_client_io_channel_h_

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET

// client endpoint of the socket io channel
//
class ESCOMM_CLASS ES_INTF_IMPL2(EsChannelIoSocketClient, EsChannelIoIntf, EsReflectedClassIntf)

protected:
  class Client : public EsSocketClient
  {
  public:
    Client(EsChannelIoSocketClient& owner);

  protected:
    /// Overridable callbacks implementation
    virtual void onSendFarewell(EsSocket::Impl& io);
    virtual void onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len);

  protected:
    EsChannelIoSocketClient& m_owner;
  };
  friend class Client;

protected:
	EsChannelIoSocketClient();

public:
	virtual ~EsChannelIoSocketClient();

	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoSocketClient )
	ES_DECL_ATTR_HANDLING_STD
	
	// socket channel configuraiton properties
	//
	ES_DECL_PROPERTY_STD(target, EsString)
	ES_DECL_PROPERTY_STD(targetPort, long)
	ES_DECL_PROPERTY_STD(operationTimeout, long)
	ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)
	ES_DECL_PROPERTY_WO(breaker, EsBaseIntfPtr)

	// class services
	//
	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

	// EsChannelIoIntf interface
	//
	// channel unique idGet string
	ES_DECL_INTF_METHOD(EsString, idGet)() const;
	// monitor interface access
	ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const;
	ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor);	
	// state manipulation
	ES_DECL_REFLECTED_INTF_METHOD0(bool, open);
	ES_DECL_REFLECTED_INTF_METHOD0(void, close);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen);
	// io breaker access
	ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, breakerGet)() const;
	ES_DECL_INTF_METHOD(void, breakerSet)(const EsBaseIntf::Ptr& breaker);
	// byte io
	ES_DECL_INTF_METHOD(esU32, bytesPut)(const esU8* data, esU32 len, esU32 tmo);
	ES_DECL_INTF_METHOD(esU32, bytesGet)(esU8* data, esU32 len, esU32 tmo);
	ES_DECL_REFLECTED_INTF_METHOD2(ulong, bytesPut, cr_EsBinBuffer, ulong);
	ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, bytesGet, ulong, ulong);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, txTimeEstimateGet, ulong);
	// port reset & error info access
	ES_DECL_REFLECTED_INTF_METHOD0(void, reset);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, errorGet);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, errorStringGet);	
	// initialize C EseChannelIo from existing EsChannelIoIntf
	ES_DECL_INTF_METHOD(void, commChannelInit)(EseChannelIo* chnl);
	
protected:
	// internal services
	EsString internalIdGet() const;
	bool internalIsBreaking() const;

	// wrappers for c channel interfacing
	static esBL cIsBreaking(EseChannelIo* p);
	static esBL cLock(EseChannelIo* p, esU32 tmo);
	static void cUnlock(EseChannelIo* p);
	static esBL cIsConnected(EseChannelIo* p);
	static esBL cConnect(EseChannelIo* p);
	static void cDisconnect(EseChannelIo* p);
	static esU32 cPutBytes(EseChannelIo* p, const esU8* data, esU32 count);
	static esU32 cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout);
	static void cResetIo(EseChannelIo* p);
	static int cGetError(EseChannelIo* p);

protected:
	// data members
	//
	// multithreading guard
	mutable EsMutex m_mx;
	Client m_cli;
	EsBaseIntf::Ptr m_breaker;
	EsBaseIntf::Ptr m_monitor;
  EsSocketAddr m_addr;
};

#endif // ES_COMM_USE_CHANNEL_IO_SOCKET

#endif // _socket_client_io_channel_h_
