// EsDes-related tests
//

TEST(EsCryptoDesLegacyTest, Basics) {

  static const GUID c_key = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsString& key = EsUtilities::GUIDtoStr(c_key);

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk js;ldkjf ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( encoded = EsUtilities::bToE(in, key) );

    EXPECT_NO_THROW( decoded = EsUtilities::eToB(encoded, key) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}
