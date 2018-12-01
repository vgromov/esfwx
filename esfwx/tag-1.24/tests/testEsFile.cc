// file stuff tests
//

TEST(EsPathTest, Basics) {
  const EsString& binRoot = binRootGet();
  PRINTF("binRoot=%s\n", EsString::toUtf8(binRoot).c_str() );

  EsPath p1 = EsPath::createFromFilePath(binRoot+esT("file_1.txt"));

  ASSERT_TRUE(p1.isOk());
  EXPECT_TRUE(p1.isAbsolute());
  ASSERT_TRUE(p1.fileNameExtGet() == esT("file_1.txt"));
  ASSERT_TRUE(p1.fileNameGet() == esT("file_1"));
  ASSERT_TRUE(p1.fileExtGet() == esT("txt"));

  EsString path = p1.pathGet();
  EsPath p2 = EsPath::createFromFilePath(path);
  ASSERT_TRUE(p2.isOk());
  EXPECT_TRUE(p2.isAbsolute());
  p1 = p2;
  p2.dirLastRemove();
  p2.dirLastRemove();

  path = p1.pathGet(
    static_cast<ulong>(EsPathFlag::Relative),
    p2.pathGet(
      static_cast<ulong>(EsPathFlag::Default)|
      static_cast<ulong>(EsPathFlag::ExcludeFile)
    )
  );
  p2 = EsPath::createFromFilePath(path);
  ASSERT_TRUE(p2.isOk());
  EXPECT_FALSE(p2.isAbsolute());
  ASSERT_TRUE(p2.isFile());
  ASSERT_TRUE(p2.fileNameExtGet() == esT("file_1.txt"));
  ASSERT_TRUE(p2.fileNameGet() == esT("file_1"));
  ASSERT_TRUE(p2.fileExtGet() == esT("txt"));

  path = p2.pathGet(
    static_cast<ulong>(EsPathFlag::Relative)|
    static_cast<ulong>(EsPathFlag::ExcludeFile)
  );
  p2 = EsPath::createFromPath(path);
  ASSERT_TRUE(p2.isOk());
  EXPECT_FALSE(p2.isAbsolute());
  ASSERT_FALSE(p2.isFile());

#if ES_OS == ES_OS_WINDOWS
  path = esT("$ComSpec");
  p2 = EsPath::createFromFilePath(path);
  path = p2.pathGet();
  const EsString& comspecPath = EsUtilities::environmentVarsListGet().valueGet(esT("ComSpec")).asString();
  ASSERT_TRUE(p2.isOk());
  EXPECT_TRUE(path == comspecPath);
  EXPECT_TRUE(p2.fileExists());
  EXPECT_FALSE(p2.dirExists());

  path = esT("$ProgramFiles/Internet Explorer/iexplore.exe");
  p2 = EsPath::createFromFilePath(path);
  path = p2.pathGet();
  ASSERT_TRUE(p2.isOk());
  EXPECT_TRUE(p2.isFile());
  EXPECT_TRUE(p2.fileNameExtGet() == esT("iexplore.exe"));
#endif
}

class EsTestEnumerator : public EsPathEnumerator
{
public:
  EsTestEnumerator() :
  EsPathEnumerator(binRootGet())
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
      0 == name.find( esT("EL3C_") )
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

  PRINTF( "Non-recursive enumerator returned %d objects\n", objs.size() );
  ASSERT_TRUE(2 == objs.size());
}

TEST(EsPathEnumeratorTest, Recursion) {

  EsTestEnumerator tenum;
  tenum.execute( static_cast<ulong>(EsPathEnumeratorFlag::Recursive) );
  const EsString::Array& objs = tenum.objsGet();

  PRINTF( "Recursive enumerator returned %d objects\n", objs.size() );
  ASSERT_TRUE( 3 == objs.size() );
}

TEST(EsFileTest, Basics) {

  const EsString& binRoot = binRootGet();
  EsPath path = EsPath::createFromFilePath(binRoot+esT("file_1.txt"));
  EsFile file(
    path.pathGet(),
    static_cast<ulong>(EsFileFlag::Read)|
    static_cast<ulong>(EsFileFlag::Text)
  );

  EXPECT_TRUE(file.open());
  const EsString& contents = file.readAllAsString();
  file.close();

  path.fileNameSet(esT("file_2"));
  file.set_name(path.pathGet());
  file.set_flags(
    static_cast<ulong>(EsFileFlag::Write)|
    static_cast<ulong>(EsFileFlag::Text)
  );
  EXPECT_TRUE(file.open());
  file.writeAllAsString(contents);
  EXPECT_THROW( file.remove(), EsException );
  file.close();
  EXPECT_NO_THROW( file.remove() );
}

