#ifndef _es_socket_server_h_
#define _es_socket_server_h_

#ifdef ES_COMM_USE_SOCKETS

/// Server socket base class
///
class ESCOMM_CLASS EsSocketServer : public EsSocket
{
public:
  enum {
    buffLenMin    = 1024,
    buffLenDef    = 4096,
    buffLenMax    = 65535,
    connCntDef    = 50,
    connCntMax    = 100,
    acceptTmoMin  = 500,
    acceptTmoDef  = 1000,
    acceptTmoMax  = 5000,
    ttlMin        = 5000,
    ttlDef        = 15000,
    ttlMax        = 300000,
    packetTmoMin  = 100,
    packetTmoDef  = 1000,
    packetTmoMax  = 10000
  };

protected:
	// Internal types
	//

	/// Incoming client connection listener
	class ESCOMM_CLASS Listener : public EsThread
	{
	public:
		Listener(EsSocketServer& owner);
		virtual ~Listener();

	protected:
		virtual long worker() ES_OVERRIDE;
		virtual void onExitWorker() ES_OVERRIDE;

    void connectionListen();
    void connectionlessListen();
    void disconnectedCleanup();

	protected:
		EsSocketServer& m_owner;
    EsBinBuffer m_buff;
    bool m_connOriented;

  private:
    Listener() ES_REMOVEDECL;
    Listener(const Listener&) ES_REMOVEDECL;
    Listener& operator=(const Listener&) ES_REMOVEDECL;

		friend class EsSocketServer;
	};
  friend class Listener;

  /// Client request handler
	class ESCOMM_CLASS Handler : public EsThread
	{
  public:
    enum {
      forcedSleep = 10
    };

    typedef std::shared_ptr<Handler> Ptr;

  public:
		Handler(EsSocketServer& owner, const EsSocketAddr& addr, EsSocketType type);
    Handler(EsSocketServer& owner, EsSocket::Impl* impl);
    virtual ~Handler();

    /// Check if hander is still alive
    bool aliveGet() const { return m_alive; }

    /// Assign output data with interlock
    void outDataSet(EsBinBuffer::const_pointer out, ulong len);

    /// Data received on listener socket (for Connection-less mode)
    void dataReceived(EsBinBuffer::const_pointer data, ulong len);

    /// Break aux io data wait lock
    void auxDataWaitBreak();

    /// Wait for incoming aux data
    bool auxDataWait(ulong ms);

	protected:
		virtual long worker() ES_OVERRIDE;
		virtual void onExitWorker() ES_OVERRIDE;

    bool response(EsBinBuffer& out, bool isAux);

    void ttlDecrement(ulong delta);
    void disconnectionDetect();
    void connectionHandle();
    void connectionlessHandle();

  protected:
		EsSocketServer& m_owner;
    EsSemaphore m_aux;
    EsSocketAddr m_addr;
#ifdef ES_MODERN_CPP
    std::unique_ptr<EsSocket::Impl> m_io;
#else
    std::auto_ptr<EsSocket::Impl> m_io;
#endif
    EsBinBuffer m_in;
    EsCriticalSection m_csout;
    EsBinBuffer m_out;
    volatile ulong m_ttl;
    volatile bool m_noFarewell;
    volatile bool m_alive;
    volatile bool m_disconnected;
    bool m_connOriented;

  private:
    Handler() ES_REMOVEDECL;
    Handler(const Handler&) ES_REMOVEDECL;
    Handler& operator=(const Handler&) ES_REMOVEDECL;

		friend class EsSocketServer;
	};

public:
  EsSocketServer();

  /// Server properties access
  ///
  ulong listenerBuffLenGet() const { return m_listenerBuffLen; }
  void listenerBuffLenSet(ulong len);

  ulong listenerMaxIncomingConnGet() const { return m_listenerMaxIncomingConn; }
  void listenerMaxIncomingConnSet(ulong cnt);

  ulong listenerAcceptTmoGet() const { return m_listenerAcceptTmo; }
  void listenerAcceptTmoSet(ulong tmo);

  ulong handlerBuffLenGet() const { return m_handlerBuffLen; }
  void handlerBuffLenSet(ulong len);

  ulong handlerTtlGet() const { return m_handlerTtl; }
  void handlerTtlSet(ulong ttl);

  ulong packetTmoGet() const { return m_packetTmo; }
  void packetTmoSet(ulong tmo);

  /// Return count of active clients
  ulong clientsCountGet() const;

  /// Return addresses of all active clients
  EsSocketAddresses clientAddressesGet() const;

  /// Signal disconnected flag for handler at specific address - schedule handler for closing
  void disconnectedSet(const EsSocketAddr& addr, bool noFarewell = false);

  /// Base class overrides.
  ///

  /// Server socket interface allows to send data either to all clients with send call,
  /// or to specific client, via sentTo call. sentTo addr parameter should match an active client
  ///

  using EsSocket::send;
  using EsSocket::sendTo;

  /// Send data to all active clients
  virtual ulong send(const esU8* data, ulong len, ulong tmo, bool doThrow) ES_OVERRIDE;

  /// Send data to particular client
  virtual ulong sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow) ES_OVERRIDE;

protected:
  /// Internal services
  ///
  void clientAdd(const Handler::Ptr& handler, const EsSocketAddr& addr);
  void clientRemove(const EsSocketAddr& addr);

  Handler::Ptr clientGet(const EsSocketAddr& addr, bool doThrow) const;
  Handler::Ptr handlerFind(const EsSocketAddr& addr) const;

  /// Overridable socket factory
  Handler::Ptr handlerCreate(const EsSocketAddr& addr, EsSocket::Impl* impl = nullptr);

  /// Overridable client management services
  virtual bool canAddClient(const EsSocketAddr& addr) const;
  virtual void onClientCannotBeAdded(const EsSocketAddr& addr) const = 0;
  virtual void onClientAdding(const EsSocketAddr& addr) = 0;
  virtual void onClientAdded(const EsSocketAddr& addr) = 0;
  virtual void onClientRemoving(const EsSocketAddr& addr) = 0;
  virtual void onClientRemoved(const EsSocketAddr& addr) = 0;
  virtual void onClientDisconnected(const EsSocketAddr& addr) = 0;
  virtual void onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io) = 0;
  virtual void onClientClosed(const EsSocketAddr& addr) = 0;
  virtual void onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& response) = 0;

  /// Return true if socket may be considered active
  virtual bool checkActive() const;
  /// Perform operations upon object opening
  virtual bool doOpen(bool doThrow);
  /// Perform operations to close object
  virtual void doClose(bool graceful, bool doThrow);

protected:
  Listener m_listener;
  std::map<EsSocketAddr, Handler::Ptr> m_clients;
  mutable EsCriticalSection m_cs;
  volatile ulong m_listenerBuffLen;
  volatile ulong m_listenerMaxIncomingConn;
  volatile ulong m_listenerAcceptTmo;
  volatile ulong m_handlerBuffLen;
  volatile ulong m_handlerTtl;
  volatile ulong m_packetTmo;

private:
  // Prohibited functionality
  //
  EsSocketServer(const EsSocketServer&) ES_REMOVEDECL;
  EsSocketServer& operator=(const EsSocketServer&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// Reflected Socket server implementation
///
namespace EsReflection {

typedef EsSocketServer EsSocketServerBase;

class ES_INTF_IMPL1(EsSocketServer, EsReflectedClassIntf)

protected:
  /// Actual server implementation
  ///
  class Server : public EsSocketServerBase
  {
  public:
    Server(EsReflection::EsSocketServer& owner);

  protected:
    virtual bool canAddClient(const EsSocketAddr& addr) const ES_OVERRIDE;
    virtual void onClientCannotBeAdded(const EsSocketAddr& addr) const ES_OVERRIDE;
    virtual void onClientAdding(const EsSocketAddr& addr) ES_OVERRIDE;
    virtual void onClientAdded(const EsSocketAddr& addr) ES_OVERRIDE;
    virtual void onClientRemoving(const EsSocketAddr& addr) ES_OVERRIDE;
    virtual void onClientRemoved(const EsSocketAddr& addr) ES_OVERRIDE;
    virtual void onClientDisconnected(const EsSocketAddr& addr) ES_OVERRIDE;
    virtual void onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io) ES_OVERRIDE;
    virtual void onClientClosed(const EsSocketAddr& addr) ES_OVERRIDE;
    virtual void onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& out) ES_OVERRIDE;

  protected:
    EsReflection::EsSocketServer& m_owner;
  };
  friend class Server;

public:
  EsSocketServer();

	ES_DECL_REFLECTED_CLASS_BASE( EsSocketServer )
	ES_DECL_ATTR_HANDLING_STD

  /// Reflected services
  ///

  /// Create server socket of specific kind
 	ES_DECL_REFLECTED_CLASS_CTOR1(EsVariant, ulong);

  /// Open-close
  ES_DECL_REFLECTED_METHOD0(bool, open);
  ES_DECL_REFLECTED_METHOD0(void, close);

  /// Send-receive
  ///

  /// Send data buffer to all clients
  /// @param[in]  data  EsBinaryBuffer to send (or variant, implicitly convertible to BB)
  /// @param[in]  tmo   ulong - timeout for send operation
  /// @return           count of sent bytes
  ///
  ES_DECL_REFLECTED_METHOD2(EsVariant, send, cr_EsVariant, cr_EsVariant);

  /// Send data buffer to specific client. Client address is EsSocketAddr object
  /// @param[in]  addr  EsSocketArrd object to send data to
  /// @param[in]  data  EsBinaryBuffer to send (or variant, implicitly convertible to BB)
  /// @param[in]  tmo   ulong - timeout for send operation
  /// @return           count of sent bytes
  ///
  ES_DECL_REFLECTED_METHOD3(EsVariant, send, cr_EsVariant, cr_EsVariant, cr_EsVariant);

  /// Disconnect client at specified address
  /// @param[in]  addr        EsSocketAddr address of client to set to disconnected state
  /// @param[in]  noFarewell  no server farewell should be sent to client upon disconnection
  ///
  ES_DECL_REFLECTED_METHOD2(void, clientDisconnect, cr_EsVariant, cr_EsVariant);

  /// Reset socket state
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// Reflected properties
  ///
	ES_DECL_PROPERTY_RO(  isOk, bool)
	ES_DECL_PROPERTY_RO(  isActive, bool)
	ES_DECL_PROPERTY_RO(  error, long)
	ES_DECL_PROPERTY_RO(  clientsCount, ulong)
	ES_DECL_PROPERTY_RO(  clientAddresses, EsVariant) ///< Return client addresses collection as [addr0, addr1, ... addrN],
                                                    ///  where addrX is EsSocketAddr object
  ES_DECL_PROPERTY_WO(  ioHandler, EsVariant)       ///< Attacheable IO handler for the server. Its reflected methods will be called
                                                    ///  in server Listener or Handler contexts. It should implement some, or all methods
                                                    ///  with the same names as Server overridables. Handler implementation must have an additional
                                                    ///  first parameter, in which a non-refcounted reference to this object instance is passed.
                                                    ///  If no Handler is assigned, the server object just does nothing.
  ES_DECL_PROPERTY_STD( socketType, ulong)
	ES_DECL_PROPERTY(     address, EsVariant)
	ES_DECL_PROPERTY(     maxIncomingConnections, ulong)
	ES_DECL_PROPERTY(     listenerBuffLen, ulong)
  ES_DECL_PROPERTY(     listenerAcceptTmo, ulong)
	ES_DECL_PROPERTY(     handlerBuffLen, ulong)
	ES_DECL_PROPERTY(     handlerTtl, ulong)
	ES_DECL_PROPERTY(     packetTimeout, ulong)
	ES_DECL_PROPERTY_STD( doThrow, bool)

protected:
  Server m_srv;
  mutable EsCriticalSection m_cs;
  EsSocketAddr m_addr;
  EsVariant m_ioHandler;
};

} // namespace EsReflection

//---------------------------------------------------------------------------

#endif // ES_COMM_USE_SOCKETS

#endif // _es_socket_server_h_