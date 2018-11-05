// testing script library
// 

TEST( ESScriptTest, Enums ) {

	EsScriptIntf::Ptr script = EsScript::create();
	ASSERT_TRUE(script);
	EsScriptMachine& vm = script->vm();
	EsEnumerationIntf::Ptr e;
	ASSERT_NO_THROW( e = vm.enumDeclare(esT("TestScriptEnum")) );
	ASSERT_TRUE( e );
	ASSERT_NO_THROW( e->itemAdd(esT("Sym1"), 1, EsString::null()) );
	ASSERT_NO_THROW( e->itemAdd(esT("Sym2"), 2, EsString::null()) );
	ASSERT_NO_THROW( e->itemAdd(esT("Sym3"), 3, esT("Symbol 3 label")) );
	ASSERT_THROW( e->itemAdd(esT("Sym1"), 1, esT("Duplicate symbol")), EsException );
	EXPECT_TRUE( e->hasSymbol(esT("Sym2")) );
	// test reflected interface calls
	EsReflectedClassIntf::Ptr r = e;
	ASSERT_TRUE( r );
 	EXPECT_TRUE( r->call(esT("hasSymbol"), esT("Sym2")).asBool() );
}

EsScriptIntf::Ptr script;

#ifdef ES_USE_COMPILER_IMPLEMENTATION
TEST( EsScriptTest, Compilation) {

  const EsString& binRoot = binRootGet();
	script = EsScript::create();
	ASSERT_TRUE(script);
	const EsPath& path = EsPath::createFromFilePath(
  	binRoot + esT("../scripts-core/all.ess")
	);

	EXPECT_NO_THROW( script->includePathAdd(path.pathGet(EsPath::flagDefault|EsPath::flagExcludeFile)) );
	bool ok;
	EXPECT_NO_THROW( ok = script->compileFile(path.pathGet()) );
	EXPECT_TRUE( ok );
}
#endif

TEST( EsScriptTest, BinarySave) {

  const EsString& binRoot = binRootGet();
	const EsString& key = esT("Encryption key");
	const EsBinBuffer& bin = script->compiledBinaryGet();
	const EsBinBuffer& bine = EsUtilities::bToE(bin, key);
	const EsPath& fpath = EsPath::createFromFilePath(binRoot+esT("compiled_binary.essle"));
	EsFile f(fpath.pathGet(), EsFile::flagWrite);
	EXPECT_TRUE( f.open() );
	EXPECT_NO_THROW( f.writeAllAsBinBuffer(bine) );
	EXPECT_NO_THROW( f.close() );
}

TEST( EsScriptTest, BinaryLoad) {

  const EsString& binRoot = binRootGet();
  const EsString& key = esT("Encryption key");
	const EsPath& fpath = EsPath::createFromFilePath(binRoot+esT("compiled_binary.essle"));
	const EsString& path = fpath.pathGet();
	EXPECT_NO_THROW( script->vm().reset() );
	EXPECT_NO_THROW( script->compiledBinaryLoad(path, true, key) );
	EXPECT_NO_THROW( EsFile::remove(path) );
}
