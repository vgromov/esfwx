// EsUtilities tests
//
#include <escore/EsCryptoDesLegacy.h>

TEST(EsUtilitiesTest, ByteInversion) {

  esU16 u16i = EsUtilities::inverse2(0x0123);
  ASSERT_TRUE(0x2301 == u16i);

  esU32 u32i = EsUtilities::inverse4(0x01234567);
  ASSERT_TRUE(0x67452301 == u32i);

  esU64 u64i = EsUtilities::inverse8(0x0123456789012345ULL);
  ASSERT_TRUE(0x4523018967452301ULL == u64i);
}

TEST(EsUtilitiesTest, Zip) {

  const EsString& src = esT("Some string \n предназначенная \tдля compression test");
  EsBinBuffer in(src.size()*EsString::value_size);
  memcpy(&in[0], (const esU8*)src.data(), in.size());

  EsZipCompressor zipc;
  const EsBinBuffer& outc = zipc.compress(in);

  EsZipDecompressor zipd;
  const EsBinBuffer& outd = zipd.decompress(outc);
  ASSERT_TRUE(in.size() == outd.size());
  ASSERT_TRUE(in == outd);

  EsString srcd(outd.size()/EsString::value_size, 0);
  memcpy((esU8*)srcd.data(), &outd[0], outd.size());
  ASSERT_TRUE(src == srcd);
}

TEST(EsUtilitiesTest, DesLegacy) {

  const EsString& key = esT("Des codec key must be at least 64 bytes long, of which only first 64 bytes is used");
  const EsString& src = esT("Some string \n предназначенная \tдля codec test");
  EsBinBuffer in(src.size()*EsString::value_size);
  memcpy(&in[0], (const esU8*)src.data(), in.size());

  EsCryptoDesLegacy des(key);
  EsBinBuffer outc;
  ASSERT_NO_THROW( des.process(outc, in) );
  ASSERT_TRUE( !outc.empty() );
  EsBinBuffer outd;
  ASSERT_NO_THROW( des.process(outd, outc, false) );
  ASSERT_TRUE( !outd.empty() );
  ASSERT_TRUE( outd == in );

  EsString srcd(outd.size()/EsString::value_size, 0);
  memcpy((esU8*)srcd.data(), &outd[0], outd.size());
  ASSERT_TRUE(src == srcd);
}

TEST(EsUtilitiesTest, Codec) {

  const EsString& key = esT("Des codec key must be at least 64 bytes long, of which only first 64 bytes is used");
  const EsString& src = esT("Some string \n предназначенная \tдля codec test");

  EsBinBuffer in(src.size()*EsString::value_size);
  memcpy(&in[0], (const esU8*)src.data(), in.size());
  const EsBinBuffer& oute = EsUtilities::bToE(in, key);
  const EsBinBuffer& outb = EsUtilities::eToB(oute, key);
  ASSERT_TRUE( outb == in );

  EsString srcd(outb.size()/EsString::value_size, 0);
  memcpy((esU8*)srcd.data(), &outb[0], outb.size());
  ASSERT_TRUE(src == srcd);

  const EsString& se = EsUtilities::sToE(src, key);
  const EsString& ss = EsUtilities::eToS(se, key);
  ASSERT_TRUE( ss == src );
}
