/// Windows sockets engine initialization|uninitialization
///
//---------------------------------------------------------------------------
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef ES_COMM_USE_BLUETOOTH
# include <ws2bth.h>
#endif

static WSADATA s_w;
//---------------------------------------------------------------------------

static long socketErrorConvert(int error)
{
  switch( error )
  {
  case 0:
    return static_cast<long>(EsSocketError::None);
  case WSANOTINITIALISED:
    return static_cast<long>(EsSocketError::EngineNotInitialized);
  case WSAEADDRNOTAVAIL:
    return static_cast<long>(EsSocketError::AddressNotAvailable);
  case WSAENETDOWN:
    return static_cast<long>(EsSocketError::NetDown);
  case WSAEINVAL:
    return static_cast<long>(EsSocketError::InvalidParameter);
  case WSAEINPROGRESS:
  case WSAEALREADY:
    return static_cast<long>(EsSocketError::Busy);
  case WSAENOTCONN:
    return static_cast<long>(EsSocketError::NotConnected);
  case WSAENOTSOCK:
    return static_cast<long>(EsSocketError::InvalidSocket);
  case WSATRY_AGAIN:
    return static_cast<long>(EsSocketError::TemporaryError);
  case WSANO_RECOVERY:
    return static_cast<long>(EsSocketError::NonRecoverableError);
  case WSAEAFNOSUPPORT:
    return static_cast<long>(EsSocketError::AddressTypeNotSupported);
  case WSA_NOT_ENOUGH_MEMORY:
    return static_cast<long>(EsSocketError::NotEnoughMemory);
  case WSAHOST_NOT_FOUND:
    return static_cast<long>(EsSocketError::HostNotFound);
  case WSAEMFILE:
    return static_cast<long>(EsSocketError::NoMoreDescriptors);
  case WSAENOBUFS:
    return static_cast<long>(EsSocketError::NoBuffersAvailable);
  case WSAEPROTONOSUPPORT:
    return static_cast<long>(EsSocketError::ProtocolNotSupported);
  case WSAENOPROTOOPT:
    return static_cast<long>(EsSocketError::ProtocolOptionNotSupported);
  case WSAEPROTOTYPE:
    return static_cast<long>(EsSocketError::WrongProtocolForType);
  case WSAESOCKTNOSUPPORT:
    return static_cast<long>(EsSocketError::WrongSocketTypeForAddress);
  case WSAEADDRINUSE:
    return static_cast<long>(EsSocketError::AddressInUse);
  case WSAEISCONN:
    return static_cast<long>(EsSocketError::AlreadyConnected);
  case WSAEOPNOTSUPP:
    return static_cast<long>(EsSocketError::OperationNotSupported);
  case WSAECONNABORTED:
    return static_cast<long>(EsSocketError::ConnectionAborted);
  case WSAECONNRESET:
    return static_cast<long>(EsSocketError::ConnectionResetByPeer);
  case WSAENETRESET:
    return static_cast<long>(EsSocketError::ConnectionDropped);
  case WSAECONNREFUSED:
    return static_cast<long>(EsSocketError::ConnectionRefused);
  case WSAEINTR:
    return static_cast<long>(EsSocketError::OperationCancelled);
  case WSAETIMEDOUT:
    return static_cast<long>(EsSocketError::TimedOut);
  case WSANO_DATA:
    return static_cast<long>(EsSocketError::NoDataOfRequestedType);
  case WSAEFAULT:
    return static_cast<long>(EsSocketError::PointerParameterIsInvalid);
  case WSAEMSGSIZE:
    return static_cast<long>(EsSocketError::MessageIsTooLong);
  default:
    return static_cast<long>(EsSocketError::Unknown);
  }
}
//---------------------------------------------------------------------------

/// Initialize sockets engine
void EsSockets::initialize()
{
  int error = WSAStartup(0x0202, &s_w);
  long err = socketErrorConvert(error);
  EsSockets::errorCheck(err);

#ifdef ES_COMM_USE_BLUETOOTH
  EsBluetooth::initialize();
#endif
}
//---------------------------------------------------------------------------

/// Uninitialize sockets engine
void EsSockets::uninitialize()
{
  WSACleanup();

#ifdef ES_COMM_USE_BLUETOOTH
  EsBluetooth::uninitialize();
#endif
}
//---------------------------------------------------------------------------

// Windows socket addresses implementation
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
  virtual const EsString& anyAddrGet() const ES_OVERRIDE
  {
    return anyAddr();
  }

  virtual EsSocketAddr::Impl* clone() const ES_OVERRIDE
  {
    return new EsSocketAddrIPV4(*this);
  }

  virtual void* nativeGet() const ES_OVERRIDE
  {
    return (void*)&m_addr;
  }

  virtual size_t nativeSizeGet() const ES_OVERRIDE
  {
    return sizeof(m_addr);
  }

  virtual void rawSet(const EsString& raw) ES_OVERRIDE
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

  virtual void portSet(ulong port) ES_OVERRIDE
  {
    m_error = 0;
    EsNumericCheck::checkRangeUInteger(0, 65535, port, _("Port number"));
    m_port = port;
    m_addr.sin_port = esHostToBe2( static_cast<u_short>(port) );

    m_flags |= flagPortResolved;

    m_svc.clear();
    m_flags &= ~flagSvcResolved;
  }

  virtual void serviceSet(const EsString& svc) ES_OVERRIDE
  {
    m_error = 0;
    if( m_svc != svc )
    {
      m_svc = svc;
      m_flags |= flagSvcResolved;
    }
  }

  virtual bool isEqualTo(const EsSocketAddr::Impl* other) const ES_OVERRIDE
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

  virtual bool isLessThan(const EsSocketAddr::Impl* other) const ES_OVERRIDE
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

  virtual bool rawResolve(bool passive) ES_OVERRIDE
  {
    m_error = 0;
    if( !(m_flags & flagRawResolved) )
    {
      if( !m_name.empty() )
      {
        ADDRINFOT hints;
        ADDRINFOT* result = 0;
        memset(&hints, 0, sizeof(hints));

        if( passive )
          hints.ai_flags = AI_PASSIVE;
        hints.ai_family = m_addr.sin_family;

        int ai = GetAddrInfo(
            m_name.c_str(),
            0,
            &hints,
            &result
          );

        if( 0 != ai )
          m_error = socketErrorConvert( WSAGetLastError() );

        if( noError() )
        {
          ES_ASSERT(result);
          sockaddr_in* ain = reinterpret_cast<sockaddr_in*>(result->ai_addr);

          esU8* raw = reinterpret_cast<esU8*>(&ain->sin_addr.S_un.S_un_b);
          ipFormat(raw);
          m_addr.sin_addr = ain->sin_addr;
          m_flags |= flagRawResolved;
        }

        if( result )
          FreeAddrInfo(result);
      }
      else
      {
        m_raw = anyAddr();
        m_flags |= flagRawResolved;
      }
    }

    return (m_flags & flagRawResolved);
  }

  virtual bool nameResolve() ES_OVERRIDE
  {
    m_error = 0;
    m_aliases.clear();
    if( !(m_flags & flagNameResolved) )
    {
      ES_ASSERT( rawParse(m_raw) );

      HOSTENT* result = gethostbyaddr(
          (const char*)&m_addr.sin_addr,
          4,
          m_addr.sin_family
        );

      if( 0 == result )
        m_error = socketErrorConvert( WSAGetLastError() );

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

  virtual bool serviceResolve() ES_OVERRIDE
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

  virtual bool portResolve() ES_OVERRIDE
  {
    m_error = 0;
    if( !(m_flags & flagPortResolved) )
    {
      const EsIanaRegistry::Node& node = EsIanaRegistry::lookupByService(m_svc);
      if( node.isOk() )
      {
        m_port = node.portGet();
        m_addr.sin_port = esHostToBe2( static_cast<esU16>(m_port) );

        m_flags |= flagPortResolved;
      }
    }

    return (m_flags & flagPortResolved);
  }

  virtual void reconstructFromNative() ES_OVERRIDE
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
  virtual bool rawParse(const EsString& raw, esU8* out = 0) const ES_OVERRIDE
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

// Windows socket implementation class
//
class EsSocketImpl : public EsSocket::Impl
{
public:
  EsSocketImpl(const EsSocketAddr& addr, EsSocketType type) :
  EsSocket::Impl(addr, type),
  m_sock(0)
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
    case EsSocketType::Bluetooth:
      st = SOCK_STREAM;
      proto = BTHPROTO_RFCOMM;
      break;
#endif
    // case EsSocketType::IrDA:
    //  st = SOCK_STREAM;
    //  proto = 0;
    //  break;
    }

    ES_ASSERT(-1 != st);
    ES_ASSERT(-1 != proto);

    m_sock = ::socket(
      *((esI16*)m_addr.nativeGet()),
      st,
      proto
    );

    if( INVALID_SOCKET == m_sock )
    {
      m_sock = 0;
      checkError( WSAGetLastError(), m_error, true );
    }
    else
      ioChunkParamsGet();
  }

  // Direct connected socket creation (used in accept implementation)
  EsSocketImpl(SOCKET sock, const EsSocketAddr& addr, EsSocketType type) :
  EsSocket::Impl(addr, type),
  m_sock(sock)
  {
    ES_ASSERT(m_sock);

    ioChunkParamsGet();

    m_flags |= flagConnected;
  }

  virtual ~EsSocketImpl()
  {
    // Just in case, resources cleanup
    if( m_sock )
      closesocket(m_sock);
  }

  virtual bool isOk() const
  {
    return 0 != m_sock;
  }

  virtual bool bind(bool doThrow)
  {
    m_error = 0;
    if( m_sock )
    {
      if( !isBound() )
      {
        int err = ::bind(
          m_sock,
          (const sockaddr*)m_addr.nativeGet(),
          static_cast<int>(m_addr.nativeSizeGet())
        );

        if( SOCKET_ERROR == err )
        {
          checkError( WSAGetLastError(), m_error, doThrow );
          return false;
        }
        else
          m_flags |= flagBound;
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
    if( m_sock )
    {
      if( isBound() )
      {
        if( !isListening() )
        {
          int err = ::listen(
            m_sock,
            backlog
          );

          if( SOCKET_ERROR == err )
          {
            checkError( WSAGetLastError(), m_error, doThrow );
            return false;
          }

          m_flags |= flagListening;
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
    if( m_sock )
    {
      if( isBound() && isListening() )
      {
        if( readOpReady(tmo, doThrow) )
        {
          // accepted socket address is of the same kind as our listener's
          // so use cloning to create address object for the new incoming socket
          EsSocketAddr newAddr(m_addr.typeGet());
          int addrLen = static_cast<int>(newAddr.nativeSizeGet());
          ES_ASSERT( newAddr.isOk() );

          SOCKET newSock = ::accept(
            m_sock,
            (sockaddr*)newAddr.nativeGet(),
            &addrLen
          );

          if( INVALID_SOCKET == newSock )
            checkError( WSAGetLastError(), m_error, doThrow );
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
              closesocket(newSock);
              newSock = 0;
              m_error = static_cast<long>(EsSocketError::UnsupportedSocketType);
              if( doThrow )
                EsSockets::errorCheck(m_error);
            }

            if( newSock )
              // create socket wrapper and return it
              return new EsSocketImpl(newSock, newAddr, type);
          }
        }
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
    if( m_sock )
    {
      if( isBound() )
      {
        if( doThrow )
          EsException::Throw(_("Could not connect bound socket"));
      }
      else if( !isConnected() )
      {
        int err = ::connect(
          m_sock,
          (sockaddr*)m_addr.nativeGet(),
          static_cast<int>(m_addr.nativeSizeGet())
        );

        if( SOCKET_ERROR == err )
        {
          checkError(WSAGetLastError(), m_error, doThrow);
          return false;
        }
        else
        {
          m_flags |= flagConnected;
          return true;
        }
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
    if( m_sock )
    {
      if( isBound() || isConnected() )
      {
        int smode = -1;
        switch( mode )
        {
        case EsSocketShutdownMode::Receive:
          smode = SD_RECEIVE;
          break;
        case EsSocketShutdownMode::Transmit:
          smode = SD_SEND;
          break;
        case EsSocketShutdownMode::ReceiveTransmit:
          smode = SD_BOTH;
          break;
        }

        ES_ASSERT(-1 != smode);

        int err = ::shutdown(m_sock, smode);

        if( SOCKET_ERROR == err )
        {
          checkError(WSAGetLastError(), m_error, doThrow);
          return false;
        }
      }

      return true;
    }
    else
      signalNotInitialized(doThrow);

    return false;
  }

  virtual void close(bool doThrow)
  {
    m_error = 0;
    if( m_sock )
    {
      int err = closesocket(m_sock);

      if( SOCKET_ERROR == err )
      {
        checkError(
          WSAGetLastError(),
          m_error,
          doThrow
        );
      }
      else
      {
        m_flags = 0;
        m_sock = 0;
      }
    }
  }

  virtual ulong send(const esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    const esU8* pos = data;
    if( m_sock && len )
    {
      if( isConnected() || (isMessageOriented() && !isBound()) )
      {
        ES_ASSERT(m_maxOutChunk);

        const esU8* end = data+len;
        while( pos < end )
        {
          if( writeOpReady(tmo, doThrow) )
          {
            int toSend = esMin(
              static_cast<int>(end-pos),
              static_cast<int>(m_maxOutChunk)
            );
            int result = 0;

            // if message oriented and not bound explicitly
            if( isMessageOriented() && !isBound() )
              result = sendto(
                m_sock,
                reinterpret_cast<const char*>(pos),
                toSend,
                0,
                (const sockaddr*)m_addr.nativeGet(),
                static_cast<int>(m_addr.nativeSizeGet())
              );
            else
              result = ::send(
                m_sock,
                reinterpret_cast<const char*>(pos),
                toSend,
                0
              );

            if( SOCKET_ERROR == result )
            {
              checkError(WSAGetLastError(), m_error, doThrow);
              break;
            }
            else
              pos += result;
          }
          else
            break;
        }
      }
      else if( doThrow )
        EsException::Throw(_("Could not send to disconnected or bound socket"));
    }
    else
      signalNotInitialized(doThrow);

    return static_cast<ulong>(pos-data);
  }

  virtual ulong sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    const esU8* pos = data;
    if( m_sock && len )
    {
      if( addr.isOk() )
      {
        ES_ASSERT(m_maxOutChunk);

        const esU8* end = data+len;
        while( pos < end )
        {
          if( writeOpReady(tmo, doThrow) )
          {
            int toSend = esMin(
              static_cast<int>(end-pos),
              static_cast<int>(
                m_maxOutChunk
              )
            );

            int result = sendto(
              m_sock,
              reinterpret_cast<const char*>(pos),
              toSend,
              0,
              (const sockaddr*)m_addr.nativeGet(),
              static_cast<int>(m_addr.nativeSizeGet())
            );

            if( SOCKET_ERROR == result )
            {
              checkError(WSAGetLastError(), m_error, doThrow);
              break;
            }
            else
              pos += result;
          }
          else
          {
            ES_DEBUG_TRACE(
              esT("Socket sendTo writeOpReady wait expired")
            );
            break;
          }
        }
      }
      else if( doThrow )
        EsException::Throw(_("Could not send to invalid address"));
    }
    else
      signalNotInitialized(doThrow);

    return static_cast<ulong>(pos-data);
  }

  virtual ulong receive(esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    if( !len )
      return 0;

    if( !m_sock )
    {
      signalNotInitialized(doThrow);
      return 0;
    }

    if( !isConnected() )
    {
      if( doThrow )
        EsException::Throw(_("Could not receive from disconnected socket"));

      return 0;
    }

    esU8* pos = data;
    esU8* end = data+len;

    ES_ASSERT(m_maxInChunk);

    while( pos < end )
    {
      int toRead = incomingSizeGet(tmo, doThrow);

      if( noError() && toRead )
      {
        toRead = esMin(
          static_cast<int>(end-pos),
          static_cast<int>(
            esMin(
              toRead,
              m_maxInChunk
            )
          )
        );

        int result = recv(
          m_sock,
          reinterpret_cast<char*>(pos),
          toRead,
          0
        );

        if( SOCKET_ERROR == result )
        {
          checkError(
            WSAGetLastError(),
            m_error,
            doThrow
          );

          break;
        }

        pos += result;
      }
      else
        break;
    }

    return static_cast<ulong>(pos-data);
  }

  virtual ulong receiveFrom(EsSocketAddr& addr, esU8* data, ulong len, ulong tmo, bool doThrow)
  {
    m_error = 0;
    if( !m_sock )
    {
      signalNotInitialized(doThrow);
      return 0;
    }

    if( !len )
      return 0;

    ulong toRead = incomingSizeGet(tmo, doThrow);
    if( noError() && toRead )
    {
      ES_ASSERT(m_maxInChunk);

      toRead = esMin(
        len,
        toRead
      );

      // Use address of the same type as ours
      addr = EsSocketAddr(m_addr.typeGet());
      int addrLen = static_cast<int>(addr.nativeSizeGet());

      int result = recvfrom(
        m_sock,
        reinterpret_cast<char*>(data),
        toRead,
        0,
        (sockaddr*)addr.nativeGet(),
        &addrLen
      );

      if( SOCKET_ERROR == result )
        checkError(
          WSAGetLastError(),
          m_error,
          doThrow
        );
      else
      {
        addr.reconstructFromNative();
        return result;
      }
    }

    return 0;
  }

  virtual ulong incomingSizeGet(ulong tmo, bool doThrow)
  {
    if( readOpReady(tmo, doThrow) )
    {
      u_long bytes_available = 0;
      int result = ioctlsocket(
        m_sock,
        FIONREAD,
        &bytes_available
      );

      if( SOCKET_ERROR == result )
        checkError(
          WSAGetLastError(),
          m_error,
          doThrow
        );
      else
        return bytes_available;
    }

    return 0;
  }

protected:
  bool noError() const
  {
    return static_cast<long>(EsSocketError::None) == m_error;
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

  template <typename T>
  static T socketGetIntOption(SOCKET sock, int opt, EsSocketType type, long& error, bool doThrow = true)
  {
    ES_ASSERT(sock);

    int lvl =
#ifdef ES_COMM_USE_BLUETOOTH
      (EsSocketType::Bluetooth == type) ? SOL_RFCOMM :
#endif
      SOL_SOCKET;

    T optval = 0;
    int optlen = sizeof(T);

    int err = getsockopt(
      sock,
      lvl,
      opt,
      (char*)&optval,
      &optlen
    );

    if( SOCKET_ERROR == err )
      checkError(
        WSAGetLastError(),
        error,
        doThrow
      );

    return optval;
  }

  void ioChunkParamsGet()
  {
    switch( m_type )
    {
    case EsSocketType::ConnectionOriented:
      m_maxOutChunk = socketGetIntOption<int>(m_sock, SO_SNDBUF, m_type, m_error);
      m_maxInChunk = socketGetIntOption<int>(m_sock, SO_RCVBUF, m_type, m_error);
      break;
#ifdef ES_COMM_USE_BLUETOOTH
    case EsSocketType::Bluetooth:
      m_maxOutChunk = socketGetIntOption<ulong>(m_sock, SO_BTH_MTU, m_type, m_error, false);
      m_maxInChunk = m_maxOutChunk;
      if( !noError() )
      {
        m_maxOutChunk = 1024;
        m_maxInChunk = 1024;
      }
      break;
#endif
    case EsSocketType::MessageOriented:
      m_maxOutChunk = socketGetIntOption<unsigned int>(m_sock, SO_MAX_MSG_SIZE, m_type, m_error);
      m_maxInChunk = m_maxOutChunk;
      break;
    }
  }

  void signalNotInitialized(bool doThrow)
  {
    ES_ASSERT( 0 == m_sock );
    m_error = static_cast<long>(EsSocketError::NotInitialized);
    if( doThrow )
      EsSockets::errorCheck(m_error);
  }

  bool readOpReady(ulong tmo, bool doThrow)
  {
    ES_ASSERT(m_sock);
    fd_set set;
    TIMEVAL timeout;
    timeout.tv_sec = tmo/1000;
    timeout.tv_usec = (tmo % 1000)*1000;

    FD_ZERO(&set);
    FD_SET(m_sock, &set);

    int result = select(
      0,
      &set,
      0,
      0,
      &timeout
    );

    if( SOCKET_ERROR == result )
    {
      checkError(WSAGetLastError(), m_error, doThrow);
      return false;
    }

    return 0 < result;
  }

  bool writeOpReady(ulong tmo, bool doThrow)
  {
    ES_ASSERT(m_sock);
    fd_set set;
    TIMEVAL timeout;
    timeout.tv_sec = tmo/1000;
    timeout.tv_usec = (tmo % 1000)*1000;

    FD_ZERO(&set);
    FD_SET(m_sock, &set);

    int result = select(
      0,
      0,
      &set,
      0,
      &timeout
    );

    if( SOCKET_ERROR == result )
    {
      checkError(WSAGetLastError(), m_error, doThrow);
      return false;
    }

    return 0 < result;
  }

protected:
  SOCKET m_sock;
};
//---------------------------------------------------------------------------


