// testEsOsInfo.cc file
// Regular expressions test
//

TEST(EsOsInfoTest, Test) {

	PRINTF( "Os info: '%s'\n", EsString::toAscii(EsOsInfo::asString()).c_str() );
 	PRINTF( "Os name: '%s'\n", EsString::toAscii(EsOsInfo::nameGet()).c_str() );
 	PRINTF( "Os SP name: '%s'\n", EsString::toAscii(EsOsInfo::spNameGet()).c_str() );

  PRINTF( "Os architecture: '%0X'\n", EsOsInfo::architectureGet() );
  PRINTF( "Os platform: '%0X'\n", EsOsInfo::platformGet() );
  PRINTF( "Os build: '%0X'\n", EsOsInfo::buildGet() );
  PRINTF( "Os version: '%d.%d'\n", EsOsInfo::majorGet(), EsOsInfo::minorGet() );
  PRINTF( "Os SP version: '%d.%d'\n", EsOsInfo::spMajorGet(), EsOsInfo::spMinorGet() );

}

