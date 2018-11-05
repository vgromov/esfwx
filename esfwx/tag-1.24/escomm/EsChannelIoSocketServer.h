#ifndef _socket_server_io_channel_h_
#define _socket_server_io_channel_h_

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET

// server endpoint of the socket io channel
//
class ESCOMM_CLASS ES_INTF_IMPL2(EsChannelIoSocketServer, EsChannelIoIntf, EsReflectedClassIntf)

protected:
  class ESCOMM_CLASS Server : public EsSocketServer
  {
  public:
    Server(EsChannelIoSocketServer& owner);

  protected:
    // Base class overrides
    virtual void onClientCannotBeAdded(const EsSocketAddr& addr) const;
    virtual void onClientAdding(const EsSocketAddr& addr);
    virtual void onClientAdded(const EsSocketAddr& addr);
    virtual void onClientRemoving(const EsSocketAddr& addr);
    virtual void onClientRemoved(const EsSocketAddr& addr);
    virtual void onClientDisconnected(const EsSocketAddr& addr);
    virtual void onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io);
    virtual void onClientClosed(const EsSocketAddr& addr);
    virtual void onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& response);

  protected:
    EsChannelIoSocketServer& m_owner;
  };
  friend class Server;

protected:
	EsChannelIoSocketServer();

public:
  virtual ~EsChannelIoSocketServer();

	// reflection declaration
	ES_DECL_REFLECTED_CLASS_BASE(EsChannelIoSocketServer)
	ES_DECL_ATTR_HANDLING_STD

	// socket channel configuraiton properties
	//
	ES_DECL_PROPERTY_STD(port, long)
	ES_DECL_PROPERTY(operationTimeout, long)
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

  // Server event adapters
  void onClientCannotBeAdded(const EsSocketAddr& addr) const;
  void onClientAdding(const EsSocketAddr& addr);
  void onClientAdded(const EsSocketAddr& addr);
  void onClientRemoving(const EsSocketAddr& addr);
  void onClientRemoved(const EsSocketAddr& addr);
  void onClientDisconnected(const EsSocketAddr& addr);
  void onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io);
  void onClientClosed(const EsSocketAddr& addr);
  void onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& response);

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
	// MT guard
	mutable EsMutex m_mx;

  Server m_srv;
  EsCriticalSection m_csin;
  EsBinBuffer m_in;
  EsSemaphore m_sin;
	EsBaseIntf::Ptr m_breaker;
	EsBaseIntf::Ptr m_monitor;
  volatile ulong m_expectedLen;
};

#endif // ES_COMM_USE_CHANNEL_IO_SOCKET

#endif // _socket_server_io_channel_h_
