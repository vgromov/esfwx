// socket stuff tests
//
TEST(EsSocketChannelTest, ServerClientIoChannels) {

  static const size_t sc_cnt = 24096;
  std::vector<esU32> out;
  out.resize(sc_cnt);
  for(size_t idx = 0; idx < sc_cnt; ++idx)
    out[idx] = idx;

  EsVariant srv = EsChannelIoSocketServer::NEW();
  EXPECT_TRUE( srv.call("open").asBool() );

  EsVariant cli = EsChannelIoSocketClient::NEW();
  EXPECT_TRUE( cli.call("open").asBool() );

  esU32 bcnt = out.size()*sizeof(esU32);
  const esU8* begin = reinterpret_cast<const esU8*>(&out[0]);

  EsBinBuffer bb(
    begin,
    begin+bcnt
  );

  ulong sent = cli.call(
    "bytesPut",
    bb,
    1000
  ).asULong();

  EXPECT_TRUE( sent == bcnt );

  const EsBinBuffer& in = srv.call(
    "bytesGet",
    bcnt,
    10000
  ).asBinBuffer();

  EXPECT_TRUE( bcnt == in.size() );

  sent = srv.call(
    "bytesPut",
    in,
    1000
  ).asULong();

  EXPECT_TRUE( sent == in.size() );

  const EsBinBuffer& echo = cli.call(
    "bytesGet",
    bcnt,
    1000
  ).asBinBuffer();

  EXPECT_TRUE( bcnt == echo.size() );
  EXPECT_TRUE( echo == in );

  cli.call(
    "close"
  );
  srv.call(
    "close"
  );
}

