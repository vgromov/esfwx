// testEsOsInfo.cc file
// UART channel basic tests
//

#if ES_OS == ES_OS_MAC

TEST(MAC_OS_EsUartTest, Enumeration) {

  EsUartEnumerator enu;
  enu.enumerate();
  const EsUartInfos& infos = enu.resultGet();

  PRINTF("%d UARTs found\n", infos.size());
  for(size_t idx = 0; idx < infos.size(); ++idx )
  {
    EsUartInfo::Ptr info = infos[idx];
    PRINTF("UART[%d]:\n", idx);

    PRINTF("Is Modem: %s\n", info->isModem() ? "yes" : "no");
    PRINTF("Is USB: %s\n", info->isUSBdevice() ? "yes" : "no");
    PRINTF("Path: '%s'\n", EsString::toUtf8(info->devicePathGet()).c_str() );
    PRINTF("Name: '%s'\n", EsString::toUtf8(info->nameGet()).c_str() );
    PRINTF("Friendly name: '%s'\n", EsString::toUtf8(info->friendlyNameGet()).c_str() );
    PRINTF("Instance path: '%s'\n\n", EsString::toUtf8(info->instancePathGet()).c_str() );
  }
}

// MAC OS test expects, that device like LUPIN is attached,
// providing steady incoming flow of bytes
//

TEST(MAC_OS_EsUartTest, Communication) {

  EsChannelIoIntf::Ptr ch = EsChannelIoFactory::channelCreate(esT("EsChannelIoUart"));
  EsReflectedClassIntf::Ptr rch = ch;

  EsUartEnumerator enu;
  enu.enumerate();
  const EsUartInfos& infos = enu.resultGet();

  ASSERT_TRUE( !infos.empty() );

  EXPECT_NO_THROW(rch->propertySet(esT("port"), infos[0]->devicePathGet() ));
  EXPECT_NO_THROW(rch->propertySet(esT("baud"), 115200));

  EXPECT_NO_THROW(ch->open());
  ASSERT_TRUE(ch->isOpen());

  ulong toRead = 24000;
  ulong tmo = 2000;

  PRINTF("Reading %d bytes\n", toRead);

  EsBinBuffer bb;
  EXPECT_NO_THROW(bb = ch->bytesGet(toRead, tmo));
  PRINTF("Read %d out of %d bytes in %d ms\n", bb.size(), toRead, tmo);

  if( bb.empty() )
    PRINTF("No data received in %d ms, check if dumb Lupin is attached, not smart eLight\n", tmo);

  ASSERT_TRUE(toRead == bb.size());
  EXPECT_NO_THROW(ch->close());

  EsBinBuffer marker;
  marker.append(12, (EsBinBuffer::const_pointer)"OSCILLOSCOPE");
  ASSERT_TRUE( EsBinBuffer::npos != bb.search(marker) );
}

#endif
