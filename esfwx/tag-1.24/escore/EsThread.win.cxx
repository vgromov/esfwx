EsThread::EsThread(size_t stack /*= 0*/) :
m_state(EsThreadState::None),
m_started(0, 1),
m_pause(0, 1),
m_stop(0, 1),
m_stopped(1, 1),
m_errorCode(0),
m_stack(stack),
m_id(EsThreadIdNone),
m_thread(0)
{}
//---------------------------------------------------------------------------

EsThread::~EsThread()
{
  stopAndWait();
}
//---------------------------------------------------------------------------

void EsThread::cleanup()
{
  if( m_thread )
  {
    // Finally, close and nullify thread handle
    ::WaitForSingleObject(m_thread, ES_INFINITE);  // make sure thread actually exited
    ::CloseHandle(m_thread);
    m_thread = 0;
  }

  // reset stop condition, just in case
  m_state = EsThreadState::None;
  m_stop.tryWait();
  m_id = EsThreadIdNone;
}
//---------------------------------------------------------------------------

int EsThread::priorityCalc()
{
  if(m_priority <= 20)
    return THREAD_PRIORITY_LOWEST;
  else if (m_priority <= 40)
    return THREAD_PRIORITY_BELOW_NORMAL;
  else if (m_priority <= 60)
    return THREAD_PRIORITY_NORMAL;
  else if (m_priority <= 80)
    return THREAD_PRIORITY_ABOVE_NORMAL;
  else
    return THREAD_PRIORITY_HIGHEST;
}
//---------------------------------------------------------------------------

void EsThread::threadCreate()
{
  ES_ASSERT(0 == m_thread);
  m_thread = ::CreateThread(
    0,
    m_stack,
    reinterpret_cast<LPTHREAD_START_ROUTINE>(&EsThread::threadWorker),
    (LPVOID)this,
    CREATE_SUSPENDED,
    &m_id
  );

  if( !m_thread )
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );
  else
  {
    // set thread priority and start the thread
    ::SetThreadPriority(m_thread, priorityCalc());
    if( (esU32)-1 == ::ResumeThread(m_thread) )
    {
      ::TerminateThread(m_thread, -1);
      ::CloseHandle(m_thread);
      m_thread = 0;

      EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
    }
  }
}
//---------------------------------------------------------------------------

#define ES_THREAD_EXIT
//---------------------------------------------------------------------------

EsThreadId EsThread::currentIdGet()
{
  return static_cast<EsThreadId>( ::GetCurrentThreadId() );
}
//---------------------------------------------------------------------------

void EsThread::sleep(ulong ms)
{
  ::Sleep(ms);
}
//---------------------------------------------------------------------------

void EsThread::yield()
{
  EsThread::sleep(0);
}
//---------------------------------------------------------------------------


