// Scripting profiling tests
//
#ifdef ES_TEST_DO_SCRIPT_PROFILING
# define ES_SCRIPT_REPEAT_BEGIN \
  { int cnt = ES_SCRIPT_REPEAT_CNT; while( cnt-- ) {

# define ES_SCRIPT_REPEAT_END \
  }}

#else //< ES_TEST_DO_SCRIPT_PROFILING

# define ES_SCRIPT_REPEAT_BEGIN
# define ES_SCRIPT_REPEAT_END

#endif


class EsScriptProfileTest : public ::testing::Test
{
protected:
  void SetUp()
  {
    EsPath profilingScript = EsPath::create(
      EsPath::stdDocsGet(),
      esT("profiling"),
      esT("cesse")
    );

    script = EsScript::create();

    static const GUID c_key = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};

    EsBinBuffer compiled;
    const EsString& fpath = profilingScript.pathGet();
    EsFile f(fpath, static_cast<ulong>(EsFileFlag::Read));

    if( f.open() )
    {
      compiled = f.readAllAsBinBuffer();
      f.close();
    }

    script->compiledBinaryLoad(
      profilingScript.pathGet(),
      true,
      EsUtilities::GUIDtoStr(c_key)
    );

  }

  void TearDown()
  {
    script.reset();
  }

protected:
  EsScriptIntf::Ptr script;
};

#ifdef ES_SCRIPT_PROFILE_BASIC_FOR_LOOP
TEST_F(EsScriptProfileTest, BasicsForLoop) {

  EsVariant to = script->objectCreate(esT("ProfilingTest"));
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call(esT("basicsForLoop"));
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_WITH_NAMED_CONST_CNT
TEST_F( EsScriptProfileTest, BasicsForLoopWithNamedConstCnt ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call( esT( "basicsForLoopWithNamedConstCnt" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF
TEST_F(EsScriptProfileTest, BasicsForLoopFillBuff) {

  EsVariant to = script->objectCreate(esT("ProfilingTest"));
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call(esT("basicsForLoopFillBuff"));
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF_WITH_NAMED_COSNT_CNT
TEST_F( EsScriptProfileTest, BasicsForLoopFillBuffWithNamedConstCnt ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call( esT( "basicsForLoopFillBuffWithNamedConstCnt" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF_WITH_PREALLOC
TEST_F(EsScriptProfileTest, BasicsForLoopFillBuffWithPreAlloc) {

  EsVariant to = script->objectCreate(esT("ProfilingTest"));
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call(esT("basicsForLoopFillBuffWithPrealloc"));
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_FOR_LOOP_REFLECTED_STATIC_CALL_RET_NO_PARAMS
TEST_F(EsScriptProfileTest, ForLoopReflectedStaticCallRetNoParams) {

  EsVariant to = script->objectCreate(esT("ProfilingTest"));
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call(esT("forLoopReflectedStaticCallRetNoParams"));
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_FOR_LOOP_REFLECTED_STATIC_CALL_RET_4_PARAMS
TEST_F(EsScriptProfileTest, ForLoopReflectedStaticCallRet4params) {

  EsVariant to = script->objectCreate(esT("ProfilingTest"));
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call(esT("forLoopReflectedStaticCallRet4params"));
  ES_SCRIPT_REPEAT_END
}
#endif

//---------------------------------------------------------------------------
// Advanced profiling
//
#ifdef ES_SCRIPT_PROFILE_OBJ_COLLECTION_INIT
TEST_F( EsScriptProfileTest, ObjectCollectionInit ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call( esT( "dataObjectCollectionInit" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_OBJ_COLLECTION_INIT_WITH_MIDDLE_FLD_ASSIGN
TEST_F( EsScriptProfileTest, ObjectCollectionInitWithMiddleFieldAssign ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
  to.call( esT( "dataObjectCollectionInitWithMiddleFieldAssign" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT
TEST_F( EsScriptProfileTest, DerivedDataObjectCollectionInit ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "derivedDataObjectCollectionInit" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT_WITH_THIS_FLD_ASSIGN
TEST_F( EsScriptProfileTest, DerivedDataObjectCollectionInitWithThisFieldAssign ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "derivedDataObjectCollectionInitWithThisMiddleFieldAssign" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT_WITH_BASE_FLD_ASSIGN
TEST_F( EsScriptProfileTest, DerivedDataObjectCollectionInitWithBaseFieldAssign ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "derivedDataObjectCollectionInitWithBaseMiddleFieldAssign" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT
TEST_F( EsScriptProfileTest, SimpleArrayObjectInitTest ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "simpleArrayObjectInitTest" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_DERIVED_OBJ_ARRAY_INIT
TEST_F( EsScriptProfileTest, DerivedArrayObjectInitTest ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "derivedArrayObjectInitTest" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT_IDX_ITERATION_AND_FLD_ASN
TEST_F( EsScriptProfileTest, SimpleArrayObjectInitWithIndexedIterationAndMiddleFldAssignTest ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "simpleArrayObjectInitWithIndexedIterationAndMiddleFldAssignTest" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT_IDX_ITERATION_CACHED_CNT_AND_FLD_ASN
TEST_F( EsScriptProfileTest, SimpleArrayObjectInitWithIndexedIterationCachedCntAndMiddleFldAssignTest ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "simpleArrayObjectInitWithIndexedIterationCachedCntAndMiddleFldAssignTest" ) );
  ES_SCRIPT_REPEAT_END
}
#endif

#ifdef ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT_FOREACH_ITERATION_CACHED_CNT_AND_FLD_ASN
TEST_F( EsScriptProfileTest, SimpleArrayObjectInitWithForEachIterationAndMiddleFldAssignTest ) {

  EsVariant to = script->objectCreate( esT( "ProfilingTest" ) );
  ASSERT_TRUE( !to.isEmpty() );

  ES_SCRIPT_REPEAT_BEGIN
    to.call( esT( "simpleArrayObjectInitWithForEachIterationAndMiddleFldAssignTest" ) );
  ES_SCRIPT_REPEAT_END
}
#endif
