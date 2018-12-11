/// POSIX sockets engine adaptation
/// NB! do not include this file in project directly, it's
/// got included from EsSocket.cpp as appropriate
///
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>

#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
#   include <asm/ioctls.h>
#   include <Androidapi.Helpers.hpp>
#   include <Androidapi.JNI.Bluetooth.hpp>
# endif
#endif

static long socketErrorConvert(int error)
{
  switch( error )
  {
  case 0:
    return static_cast<long>(EsSocketError::None);
  case ENETDOWN:
    return static_cast<long>(EsSocketError::NetDown);
  case EINVAL:
    return static_cast<long>(EsSocketError::InvalidParameter);
  case EINPROGRESS:
  case EALREADY:
    return static_cast<long>(EsSocketError::Busy);
  case ENOTCONN:
    return static_cast<long>(EsSocketError::NotConnected);
  case ENOTSOCK:
    return static_cast<long>(EsSocketError::InvalidSocket);
  case EAGAIN:
    return static_cast<long>(EsSocketError::TemporaryError);
  case EAFNOSUPPORT:
    return static_cast<long>(EsSocketError::AddressTypeNotSupported);
  case ENOMEM:
    return static_cast<long>(EsSocketError::NotEnoughMemory);
  case EHOSTUNREACH:
    return static_cast<long>(EsSocketError::HostNotFound);
  case EMFILE:
    return static_cast<long>(EsSocketError::NoMoreDescriptors);
  case ENOBUFS:
    return static_cast<long>(EsSocketError::NoBuffersAvailable);
  case EPROTONOSUPPORT:
    return static_cast<long>(EsSocketError::ProtocolNotSupported);
  case ENOPROTOOPT:
    return static_cast<long>(EsSocketError::ProtocolOptionNotSupported);
  case EPROTOTYPE:
    return static_cast<long>(EsSocketError::WrongProtocolForType);
  case ESOCKTNOSUPPORT:
    return static_cast<long>(EsSocketError::WrongSocketTypeForAddress);
  case EADDRINUSE:
    return static_cast<long>(EsSocketError::AddressInUse);
  case EISCONN:
    return static_cast<long>(EsSocketError::AlreadyConnected);
  case EOPNOTSUPP:
    return static_cast<long>(EsSocketError::OperationNotSupported);
  case ECONNABORTED:
    return static_cast<long>(EsSocketError::ConnectionAborted);
  case ECONNRESET:
    return static_cast<long>(EsSocketError::ConnectionResetByPeer);
  case ENETRESET:
    return static_cast<long>(EsSocketError::ConnectionDropped);
  case EINTR:
    return static_cast<long>(EsSocketError::OperationCancelled);
  case ETIMEDOUT:
    return static_cast<long>(EsSocketError::TimedOut);
  case ENODATA:
    return static_cast<long>(EsSocketError::NoDataOfRequestedType);
  case EFAULT:
    return static_cast<long>(EsSocketError::PointerParameterIsInvalid);
  default:
    return static_cast<long>(EsSocketError::Unknown);
  }
}
//---------------------------------------------------------------------------

/// Initialize sockets engine
void EsSockets::initialize()
{
}
//---------------------------------------------------------------------------

/// Uninitialize sockets engine
void EsSockets::uninitialize()
{
}
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
struct BtClient
{
  _di_JBluetoothSocket m_socket;
  _di_JInputStream m_in;
  _di_JOutputStream m_out;

  bool __inline isConnected() const
  {
    return m_socket &&
      m_socket->isConnected();
  }

  bool __inline connect()
  {
    if( !isConnected() )
    {
      if( m_socket )
      {
        m_socket->connect();

        if( m_socket->isConnected() )
        {
          m_in = m_socket->getInputStream();
          m_out = m_socket->getOutputStream();

          return true;
        }
      }
    }

    return false;
  }

  void __inline close()
  {
    if( isConnected() )
    {
      m_in = 0;
      m_out = 0;
      m_socket->close();
    }
  }

  void __inline socketSet(const _di_JBluetoothSocket& sock)
  {
    if( m_socket != sock )
    {
      close();

      m_in = 0;
      m_out = 0;
      m_socket = sock;
      if( m_socket && m_socket->isConnected() )
      {
        m_in = m_socket->getInputStream();
        m_out = m_socket->getOutputStream();
      }
    }
  }

  ~BtClient()
  {
    try
    {
      socketSet(0);
    }
    catch(...)
    {}
  }
};
# endif
#endif
//---------------------------------------------------------------------------

// Special socket container, to augment handling
// Android BT sockets along with POSIX ones
struct EsSockHolder
{
  enum {
    typeNone,
    typeSocket,
#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
    typeClientSocketBt,
    typeServerSocketBt,
# endif
#endif
  };

  int m_type;

  union Data
  {
    int m_socket;
#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
    BtClient* m_btCli;
# endif
#endif
  } m_data;

#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
  bool __inline isBt() const
  {
    return typeClientSocketBt == m_type ||
      typeServerSocketBt == m_type;
  }

  void sockBtSet(const _di_JBluetoothSocket& socketBt)
  {
    ES_ASSERT(typeNone == m_type);
    if( 0 == m_data.m_btCli )
      m_data.m_btCli = new BtClient;

    ES_ASSERT(m_data.m_btCli);
    m_type = typeClientSocketBt;

    m_data.m_btCli->socketSet(socketBt);
  }

  _di_JBluetoothSocket sockBtGet() const
  {
    ES_ASSERT(typeClientSocketBt == m_type);
    ES_ASSERT(m_data.m_btCli);
    return m_data.m_btCli->m_socket;
  }

  bool btConnect()
  {
    if( typeClientSocketBt == m_type )
    {
      ES_ASSERT(m_data.m_btCli);
      return m_data.m_btCli->connect();
    }

    return false;
  }

  void btShutdown(EsSocketShutdownMode mode, bool doThrow)
  {
    if( typeClientSocketBt == m_type )
    {
      ES_ASSERT(m_data.m_btCli);
      switch( mode )
      {
      case EsSocketShutdownMode::Receive:
        try
        {
          if( m_data.m_btCli->m_in )
            m_data.m_btCli->m_in->close();
        }
        catch(const Exception& ex)
        {
          if( doThrow )
            EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
        }
        break;
      case EsSocketShutdownMode::Transmit:
        try
        {
          if(m_data.m_btCli->m_out)
            m_data.m_btCli->m_out->close();
        }
        catch(const Exception& ex)
        {
          if( doThrow )
            EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
        }
        break;
      case EsSocketShutdownMode::ReceiveTransmit:
        try
        {
          if(m_data.m_btCli->m_in)
            m_data.m_btCli->m_in->close();
          if(m_data.m_btCli->m_out)
            m_data.m_btCli->m_out->close();
        }
        catch(const Exception& ex)
        {
          if( doThrow )
            EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
        }
        break;
      }
    }
  }

//  typedef TJavaArray__1<System::Byte> JByteArrayT;

  void btWrite(const esU8* &pos, size_t toSend)
  {
    if( typeClientSocketBt == m_type )
    {
      ES_ASSERT(m_data.m_btCli);
      ES_ASSERT(m_data.m_btCli->m_out);

      _di_JByteBuffer ob = TJByteBuffer::JavaClass->allocate(toSend);
//      std::unique_ptr<JByteArrayT> a( new JByteArrayT(toSend) );
      for(int idx = 0; idx < toSend; ++idx)
        ob->put(idx, *pos++);
//        a->Items[idx] = *pos++;

      m_data.m_btCli->m_out->write( ob->array() /*a.get()*/ );
    }
    // else TODO:
  }

  void btRead(esU8* &pos, size_t toRead)
  {
    if( typeClientSocketBt == m_type )
    {
      ES_ASSERT(m_data.m_btCli);
      ES_ASSERT(m_data.m_btCli->m_in);

//      std::unique_ptr<JByteArrayT> a( new JByteArrayT(toRead) );
      _di_JByteBuffer ib = TJByteBuffer::JavaClass->allocate(toRead);
      int result = m_data.m_btCli->m_in->read( ib->array()/*a.get()*/ );
      for(int idx = 0; idx < result; ++idx)
        *pos++ = ib->get(idx);
        //*pos++ = a->Items[idx];
    }
    // else TODO:
  }

  ulong btIncomingSizeGet(ulong tmo)
  {
    if( typeClientSocketBt == m_type )
    {
      ES_ASSERT(m_data.m_btCli);
      ES_ASSERT(m_data.m_btCli->m_in);

      int result = m_data.m_btCli->m_in->available();
      while( !result && tmo )
      {
        EsThread::sleep(1);
        --tmo;
        result = m_data.m_btCli->m_in->available();
      }

      return result;
    }
    // else TODO:

    return 0;
  }

# endif
#endif

  bool empty() const
  {
    return typeNone == m_type;
  }

  int cleanup()
  {
    int result = 0;
#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
    if( typeClientSocketBt == m_type )
    {
      ES_DELETE(m_data.m_btCli);
    }
    else if( typeServerSocketBt == m_type )
    {
      // TODO:
    }
    else
# endif
#endif
    if( m_data.m_socket )
    {
      result = ::close(m_data.m_socket);
      m_data.m_socket = 0;
    }

    m_type = typeNone;

    return result;
  }

  // Contained socket access
  void sockSet(int sock)
  {
    ES_ASSERT(typeNone == m_type);
    m_data.m_socket = sock;
    m_type = typeSocket;
  }

  int sockGet() const
  {
    ES_ASSERT(typeSocket == m_type);
    return m_data.m_socket;
  }

  EsSockHolder() :
  m_type(typeNone)
  {
    memset(&m_data, 0, sizeof(m_data));
  }

  EsSockHolder(const EsSockHolder& src) :
  m_type(typeNone)
  {
    memset(&m_data, 0, sizeof(m_data));

#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
    if( typeClientSocketBt == src.m_type )
    {
      sockBtSet(
        src.sockBtGet()
      );
    }
    else if( typeServerSocketBt == src.m_type )
    {
      // TODO:
    }
    else
# endif
#endif
      m_data.m_socket = src.m_data.m_socket;

    m_type = src.m_type;
  }

  ~EsSockHolder()
  {
    try
    {
      cleanup();
    }
    catch(...)
    {}
  }
};
//---------------------------------------------------------------------------

// Socket addresses implementation
//
class EsSocketAddrIPV4 : public EsSocketAddr::Impl
{
public:
  EsSocketAddrIPV4()
  {
    m_type = EsSocketAddrType::IPV4;
    m_flags |= flagSvcResolved;

    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_port = 0;
    m_flags |= flagPortResolved;

    rawSet(anyAddr());
  }

  EsSocketAddrIPV4(const EsSocketAddrIPV4& src)
  {
    m_type = src.m_type;
    m_flags = src.m_flags;
    m_port = src.m_port;
    m_raw = src.m_raw;
    m_name = src.m_name;
    m_svc = src.m_svc;
    memcpy(&m_addr, &src.m_addr, sizeof(m_addr));
  }

  /// Abstract socket address interface implementation
  ///
  virtual const EsString& anyAddrGet() const
  {
    return anyAddr();
  }

  virtual EsSocketAddr::Impl* clone() const
  {
    return new EsSocketAddrIPV4(*this);
  }

  virtual void* nativeGet() const
  {
    return (void*)&m_addr;
  }

  virtual size_t nativeSizeGet() const
  {
    return sizeof(m_addr);
  }

  virtual void rawSet(const EsString& raw)
  {
    m_error = 0;
    if( m_raw != raw )
    {
      if( !rawParse(raw, reinterpret_cast<esU8*>(&m_addr.sin_addr)) )
        EsException::Throw(
          _("'%s' is not a valid IPv4 address"),
          raw
        );

      m_flags |= flagRawResolved;
      m_raw = raw;

      m_name.clear();
      m_flags &= ~flagNameResolved;
    }
  }

  virtual void portSet(ulong port)
  {
    m_error = 0;
    EsNumericCheck::checkRangeUInteger(0, 65535, port, _("Port number"));
    m_port = port;
    m_addr.sin_port = esHostToBe2( static_cast<u_short>(port) );

    m_flags |= flagPortResolved;

    m_svc.clear();
    m_flags &= ~flagSvcResolved;
  }

  virtual void serviceSet(const EsString& svc)
  {
    m_error = 0;
    if( m_svc != svc )
    {
      m_svc = svc;
      m_flags |= flagSvcResolved;

// Do not change port resolution state - consider port is always resolved
//      m_port = 0;
//      m_addr.ain_port = 0;
//      m_flags &= ~flagPortResolved;
    }
  }

  virtual bool isEqualTo(const EsSocketAddr::Impl* other) const
  {
    if( other )
    {
      if( this == other )
        return true;
      else if( other->typeGet() == m_type &&
        noError() &&
        other->noError() )
      {
        // it's enough to compare port and raw address
        return portGet() == other->portGet() &&
          rawGet(true) == other->rawGet(true);
      }
    }

    return false;
  }

  virtual bool isLessThan(const EsSocketAddr::Impl* other) const
  {
    if( isEqualTo( other ) )
      return false;

    if( other )
    {
      if( other->typeGet() > m_type )
        return true;

      if( other->typeGet() == m_type &&
        noError() &&
        other->noError() )
      {
        // it's enough to compare port and raw address
        if( portGet() < other->portGet() )
          return true;

        if( rawGet(true) < other->rawGet(true) )
          return true;
      }
    }

    return false;
  }

  virtual bool rawResolve(bool passive)
  {
    m_error = 0;
    if( !(m_flags & flagRawResolved) )
    {
      if( !m_name.empty() )
      {
        addrinfo hints;
        addrinfo* result = 0;
        memset(&hints, 0, sizeof(hints));

        if( passive )
          hints.ai_flags = AI_PASSIVE;
        hints.ai_family = m_addr.sin_family;

        int ai = getaddrinfo(
            EsString::toUtf8(m_name).c_str(),
            0,
            &hints,
            &result
          );

        if( 0 != ai )
          m_error = socketErrorConvert( errno );

        if( noError() )
        {
          ES_ASSERT(result);
          sockaddr_in* ain = reinterpret_cast<sockaddr_in*>(result->ai_addr);

          esU8* raw = reinterpret_cast<esU8*>(&ain->sin_addr.s_addr);
          ipFormat(raw);
          m_addr.sin_addr = ain->sin_addr;
          m_flags |= flagRawResolved;
        }

        if( result )
          freeaddrinfo(result);
      }
      else
      {
        m_raw = anyAddr();
        m_flags |= flagRawResolved;
      }
    }

    return (m_flags & flagRawResolved);
  }

  virtual bool nameResolve()
  {
    m_error = 0;
    m_aliases.clear();
    if( !(m_flags & flagNameResolved) )
    {
      ES_ASSERT( rawParse(m_raw) );

      hostent* result = gethostbyaddr(
          (const char*)&m_addr.sin_addr,
          4,
          m_addr.sin_family
        );

      if( 0 == result )
        m_error = socketErrorConvert( errno );

      if( noError() )
      {
        ES_ASSERT(result);
        m_name = EsString::fromUtf8(result->h_name);
        m_aliases.push_back( m_name );

        char** alias = result->h_aliases;
        while( *alias )
        {
          m_aliases.push_back( EsString::fromUtf8(*alias) );

          ++alias;
        }

        m_flags |= flagNameResolved;
      }
    }

    return (m_flags & flagNameResolved);
  }

  virtual bool serviceResolve()
  {
    m_error = 0;
    if( !(m_flags & flagSvcResolved) )
    {
      const EsIanaRegistry::Node& node = EsIanaRegistry::lookupByPort(m_port);

      if( node.isOk() )
      {
        m_svc = node.serviceGet();
        m_flags |= flagSvcResolved;
      }
      else
        m_error = static_cast<long>(EsSocketError::CouldNotResolveServiceFromPort);
    }

    return (m_flags & flagSvcResolved);
  }

  virtual bool portResolve()
  {
    m_error = 0;
    if( !(m_flags & flagPortResolved) )
    {
      const EsIanaRegistry::Node& node = EsIanaRegistry::lookupByService(m_svc);
      if( node.isOk() )
      {
        m_port = node.portGet();
        m_addr.sin_port = esHostToBe2( m_port );

        m_flags |= flagPortResolved;
      }
    }

    return (m_flags & flagPortResolved);
  }

  virtual void reconstructFromNative()
  {
    m_error = 0;
    ES_ASSERT( m_addr.sin_family == AF_INET );
    m_type = EsSocketAddrType::IPV4;
    m_port = esBeToHost2( m_addr.sin_port );
    m_flags |= flagPortResolved;

    esU8* raw = reinterpret_cast<esU8*>(&m_addr.sin_addr);
    ipFormat(raw);
    m_flags |= flagRawResolved;

    m_name.clear();
    m_flags &= ~flagNameResolved;

    m_svc.clear();
    m_flags &= ~flagSvcResolved;
  }

protected:
  // 'Any address' string
  static const EsString& anyAddr()
  {
    static const EsString s_any(esT("0.0.0.0"));
    return s_any;
  }

  // If raw is a valid ipv4 string, return true. If optional out param is not null,
  // fill it with parsed values
  virtual bool rawParse(const EsString& raw, esU8* out = 0) const
  {
    EsRegEx rex(esT("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.")
                esT("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.")
                esT("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.")
                esT("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)")
    );
    rex.set_text(raw);
    if( rex.get_matches() )
    {
      if( out )
      {
        // Get ip parse result
        const EsString& b4 = rex.matchGet(1);
        const EsString& b3 = rex.matchGet(2);
        const EsString& b2 = rex.matchGet(3);
        const EsString& b1 = rex.matchGet(4);

        long tmp =
          (EsString::toULong(b4) << 24) +
          (EsString::toULong(b3) << 16) +
          (EsString::toULong(b2) << 8) +
          EsString::toULong(b1);

        // Reorder parse result to network byte order and save
        tmp = esHostToBe4( tmp );
        *((long*)out) = tmp;
      }

      return true;
    }

    return false;
  }

  void ipFormat( const esU8* src )
  {
    // Convert network byte order to the host byte order temporarily
    long tmp = *((const long*)src);
    tmp = esBeToHost4(tmp);

    // Format ip from the host byte order
    m_raw = EsString::format(
      esT("%d.%d.%d.%d"),
      (tmp >> 24) & 0xFF,
      (tmp >> 16) & 0xFF,
      (tmp >> 8)  & 0xFF,
      tmp & 0xFF
      );
  }

protected:
  sockaddr_in m_addr;
};
//---------------------------------------------------------------------------

// POSIX|Android BT socket implementation class
//
class EsSocketImpl : public EsSocket::Impl
{
public:
  EsSocketImpl(const EsSocketAddr& addr, EsSocketType type) :
  EsSocket::Impl(addr, type),
  m_maxOutChunk(0)
  {
    ES_ASSERT(EsSocketType::None != m_type);

    int st = -1;
    int proto = -1;

    switch( m_type )
    {
    case EsSocketType::ConnectionOriented:
      st = SOCK_STREAM;
      proto = IPPROTO_TCP;
      break;
    case EsSocketType::MessageOriented:
      st = SOCK_DGRAM;
      proto = IPPROTO_UDP;
      break;
#ifdef ES_COMM_USE_BLUETOOTH
# if ES_OS == ES_OS_ANDROID
    case EsSocketType::Bluetooth:
      try
      {
        _di_JBluetoothAdapter bta =
          TJBluetoothAdapter::JavaClass->getDefaultAdapter();

        _di_JBluetoothDevice dev =
          bta->getRemoteDevice(
            StringToJString(m_addr.rawGet().c_str())
          );

        EsBluetoothServiceUUID svcuuid = m_addr.serviceGet();

        _di_JUUID uuid = TJUUID::JavaClass->fromString(
          StringToJString(svcuuid.asString(true).c_str()) );

        _di_JBluetoothSocket bts =
          dev->createRfcommSocketToServiceRecord(
            uuid
          );

        m_sock.sockBtSet(bts);
        ioChunkParamsGet();
      }
      catch(const Exception& ex)
      {
        EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
      }
      break;
# endif
#endif
    }

#ifdef ES_COMM_USE_BLUETOOTH
    if( EsSocketType::Bluetooth != m_type )
    {
#endif
      ES_ASSERT(-1 != st);
      ES_ASSERT(-1 != proto);

      int sock =
        ::socket(
          *((esI16*)m_addr.nativeGet()),
          st,
          proto
        );

      if( -1 == sock )
        checkError( errno, m_error, true );
      else
      {
        m_sock.sockSet(sock);
        ioChunkParamsGet();
      }
#ifdef ES_COMM_USE_BLUETOOTH
    }
#endif
  }

  // Direct connected socket creation (used in accept implementation)
  EsSocketImpl(const EsSockHolder& sock, const EsSocketAddr& addr, EsSocketType type) :
  EsSocket::Impl(addr, type),
  m_sock(sock),
  m_maxOutChunk(0)
  {
    ES_ASSERT(!m_sock.empty());

    ioChunkParamsGet();

    m_flags |= flagConnected;
  }

  virtual ~EsSocketImpl()
  {}

  virtual bool isOk() const
  {
    return !m_sock.empty();
  }

  virtual bool bind(bool doThrow)
  {
    m_error = 0;
    if( !m_sock.empty() )
    {
      if( !isBound() )
      {
#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
# if ES_OS == ES_OS_ANDROID
          EsException::Throw(_("Bind functionality is not supported for Android socket"));
# endif
        }
        else
        {
#endif
        int err = ::bind(
            m_sock.sockGet(),
            (const sockaddr*)m_addr.nativeGet(),
            m_addr.nativeSizeGet()
        );

        if( -1 == err )
        {
          checkError( errno, m_error, doThrow );
          return false;
        }
        else
          m_flags |= flagBound;
#ifdef ES_COMM_USE_BLUETOOTH
        }
#endif
      }
      else if( doThrow )
        EsException::Throw(_("Socket is already bound"));

      return true;
    }
    else
      signalNotInitialized(doThrow);

    return false;
  }

  virtual bool listen(int backlog, bool doThrow)
  {
    m_error = 0;
    if( !m_sock.empty() )
    {
      if( isBound() )
      {
        if( !isListening() )
        {
#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
# if ES_OS == ES_OS_ANDROID
          EsException::Throw(_("Listen functionality is not supported for Android socket"));
# endif
        }
        else
        {
#endif
          int err = ::listen(
            m_sock.sockGet(),
            backlog
          );

          if( -1 == err )
          {
            checkError( errno, m_error, doThrow );
            return false;
          }
#ifdef ES_COMM_USE_BLUETOOTH
        }
#endif
        }
        else if( doThrow )
          EsException::Throw(_("Socket is already listening"));

        return true;
      }
      else if( doThrow )
        EsException::Throw(_("Could not listen on unbound socket"));
    }
    else
      signalNotInitialized(doThrow);

    return false;
  }

  virtual EsSocket::Impl* accept(ulong tmo, bool doThrow)
  {
    m_error = 0;
    if( !m_sock.empty() )
    {
      if( isBound() && isListening() )
      {
#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
# if ES_OS == ES_OS_ANDROID
          EsException::Throw(_("Accept functionality is not supported for Android socket"));
# endif
        }
        else
        {
#endif

        if( readOpReady(tmo, doThrow) )
        {
          // accepted socket address is of the same kind as our listener's
          // so use cloning to create address object for the new incoming socket
          EsSocketAddr newAddr(m_addr.typeGet());

          unsigned addrLen = newAddr.nativeSizeGet();
          ES_ASSERT( newAddr.isOk() );

          int newSock = ::accept(
            m_sock.sockGet(),
            reinterpret_cast<sockaddr*>(newAddr.nativeGet()),
            &addrLen
          );

          if( 0 > newSock )
            checkError( errno, m_error, doThrow );
          else
          {
            ES_ASSERT(static_cast<size_t>(addrLen) == newAddr.nativeSizeGet());
            // reconstruct address object from its native representation
            newAddr.reconstructFromNative();
            // query new socket type
            EsSocketType type = m_type;
            long error = 0;
            int st = socketGetIntOption<int>(newSock, SO_TYPE, type, error);
            switch( st )
            {
            case SOCK_STREAM:
              if( EsSocketType::MessageOriented == type )
                type = EsSocketType::ConnectionOriented;
              break;
            case SOCK_DGRAM:
              if( EsSocketType::MessageOriented != type )
                type = EsSocketType::MessageOriented;
              break;
            default:
              close(newSock);
              newSock = 0;
              m_error = static_cast<long>(EsSocketError::UnsupportedSocketType);
              if( doThrow )
                EsSockets::errorCheck(m_error);
            }

            if( newSock )
            {
              EsSockHolder sh;
              sh.sockSet(newSock);
              // create socket wrapper and return it
              return new EsSocketImpl(sh, newAddr, type);
            }
          }
        }

#ifdef ES_COMM_USE_BLUETOOTH
        }
#endif
      }
      else if( doThrow )
        EsException::Throw(_("Could not perform accept on unbound socket"));
    }
    else
      signalNotInitialized(doThrow);

    return 0;
  }

  virtual bool connect(bool doThrow)
  {
    m_error = 0;
    if( !m_sock.empty() )
    {
      if( isBound() )
      {
        if( doThrow )
          EsException::Throw(_("Could not connect bound socket"));
      }
      else if( !isConnected() )
      {
#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
          bool connected = false;
          try
          {
            connected = m_sock.btConnect();
          }
          catch(const Exception& ex)
          {
            if( doThrow )
              EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
          }

          if( connected )
          {
            m_flags |= flagConnected;
            return true;
          }
        }
        else
        {
#endif

        int err = ::connect(
          m_sock.sockGet(),
          (sockaddr*)m_addr.nativeGet(),
          m_addr.nativeSizeGet()
        );

        if( -1 == err )
        {
          checkError(errno, m_error, doThrow);
          return false;
        }
        else
        {
          m_flags |= flagConnected;
          return true;
        }

#ifdef ES_COMM_USE_BLUETOOTH
        }
#endif
      }
      else
        return true;
    }
    else
      signalNotInitialized(doThrow);

    return false;
  }

  virtual bool shutdown(EsSocketShutdownMode mode, bool doThrow)
  {
    m_error = 0;
    if( !m_sock.empty() )
    {
      if( isBound() || isConnected() )
      {
#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
          m_sock.btShutdown(mode, doThrow);
        }
        else
        {
#endif

        int smode = -1;
        switch( mode )
        {
        case EsSocketShutdownMode::Receive:
          smode = SHUT_RD;
          break;
        case EsSocketShutdownMode::Transmit:
          smode = SHUT_WR;
          break;
        case EsSocketShutdownMode::ReceiveTransmit:
          smode = SHUT_RDWR;
          break;
        }

        ES_ASSERT(-1 != smode);

        int err = ::shutdown(m_sock.sockGet(), smode);

        if( -1 == err )
        {
          checkError(errno, m_error, doThrow);
          return false;
        }

#ifdef ES_COMM_USE_BLUETOOTH
        }
#endif
      }

      return true;
    }
    else
      signalNotInitialized(doThrow);

    return false;
  }

  virtual void close(bool doThrow)
  {
    int err = m_sock.cleanup();
    if( -1 == err )
    {
      checkError(
        errno,
        m_error,
        doThrow
      );
    }
    m_flags = 0;
  }

  virtual ulong send(const esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    const esU8* pos = data;
    if( !m_sock.empty() && len )
    {
      if( isConnected() || (isMessageOriented() && !isBound()) )
      {
        ES_ASSERT(m_maxOutChunk);

        const esU8* end = data+len;
        while( pos < end )
        {
#ifdef ES_COMM_USE_BLUETOOTH
          if( EsSocketType::Bluetooth == m_type )
          {
            int toSend = esMin(end-pos, static_cast<ptrdiff_t>(m_maxOutChunk));

            try
            {
              m_sock.btWrite(pos, toSend);
            }
            catch(const Exception& ex)
            {
              if( doThrow )
                EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
              else
                break;
            }
          }
          else
          {
#endif
          if( writeOpReady(tmo, doThrow) )
          {
            int toSend = esMin(end-pos, static_cast<ptrdiff_t>(m_maxOutChunk));
            int result = 0;

            // if message oriented and not bound explicitly
            if( isMessageOriented() && !isBound() )
              result = sendto(
                m_sock.sockGet(),
                reinterpret_cast<const char*>(pos),
                toSend,
                0,
                (const sockaddr*)m_addr.nativeGet(),
                m_addr.nativeSizeGet()
              );
            else
              result = ::send(
                m_sock.sockGet(),
                reinterpret_cast<const char*>(pos),
                toSend,
                0
              );

            if( -1 == result )
            {
              checkError(errno, m_error, doThrow);
              break;
            }
            else
              pos += result;
          }
          else
            break;

#ifdef ES_COMM_USE_BLUETOOTH
          }
#endif
        }
      }
      else if( doThrow )
        EsException::Throw(_("Could not send to disconnected or bound socket"));
    }
    else
      signalNotInitialized(doThrow);

    return pos-data;
  }

  virtual ulong sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    const esU8* pos = data;
    if( !m_sock.empty() && len )
    {
      if( addr.isOk() )
      {
        ES_ASSERT(m_maxOutChunk);

#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
# if ES_OS == ES_OS_ANDROID
          EsException::Throw(_("sendTo functionality is not implemented for Android Bluetooth socket"));
# endif
        }
        else
        {
#endif

        const esU8* end = data+len;
        while( pos < end )
        {
          if( writeOpReady(tmo, doThrow) )
          {
            int toSend = esMin(end-pos, static_cast<ptrdiff_t>(m_maxOutChunk));
            int result = 0;

            result = sendto(
              m_sock.sockGet(),
              reinterpret_cast<const char*>(pos),
              toSend,
              0,
              (const sockaddr*)m_addr.nativeGet(),
              m_addr.nativeSizeGet()
            );

            if( -1 == result )
            {
              checkError(errno, m_error, doThrow);
              break;
            }
            else
              pos += result;
          }
          else
            break;
        }

#ifdef ES_COMM_USE_BLUETOOTH
        }
#endif
      }
      else if( doThrow )
        EsException::Throw(_("Could not send to invalid address"));
    }
    else
      signalNotInitialized(doThrow);

    return pos-data;
  }

  virtual ulong receive(esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    esU8* pos = data;
    if( !m_sock.empty() && len )
    {
      if( isConnected() )
      {
        esU8* end = data+len;
        while( pos < end )
        {
          int toRead = incomingSizeGet(tmo, doThrow);

          if( noError() && toRead )
          {
            toRead = esMin(end-pos, static_cast<ptrdiff_t>(toRead));

#ifdef ES_COMM_USE_BLUETOOTH
            if( EsSocketType::Bluetooth == m_type )
            {
              try
              {
                m_sock.btRead(pos, toRead);
              }
              catch( const Exception& ex )
              {
                if( doThrow )
                  EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
                else
                  break;
              }
            }
            else
            {
#endif

            int result = recv(
              m_sock.sockGet(),
              reinterpret_cast<char*>(pos),
              toRead,
              0
            );

            if( -1 == result )
            {
              checkError(errno, m_error, doThrow);
              break;
            }
            else
              pos += result;

#ifdef ES_COMM_USE_BLUETOOTH
            }
#endif
          }
          else
            break;
        }
      }
      else if( doThrow )
        EsException::Throw(_("Could not receive from disconnected socket"));
    }
    else
      signalNotInitialized(doThrow);

    return pos-data;
  }

  virtual ulong receiveFrom(EsSocketAddr& addr, esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    if( !m_sock.empty() && len )
    {
      ulong toRead = incomingSizeGet(tmo, doThrow);

      if( noError() && toRead )
      {
        toRead = esMin(len, toRead);

#ifdef ES_COMM_USE_BLUETOOTH
        if( EsSocketType::Bluetooth == m_type )
        {
# if ES_OS == ES_OS_ANDROID
          EsException::Throw(_("receiveFrom functionality is not implemented for Android socket"));
# endif
        }
        else
        {
#endif
        // Use address of the same type as ours
        addr = EsSocketAddr(m_addr.typeGet());

        unsigned addrLen = addr.nativeSizeGet();

        int result = recvfrom(
          m_sock.sockGet(),
          reinterpret_cast<char*>(data),
          toRead,
          0,
          reinterpret_cast<sockaddr*>(addr.nativeGet()),
          &addrLen
        );

        if( 0 > result )
          checkError(errno, m_error, doThrow);
        else
        {
          addr.reconstructFromNative();
          return result;
        }

#ifdef ES_COMM_USE_BLUETOOTH
      }
#endif
      }
    }
    else
      signalNotInitialized(doThrow);

    return 0;
  }

  virtual ulong incomingSizeGet(ulong tmo, bool doThrow)
  {
#ifdef ES_COMM_USE_BLUETOOTH
    if( EsSocketType::Bluetooth == m_type )
    {
      ulong result = 0;
      try
      {
        result = m_sock.btIncomingSizeGet(tmo);
      }
      catch( const Exception& ex )
      {
        if( doThrow )
          EsSocketException::Throw(EsSocketError::Generic, ex.Message.c_str());
      }

      return result;
    }
    else
    {
#endif

    if( readOpReady(tmo, doThrow) )
    {
      u_long bytes_available = 0;
      int result = ::ioctl(m_sock.sockGet(), FIONREAD, &bytes_available);

      if( -1 == result )
        checkError(errno, m_error, doThrow);
      else
        return bytes_available;
    }

#ifdef ES_COMM_USE_BLUETOOTH
    }
#endif

    return 0;
  }

  static void checkError(int nativeError, long& error, bool doThrow)
  {
    error = socketErrorConvert(nativeError);
    if( static_cast<long>(EsSocketError::None) != error )
    {
      if( doThrow )
        EsException::Throw(_("Socket error encountered: '%s'"),
          EsSockets::errorStringGet(error) );
    }
  }

protected:
  bool noError() const
  {
    return static_cast<long>(EsSocketError::None) == m_error;
  }

  template <typename T>
  static T socketGetIntOption(int sock, int opt, EsSocketType type, long& error, bool doThrow = true)
  {
    ES_ASSERT(sock);

    int lvl = SOL_SOCKET;

    T optval = 0;
    socklen_t optlen = sizeof(T);

    int err = getsockopt(
      sock,
      lvl,
      opt,
      (char*)&optval,
      &optlen
    );

    if( -1 == err )
      checkError(errno, error, doThrow);

    return optval;
  }

  void ioChunkParamsGet()
  {
    switch( m_type )
    {
    case EsSocketType::ConnectionOriented:
      m_maxOutChunk = socketGetIntOption<int>(
        m_sock.sockGet(),
        SO_SNDBUF,
        m_type,
        m_error
      );
      break;
#ifdef ES_COMM_USE_BLUETOOTH
    case EsSocketType::Bluetooth:
      m_maxOutChunk = 2048;
      break;
#endif
    case EsSocketType::MessageOriented:
#if ES_OS == ES_OS_ANDROID || ES_OS == ES_OS_MAC || ES_OS == ES_OS_LINUX
      m_maxOutChunk = 1024;
#else
      m_maxOutChunk = socketGetIntOption<unsigned>(
        m_sock.sockGet(),
        SO_MAX_MSG_SIZE,
        m_type,
        m_error
      );
#endif
      break;
    }
  }

  void signalNotInitialized(bool doThrow)
  {
    ES_ASSERT( m_sock.empty() );
    m_error = static_cast<long>(EsSocketError::NotInitialized);
    if( doThrow )
      EsSockets::errorCheck(m_error);
  }

  bool readOpReady(ulong tmo, bool doThrow)
  {
    ES_ASSERT(!m_sock.empty());
    fd_set set;
    timeval timeout;
    timeout.tv_sec = tmo/1000;
    timeout.tv_usec = (tmo % 1000)*1000;

    FD_ZERO(&set);
    FD_SET(m_sock.sockGet(), &set);

    int result = select(
      0,
      &set,
      0,
      0,
      &timeout
    );

    if( -1 == result )
    {
      checkError(errno, m_error, doThrow);
      return false;
    }

    return 0 < result;
  }

  bool writeOpReady(ulong tmo, bool doThrow)
  {
    ES_ASSERT(!m_sock.empty());
    fd_set set;
    timeval timeout;
    timeout.tv_sec = tmo/1000;
    timeout.tv_usec = (tmo % 1000)*1000;

    FD_ZERO(&set);
    FD_SET(m_sock.sockGet(), &set);

    int result = select(
      0,
      0,
      &set,
      0,
      &timeout
    );

    if( -1 == result )
    {
      checkError(errno, m_error, doThrow);
      return false;
    }

    return 0 < result;
  }

protected:
  EsSockHolder m_sock;
  int m_maxOutChunk;
};
//---------------------------------------------------------------------------


