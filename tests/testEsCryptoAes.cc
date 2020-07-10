// EsCryptoAes-related tests
//

TEST(EsCryptoAesTest, AES1) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha1(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES224) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha224(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES256) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha256(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES384) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha384(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES512) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha512(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES3_224) {

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

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES3_256) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha3_256(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES3_384) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha3_384(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}

TEST(EsCryptoAesTest, AES3_512) {

  static const GUID c_key1 = {0xB7013BFB, 0xD65C, 0x5346, 0x93, 0xD0, 0x71, 0xF4, 0xF3, 0xC2, 0x8D, 0xD4};
  const EsBinBuffer& key1 = EsCryptoHash::sha3_512(
    EsUtilities::GUIDtoStr(
      c_key1
    )
  );

  EsByteString bs = "ksdkj ;laksr; lksd;lfk js;ldkjf l;skjd;lfk ;lskjd f;";
  EsBinBuffer in(bs.begin(), bs.end());
  EsBinBuffer encoded;
  EsBinBuffer decoded;

  EsCryptoAes aes( key1 );

  for(size_t idx = 0; idx < 1000; ++idx)
  {
    encoded.clear();
    decoded.clear();

    EXPECT_NO_THROW( aes.process(encoded, in, true) );

    EXPECT_NO_THROW( aes.process(decoded, encoded, false) );

    EXPECT_TRUE( in == decoded );

    in += static_cast<EsBinBuffer::value_type>(idx % 256);
  }
}
