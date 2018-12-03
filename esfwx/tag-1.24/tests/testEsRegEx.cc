// testEsRegEx.cc file
// Regular expressions test
//
EsString::const_pointer c_testI18nStr =
  esT("Проверка 1234 работы с юникодом 223344 rtrty");

TEST(EsRegExTest, MatchI18N) {

  EsRegEx re( R"(([^\s]+)\s+?([0-9]+))" );

  ASSERT_TRUE( re.isOk() );

  const EsString& txt = c_testI18nStr;
  re.set_text(txt);

  while( re.get_matches() )
  {
    ulong cnt = re.get_matchCount();

    ulong start, len;
    for(ulong idx = 0; idx < cnt; ++idx)
    {
      re.matchGet(start, len, idx);

      const EsString& matched = txt.substr(start, len);
      PRINTF( "%s\n", EsString::toUtf8(matched).c_str() );
    }

    re.set_offset(start+len);
  }
}

TEST(EsRegExTest, StrFormat) {

  const EsString& str1 = EsString::format(esT("Test: '%s'"), esVT("string to inject"));
  ASSERT_TRUE( esT("Test: 'string to inject'") == str1 );

  const EsString& str2 = EsString::format(esT("%d Test: '%s'"), 4, esVT("string to inject"));

  ASSERT_TRUE( esT("4 Test: 'string to inject'") == str2 );
}

EsString::const_pointer c_testStr =
esT("555.123.4567  +1-(800)-555-2468\n")
esT("foo@demo.net  bar.ba@test.co.uk\n")
esT("www.demo.com  http://foo.co.uk/\n")
esT("http://regexr.com/foo.html?q=bar\n");

TEST(EsRegExTest, Match) {

  EsRegEx re( R"(([a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])+))" );

  ASSERT_TRUE( re.isOk() );

  re.set_text( c_testStr );

  EXPECT_TRUE( re.get_matches() );

  ulong cnt = re.get_matchCount();
  EXPECT_TRUE( 2 == cnt );

  EsString match = re.matchGet(1);
  ASSERT_TRUE( esT("foo@demo.net") == match );

  EsVariant mr = re.matchRangeGet(1);
  ASSERT_TRUE( !mr.isEmpty() );

  ulong _0 = mr[0].asULong();
  ulong _1 = mr[1].asULong();
  PRINTF("Range: [%d:%d[\n", _0, _1);
  EXPECT_TRUE( 32 == _0 );
  EXPECT_TRUE( 44 == _1 );

  ulong offs = mr[1].asULong();
  re.set_offset( offs );

  EXPECT_TRUE( re.get_matches() );

  cnt = re.get_matchCount();
  EXPECT_TRUE( 2 == cnt );

  match = re.matchGet(1);
  ASSERT_TRUE( esT("bar.ba@test.co.uk") == match );

  mr = re.matchRangeGet(1);
  ASSERT_TRUE( !mr.isEmpty() );

  _0 = mr[0].asULong();
  _1 = mr[1].asULong();
  PRINTF("Range: [%d:%d[\n", _0, _1);
  EXPECT_TRUE( 46 == _0 );
  EXPECT_TRUE( 63 == _1 );

  offs = mr[1].asULong();
  re.set_offset( offs );

  EXPECT_FALSE( re.get_matches() );
}

EsString::const_pointer c_replaceResultStr =
esT("555.123.4567  +1-(800)-555-2468\n")
esT("\n# foo@demo.net:\n\t  \n# bar.ba@test.co.uk:\n\t\n")
esT("www.demo.com  http://foo.co.uk/\n")
esT("http://regexr.com/foo.html?q=bar\n");

TEST(EsRegExTest, Replace) {

  EsRegEx re( R"(([a-z0-9!#$%&'*+/=?^_`{|}~-]+(?:\.[a-z0-9!#$%&'*+/=?^_`{|}~-]+)*@(?:[a-z0-9](?:[a-z0-9-]*[a-z0-9])?\.)+[a-z0-9](?:[a-z0-9-]*[a-z0-9])+))" );

  ASSERT_TRUE( re.isOk() );

  re.set_text( c_testStr );
  const EsString& result = re.replace(esT("\n# $0:\n\t"));

  EXPECT_TRUE( result == c_replaceResultStr );
}

