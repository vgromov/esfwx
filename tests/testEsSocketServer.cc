// Socket Connection aware server and client test
//
static const EsByteString cs_bye = {"BYE!"};

class ServerTest : public EsSocketServer
{
protected:
  virtual void onClientCannotBeAdded(const EsSocketAddr& addr) const
  {
    ES_DEBUG_TRACE(
      esT("Client with address %s cannot be added"),
      addr.asRawString().c_str()
    );
  }

  virtual void onClientAdding(const EsSocketAddr& addr)
  {
    ES_DEBUG_TRACE(
      esT("Adding Client with address %s"),
      addr.asRawString().c_str()
    );
  }

  virtual void onClientAdded(const EsSocketAddr& addr)
  {
    ES_DEBUG_TRACE(
      esT("Client with address %s added, client count reached %d"),
      addr.asRawString().c_str(),
      clientsCountGet()
    );
  }

  virtual void onClientRemoving(const EsSocketAddr& addr)
  {
    ES_DEBUG_TRACE(
      esT("Removing Client with address %s"),
      addr.asRawString().c_str()
    );
  }

  virtual void onClientRemoved(const EsSocketAddr& addr)
  {
    ES_DEBUG_TRACE(
      esT("Client with address %s removed. Clients count is %d"),
      addr.asRawString().c_str(),
      clientsCountGet()
    );
  }

  virtual void onClientDisconnected(const EsSocketAddr& addr)
  {
    ES_DEBUG_TRACE(
      esT("Client with address %s was disconnected"),
      addr.asRawString().c_str()
    );
  }

  virtual void onClientClosing(const EsSocketAddr& addr, bool noFarewell, EsSocket::Impl& io)
  {
    if( noFarewell )
    {
      ES_DEBUG_TRACE(
        esT("Closing connection to Client %s, no farewell needed!"),
        addr.asRawString().c_str()
      );

      return;
    }

    const EsString& bye = EsString::format(
      esT("Bye, Client '%s'!"),
      addr.asRawString().c_str()
    );

    ES_DEBUG_TRACE(
      esT("Closing connection to Client %s, sending farewell='%s'!"),
      addr.asRawString().c_str(),
      bye.c_str()
    );

    const EsByteString& bs = EsString::toUtf8(bye);

    io.send(
      reinterpret_cast<EsBinBuffer::const_pointer>(bs.c_str()),
      bs.size(),
      0,
      false
    );
  }

  virtual void onClientClosed(const EsSocketAddr& addr)
  {
    ES_DEBUG_TRACE(
      esT("Connection to Client %s is closed"),
      addr.asRawString().c_str()
    );
  }

  virtual void onClientDataReceived(const EsSocketAddr& addr, EsBinBuffer::const_pointer data, ulong len, EsBinBuffer& response)
  {
    if( 0 == cs_bye.compare(0, EsByteString::npos, reinterpret_cast<EsByteString::const_pointer>(data), len) )
    {
      ES_DEBUG_TRACE(
        esT("Farewell received from client %s, schedule it for closing"),
        addr.asRawString().c_str()
      );

      disconnectedSet(addr);
      return;
    }

    ES_DEBUG_TRACE(
      esT("Data received from Client %s, echoing immediately"),
      addr.asRawString().c_str()
    );

    response.assign(
      data,
      data+len
    );
  }
};
//---------------------------------------------------------------------------

class ClientTest : public EsSocketClient
{
public:
  typedef std::shared_ptr<ClientTest> Ptr;

protected:
  virtual void onSendFarewell(EsSocket::Impl& io)
  {
    ES_DEBUG_TRACE(esT("Sending farewell to the server"));

    io.send(
      reinterpret_cast<EsBinBuffer::const_pointer>(cs_bye.c_str()),
      cs_bye.size(),
      0,
      false
    );
  }

  virtual void onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len)
  {
    EsByteString bs(data, data+len);

    const EsString& bye = EsString::fromUtf8(
      bs
    );

    ES_DEBUG_TRACE(
      esT("Farewell received from server: '%s'"),
      bye.c_str()
    );
  }
};
//---------------------------------------------------------------------------

TEST(EsSocketServerTest, ConnectionAwareServerClient) {

  static const size_t sc_cliCnt = 50;

  ServerTest srv;
  EsSocketAddr srvAddr(
    EsSocketAddrType::IPV4,
    esT("127.0.0.1"),
    5000
  );

  srv.open(
    srvAddr,
    EsSocketType::ConnectionOriented
  );

  EXPECT_TRUE( srv.isActive() );

  PRINTF("Creating client connections\n");

  /// Create clients
  std::vector<ClientTest::Ptr> clients;
  clients.reserve( sc_cliCnt );

  for(size_t idx = 0; idx < sc_cliCnt; ++idx)
  {
    ClientTest::Ptr cli = std::make_shared<ClientTest>();
    ES_ASSERT(cli);

    clients.push_back(
      cli
    );

    cli->open(
      srvAddr,
      EsSocketType::ConnectionOriented
    );

    EXPECT_TRUE( cli->isActive() );
  }

  PRINTF("Sleeping for 10s...\n");
  EsThread::sleep(10000);

  PRINTF("Sending data to clients, receive echo from server\n");

  // Send some data to server on client connection, get echo back
  for( size_t idx = 0; idx < sc_cliCnt; ++idx )
  {
    EsBinBuffer::const_pointer raw = reinterpret_cast<EsBinBuffer::const_pointer>(&idx);
    EsBinBuffer bb(
      raw,
      raw+sizeof(idx)
    );

    ClientTest::Ptr cli = clients[idx];
    ulong sent = cli->send(
      bb,
      1000,
      false
    );

    EXPECT_TRUE( sent == bb.size() );

    const EsBinBuffer& echo = cli->receive(
      bb.size(),
      1000,
      false
    );

    EXPECT_TRUE( echo == bb );
  }

  PRINTF("Sleeping 10s...\n");
  EsThread::sleep(10000);

  ulong cnt = srv.clientsCountGet();

  /// Check all clients have connected OK
  EXPECT_TRUE( clients.size() == cnt );

  PRINTF("Closing clients, from client side\n");

  /// Close clients first
  for( size_t idx = 0; idx < sc_cliCnt; ++idx )
  {
    ClientTest::Ptr cli = clients[idx];

    cli->close();
  }
  clients.clear(); //< Final cleanup

  PRINTF("Sleeping 10s...\n");
  EsThread::sleep(10000);

  /// Check all clients have disconnected
  EXPECT_TRUE( 0 == srv.clientsCountGet() );

  /// Close server last
  srv.close();
}

