#include "escommpch.h"
#pragma hdrstop

#include "EsSocketServer.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_COMM_USE_SOCKETS
//---------------------------------------------------------------------------

EsSocketServer::Listener::Listener(EsSocketServer& owner) :
EsThread(),
m_owner(owner),
m_connOriented(false)
{}
//---------------------------------------------------------------------------

EsSocketServer::Listener::~Listener()
{
  stopAndWait();
}
//---------------------------------------------------------------------------

void EsSocketServer::Listener::connectionListen()
{
  std::unique_ptr<EsSocket::Impl> impl(
    m_owner.m_impl->accept(
      m_owner.m_packetTmo,
      false
    )
  );

  if( !impl )
    return;

  const EsSocketAddr& addr = impl->addrGet();

  ES_DEBUG_TRACE(
    esT("Connection-oriented Server listener detects incoming connection from '%s'"),
    addr.asRawString()
  );

  EsSocketServer::Handler::Ptr handler = m_owner.handlerFind(addr);
  if( !handler )
  {
    if( m_owner.canAddClient(addr) ) //< Check if we may create client handler with specified address
    {
      handler = m_owner.handlerCreate(
        addr,
        impl.release()
      );

      m_owner.clientAdd(
        handler,
        addr
      );

      handler->start();
    }
    else
    {
      impl->close( false );
      m_owner.onClientCannotBeAdded(addr);
    }
  }
}
//---------------------------------------------------------------------------

void EsSocketServer::Listener::connectionlessListen()
{
  ulong rcnt = m_owner.incomingSizeGet(
    m_owner.m_packetTmo,
    false
  );

  // Read from listener socket
  EsSocketAddr addr;
  if( rcnt )
  {
    ES_DEBUG_TRACE(
      esT("Connection-less Server listener detects %d bytes incoming"),
      rcnt
    );

    if( rcnt > m_buff.size() )
      rcnt = m_buff.size();

    rcnt = m_owner.receiveFrom(
      addr,
      m_buff.data(),
      rcnt,
      0,
      false
    );
  }

  if( !rcnt )
    return;

  ES_DEBUG_TRACE(
    esT("Connection-less Server listener received data packet of %d bytes"),
    rcnt
  );

  // Interpret incoming data and decide on acceptance
  EsSocketServer::Handler::Ptr handler = m_owner.handlerFind(addr);
  if( !handler )
  {
    if( m_owner.canAddClient(addr) ) //< Check if we may create another client handler
    {
      handler = m_owner.handlerCreate( addr );
      m_owner.clientAdd(
        handler,
        addr
      );
      handler->auxDataWait(0); //< Try turn-on aux data wait lock
      handler->start();
    }
    else
    {
      m_owner.onClientCannotBeAdded(addr);
      return;
    }
  }

  ES_ASSERT(handler);
  handler->dataReceived(
    m_buff.data(),
    rcnt
  );
}
//---------------------------------------------------------------------------

void EsSocketServer::Listener::disconnectedCleanup()
{
  std::set<EsSocketAddr> addrs;

#ifdef ES_MODERN_CPP
  for( auto &addr : m_owner.m_clients )
  {

#else
  for( std::map<EsSocketAddr, Handler::Ptr>::const_iterator cit = m_owner.m_clients.begin(); cit != m_owner.m_clients.end(); ++cit )
  {
    const std::map<EsSocketAddr, Handler::Ptr>::value_type& addr = (*cit);

#endif

    if( !addr.second->aliveGet() )
      addrs.insert(
        addr.first
      );
  }

#ifdef ES_MODERN_CPP
  for( auto &addr : addrs )
  {

#else
  for( std::set<EsSocketAddr>::const_iterator cit = addrs.begin(); cit != addrs.end(); ++cit )
  {
    const EsSocketAddr& addr = (*cit);

#endif

    m_owner.clientRemove(
      addr
    );
  }
}
//---------------------------------------------------------------------------

long EsSocketServer::Listener::worker()
{
  m_connOriented =
    EsSocketType::ConnectionOriented == m_owner.typeGet()
#ifdef ES_COMM_USE_BLUETOOTH
    || EsSocketType::Bluetooth == m_owner.typeGet()
#endif
  ;

  m_buff.resize(
    esMax(
      m_owner.listenerBuffLenGet(),
      m_owner.maxInChunkSizeGet()
    )
  );

  while( !checkForStopping() )
  {
    disconnectedCleanup();

    if( m_connOriented ) //< Check if we may initiate accepting new connections
    {
      connectionListen();
    }
    else //< Connection-less handling
    {
      connectionlessListen();
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

void EsSocketServer::Listener::onExitWorker()
{
  ES_DEBUG_TRACE(
    esT("EsSocketServer::Listener::onExitWorker")
  );

  // Close existing client connections gracefully
  const EsSocketAddresses& addrs = m_owner.clientAddressesGet();

#ifdef ES_MODERN_CPP
  for( auto const& addr : addrs )
  {
#else

  size_t cnt = addrs.size();
  for( size_t idx = 0; idx < cnt; ++idx )
  {
    const EsSocketAddr& addr = addrs[idx];

#endif

    m_owner.clientRemove(
      addr
    );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsSocketServer::Handler::Handler(EsSocketServer& owner, const EsSocketAddr& addr, EsSocketType type) :
EsThread(),
m_owner(owner),
m_aux(0, 1),
m_addr(addr),
m_ttl(m_owner.m_handlerTtl),
m_noFarewell(false),
m_alive(true),
m_disconnected(false),
m_connOriented(
  EsSocketType::ConnectionOriented == m_owner.typeGet()
#ifdef ES_COMM_USE_BLUETOOTH
  || EsSocketType::Bluetooth == m_owner.typeGet()
#endif
)
{
  m_io.reset(
    EsSocket::implCreate(
      addr,
      type
    )
  );
}
//---------------------------------------------------------------------------

EsSocketServer::Handler::Handler(EsSocketServer& owner, EsSocket::Impl* impl) :
EsThread(),
m_owner(owner),
m_ttl(m_owner.m_handlerTtl),
m_noFarewell(false),
m_alive(true),
m_disconnected(false),
m_connOriented(
  EsSocketType::ConnectionOriented == m_owner.typeGet()
#ifdef ES_COMM_USE_BLUETOOTH
  || EsSocketType::Bluetooth == m_owner.typeGet()
#endif
)
{
  m_io.reset(impl);

#ifdef ES_MODERN_CPP
  ES_ASSERT(m_io);
#else
  ES_ASSERT(m_io.get());
#endif

  m_addr = m_io->addrGet();

  if( m_connOriented )
  {
    size_t inSize = esMax(
      m_owner.handlerBuffLenGet(),
      m_owner.maxInChunkSizeGet()
    );

    m_in.resize(
      inSize
    );

    ES_DEBUG_TRACE(
      esT("Reserving %d bytes in-buffer for connection-oriented server handler"),
      inSize
    );
  }
}
//---------------------------------------------------------------------------

EsSocketServer::Handler::~Handler()
{
  stopAndWait();
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::ttlDecrement(ulong delta)
{
  m_ttl = (
    (m_ttl > delta) ?
      m_ttl-delta :
      0
  );

  ES_DEBUG_TRACE(
    esT("Server handler session TTL decremented: %d"),
    m_ttl
  );
}
//---------------------------------------------------------------------------

bool EsSocketServer::Handler::response(EsBinBuffer& out, bool isAux)
{
  if( isAux )
  {
    EsCriticalSectionLocker lock(m_csout);

    // If we have scheduled extra output data,
    // work on its local copy
    //
    out = m_out;
  }

  if( out.empty() ) //< Do we have anything to respond with?
    return false;

  ulong wcnt = 0;
  if( m_connOriented )
    wcnt = m_io->send(
      out.data(),
      out.size(),
      m_owner.m_packetTmo,
      false
    );
  else
    wcnt = m_io->sendTo(
      m_addr,
      out.data(),
      out.size(),
      m_owner.m_handlerTtl,
      false
    );

  disconnectionDetect();

  if( wcnt )
  {
    m_ttl = m_owner.m_handlerTtl;

    if( isAux ) //< Sent OK, cleanup send portion, if using auxiliary-assigned out buffer
    {
      EsCriticalSectionLocker lock(m_csout);

      m_out.erase(
        m_out.begin(),
        m_out.begin()+wcnt
      );
    }
  }

  return 0 != wcnt;
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::disconnectionDetect()
{
  // Remote client disconnection detection
  long err = m_io->errorGet();
  if( 0 != err )
  {
    errorLogAppend(
      EsSockets::errorStringGet(
        err
      )
    );

    EsSocketError serr = static_cast<EsSocketError>(err);
    if(
      EsSocketError::NotConnected == serr ||
      EsSocketError::ConnectionAborted == serr ||
      EsSocketError::ConnectionResetByPeer == serr ||
      EsSocketError::ConnectionDropped == serr
    )
      m_io->close(false);
  }
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::connectionHandle()
{
  const EsDateTime& dtStart = EsDateTime::now();

  ulong rcnt = m_io->incomingSizeGet(
    m_owner.m_packetTmo,
    false
  );

  if( rcnt )
  {
    if( rcnt > m_in.size() )
      rcnt = m_in.size();

    ES_ASSERT(rcnt);

    rcnt = m_io->receive(
      m_in.data(),
      rcnt,
      0,
      false
    );
  }

  EsBinBuffer out;
  bool handled = false;
  if( rcnt ) //< Reset inactivity timeout counter, process input data
  {
    ES_ASSERT( m_in.size() );

    m_owner.onClientDataReceived(
      m_addr,
      m_in.data(),
      rcnt,
      out
    );

    // Handle immediate response
    response(
      out,
      false
    );

    m_ttl = m_owner.m_handlerTtl;
    handled = true;
  }

  // Handle scheduled response
  if(
    response(
      out,
      true
    ) ||
    handled
  )
    return;

  // Othrwise, decrement timeout counter
  const EsTimeSpan& ts = EsDateTime::now()-dtStart;
  ullong ms = ts.get_allMilliseconds();
  if (ms > 0xFFFFFFFFUL)
    ms = 0xFFFFFFFFUL;
  else if( ms < forcedSleep )
  {
    ms = forcedSleep;
    EsThread::sleep( 
      static_cast<ulong>(ms) 
    );
  }

  ttlDecrement(
    static_cast<ulong>(ms)
  );
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::connectionlessHandle()
{
  EsBinBuffer out;
  bool handled = false;
  if( !m_in.empty() )
  {
    m_owner.onClientDataReceived(
      m_addr,
      m_in.data(),
      m_in.size(),
      out
    );

    m_in.clear();

    // Handle immediate response
    response(
      out,
      false
    );

    m_ttl = m_owner.m_handlerTtl;
    handled = true;
  }

  // Handle scheduled response
  if(
    response(
      out,
      true
    ) ||
    handled
  )
    return;

  if( !auxDataWait( m_owner.m_packetTmo ) ) //< Otherwise, put us to breakable sleep for at most m_owner.m_packetTmo
  {
    ttlDecrement(
      m_owner.m_packetTmo
    );

#ifdef ES_DEBUG
    if( !m_ttl )
      ES_DEBUG_TRACE(
        esT("Connection-less handler request handling timeout expired")
      );
#endif
  }
}
//---------------------------------------------------------------------------

long EsSocketServer::Handler::worker()
{
  while(
    m_ttl &&
    !m_disconnected &&
    !checkForStopping()
  )
  {
    if( m_connOriented )
    {
      if( !m_io->isConnected() )  //< Check if we're still connected
      {
        m_owner.onClientDisconnected( m_addr );
        break;
      }

      connectionHandle();
    }
    else //< Wait for m_ttl for external data to come in
    {
      connectionlessHandle();
    }
  }

  return 0;
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::onExitWorker()
{
  if( m_connOriented && m_io->isConnected() )
  {
    m_io->shutdown(
      EsSocketShutdownMode::Receive,
      false
    );

    m_owner.onClientClosing(
      m_addr,
      m_noFarewell,
      *m_io.get()
    );
  }

  m_io->close(false);
  m_disconnected = true;

  m_owner.onClientClosed(
    m_addr
  );

  ES_DEBUG_TRACE(
    esT("EsSocketServer::Handler::onExitWorker")
  );

  m_alive = false;
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::auxDataWaitBreak()
{
  m_aux.post();
}
//---------------------------------------------------------------------------

bool EsSocketServer::Handler::auxDataWait(ulong ms)
{
  if( EsSemaphore::resultOk == m_aux.wait(ms) )
  {
    m_aux.post(); // allow multiple pause re-query by releasing semaphore
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::dataReceived(EsBinBuffer::const_pointer data, ulong len)
{
  ES_ASSERT( !m_connOriented );

  m_in.assign(
    data,
    data+len
  );

  // Un-pause handler thread upon receiving connection-less data
  auxDataWaitBreak();
}
//---------------------------------------------------------------------------

void EsSocketServer::Handler::outDataSet(EsBinBuffer::const_pointer out, ulong len)
{
  if( !out || 0 == len )
    return;

  EsCriticalSectionLocker lock(m_csout);
  m_out.append(len, out);

  // Break aux waiting
  auxDataWaitBreak();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsSocketServer::EsSocketServer() :
m_listener(*this),
m_listenerBuffLen(buffLenDef),
m_listenerMaxIncomingConn(connCntDef),
m_listenerAcceptTmo(acceptTmoDef),
m_handlerBuffLen(buffLenDef),
m_handlerTtl(ttlDef),
m_packetTmo(packetTmoDef)
{
}
//---------------------------------------------------------------------------

void EsSocketServer::listenerBuffLenSet(ulong len)
{
  if( len < buffLenMin )
    len = buffLenMin;
  else if( len > buffLenMax )
    len = buffLenMax;

  m_listenerBuffLen = len;
}
//---------------------------------------------------------------------------

void EsSocketServer::listenerMaxIncomingConnSet(ulong cnt)
{
  if( cnt < 1 )
    cnt = 1;
  else if( cnt > connCntMax )
    cnt = connCntMax;

  m_listenerMaxIncomingConn = cnt;
}
//---------------------------------------------------------------------------

void EsSocketServer::listenerAcceptTmoSet(ulong tmo)
{
  if( tmo < acceptTmoMin )
    tmo = acceptTmoMin;
  else if( tmo > acceptTmoMax )
    tmo = acceptTmoMax;

  m_listenerAcceptTmo = tmo;
}
//---------------------------------------------------------------------------

void EsSocketServer::handlerBuffLenSet(ulong len)
{
  if( len < buffLenMin )
    len = buffLenMin;
  else if( len > buffLenMax )
    len = buffLenMax;

  m_handlerBuffLen = len;
}
//---------------------------------------------------------------------------

void EsSocketServer::handlerTtlSet(ulong ttl)
{
  if( ttl < ttlMin )
    ttl = ttlMin;
  else if( ttl > ttlMax )
    ttl = ttlMax;

  m_handlerTtl = ttl;
}
//---------------------------------------------------------------------------

void EsSocketServer::packetTmoSet(ulong tmo)
{
  if( tmo < packetTmoMin )
    tmo = packetTmoMin;
  else if( tmo > packetTmoMax )
    tmo = packetTmoMax;

  m_packetTmo = tmo;
}
//---------------------------------------------------------------------------

void EsSocketServer::clientAdd(const Handler::Ptr& handler, const EsSocketAddr& addr)
{
  ES_ASSERT( handler );
  ES_ASSERT( !handlerFind(addr) );

  onClientAdding(addr);

  {
    EsCriticalSectionLocker lock(m_cs);
    m_clients[addr] = handler;
  }

  ES_DEBUG_TRACE(
    esT("EsSocketServer::clientAdd: Client handler added for addr: '%s'"),
    addr.asRawString()
  );

  onClientAdded(addr);
}
//---------------------------------------------------------------------------

void EsSocketServer::clientRemove(const EsSocketAddr& addr)
{
  EsSocketServer::Handler::Ptr handler;
  {
    EsCriticalSectionLocker lock(m_cs);
    handler = handlerFind(addr);
    ES_ASSERT(handler);
  }

  onClientRemoving( addr );

  handler->stop();              //< Signal thread to stop
  handler->auxDataWaitBreak();  //< Unlock handler from possible data wait lock
  handler->stopAndWait();       //< Now wait for it to shutdown completely

  ES_DEBUG_TRACE(
    esT("EsSocketServer::clientRemove: Client handler removed for addr: '%s'"),
    addr.asRawString()
  );

  EsSocketAddr tmp = addr;

  {
    EsCriticalSectionLocker lock(m_cs);
    m_clients.erase( addr );
  }

  onClientRemoved( tmp );
}
//---------------------------------------------------------------------------

ulong EsSocketServer::clientsCountGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_clients.size();
}
//---------------------------------------------------------------------------

EsSocketServer::Handler::Ptr EsSocketServer::handlerFind(const EsSocketAddr& addr) const
{
#ifdef ES_MODERN_CPP
  auto cit
#else
  std::map<EsSocketAddr, Handler::Ptr>::const_iterator cit
#endif
    = m_clients.find(addr);

  if( cit != m_clients.end() )
    return (*cit).second;

#ifdef ES_MODERN_CPP
  return nullptr;
#else
  return EsSocketServer::Handler::Ptr();
#endif
}
//---------------------------------------------------------------------------

EsSocketServer::Handler::Ptr EsSocketServer::handlerCreate(const EsSocketAddr& addr, EsSocket::Impl* impl /*= nullptr*/)
{
  if( impl )
#ifdef ES_MODERN_CPP
    return std::make_shared<Handler>(
      *this,
      impl
    );
#else
    return EsSocketServer::Handler::Ptr(
      new Handler(
        *this,
        impl
      )
    );
#endif
  else
#ifdef ES_MODERN_CPP
    return std::make_shared<Handler>(
      *this,
      addr,
      typeGet()
    );
#else
    return EsSocketServer::Handler::Ptr(
      new Handler(
        *this,
        addr,
        typeGet()
      )
    );
#endif
}
//---------------------------------------------------------------------------

void EsSocketServer::disconnectedSet(const EsSocketAddr& addr, bool noFarewell /*= false*/)
{
  Handler::Ptr handler = handlerFind( addr );
  if( handler )
  {
    ES_DEBUG_TRACE(
      esT("EsSocketServer::disconnectedSet: for %s"),
      addr.asRawString()
    );

    handler->m_noFarewell = noFarewell;
    handler->m_disconnected = true;
  }
}
//---------------------------------------------------------------------------

bool EsSocketServer::checkActive() const
{
  return EsThreadState::None != m_listener.stateGet() &&
    m_impl && m_impl->isBound() && m_impl->isListening();
}
//---------------------------------------------------------------------------

bool EsSocketServer::doOpen(bool doThrow)
{
  if( EsThreadState::None != m_listener.stateGet() )
    return true;

  if( !m_impl->bind(doThrow) )
    return false;

  if(
    EsSocketType::ConnectionOriented == typeGet()
#ifdef ES_COMM_USE_BLUETOOTH
    || EsSocketType::Bluetooth == typeGet()
#endif
  )
  {
    if( !m_impl->listen(
        m_listenerMaxIncomingConn,
        doThrow
      )
    )
      return false;

    ES_DEBUG_TRACE(
      esT("Server listener starts in connection-oriented mode")
    );
  }

  m_listener.start();
  return true;
}
//---------------------------------------------------------------------------

void EsSocketServer::doClose(bool graceful, bool doThrow)
{
  m_listener.stopAndWait();
}
//---------------------------------------------------------------------------

EsSocketAddresses EsSocketServer::clientAddressesGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  EsSocketAddresses result;
  result.reserve( m_clients.size() );

#ifdef ES_MODERN_CPP
  for(auto &v: m_clients)
  {
#else
  for(std::map<EsSocketAddr, Handler::Ptr>::const_iterator cit = m_clients.begin(); cit != m_clients.end(); ++cit )
  {
    const std::map<EsSocketAddr, Handler::Ptr>::value_type& v = (*cit);
#endif

    result.push_back(
      v.first
    );
  }

  return result;
}
//---------------------------------------------------------------------------

EsSocketServer::Handler::Ptr EsSocketServer::clientGet(const EsSocketAddr& addr, bool doThrow) const
{
  EsCriticalSectionLocker lock(m_cs);
  Handler::Ptr handler = handlerFind(addr);

  if( !handler && doThrow )
    EsSocketException::Throw(
      EsSocketError::InvalidSocket,
      EsString::format(
        esT("Could not find socket handler for address '%s'"),
        addr.asRawString()
      )
    );

  return handler;
}
//---------------------------------------------------------------------------

ulong EsSocketServer::send(const esU8* data, ulong len, ulong tmo, bool doThrow)
{
  if( !data || 0 == len )
    return 0;

  EsCriticalSectionLocker lock(m_cs);

  // Send data on each handler object
#ifdef ES_MODERN_CPP
  for(auto &v: m_clients)
  {
#else
  for(std::map<EsSocketAddr, Handler::Ptr>::const_iterator cit = m_clients.begin(); cit != m_clients.end(); ++cit )
  {
    const std::map<EsSocketAddr, Handler::Ptr>::value_type& v = (*cit);
#endif

    ES_DEBUG_TRACE(
      esT("EsSocketServer::send Sending %d bytes with %d ms tmo, on %s"),
      len,
      tmo,
      v.first.asRawString()
    );

    v.second->outDataSet(
      data,
      len
    );
  }

  return len;
}
//---------------------------------------------------------------------------

ulong EsSocketServer::sendTo(const EsSocketAddr& addr, const esU8* data, ulong len, ulong tmo, bool doThrow)
{
  Handler::Ptr handler;

  {
    EsCriticalSectionLocker lock(m_cs);
    handler = clientGet(addr, doThrow);
  }

  if( !handler )
    return 0;

  if( !data || 0 == len )
    return 0;

  ES_DEBUG_TRACE(
    esT("EsSocketServer::sendTo Sending %d bytes with %d ms tmo, on %s"),
    len,
    tmo,
    addr.asRawString()
  );

  handler->outDataSet(
    data,
    len
  );

  return len;
}
//---------------------------------------------------------------------------

bool EsSocketServer::canAddClient(const EsSocketAddr& addr) const
{
  if( handlerFind(addr) )
    return false;

  if( m_clients.size() >= m_listenerMaxIncomingConn )
    return false;

  return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

typedef EsReflection::EsSocketServer EsSocketServerReflected;
//---------------------------------------------------------------------------

EsSocketServerReflected::Server::Server(EsSocketServerReflected& owner) :
EsReflection::EsSocketServerBase(),
m_owner(owner)
{}
//---------------------------------------------------------------------------

bool EsSocketServerReflected::Server::canAddClient(const EsSocketAddr& addr) const
{
  bool result = EsSocketServerBase::canAddClient(addr);

  if( !result )
    return result;

  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "canAddClient",
      2
    )
  )
  {
    result = m_owner.m_ioHandler.call(
      "canAddClient",
      m_owner.asWeakReference(),
      addr.clone()
    ).asBool();
  }

  return result;
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientCannotBeAdded(const EsSocketAddr& addr) const
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientCannotBeAdded",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientCannotBeAdded",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientAdding(const EsSocketAddr& addr)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientAdding",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientAdding",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientAdded(const EsSocketAddr& addr)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientAdded",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientAdded",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientRemoving(const EsSocketAddr& addr)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientRemoving",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientRemoving",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientRemoved(const EsSocketAddr& addr)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientRemoved",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientRemoved",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientDisconnected(const EsSocketAddr& addr)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientDisconnected",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientDisconnected",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientClosing",
      3
    )
  )
  {
    const EsVariant& result =
      m_owner.m_ioHandler.call(
        "onClientClosing",
        m_owner.asWeakReference(),
        addr.clone(),
        noFarewell
      );

    if( !noFarewell && !result.isEmpty() )
    {
      const EsBinBuffer& bb = result.asBinBuffer();
      if( io.isMessageOriented() )
      {
        io.sendTo(
          addr,
          bb.data(),
          bb.size(),
          m_owner.get_packetTimeout(),
          m_owner.m_doThrow
        );
      }
      else
      {
        io.send(
          bb.data(),
          bb.size(),
          m_owner.get_packetTimeout(),
          m_owner.m_doThrow
        );
      }
    }
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientClosed(const EsSocketAddr& addr)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientClosed",
      2
    )
  )
  {
    m_owner.m_ioHandler.call(
      "onClientClosed",
      m_owner.asWeakReference(),
      addr.clone()
    );
  }
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::Server::onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& out)
{
  EsCriticalSectionLocker lock(m_owner.m_cs);
  if(
    !m_owner.m_ioHandler.isEmpty() &&
    m_owner.m_ioHandler.hasMethod(
      "onClientDataReceived",
      3
    )
  )
  {
    EsBinBuffer in(
      data,
      data+len
    );

    const EsVariant& result = m_owner.m_ioHandler.call(
      "onClientDataReceived",
      m_owner.asWeakReference(),
      addr.clone(),
      in
    );

    if( !result.isEmpty() )
      out = result.asBinBuffer();
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

namespace EsReflection {

ES_DECL_BASE_CLASS_INFO_BEGIN( EsSocketServer, NO_CLASS_DESCR)
  // Ctors
  ES_DECL_REFLECTED_CTOR_INFO(                  EsSocketServer, EsVariant_ClassCall_ulong, NO_METHOD_DESCR)
  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(            EsSocketServer, open, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(            EsSocketServer, close, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(            EsSocketServer, send, EsVariant_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(            EsSocketServer, send, EsVariant_Call_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(            EsSocketServer, clientDisconnect, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(            EsSocketServer, reset, void_Call, NO_METHOD_DESCR)
  /// Properties
  ES_DECL_PROP_INFO_RO(                         EsSocketServer, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(                         EsSocketServer, isActive, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(                         EsSocketServer, error, long, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(                         EsSocketServer, clientsCount, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RO(                         EsSocketServer, clientAddresses, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(                         EsSocketServer, ioHandler, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(      EsSocketServer, socketType, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(::EsSocketType::MessageOriented), ES_ENUM(EsSocketType), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(                 EsSocketServer, address, EsVariant, NO_PROPERTY_LABEL, EsSocketAddr::null(), NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_PERSISTENT(                 EsSocketServer, doThrow, bool, NO_PROPERTY_LABEL, false, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(           EsSocketServer, maxIncomingConnections, ulong, NO_PROPERTY_LABEL, ::EsSocketServer::connCntDef, 1, ::EsSocketServer::connCntMax, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(           EsSocketServer, listenerBuffLen, ulong, NO_PROPERTY_LABEL, ::EsSocketServer::buffLenDef, ::EsSocketServer::buffLenMin, ::EsSocketServer::buffLenMax, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT(           EsSocketServer, listenerAcceptTmo, ulong, NO_PROPERTY_LABEL, ::EsSocketServer::acceptTmoDef, ::EsSocketServer::acceptTmoMin, ::EsSocketServer::acceptTmoMax, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(           EsSocketServer, handlerBuffLen, ulong, NO_PROPERTY_LABEL, ::EsSocketServer::buffLenDef, ::EsSocketServer::buffLenMin, ::EsSocketServer::buffLenMax, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(           EsSocketServer, handlerTtl, ulong, NO_PROPERTY_LABEL, ::EsSocketServer::ttlDef, ::EsSocketServer::ttlMin, ::EsSocketServer::ttlMax, NO_PROPERTY_DESCR)
	ES_DECL_PROP_INFO_RANGE_PERSISTENT(           EsSocketServer, packetTimeout, ulong, NO_PROPERTY_LABEL, ::EsSocketServer::packetTmoDef, ::EsSocketServer::packetTmoMin, ::EsSocketServer::packetTmoMax, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

} // namespace EsReflection
//---------------------------------------------------------------------------

EsSocketServerReflected::EsSocketServer() :
m_srv(*this)
{
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

EsVariant EsSocketServerReflected::NEW(ulong type)
{
#ifdef ES_MODERN_CPP
	std::unique_ptr<EsSocketServer> p = std::make_unique<EsSocketServer>();
#else
	std::unique_ptr<EsSocketServer> p( new EsSocketServer );
#endif

	ES_ASSERT(p.get());
	p->m_dynamic = true;

  p->set_socketType(type);

	return p.release()->asBaseIntfPtrDirect();
}
//---------------------------------------------------------------------------

bool EsSocketServerReflected::open()
{
  return m_srv.open(
    m_addr,
    static_cast< ::EsSocketType >(m_socketType)
  );
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::close()
{
  m_srv.close(
    true, //< Always try to close gracefully
    m_doThrow
  );
}
//---------------------------------------------------------------------------

EsVariant EsSocketServerReflected::send(cr_EsVariant vdata, cr_EsVariant vtmo)
{
  const EsBinBuffer& bb = vdata.asBinBuffer();

  return m_srv.send(
    bb,
    vtmo.asULong(),
    m_doThrow
  );
}
//---------------------------------------------------------------------------

EsVariant EsSocketServerReflected::send(cr_EsVariant vaddr, cr_EsVariant vdata, cr_EsVariant vtmo)
{
  EsSocketAddr* addr = EsSocketAddr::fromVariant(vaddr);
  ES_ASSERT(addr);

  const EsBinBuffer& bb = vdata.asBinBuffer();

  return m_srv.sendTo(
    *addr,
    bb,
    vtmo.asULong(),
    m_doThrow
  );
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::clientDisconnect(cr_EsVariant vaddr, cr_EsVariant noFarewell)
{
  EsSocketAddr* addr = EsSocketAddr::fromVariant(vaddr);
  ES_ASSERT(addr);

  m_srv.disconnectedSet(
    *addr,
    noFarewell.asBool()
  );
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::reset()
{
  m_srv.reset();
}
//---------------------------------------------------------------------------

bool EsSocketServerReflected::get_isOk() const
{
  return m_srv.isOk();
}
//---------------------------------------------------------------------------

bool EsSocketServerReflected::get_isActive() const
{
  return m_srv.isActive();
}
//---------------------------------------------------------------------------

long EsSocketServerReflected::get_error() const
{
  return m_srv.errorGet();
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_clientsCount() const
{
  return m_srv.clientsCountGet();
}
//---------------------------------------------------------------------------

EsVariant EsSocketServerReflected::get_clientAddresses() const
{
  EsVariant::Array result;

  const EsSocketAddresses& addrs = m_srv.clientAddressesGet();
  result.reserve( addrs.size() );

#ifdef ES_MODERN_CPP
  for( auto const &v: addrs )
  {
#else
  size_t cnt = addrs.size();
  for(size_t idx = 0; idx < cnt; ++idx )
  {
    const EsSocketAddr& v = addrs[idx];
#endif

    result.push_back(
      v.clone()
    );
  }

  return result;
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_ioHandler(const EsVariant& handler)
{
  EsCriticalSectionLocker locker( m_cs );
  m_ioHandler = handler;
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_socketType() const
{
  return m_socketType;
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_socketType(const ulong& type)
{
  m_socketType = type;
}
//---------------------------------------------------------------------------

EsVariant EsSocketServerReflected::get_address() const
{
  return m_addr.clone();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_address(const EsVariant& vaddr)
{
  EsSocketAddr* addr = EsSocketAddr::fromVariant(vaddr);
  ES_ASSERT(addr);

  m_addr = *addr;
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_maxIncomingConnections() const
{
  return m_srv.listenerMaxIncomingConnGet();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_maxIncomingConnections(const ulong& cnt)
{
  m_srv.listenerMaxIncomingConnSet(cnt);
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_listenerBuffLen() const
{
  return m_srv.listenerBuffLenGet();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_listenerBuffLen(const ulong& len)
{
  m_srv.listenerBuffLenSet(len);
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_listenerAcceptTmo() const
{
  return m_srv.listenerAcceptTmoGet();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_listenerAcceptTmo(const ulong& tmo)
{
  m_srv.listenerAcceptTmoSet(tmo);
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_handlerBuffLen() const
{
  return m_srv.handlerBuffLenGet();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_handlerBuffLen(const ulong& len)
{
  m_srv.handlerBuffLenSet(len);
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_handlerTtl() const
{
  return m_srv.handlerTtlGet();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_handlerTtl(const ulong& ttl)
{
  m_srv.handlerTtlSet(ttl);
}
//---------------------------------------------------------------------------

ulong EsSocketServerReflected::get_packetTimeout() const
{
  return m_srv.packetTmoGet();
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_packetTimeout(const ulong& tmo)
{
  m_srv.packetTmoSet(tmo);
}
//---------------------------------------------------------------------------

bool EsSocketServerReflected::get_doThrow() const
{
  return m_doThrow;
}
//---------------------------------------------------------------------------

void EsSocketServerReflected::set_doThrow(const bool& doThrow)
{
  EsCriticalSectionLocker locker( m_cs );
  m_doThrow = doThrow;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_SOCKETS

