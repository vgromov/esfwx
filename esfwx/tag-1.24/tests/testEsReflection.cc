// reflection framework tests
//

TEST(EsReflectionTest, Basics) {

  EsBaseIntfPtr dt = EsDateTime::reflectedNow();
  EsReflectedClassIntf::Ptr r = dt;
  ASSERT_TRUE(r);

  EsString result;
  ASSERT_NO_THROW(result = r->call(esT("asString")).asString());
}

TEST(EsReflectionTest, MultiInterfaceAccess) {

  EsEnumerationIntf::Ptr enu = EsVariantType::instanceGet();
  ASSERT_TRUE(enu);

  EsReflectedClassIntf::Ptr ref = enu;
  ASSERT_TRUE(ref);

  EsVariant allEnu;
  ASSERT_NO_THROW(allEnu = enu->symbolNamesGet());

  EsVariant allRef;
  ASSERT_NO_THROW(allRef = ref->propertyGet(esT("symbols")));

  ASSERT_TRUE(allEnu == allRef);

  ASSERT_TRUE(enu->hasSymbol(esT("VAR_VARIANT_COLLECTION")));

  ASSERT_TRUE(ref->call(esT("hasSymbol"), esVT("VAR_VARIANT_COLLECTION")).asBool());
}
