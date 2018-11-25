// EsString-related tests
//

TEST(EsStringTest, Basics) {
	EsString s1 = esT("1234");
	EsString s2 = esT("124");

	EXPECT_TRUE( s2 != s1 );
	EXPECT_TRUE( s2 > s1 );

	s1 += s2;
	EXPECT_TRUE( s1 == esT("1234124") );

	// check 0-termination
	s1 = EsString::format(esT("Formatted string %s"), s1);
	EXPECT_TRUE( s1 == esT("Formatted string 1234124") );
	s1 += s2;
	EXPECT_TRUE( s1 == esT("Formatted string 1234124124") );
}

TEST(EsStringTest, Encoding16) {

	EsWideString16 src = esT16("тестовая строка по-русски");
	EsStringConverter::Ptr cnv16to8 = EsStringConverter::convGet("UTF-8", "UTF-16");
	EsStringConverter::Ptr cnv8to16 = EsStringConverter::convGet("UTF-16", "UTF-8");

	const EsByteString& bs = cnv16to8->w16toC(src);
	const EsWideString16& dest = cnv8to16->cToW16(bs);
	ASSERT_TRUE(src.size() == dest.size());
	ASSERT_TRUE(src == dest);
}

TEST(EsStringTest, EncodingStr) {

	EsString src = esT("тестовая строка по-русски");

	const EsByteString& bs = EsString::toUtf8(src);
	const EsString& dest = EsString::fromUtf8(bs);
	ASSERT_TRUE(src.size() == dest.size());
	ASSERT_TRUE(src == dest);
}

TEST(EsStringTest, CulturalFormatting) {

  const std::locale& loc = EsLocale::locale("ru_RU");

  EsString str = EsString::format(esT("Float fmt: %f"), 12.6);
  EsString str1 = EsString::format(loc, esT("Float fmt: %f"), 12.6);

	ASSERT_TRUE(str != str1);
  ASSERT_TRUE(str1 == esT("Float fmt: 12,600000"));

  str1 = EsString::format(loc, esT("Int fmt: %d"), 1234567890);
  EsString::value_type thSep = EsLocale::thousandSeparatorGet(loc);

  str = esT("Int fmt: 1");
  str += thSep;
  str += esT("234");
  str += thSep;
  str += esT("567");
  str += thSep;
  str += esT("890");

  ASSERT_TRUE(str1 == str);
}

TEST(EsStringTest, CulturalFormattingExtra) {

  const std::locale& loc = EsLocale::locale("ru_RU");

  /// Numeric, list, and money separators
  EsString::value_type sep = EsLocale::decimalSeparatorGet(loc);
  ASSERT_TRUE(esT(',') == sep);

  sep = EsLocale::thousandSeparatorGet(loc);
  ASSERT_TRUE(esT('\x00A0') == sep);

  sep = EsLocale::listSeparatorGet(loc);
  ASSERT_TRUE(esT(';') == sep);

  sep = EsLocale::moneyDecimalSeparatorGet(loc);
  ASSERT_TRUE(esT(',') == sep);

  sep = EsLocale::moneyThousandSeparatorGet(loc);
  ASSERT_TRUE(esT('\x00A0') == sep);

  EsString str = EsLocale::moneySymbolGet(loc);
  ASSERT_TRUE(esT("₽") == str);

  /// Date-Time cultural-aware formatting
  ///
  sep = EsLocale::timeSeparatorGet(loc);
  ASSERT_TRUE(esT(':') == sep);

  sep = EsLocale::dateSeparatorGet(loc);
  ASSERT_TRUE(esT('.') == sep);

  str = EsLocale::timeFmtGet(loc);
  ASSERT_TRUE(esT("H:mm:ss") == str);

  str = EsLocale::dateFmtGet(loc);
  ASSERT_TRUE(esT("d MMMM yyyy 'г.'") == str);

  str = EsLocale::timeFmtShortGet(loc);
  ASSERT_TRUE(esT("H:mm") == str);

  str = EsLocale::dateFmtShortGet(loc);
  ASSERT_TRUE(esT("dd.MM.yyyy") == str);

  str = EsLocale::dateTimeFmtGet(loc);
  ASSERT_TRUE(esT("d MMMM yyyy 'г.' H:mm:ss") == str);

  str = EsLocale::timeAmGet(loc);
  ASSERT_TRUE(esT("ДП") == str);

  str = EsLocale::timePmGet(loc);
  ASSERT_TRUE(esT("ПП") == str);
}

TEST(EsStringTest, CulturalFormattingMoney) {

  const std::locale& loc = EsLocale::locale("ru_RU");

  EsString::value_type sep = EsLocale::moneyThousandSeparatorGet(loc);

  EsString str = esT("12");
  str += sep;
  str += esT("345");
  str += esT(",67 ");

  std::stringstream oss;
  oss.imbue(loc);

  oss << std::put_money(1234567.46);
  EsString str1 = EsString::fromUtf8(oss.str());

  ASSERT_TRUE(str1 == str);

  str += EsLocale::moneySymbolGet(loc);

  oss.str(nullByteString());

  oss << std::showbase << std::put_money(1234567.46);
  str1 = EsString::fromUtf8(oss.str());

  ASSERT_TRUE(str1 == str);
}

TEST(EsTokenizerTest, All) {
	EsString s = esT("Some string;,with.+ consecutive,tokens");
	EsString t = esT(" ;,.+");
	EsStringTokenizer tok(s, t);

	ASSERT_TRUE( 5 == tok.get_tokensCount() );
	EXPECT_TRUE( tok.get_moreTokens() );
	EXPECT_TRUE( tok.get_nextToken() == esT("Some") );
	EXPECT_TRUE( tok.get_moreTokens() );
	EXPECT_TRUE( tok.get_nextToken() == esT("string") );
	EXPECT_TRUE( tok.get_moreTokens() );
	EXPECT_TRUE( tok.get_nextToken() == esT("with") );
	EXPECT_TRUE( tok.get_moreTokens() );
	EXPECT_TRUE( tok.get_nextToken() == esT("consecutive") );
	EXPECT_TRUE( tok.get_moreTokens() );
	EXPECT_TRUE( tok.get_nextToken() == esT("tokens") );
	EXPECT_FALSE( tok.get_moreTokens() );
}

//TEST(EsStringI18nTest, All) {
//
//	EsStringI18n& i18n = EsStringI18n::instanceGet();
//
//	EsString::Array searchPaths;
//  const EsString& binRoot = binRootGet();
//	searchPaths.push_back(binRoot+esT("translations1"));
//	searchPaths.push_back(binRoot+esT("translations1/translations2"));
// 	searchPaths.push_back(binRoot+esT("translations3"));
//
//	i18n.translationSearchPathsSet( searchPaths );
//	i18n.defaultLocaleSet(esT("en"));
//	i18n.currentLocaleSet(esT("ru_RU"));
//
//	const EsString& fb = i18n.translationGet(esT("test fallback english string"), esT("fallback"));
//	EXPECT_TRUE( fb == esT("English Fallback") );
//
//	const EsString& fb1 = i18n.translationGet(esT("test fallback english string"));
//	EXPECT_TRUE( fb1 == esT("English Fallback") );
//
//	const EsString& r = i18n.translationGet(esT("Segment 1-2 border:"));
//	EXPECT_TRUE( r == esT("Страница сегмент 1-2:") );
//}

