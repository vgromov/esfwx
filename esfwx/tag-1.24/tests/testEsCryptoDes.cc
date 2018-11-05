// EsCryptDes-related tests
//

TEST(EsCryptoDesTest, Basics) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha3_224(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoDes des( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( des.process(encoded, in, true) );

    EXPECT_NO_THROW( des.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoDesTest, DES2) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha224(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );
  static const GUID c_key2 = {0xB7257639, 0xC424, 0x4B27, 0x95, 0x2B, 0x83, 0x72, 0xFE, 0x95, 0x69, 0x89};
  const EsBinBuffer& key2 = EsCryptoHash::sha3_224(
    EsUtilities::GUIDtoStr(
      c_key2
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoDes des( key1, key2 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( des.process(encoded, in, true) );

    EXPECT_NO_THROW( des.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoDesTest, DES3) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha224(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );
  static const GUID c_key2 = {0xB7257639, 0xC424, 0x4B27, 0x95, 0x2B, 0x83, 0x72, 0xFE, 0x95, 0x69, 0x89};
  const EsBinBuffer& key2 = EsCryptoHash::sha3_224(
    EsUtilities::GUIDtoStr(
      c_key2
    )
  );
  static const GUID c_key3 = {0x8D2FB20D, 0x122A, 0x44A1, 0x90, 0x54, 0x38, 0x2D, 0xFF, 0x99, 0x6D, 0x52};
  const EsBinBuffer& key3 = EsCryptoHash::md5(
    EsUtilities::GUIDtoStr(
      c_key3
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoDes des( key1, key2, key3 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( des.process(encoded, in, true) );

    EXPECT_NO_THROW( des.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

