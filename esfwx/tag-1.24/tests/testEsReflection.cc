// reflection framework tests
//

TEST( EsReflectionTest, Basics ) {
  
  EsBaseIntfPtr dt = EsDateTime::reflectedNow();
  EsReflectedClassIntf::Ptr r = dt;
  ASSERT_TRUE( r );
  EsString result;
  ASSERT_NO_THROW( result = r->call(esT("asString")).asString() );
}
