// Sync object stuff tests
//
/*
TEST(EsSyncObjectTest, Basics) {

  EsCriticalSection m_cs;

  {
    EsCriticalSectionLocker lock(m_cs);

    EXPECT_TRUE(m_cs.tryEnter());
  }

  EXPECT_TRUE(m_cs.tryEnter());
}

TEST(EsSyncObjectTest, Semaphore) {

  EsSemaphore sema(0, 4);

  EXPECT_TRUE(EsSemaphore::resultOk == sema.post());
  EXPECT_TRUE(EsSemaphore::resultOk == sema.wait());
}
*/
