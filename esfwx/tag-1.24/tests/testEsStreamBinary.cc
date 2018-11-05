// binary stream serialization tests
//

class EsStreamBinaryTest0 : public ::testing::Test
{
protected:
  void SetUp()
  {
    const EsPath& allScript = EsPath::create(
      EsPath::stdDocsGet(),
      esT("xmltestadvanced"),
      esT("cesse")
    );

    script = EsScript::create();

    static const GUID c_key = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};

    script->compiledBinaryLoad(
      allScript.pathGet(),
      true,
      EsUtilities::GUIDtoStr(c_key)
    );

    // create read-only XML file stream
    const EsPath& sessTest = EsPath::create(
      EsPath::stdDocsGet(),
      esT("EL3C_CalibrationSession_new"),
      esT("xml")
    );

    streamw = EsStreamXml::create(
      static_cast<ulong>(EsStreamFlag::Read)|
      static_cast<ulong>(EsStreamFlag::File),
      sessTest.pathGet(),
      script
    );

    if( streamw->rootObjectTypeEntryLocate(esT("EL3C_CalibrationSession"), false) )
    {
      sobj = streamw->objectCreate();

      const EsPath& binTest = EsPath::create(
        EsPath::stdDocsGet(),
        esT("EL3C_CalibrationSession_new"),
        esT("bin")
      );

      if( binTest.fileExists() )
        EsPath::fileRemove( binTest.pathGet() );

      streamb = EsStreamBinary::create(
        static_cast<ulong>(EsStreamFlag::Write)|
        static_cast<ulong>(EsStreamFlag::Compressed)|
        static_cast<ulong>(EsStreamFlag::File),
        binTest.pathGet(),
        script
      );
    }
  }

  void TearDown()
  {
    streamw.reset();
    streamb.reset();
    sobj.reset();
    script.reset();
  }

protected:
  EsStreamIntf::Ptr streamw;
  EsStreamIntf::Ptr streamb;
  EsBaseIntfPtr sobj;
  EsScriptIntf::Ptr script;
};

TEST_F(EsStreamBinaryTest0, BinaryStreamObjectWrite) {

  ASSERT_TRUE( streamb );

  const EsDateTime& dtstart = EsDateTime::now();

  ASSERT_NO_THROW(
    streamb->objectWrite( sobj )
  );

  const EsTimeSpan& ts = EsDateTime::now()-dtstart;
  const EsByteString& bs = EsString::toUtf8(
    EsString::format(
      esT("%llu ms"),
      ts.get_allMilliseconds()
    )
  );

  PRINTF( bs.c_str() );
}
//---------------------------------------------------------------------------

class EsStreamBinaryTest1 : public ::testing::Test
{
protected:
  void SetUp()
  {
    const EsPath& allScript = EsPath::create(
      EsPath::stdDocsGet(),
      esT("xmltestadvanced"),
      esT("cesse")
    );

    script = EsScript::create();

    static const GUID c_key = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};

    script->compiledBinaryLoad(
      allScript.pathGet(),
      true,
      EsUtilities::GUIDtoStr(c_key)
    );

    const EsPath& binTest = EsPath::create(
      EsPath::stdDocsGet(),
      esT("EL3C_CalibrationSession_new"),
      esT("bin")
    );

    streamb = EsStreamBinary::create(
      static_cast<ulong>(EsStreamFlag::Read)|
      static_cast<ulong>(EsStreamFlag::File),
      binTest.pathGet(),
      script
    );
  }

  void TearDown()
  {
    streamb.reset();
    sobj.reset();
    script.reset();
  }

protected:
  EsStreamIntf::Ptr streamb;
  EsBaseIntfPtr sobj;
  EsScriptIntf::Ptr script;
};

TEST_F(EsStreamBinaryTest1, BinaryStreamObjectRead) {

  if( streamb->rootObjectTypeEntryLocate(esT("EL3C_CalibrationSession"), false) )
  {
    const EsDateTime& dtstart = EsDateTime::now();

    EsBaseIntfPtr sobj;
    ASSERT_NO_THROW(
      sobj = streamb->objectCreate()
    );

    ASSERT_TRUE( sobj );

    const EsTimeSpan& ts = EsDateTime::now()-dtstart;
    const EsByteString& bs = EsString::toUtf8(
      EsString::format(
        esT("%llu ms"),
        ts.get_allMilliseconds()
      )
    );

    PRINTF( bs.c_str() );
  }
}
//---------------------------------------------------------------------------

class EsStreamBinaryTest2 : public ::testing::Test
{
protected:
  void SetUp()
  {
    const EsPath& binTest = EsPath::create(
      EsPath::stdDocsGet(),
      esT("EL3C_CalibrationSession_new"),
      esT("bin")
    );

    streamb = EsStreamBinary::create(
      static_cast<ulong>(EsStreamFlag::Read)|
      static_cast<ulong>(EsStreamFlag::File),
      binTest.pathGet()
    );

    const EsPath& xmlTest = EsPath::create(
      EsPath::stdDocsGet(),
      esT("EL3C_CalibrationSession_new3"),
      esT("xml")
    );

    streamw = EsStreamXml::create(
      static_cast<ulong>(EsStreamFlag::Write)|
      static_cast<ulong>(EsStreamFlag::File),
      xmlTest.pathGet()
    );
  }

  void TearDown()
  {
    streamb.reset();
    streamw.reset();
  }

protected:
  EsStreamIntf::Ptr streamb;
  EsStreamIntf::Ptr streamw;
};

TEST_F(EsStreamBinaryTest2, Stream2StreamCopy) {

  const EsDateTime& dtstart = EsDateTime::now();

  ASSERT_NO_THROW(
    streamw->fromStream(
      streamb
    )
  );

  const EsTimeSpan& ts = EsDateTime::now()-dtstart;
  const EsByteString& bs = EsString::toUtf8(
    EsString::format(
      esT("%llu ms"),
      ts.get_allMilliseconds()
    )
  );

  PRINTF( bs.c_str() );
}
//---------------------------------------------------------------------------

