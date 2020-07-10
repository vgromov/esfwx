// Mutex
//
EsMutex::EsMutex(EsMutex::Type type /*= typeDefault*/) :
m_type(type),
m_owningThreadId(0),
m_mx(0)
{
  m_mx = ::CreateMutex( 0,       // default secutiry attributes
                        FALSE,   // not initially locked
                        0        // no name
                      );
  if( 0 == m_mx )
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

EsMutex::~EsMutex()
{
  if( m_mx )
  {
    ::CloseHandle( m_mx );
    m_mx = 0;
  }
}
//---------------------------------------------------------------------------

bool EsMutex::isOk() const
{
  return 0 != m_mx;
}
//---------------------------------------------------------------------------

EsMutex::Result EsMutex::lock(ulong ms)
{
  if( m_type != typeRecursive &&
      0 != m_owningThreadId &&
      m_owningThreadId == EsThread::currentIdGet() )
    return resultDeadlock;

  if( !isOk() )
    return resultInvalid;

  esU32 rc = ::WaitForSingleObject(m_mx, ms);
  switch( rc )
  {
  case WAIT_ABANDONED:
    // the previous caller died without releasing the mutex, so even
    // though we did get it, log a message about this
    ES_DEBUG_TRACE(esT("WaitForSingleObject() returned WAIT_ABANDONED"));
    // fall through
  case WAIT_OBJECT_0:
    // ok
    break;
  case WAIT_TIMEOUT:
    return resultTimeout;
  default:
    ES_FAIL;
    // fall through
  case WAIT_FAILED:
    return resultError;
  }

  if(m_type == typeDefault) // required for checking recursion
    m_owningThreadId = EsThread::currentIdGet();

  return resultOk;
}
//---------------------------------------------------------------------------

EsMutex::Result EsMutex::unlock()
{
  // required for checking recursion
  m_owningThreadId = 0;
  if( !isOk() )
    return resultInvalid;

  if( !::ReleaseMutex(m_mx) )
    return resultError;

  return resultOk;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Critical section
//
EsCriticalSection::EsCriticalSection()
{
  ::InitializeCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------

EsCriticalSection::~EsCriticalSection()
{
  ::DeleteCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------

void EsCriticalSection::enter()
{
  ::EnterCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------

bool EsCriticalSection::tryEnter()
{
  return 0 != ::TryEnterCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------

void EsCriticalSection::leave()
{
  ::LeaveCriticalSection(&m_cs);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Semaphore
//

EsSemaphore::EsSemaphore( ulong initialCount /*= 0*/, ulong maxCount /*= 0 */) :
m_sem(0)
{
  if( maxCount == 0 ) // make it practically infinite
    maxCount = INT_MAX;

  if( initialCount > maxCount )
    initialCount = maxCount;

  m_sem = ::CreateSemaphore(0,           // default security attributes
                              initialCount,
                              maxCount,
                              0            // no name
                              );
  if( 0 == m_sem )
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

EsSemaphore::~EsSemaphore()
{
  if( m_sem )
  {
    ::CloseHandle(m_sem);
    m_sem = 0;
  }
}
//---------------------------------------------------------------------------

bool EsSemaphore::isOk() const
{
  return 0 != m_sem;
}
//---------------------------------------------------------------------------

EsSemaphore::Result EsSemaphore::wait(ulong ms)
{
  if( !isOk() )
    return resultInvalid;

  esU32 rc = ::WaitForSingleObject( m_sem, ms );
  switch( rc )
  {
  case WAIT_OBJECT_0:
      return resultOk;
  case WAIT_TIMEOUT:
      return resultTimeout;
  }

  return resultError;
}
//---------------------------------------------------------------------------

EsSemaphore::Result EsSemaphore::post()
{
  if( !isOk() )
    return resultInvalid;

  if( !::ReleaseSemaphore(m_sem, 1, 0) )
  {
    if( ERROR_TOO_MANY_POSTS == EsUtilities::osErrorCodeGet() )
      return resultOverflow;
    else
    {
      ES_DEBUG_TRACE(esT("EsSemaphore::post result in error"));
      return resultError;
    }
  }

  return resultOk;
}
//---------------------------------------------------------------------------
