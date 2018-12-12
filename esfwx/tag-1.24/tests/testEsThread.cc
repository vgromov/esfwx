// threading stuff tests
//

class EsThreadTest1 : public EsThread
{
public:
  EsThreadTest1() : m_idx(0) {}

  int idxGet() const { return m_idx; }

protected:
  virtual long worker() ES_OVERRIDE
  {
    m_idx = 1;
    while( !checkForStopping(5) )
      ++m_idx;

    return 0;
  }

protected:
  volatile int m_idx;
};

TEST(EsThreadTest, Basics) {

  EsThreadTest1 thread;
  ASSERT_NO_THROW(thread.start());
  EsThread::sleep(500);
  EXPECT_TRUE(thread.isMain());

  thread.stopAndWait();
  EXPECT_TRUE(EsThreadState::None == thread.stateGet());
  EXPECT_TRUE(thread.idxGet() != 0);
}

class EsThreadTest2 : public EsThread
{
public:
  EsThreadTest2() : EsThread(), m_idx(0) { m_sub.categoryAdd(esT("thread-test2")); }
  int idxGet() const { return m_idx; }

protected:
  virtual long worker() ES_OVERRIDE
  {
    m_idx = 0;
    while( !checkForStopping(5) )
    {
      EsEventIntf::Ptr evt = m_sub.eventReceive();
      if( evt && 4444 == evt->idGet() )
      {
        evt = EsEvent::create(
          esT("from-tread2"),
          123
        );
        EsEventDispatcher::eventPost(evt);
        break;
      }

      ++m_idx;
    }

    return 0;
  }

protected:
  volatile int m_idx;
  EsEventSubscriberAsync m_sub;
};

class EsEventSubscriberTest : public EsEventSubscriber
{
public:
  EsEventSubscriberTest() : EsEventSubscriber(esT("from-tread2")), m_final(false) {}

  ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt) ES_OVERRIDE
  {
    if(123 == evt->idGet())
      m_final = true;
  }

  bool isFinal() const { return m_final; }

private:
  bool m_final;
};

TEST(EsThreadTest, EventDispatch) {

  EsThreadTest2 thread;
  EsEventSubscriberTest sub2;

  ASSERT_NO_THROW(thread.start());

  long loop = 0;
  while(1)
  {
    EsThread::sleep(5);
    EsEventDispatcher::process();

    if( sub2.isFinal() )
      break;

    if( loop > 100 )
    {
      loop = 0;
      EsEventDispatcher::eventPost( EsEvent::create(esT("thread-test2"), 4444) );
    }

    ++loop;
  }

  thread.stopAndWait();
  EXPECT_TRUE(EsThreadState::None == thread.stateGet());
  EXPECT_TRUE(thread.idxGet() != 0);
}


