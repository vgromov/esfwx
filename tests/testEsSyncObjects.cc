// Sync object stuff tests
//

TEST(EsSyncObjectTest, Mutex) {

  EsMutex m_mx;

  m_mx.lock();
  m_mx.unlock();
}

TEST(EsSyncObjectTest, CriticalSection) {

  EsCriticalSection m_cs;

  bool entered = false;
  {
    EsCriticalSectionLocker lock(m_cs);

    EXPECT_TRUE(entered = m_cs.tryEnter());
    if(entered)  
        m_cs.leave();    
  }

  EXPECT_TRUE(entered = m_cs.tryEnter());
  if(entered)  
    m_cs.leave();
}

TEST(EsSyncObjectTest, Semaphore) {

  EsSemaphore sema(0, 4);

  EXPECT_TRUE(EsSemaphore::resultOk == sema.post());
  EXPECT_TRUE(EsSemaphore::resultOk == sema.wait());
}

