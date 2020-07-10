// socket stuff tests
//
TEST(EsSocketTest, IANA_Registry) {

  const EsIanaRegistry::Node& node1 = EsIanaRegistry::lookupByService(esT("FTP"));
  ASSERT_TRUE(node1.portGet() == 21);
  ASSERT_TRUE(node1.serviceGet() == esT("ftp"));

  const EsIanaRegistry::Node& node2 = EsIanaRegistry::lookupByService(esT("routed"));
  ASSERT_TRUE(node2.portGet() == 520);
  ASSERT_TRUE(node2.serviceGet() == esT("router"));

  const EsIanaRegistry::Node& node3 = EsIanaRegistry::lookupByService(esT("dhcpc"));
  ASSERT_TRUE(node3.portGet() == 68);
  ASSERT_TRUE(node3.serviceGet() == esT("bootpc"));

  const EsIanaRegistry::Node& node4 = EsIanaRegistry::lookupByService(esT("pupkinzon"));
  EXPECT_FALSE( node4.isOk() );
}

TEST(EsSocketTest, AddressIpV4) {

  EsSocketAddr ipv4(
    EsSocketAddrType::IPV4,
    esT("127.0.0.1"),
    21
  );

  EsString addr = ipv4.nameGet();
  EsString svc = ipv4.serviceGet();

  EXPECT_TRUE( ipv4.isOk() );

  ipv4.errorReset();
  EXPECT_TRUE( addr == esT("localhost") );
  EXPECT_TRUE( svc == esT("ftp") );

  ipv4.nameSet( esT("localhost") );
  addr = ipv4.rawGet();

  ASSERT_TRUE( addr == esT("127.0.0.1") );

  ipv4.serviceSet( esT("ftp") );
  ASSERT_TRUE( ipv4.portGet() == 21 );

  EsSocketAddr ipv4_1(
    EsSocketAddrType::IPV4,
    esT("127.0.0.1"),
    443
  );

  ASSERT_NE( ipv4, ipv4_1 );

  addr = ipv4.nameGet();
  const EsString& name1 = ipv4_1.nameGet();
  const EsString& svc1 = ipv4_1.serviceGet();

  EXPECT_TRUE( addr == name1 );
  EXPECT_TRUE( svc1 == esT("https") );
}

#ifdef ES_USE_BT_TESTS
TEST(EsSocketTest, SocketBT) {

  const EsString& addr = //esT("E4:B0:21:AA:4D:25"); //< My smartphone with SPP service moniker
    esT("00:01:95:15:36:48"); //< Parani BT module test
  EsBluetoothServiceUUID svcuuid( EsBluetoothServiceUUID::UUID16::SerialPort );
  EsSocketAddr bta(
    EsSocketAddrType::Bluetooth,
    addr
  );
  bta.serviceSet( svcuuid.asString() );

  EXPECT_TRUE( bta.nameGet() == esT("Ekosf Test Bluetooth") );
  //EXPECT_TRUE( bta.nameGet() == esT("GT-N7000") );
  ASSERT_TRUE( bta.isOk() );

  EsSocketClient btcli;
  if( btcli.open(bta) )
  {
    static const char c_data[] = "Hello over BT socket!";
    ulong sent = btcli.send((const esU8*)c_data, sizeof(c_data));
    EXPECT_EQ(sent, sizeof(c_data));
    btcli.close();
  }
  else
    EsSockets::errorCheck( btcli.errorGet() );
}
#endif
