
static const GUID c_key = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};

TEST(MacosFileTest, Basics) {

  EsPath path = EsPath::create(
    EsPath::stdDocsGet(),
    esT("eap"),
    esT("cesse")
  );

  EsFile file(
    path.pathGet(),
    static_cast<ulong>(EsFileFlag::Read)
  );

  EXPECT_TRUE(file.open());

  if( file.get_opened() )
  {
     const EsString& k = EsUtilities::GUIDtoStr(c_key);
    EXPECT_TRUE( !k.empty() );

    ullong len = file.get_length();

    const EsBinBuffer& e = file.readAllAsBinBuffer();
    EXPECT_TRUE( !e.empty() );

    const EsBinBuffer& compiled = EsUtilities::eToB(e, k);
    EXPECT_TRUE( !compiled.empty() );
  }

  file.close();
}

class EsTestEnumerator : public EsPathEnumerator
{
public:
  EsTestEnumerator() :
  EsPathEnumerator( EsPath::stdDocsGet() )
  {}

  const EsString::Array& objsGet() const
  {
    return m_objs;
  }

protected:
  virtual void onStart()
  {
    m_objs.clear();
  }

  virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
  {
    if(
      !isDir &&
      0 == name.find( esT("EL3C_") ) //< Starts with desired substring
    )
      m_objs.push_back( curPath + name );

    return true;
  }

private:
  EsString::Array m_objs;
};

TEST(EsPathEnumeratorTest, Basics) {

  EsTestEnumerator tenum;
  tenum.execute();
  const EsString::Array& objs = tenum.objsGet();

  ASSERT_TRUE(3 == objs.size());
}

TEST(EsPathEnumeratorTest, Recursion) {

  EsTestEnumerator tenum;
  tenum.execute( static_cast<ulong>(EsPathEnumeratorFlag::Recursive) );
  const EsString::Array& objs = tenum.objsGet();

  ASSERT_TRUE(4 == objs.size());
}

