﻿// XML and serialization tests
//

class EsStreamXmlTest0 : public ::testing::Test
{
protected:
  void SetUp()
  {
    // create write-only XML file stream
    const EsPath& path = EsPath::create(
      EsPath::stdDocsGet(),
      esT("contextsXmlTest"),
      esT("xml")
    );

    streamPath = path.pathGet();

    setUp();
  }

  void TearDown()
  {
    streamw.reset();
  }

  void stringsSetup()
  {
    strEng = esT("Some <> &lt; &amp; string;");

    strEngCesc = esT("Some other string\n")
      esT("Multiline1\x0A")
      esT("Multiline2");

    strRu0 = EsString(
      esT8("Русская строка, ? : <> = -+ () & # @ тест"),
      true
    );
  }

  virtual void setUp()
  {
    if( EsPath::fileExists(streamPath, EsString::null()) )
      EsFile::remove(streamPath);

    stringsSetup();
  }

protected:
  EsStreamIntf::Ptr streamw;
  EsString streamPath;
  EsString strEng;
  EsString strEngCesc;
  EsString strRu0;
};

TEST_F(EsStreamXmlTest0, ContextsWrite) {

  // create write-only XML file stream
  ASSERT_NO_THROW( streamw = EsStreamXml::create(
    static_cast<ulong>(EsStreamFlag::Write)|
    static_cast<ulong>(EsStreamFlag::File),
    streamPath
    )
  );
  ASSERT_TRUE( streamw );

  // context tests
  EXPECT_THROW(streamw->contextOpenCreate(esT("ROOT")), EsException);

  // values - only context
  EXPECT_NO_THROW(streamw->contextOpenCreate(esT("ctx1")));
  EXPECT_NO_THROW(streamw->valueWrite(esT("value_float"), 123.13));
  EXPECT_NO_THROW(streamw->valueWrite(esT("value_str"), strEng));
  EXPECT_NO_THROW(streamw->valueWrite(esT("value_int"), 1356));
  EXPECT_NO_THROW(streamw->contextCurrentClose());
  // write some value at root context
  EXPECT_NO_THROW(streamw->valueWrite(esT("value_float"), 13.138903));
  // try to read write-only stream
  EXPECT_THROW(streamw->valueRead(esT("value_float")), EsException);

  // mixed value-object context
  EXPECT_NO_THROW(streamw->contextOpenCreate(esT("ctx2")));
  EXPECT_NO_THROW(streamw->valueWrite(esT("value_str"), strEngCesc));
  EXPECT_NO_THROW(streamw->valueWrite(esT("value_int"), 9998));

  EsChannelIoIntf::Ptr ch;
  ASSERT_NO_THROW(ch = EsChannelIoFactory::channelCreate(esT("EsChannelIoUart")));
  ASSERT_TRUE(ch);
  EXPECT_NO_THROW(streamw->objectWrite(ch));

  EXPECT_NO_THROW(streamw->valueWrite(esT("value_float"), 99.9876));
  EXPECT_NO_THROW(streamw->valueWrite(esT("val_str"), strRu0));
  EXPECT_NO_THROW(streamw->contextCurrentClose());
}
//---------------------------------------------------------------------------

class EsStreamXmlTest1 : public EsStreamXmlTest0
{
protected:
  virtual void setUp()
  { // base class override - do not erase pre-created xml stream on startup
    stringsSetup();
  }
};

TEST_F(EsStreamXmlTest1, ContextsRead) {

  ASSERT_NO_THROW(
    streamw = EsStreamXml::create(
      static_cast<ulong>(EsStreamFlag::Read)|
      static_cast<ulong>(EsStreamFlag::File),
      streamPath
    )
  );
  ASSERT_TRUE( streamw );
  EXPECT_NO_THROW(streamw->contextOpenExisting(esT("ctx1")));

  EsString str;
  EXPECT_NO_THROW(str = streamw->valueRead(esT("value_str")).asString());
  EXPECT_TRUE(str == strEng);

  EXPECT_NO_THROW(streamw->contextCurrentClose());

  EXPECT_NO_THROW(streamw->contextOpenExisting(esT("ctx2")));

  EXPECT_NO_THROW(str = streamw->valueRead(esT("value_str")).asString());
  EXPECT_TRUE(str == strEngCesc);

  EsChannelIoIntf::Ptr ch;
  ASSERT_NO_THROW(ch = EsChannelIoFactory::channelCreate(esT("EsChannelIoUart")));
  ASSERT_TRUE(ch);
  ASSERT_TRUE(streamw->rootObjectTypeEntryLocate(ch->typeNameGet(), false));
  EXPECT_NO_THROW(streamw->objectRead(ch));

  EXPECT_NO_THROW(str = streamw->valueRead(esT("val_str")).asString());
  EXPECT_TRUE(str == strRu0);
  EXPECT_NO_THROW(streamw->contextCurrentClose());
}
//---------------------------------------------------------------------------

class EsStreamXmlTest2 : public ::testing::Test
{
protected:
  void SetUp()
  {
    setUp();
  }

  void TearDown()
  {
    tearDown();
  }

  virtual void setUp()
  {
    // create read-write XML file stream
    const EsPath& path = EsPath::create(
      EsPath::stdDocsGet(),
      esT("streamXmlTestAdvanced"),
      esT("xml")
    );
    const EsString& xmlFile = path.pathGet();

    streamw = EsStreamXml::create(
      static_cast<ulong>(EsStreamFlag::Read)|
      static_cast<ulong>(EsStreamFlag::Write)|
      static_cast<ulong>(EsStreamFlag::File)|
      static_cast<ulong>(EsStreamFlag::NonStrictFields)|
      static_cast<ulong>(EsStreamFlag::NonStrictProperties),
      xmlFile
    );

    const EsPath& allScript = EsPath::create(
      EsPath::stdDocsGet(),
      esT("xmltestadvanced"),
      esT("cesse")
    );

    script = EsScript::create();
    ES_ASSERT(script);

    static const GUID sc_key = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};

    const EsString& scriptPath = allScript.pathGet();
    script->compiledBinaryLoad(
      scriptPath,
      true,
      EsUtilities::GUIDtoStr(sc_key)
    );

    streamw->factorySet(script);
  }

  virtual void tearDown()
  {
    streamw.reset();
    script.reset();
  }

protected:
  EsStreamIntf::Ptr streamw;
  EsScriptIntf::Ptr script;
};

TEST_F(EsStreamXmlTest2, ReSaveStream) {

  const EsPath& xmlTestRootNew = EsPath::create(
    EsPath::stdDocsGet(),
    esT("streamXmlTestAdvanced_new"),
    esT("xml")
  );

  EsVariant var = streamw;
  ASSERT_NO_THROW(
    var.call(
      esT("save"),
      xmlTestRootNew.pathGet()
    )
  );
}

TEST_F(EsStreamXmlTest2, ObjectCreateFromStream)
{
  if( streamw->rootObjectTypeEntryLocate(esT("EL3C_CalibrationSession"), false) )
  {
    EsBaseIntfPtr sobj;
    ASSERT_NO_THROW(
      sobj = streamw->objectCreate()
    );
  }
}
//---------------------------------------------------------------------------

class EsStreamXmlTest3 : public EsStreamXmlTest2
{
protected:
  virtual void setUp()
  {
    EsStreamXmlTest2::setUp();

    if( streamw->rootObjectTypeEntryLocate(esT("EL3C_CalibrationSession"), false) )
      sobj = streamw->objectCreate();

    const EsPath& sessTestRoot = EsPath::create(
      EsPath::stdDocsGet(),
      esT("EL3C_CalibrationSession_new"),
      esT("xml")
    );

    streamw = EsStreamXml::create(
      static_cast<ulong>(EsStreamFlag::Write)|
      static_cast<ulong>(EsStreamFlag::File),
      sessTestRoot.pathGet(),
      script
    );
  }

  virtual void tearDown()
  {
    sobj.reset();
    EsStreamXmlTest2::tearDown();
  }

protected:
  EsBaseIntfPtr sobj;
};

TEST_F(EsStreamXmlTest3, ObjectWriteToStream)
{
  ASSERT_NO_THROW(
    streamw->objectWrite( sobj )
  );
}
//---------------------------------------------------------------------------


