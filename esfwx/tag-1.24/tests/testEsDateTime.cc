// EsDateTime && EsTimeSpan tests
//

TEST(EsDateTimeTest, Now) {
	const EsDateTime& dt1 = EsDateTime::now();
	const EsDateTime& dt2 = EsDateTime::now();
	
	ASSERT_TRUE(dt1.isOk());
	ASSERT_TRUE(dt2.isOk());
	EXPECT_TRUE(0 < (esDT)dt1);
	EXPECT_TRUE(dt1 <= dt2);
}

TEST(EsDateTimeTest, DiffSum) {
	EsDateTime dt1;
	EsDateTime dt2;

	dt1.set_year(1999);
	dt1.set_month(3);
	dt1.set_dayOfMonth(12);

	dt2.set_year(2013);
	dt2.set_month(10);
	dt2.set_dayOfMonth(4);

	ASSERT_TRUE(dt2 > dt1);

	EsTimeSpan dts = dt1-dt2;

	ASSERT_TRUE(dts.get_negative());
	EXPECT_FALSE(dt2-dts == dt1);
	ASSERT_TRUE(dt2+dts == dt1);
	EXPECT_FALSE(dt1+dts == dt2);
	ASSERT_TRUE(dt1-dts == dt2);

	dts = dt2-dt1;
	ASSERT_FALSE(dts.get_negative());
	EXPECT_FALSE(dt2+dts == dt1);
	ASSERT_TRUE(dt2-dts == dt1);
	EXPECT_FALSE(dt1-dts == dt2);
	ASSERT_TRUE(dt1+dts == dt2);
}

TEST(EsDateTimeTest, StringFormat) {
	EsDateTime dt1;
	dt1.set_year(2013);
	dt1.set_month(10);
	dt1.set_dayOfMonth(4);
	dt1.set_hours(14);
	dt1.set_minutes(56);
	dt1.set_seconds(12);
	dt1.set_milliseconds(998);

	EXPECT_TRUE( dt1.asString() == esT("2013-10-04 14:56:12") );
	EXPECT_TRUE( dt1.asDateString() == esT("2013-10-04") );
	EXPECT_TRUE( dt1.asTimeString() == esT("14:56:12") );
	EXPECT_TRUE( dt1.asString(esT("%Y-%m-%d %H:%M:%S.%l")) == esT("2013-10-04 14:56:12.998"));
}

TEST(EsDateTimeTest, StringFormatLoc) {
	EsDateTime dt1;
	dt1.set_year(2013);
	dt1.set_month(10);     //< Ocrober
	dt1.set_dayOfMonth(4); //< Friday
	dt1.set_hours(14);
	dt1.set_minutes(56);
	dt1.set_seconds(12);
	dt1.set_milliseconds(99);

  const std::locale& loc = EsLocale::locale(esT("ru_RU"));

  EsString str = dt1.asString(loc, esT("%A"));
	EXPECT_TRUE( str == esT("Friday") );  //< No I18N loaded

  str = dt1.asString(loc, esT("%a"));
	EXPECT_TRUE( str == esT("Fri") );     //< No I18N loaded

  str = dt1.asString(loc, esT("%B"));
	EXPECT_TRUE( str == esT("October") ); //< No I18N loaded

  str = dt1.asString(loc, esT("%b"));
	EXPECT_TRUE( str == esT("Oct") );     //< No I18N loaded

  str = dt1.asString(loc, esT("%h"));
	EXPECT_TRUE( str == esT("Oct") );     //< No I18N loaded

  str = dt1.asString(loc, esT("%c"));
	EXPECT_TRUE( str == esT("4 October 2013 г. 14:56:12") ); //< No I18N loaded

  str = dt1.asString(loc, esT("%C"));
	EXPECT_TRUE( str == esT("20") );

  str = dt1.asString(loc, esT("%d"));
	EXPECT_TRUE( str == esT("04") );

  str = dt1.asString(loc, esT("%-d"));
	EXPECT_TRUE( str == esT("4") );

  str = dt1.asString(loc, esT("%D"));
	EXPECT_TRUE( str == esT("04.10.2013") );

  str = dt1.asString(loc, esT("%r"));
	EXPECT_TRUE( str == esT("02:56:12 ПП") );

  str = dt1.asString(loc, esT("%R"));
	EXPECT_TRUE( str == esT("14:56") );

  str = dt1.asString(loc, esT("%S"));
	EXPECT_TRUE( str == esT("12") );

  str = dt1.asString(loc, esT("%T"));
	EXPECT_TRUE( str == esT("14:56:12") );

  str = dt1.asString(loc, esT("%U"));
	EXPECT_TRUE( str == esT("41") );

  str = dt1.asString(loc, esT("%W"));
	EXPECT_TRUE( str == esT("41") );

  str = dt1.asString(loc, esT("%w"));
	EXPECT_TRUE( str == esT("5") );  //< Friday = 5th day of week

  str = dt1.asString(loc, esT("%x"));
	EXPECT_TRUE( str == esT("4 October 2013 г.") ); //< No I18N loaded

  str = dt1.asString(loc, esT("%X"));
	EXPECT_TRUE( str == esT("14:56:12") );

  str = dt1.asString(loc, esT("%y"));
	EXPECT_TRUE( str == esT("13") );

  str = dt1.asString(loc, esT("%Y"));
	EXPECT_TRUE( str == esT("2013") );

  str = dt1.asString(loc, esT("%l"));
	EXPECT_TRUE( str == esT("099") );

  str = dt1.asString(loc, esT("%-l"));
	EXPECT_TRUE( str == esT("99") );
}
