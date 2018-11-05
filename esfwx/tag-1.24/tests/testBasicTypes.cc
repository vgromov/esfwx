// test for basic types behaviour on target platform
//

TEST( BasicTypesTest, testINT64 ) {

	const esU64 c64 = 0xFFFFFFFFFFFFFFFFULL;
	esU64 u64 = c64;
	u64 >>= 32;

	ASSERT_EQ( (c64 & 0xFFFFFFFF00000000ULL) >> 32, 0xFFFFFFFFUL );
	ASSERT_EQ( c64 >> 32, 0xFFFFFFFFUL );
	ASSERT_EQ( u64, 0xFFFFFFFFUL );
}

TEST( BasicTypesTest, EsStringHash ) {

	EsString s1 = esT("Some string 1");
	EsString s2 = esT("Some string 2");

	ASSERT_NE( s1.hashGet(), s2.hashGet() );
}
