// EsBluetooth sub-system tests testEsBluetooth.cc
//

TEST(EsBluetoothTest, Basics) {

  ASSERT_TRUE( EsBluetooth::initialized() );

  EsBluetoothRadioEnumerator btrenum;
  btrenum.execute();
  const EsBluetoothRadioInfos& rinfos = btrenum.resultGet();
  for(size_t idx = 0; idx < rinfos.size(); ++idx)
  {
    const EsBluetoothRadioInfo& info = rinfos[idx];
    esCout << esT("Radio: ") << info.nameGet().c_str() << std::endl
      << esT(" Address: ") << info.addrGet().c_str() << std::endl
      << esT(" Class: ") << info.classGet() << std::endl
      << esT(" Class string: ") << info.classGet().asString().c_str() << std::endl
      << esT(" Mfg: ") << info.manufacturerGet() << std::endl
      << esT(" Mfg string: ") << info.manufacturerStringGet().c_str() << std::endl;
  }

  EsBluetoothDeviceEnumerator btenum( EsBluetoothDeviceFlag::All );
  btenum.execute();
  const EsBluetoothDeviceInfos& infos = btenum.resultGet();

  for(size_t idx = 0; idx < infos.size(); ++idx)
  {
    const EsBluetoothDeviceInfo& info = infos[idx];
    esCout << esT("Device: ") << info.nameGet().c_str() << std::endl
      << esT(" Address: ") << info.addrGet().c_str() << std::endl
      << esT(" Connected: ") << info.isConnected() << std::endl
      << esT(" Remembered: ") << info.isRemembered() << std::endl
      << esT(" Authenticated: ") << info.isAuthenticated() << std::endl
      << esT(" Class: ") << info.classGet() << std::endl
      << esT(" Class string: ") << info.classGet().asString().c_str() << std::endl
      << esT(" Last seen at: ") << info.lastSeenGet().asString().c_str() << std::endl
      << esT(" Last used at: ") << info.lastUsedGet().asString().c_str() << std::endl;

    const EsBluetoothServiceUUIDs& uuids = info.servicesGet();
    if( !uuids.empty() )
    {
      esCout << esT(" Device service UUIDs:") << std::endl;

      for(size_t sidx = 0; sidx < uuids.size(); ++sidx)
      {
        const EsBluetoothServiceUUID& uuid = uuids[sidx];
        EsString classification = esT("(Unclassified)");
        if( uuid.isProtocol() )
          classification = esT("(Protocol)");
        else if( uuid.isService() )
        {
          classification = esT("(");
          EsString tmp;
          if( uuid.isServiceClass() )
            tmp = esT("Service");

          if( uuid.isServiceProfile() )
          {
            if( !tmp.empty() )
              tmp += esT("/");
            tmp += esT("Profile");
          }
          classification += tmp + esT(")");
        }

        esCout << esT(" UUID: ") << uuid.asString().c_str() <<
          esT(" ") << classification.c_str() <<
          esT("; Name: ") << uuid.nameGet().c_str() << std::endl;
      }
    }
  }
}

