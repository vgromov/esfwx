#ifndef _es_socket_h_
#define _es_socket_h_

#ifdef ES_COMM_USE_SOCKETS

/// Socket error identifiers
///
enum class EsSocketError : long
{
  Unknown           = -1,
  None              = 0,
  Generic,
  NotInitialized,
  EngineNotInitialized,
  NetDown,
  UnsupportedSocketType,
  InvalidParameter,
  Busy,
  NotConnected,
  InvalidSocket,
  TemporaryError,
  NonRecoverableError,
  AddressTypeNotSupported,
  AddressInvalid,
  AddressTypeNotSpecified,
  NotEnoughMemory,
  HostNotFound,
  NoMoreDescriptors,
  NoBuffersAvailable,
  ProtocolNotSupported,
  ProtocolOptionNotSupported,
  WrongProtocolForType,
  WrongSocketTypeForAddress,
  AddressInUse,
  AlreadyConnected,
  OperationNotSupported,
  ConnectionAborted,
  ConnectionResetByPeer,
  ConnectionDropped,
  ConnectionRefused,
  OperationCancelled,
  TimedOut,
  NoDataOfRequestedType,
  PointerParameterIsInvalid,
  CouldNotResolveServiceFromPort,
  CouldNotResolvePortFromService,
  AddressNotAvailable,
  MessageIsTooLong,
  _top ///< Special value, do not use directly
};
//---------------------------------------------------------------------------

/// Address types
///
enum class EsSocketAddrType : ulong
{
  None,
  IPV4,
  IPV6,
#ifdef ES_COMM_USE_BLUETOOTH
  Bluetooth,
#endif
};
//---------------------------------------------------------------------------

/// Socket type
///
enum class EsSocketType : ulong
{
  None,
  ConnectionOriented,
  MessageOriented,
#ifdef ES_COMM_USE_BLUETOOTH
  Bluetooth
#endif
};
//---------------------------------------------------------------------------

/// Socket shutdown mode
///
enum class EsSocketShutdownMode : ulong
{
  Receive,
  Transmit,
  ReceiveTransmit
};
//---------------------------------------------------------------------------

/// Socket exception specilization
///
class ESCOMM_CLASS EsSocketException : public EsException
{
private:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsSocketException(EsSocketError code, const EsString& msg);

public:
  static void Throw(EsSocketError code, const EsString& msg = EsString::null());
};
//---------------------------------------------------------------------------

/// Network socket address class
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsSocketAddr, EsReflectedClassIntf)

public:
  /// Abstract socket address interface
  ///
  class ESCOMM_CLASS Impl
  {
  public:
    virtual Impl* clone() const = 0;
    virtual ~Impl() {}

    virtual const EsString& anyAddrGet() const = 0;

    const EsString& rawGet(bool passive) const;
    virtual void rawSet(const EsString& raw) = 0;

    const EsString& nameGet() const;
    void nameSet(const EsString& name);

    const EsString::Array& nameAliasesGet() const { return m_aliases; }

    ulong portGet() const;
    virtual void portSet(ulong port) = 0;

    const EsString& serviceGet() const;
    virtual void serviceSet(const EsString& svc) = 0;

    virtual bool rawResolve(bool passive) = 0;
    virtual bool nameResolve() = 0;
    virtual bool serviceResolve() = 0;
    virtual bool portResolve() = 0;

    virtual bool isEqualTo(const Impl* other) const = 0;
    virtual bool isLessThan(const Impl* other) const = 0;

    virtual void* nativeGet() const = 0;
    virtual size_t nativeSizeGet() const = 0;
    virtual void reconstructFromNative() = 0;

    EsSocketAddrType typeGet() const { return m_type; }

    ulong flagsGet() const { return m_flags; }

    long errorGet() const { return m_error; }
    void errorReset() { m_error = 0; }

    bool noError() const { return static_cast<long>(EsSocketError::None) == m_error; }

  protected:
    virtual bool rawParse(const EsString& raw, esU8* out = 0) const = 0;

  protected:
    EsSocketAddrType m_type;
    mutable ulong m_flags;
    ulong m_port;
    long m_error;
    mutable EsString m_raw;
    mutable EsString m_name;
    mutable EsString m_svc;
    mutable EsString::Array m_aliases;

    friend class EsSocketAddr;
  };

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsSocketAddr)
  ES_DECL_ATTR_HANDLING_STD

  /// Default constructor
  EsSocketAddr(
    EsSocketAddrType type = EsSocketAddrType::None,
    const EsString& addr = EsString::null(),
    ulong port = 0
  );

  /// Construct with known address type, address, and service name
  EsSocketAddr(
    EsSocketAddrType type,
    const EsString& addr,
    const EsString& svc
  );

  /// Copy constructor
  EsSocketAddr(const EsSocketAddr& addr);

  /// Destructor service
  virtual ~EsSocketAddr();

  /// Assignment operator
  EsSocketAddr& operator=(const EsSocketAddr& src);

#ifdef ES_MODERN_CPP
  /// Move constructor
  EsSocketAddr(EsSocketAddr&& src);

  /// Move operator
  EsSocketAddr& operator=(EsSocketAddr&& src);
#endif

  /// Return true if address is set-up and valid
  bool isOk() const;

  /// Get socket address type
  EsSocketAddrType typeGet() const;

  /// Set socket address type
  void typeSet(EsSocketAddrType type);

  /// Get 'raw' address representation as string (like x.x.x.x for ip v4 or x:x:x:x for ip v6)
  const EsString& rawGet(bool passive = false) const;

  /// Set 'raw' address string representation
  void rawSet(const EsString& addr);

  /// Get 'resolved' address representation as string (like host name, or device name)
  const EsString& nameGet() const;

  /// Set 'resolved' address string representation
  void nameSet(const EsString& name);

  /// Get service name
  const EsString& serviceGet() const;

  /// Set service name
  void serviceSet(const EsString& svc);

  /// Get address port value
  ulong portGet() const;

  /// Set address port value
  void portSet(ulong port);

  /// Equality check operators
  bool operator==(const EsSocketAddr& other) const;
  bool operator!=(const EsSocketAddr& other) const
  { return !operator==(other); }

  /// Comparison operators
  bool operator<(const EsSocketAddr& other) const;

  /// Resolve raw address to the name
  /// @param[in] passive    If true, socket engine will try to resolve an address using passive approach
  /// @return               Return true if address is successfully resolved
  ///
  ES_DECL_REFLECTED_METHOD1(bool, rawResolve, bool);

  /// Resolve name to the raw address
  ES_DECL_REFLECTED_METHOD0(bool, nameResolve);

  /// Resolve port from service
  ES_DECL_REFLECTED_METHOD0(bool, portResolve);

  /// Resolve service from port
  ES_DECL_REFLECTED_METHOD0(bool, serviceResolve);

  /// Retrieve recent error code
  long errorGet() const;

  /// Reset recent error code
  void errorReset();

  /// Retrieve reference to the native address structure
  void* nativeGet() const;

  /// Retrieve size of the native address structure in bytes
  size_t nativeSizeGet() const;

  /// Reconstruct address from native internals
  void reconstructFromNative();

  /// Return default empty address object
  static const EsSocketAddr& none();

  /// Reflected services and properties
  ///

  /// Create an empty address object
  /// @return                   An empty address object dynamic instance
  ///
  ES_DECL_REFLECTED_CLASS_CTOR0(EsVariant);

  /// Create an address object of a specific type, with address string and port or service
  /// @param[in] type           Address type, one of EsSocketAddrType members
  /// @param[in] addr           Address string
  /// @param[in] port|service   Address port (if numeric), or service, if string
  /// @return                   Address object dynamic instance
  ///
  ES_DECL_REFLECTED_CLASS_CTOR3(EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

  /// Return our dynamic copy as EsBaseIntfPtr
  ///
  ES_DECL_REFLECTED_CONST_METHOD0(EsBaseIntfPtr, clone);

  /// Retrun 'none' address object
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, null);

  /// Properties
  ///
  ES_DECL_PROPERTY_RO(isOk, bool)
  ES_DECL_PROPERTY_RO(error, ulong)
  ES_DECL_PROPERTY(addressType, ulong)
  ES_DECL_PROPERTY(rawAddress, EsString)
  ES_DECL_PROPERTY(resolvedAddress, EsString)
  ES_DECL_PROPERTY(service, EsString)
  ES_DECL_PROPERTY(port, ulong)

  /// Reflected transformation helper
  ///

  /// Try to interpret variant as EsMathArrayComplex*.
  /// If variant could not be referenced as EsMathArrayComplex*,
  /// return nullptr
  ///
  static EsSocketAddr* fromVariant(const EsVariant& in);

  /// Return raw address string in form [raw-addr]:[raw-port]
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asRawString);

  /// Return readable address string in form [raw-addr|resolved-name]:[port]
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asReadableString);

protected:
  void validCheck() const;
  void errorCheck() const;

protected:
  // Data members
  //
  // Actual socket address implementation instance
  Impl* m_impl;
};
typedef std::vector<EsSocketAddr> EsSocketAddresses;
//---------------------------------------------------------------------------

/// Network communication socket base class
///
class ESCOMM_CLASS EsSocket
{
public:
  /// Miscellaneous constants
  enum {
    tmoMin = 500,   ///< Minimum operation timeout in ms
  };

  /// Abstract socket interface
  ///
  class ESCOMM_CLASS Impl
  {
  public:
    Impl(const EsSocketAddr& addr, EsSocketType type);
    virtual ~Impl() {}

    virtual bool isOk() const = 0;
    bool isListening() const;
    bool isBound() const;
    bool isConnected() const;
    bool isMessageOriented() const;

    virtual bool bind(bool doThrow) = 0;
    virtual bool listen(int backlog, bool doThrow) = 0;
    virtual Impl* accept(ulong tmo, bool doThrow) = 0;
    virtual bool connect(bool doThrow) = 0;

    virtual bool shutdown(EsSocketShutdownMode mode, bool doThrow) = 0;
    virtual void close(bool doThrow) = 0;

    /// Send data over established connection
    virtual ulong send(const esU8* data, ulong len, ulong tmo, bool doThrow) = 0;

    /// Send data to address, implicitly bounding socket, if needed
    virtual ulong sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow) = 0;

    /// Receive data over established connection
    virtual ulong receive(esU8* data, ulong len, ulong tmo, bool doThrow) = 0;

    /// Receive data from, and capture sender address
    virtual ulong receiveFrom(EsSocketAddr& addr, esU8* data, ulong len, ulong tmo, bool doThrow) = 0;

    /// Retrieve pending incoming data size
    virtual ulong incomingSizeGet(ulong tmo, bool doThrow) = 0;

    const EsSocketAddr& addrGet() const { return m_addr; }

    long errorGet() const { return m_error; }

    EsSocketType typeGet() const { return m_type; }

    ulong flagsGet() const { return m_flags; }

    void reset();

    int maxInChunkGet() const { return m_maxInChunk; }
    int maxOutChunkGet() const { return m_maxOutChunk; }

  protected:
    void checkSocketDefaultType();
    void checkSocketTypeAddrMatch();

  protected:
    EsSocketAddr m_addr;
    long m_error;
    EsSocketType m_type;
    ulong m_flags;
    int m_maxInChunk;
    int m_maxOutChunk;
  };

protected:
  EsSocket();
  EsSocket(Impl* impl);

public:
  static Impl* implCreate(const EsSocketAddr& addr, EsSocketType type);

  virtual ~EsSocket();

  /// Check if socket was created
  bool isOk() const;

  /// Return true if socket is OK and active
  bool isActive() const;

  /// Return type if OK, otherwise, return None
  EsSocketType typeGet() const;

  /// Reset socket state
  void reset() { if( m_impl ) m_impl->reset(); }

  /// Open socket using specified address and type
  bool open(const EsSocketAddr& addr, EsSocketType type = EsSocketType::None);

  /// Close socket, gracefully terminating the connection, if requested
  void close(bool graceful = true, bool doThrow = false);

  /// Get recent socket error code
  long errorGet() const;

  /// Get address associated with the socket
  const EsSocketAddr& addressGet(bool doThrow = false) const;

  /// Send data
  virtual ulong send(const esU8* data, ulong len, ulong tmo, bool doThrow);
  ulong send(const EsBinBuffer& bb, ulong tmo, bool doThrow);

  /// Send data to address
  virtual ulong sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow);
  ulong sendTo(const EsSocketAddr& addr, const EsBinBuffer& bb, ulong tmo, bool doThrow);

  /// Receive data
  ulong receive(esU8* data, ulong len, ulong tmo, bool doThrow);
  EsBinBuffer receive(ulong len, ulong tmo, bool doThrow);

  /// Receive data, capturing sender's address
  ulong receiveFrom(EsSocketAddr& addr, esU8* data, ulong len, ulong tmo, bool doThrow);
  EsBinBuffer receiveFrom(EsSocketAddr& addr, ulong len, ulong tmo, bool doThrow);

  /// Get size of incoming pending data
  ulong incomingSizeGet(ulong tmo = 0, bool doThrow = false);

  /// Maximum allowed input data chunk, in bytes
  ulong maxInChunkSizeGet() const;

  /// Maximum allowed output data chunk, in bytes
  ulong maxOutChunkSizeGet() const;

protected:
  // Throw uninitialized socket exception
  static void throwNotInitialized();

  /// Internal services. To be overridden in derived classes
  ///
  /// Return true if socket may be considered active
  virtual bool checkActive() const = 0;
  /// Perform operations upon object opening
  virtual bool doOpen(bool doThrow) = 0;
  /// Perform operations to close object
  virtual void doClose(bool graceful, bool doThrow) = 0;

protected:
  /// Actual socket implementation instance
  Impl* m_impl;

private:
  /// Prohibited functionality
  ///
  EsSocket(const EsSocket&) ES_REMOVEDECL;
  EsSocket& operator=(const EsSocket&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

/// Global socket reflected services
///
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN( EsSockets, ESCOMM_CLASS )

  /// Initialize sockets engine
  ES_DECL_REFLECTED_CLASS_METHOD0(void, initialize);

  /// Uninitialize sockets engine
  ES_DECL_REFLECTED_CLASS_METHOD0(void, uninitialize);

  /// Retrieve socket error string by its code
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, errorStringGet, long );

  /// Check for error code, and, if set, throws an error
  ES_DECL_REFLECTED_CLASS_METHOD1(void, errorCheck, long );

ES_DECL_REFLECTED_SERVICES_END

//---------------------------------------------------------------------------
/// Reflected socket enumerations
///
namespace EsReflection {

ES_DECL_ENUMERATION_BEGIN(EsSocketError)
  ES_DECL_ENUMERATION_ITEM(Unknown,                         static_cast<long>(::EsSocketError::Unknown))
  ES_DECL_ENUMERATION_ITEM(None,                            static_cast<long>(::EsSocketError::None))
  ES_DECL_ENUMERATION_ITEM(Generic,                         static_cast<long>(::EsSocketError::Generic))
  ES_DECL_ENUMERATION_ITEM(NotInitialized,                  static_cast<long>(::EsSocketError::NotInitialized))
  ES_DECL_ENUMERATION_ITEM(EngineNotInitialized,            static_cast<long>(::EsSocketError::EngineNotInitialized))
  ES_DECL_ENUMERATION_ITEM(NetDown,                         static_cast<long>(::EsSocketError::NetDown))
  ES_DECL_ENUMERATION_ITEM(UnsupportedSocketType,           static_cast<long>(::EsSocketError::UnsupportedSocketType))
  ES_DECL_ENUMERATION_ITEM(InvalidParameter,                static_cast<long>(::EsSocketError::InvalidParameter))
  ES_DECL_ENUMERATION_ITEM(Busy,                            static_cast<long>(::EsSocketError::Busy))
  ES_DECL_ENUMERATION_ITEM(NotConnected,                    static_cast<long>(::EsSocketError::NotConnected))
  ES_DECL_ENUMERATION_ITEM(InvalidSocket,                   static_cast<long>(::EsSocketError::InvalidSocket))
  ES_DECL_ENUMERATION_ITEM(TemporaryError,                  static_cast<long>(::EsSocketError::TemporaryError))
  ES_DECL_ENUMERATION_ITEM(NonRecoverableError,             static_cast<long>(::EsSocketError::NonRecoverableError))
  ES_DECL_ENUMERATION_ITEM(AddressTypeNotSupported,         static_cast<long>(::EsSocketError::AddressTypeNotSupported))
  ES_DECL_ENUMERATION_ITEM(AddressInvalid,                  static_cast<long>(::EsSocketError::AddressInvalid))
  ES_DECL_ENUMERATION_ITEM(AddressTypeNotSpecified,         static_cast<long>(::EsSocketError::AddressTypeNotSpecified))
  ES_DECL_ENUMERATION_ITEM(NotEnoughMemory,                 static_cast<long>(::EsSocketError::NotEnoughMemory))
  ES_DECL_ENUMERATION_ITEM(HostNotFound,                    static_cast<long>(::EsSocketError::HostNotFound))
  ES_DECL_ENUMERATION_ITEM(NoMoreDescriptors,               static_cast<long>(::EsSocketError::NoMoreDescriptors))
  ES_DECL_ENUMERATION_ITEM(NoBuffersAvailable,              static_cast<long>(::EsSocketError::NoBuffersAvailable))
  ES_DECL_ENUMERATION_ITEM(ProtocolNotSupported,            static_cast<long>(::EsSocketError::ProtocolNotSupported))
  ES_DECL_ENUMERATION_ITEM(ProtocolOptionNotSupported,      static_cast<long>(::EsSocketError::ProtocolOptionNotSupported))
  ES_DECL_ENUMERATION_ITEM(WrongProtocolForType,            static_cast<long>(::EsSocketError::WrongProtocolForType))
  ES_DECL_ENUMERATION_ITEM(WrongSocketTypeForAddress,       static_cast<long>(::EsSocketError::WrongSocketTypeForAddress))
  ES_DECL_ENUMERATION_ITEM(AddressInUse,                    static_cast<long>(::EsSocketError::AddressInUse))
  ES_DECL_ENUMERATION_ITEM(AlreadyConnected,                static_cast<long>(::EsSocketError::AlreadyConnected))
  ES_DECL_ENUMERATION_ITEM(OperationNotSupported,           static_cast<long>(::EsSocketError::OperationNotSupported))
  ES_DECL_ENUMERATION_ITEM(ConnectionAborted,               static_cast<long>(::EsSocketError::ConnectionAborted))
  ES_DECL_ENUMERATION_ITEM(ConnectionResetByPeer,           static_cast<long>(::EsSocketError::ConnectionResetByPeer))
  ES_DECL_ENUMERATION_ITEM(ConnectionDropped,               static_cast<long>(::EsSocketError::ConnectionDropped))
  ES_DECL_ENUMERATION_ITEM(OperationCancelled,              static_cast<long>(::EsSocketError::OperationCancelled))
  ES_DECL_ENUMERATION_ITEM(TimedOut,                        static_cast<long>(::EsSocketError::TimedOut))
  ES_DECL_ENUMERATION_ITEM(NoDataOfRequestedType,           static_cast<long>(::EsSocketError::NoDataOfRequestedType))
  ES_DECL_ENUMERATION_ITEM(PointerParameterIsInvalid,       static_cast<long>(::EsSocketError::PointerParameterIsInvalid))
  ES_DECL_ENUMERATION_ITEM(CouldNotResolveServiceFromPort,  static_cast<long>(::EsSocketError::CouldNotResolveServiceFromPort))
  ES_DECL_ENUMERATION_ITEM(CouldNotResolvePortFromService,  static_cast<long>(::EsSocketError::CouldNotResolvePortFromService))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsSocketAddrType)
  ES_DECL_ENUMERATION_ITEM(None,                            static_cast<ulong>(::EsSocketAddrType::None))
  ES_DECL_ENUMERATION_ITEM(IPV4,                            static_cast<ulong>(::EsSocketAddrType::IPV4))
  ES_DECL_ENUMERATION_ITEM(IPV6,                            static_cast<ulong>(::EsSocketAddrType::IPV6))
#ifdef ES_COMM_USE_BLUETOOTH
  ES_DECL_ENUMERATION_ITEM(Bluetooth,                       static_cast<ulong>(::EsSocketAddrType::Bluetooth))
#endif
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsSocketType)
  ES_DECL_ENUMERATION_ITEM(None,                            static_cast<ulong>(::EsSocketType::None))
  ES_DECL_ENUMERATION_ITEM(ConnectionOriented,              static_cast<ulong>(::EsSocketType::ConnectionOriented))
  ES_DECL_ENUMERATION_ITEM(MessageOriented,                 static_cast<ulong>(::EsSocketType::MessageOriented))
#ifdef ES_COMM_USE_BLUETOOTH
  ES_DECL_ENUMERATION_ITEM(Bluetooth,                       static_cast<ulong>(::EsSocketType::Bluetooth))
#endif
ES_DECL_ENUMERATION_END

}

#endif // ES_COMM_USE_SOCKETS

#endif // _socket_io_adapter_h_
