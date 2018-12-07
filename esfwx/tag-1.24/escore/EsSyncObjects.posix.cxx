//---------------------------------------------------------------------------
EsMutex::EsMutex(EsMutex::Type type /*= typeDefault*/) :
m_type(type),
m_owningThreadId(0)
{
  EsThread::checkPthreadError(
    pthread_mutexattr_init(&m_mxAttrs)
  );

  EsThread::checkPthreadError(
    pthread_mutexattr_setpshared(
      &m_mxAttrs,
      PTHREAD_PROCESS_PRIVATE
    )
  );

  if( typeDefault == type  )
    EsThread::checkPthreadError(
      pthread_mutexattr_settype(
        &m_mxAttrs,
        PTHREAD_MUTEX_NORMAL
      )
    );
  else // create recursive mutex
    EsThread::checkPthreadError(
      pthread_mutexattr_settype(
        &m_mxAttrs,
        PTHREAD_MUTEX_RECURSIVE
      )
    );

#ifndef ES_PMUTEX_NO_ROBUST
  EsThread::checkPthreadError(
    pthread_mutexattr_setrobust(
      &m_mxAttrs,
      PTHREAD_MUTEX_ROBUST
    )
  );
#endif

  m_mx = std::make_unique<pthread_mutex_t>();
  ES_ASSERT(m_mx);

  EsThread::checkPthreadError(
    pthread_mutex_init(
      m_mx.get(),
      &m_mxAttrs
    )
  );
}
//---------------------------------------------------------------------------

EsMutex::~EsMutex() ES_NOTHROW
{
  try
  {
#ifdef ES_DEBUG
  int err =
#endif
  pthread_mutex_lock(m_mx.get());

#ifdef ES_DEBUG
  ES_DEBUG_TRACE(esT("pthread_mutex_lock returned: %d"), err);
#endif

#ifdef ES_DEBUG
  err =
#endif
  pthread_mutex_destroy(m_mx.get());

#ifdef ES_DEBUG
  ES_DEBUG_TRACE(esT("pthread_mutex_destroy returned: %d"), err);
#endif

#ifdef ES_DEBUG
  err =
#endif

  pthread_mutexattr_destroy(&m_mxAttrs);

#ifdef ES_DEBUG
  ES_DEBUG_TRACE(esT("pthread_mutexattr_destroy returned: %d"), err);
#endif

  }
  catch(...)
  {}
}
//---------------------------------------------------------------------------

// validity check
bool EsMutex::isOk() const
{
  return nullptr != m_mx.get();
}
//---------------------------------------------------------------------------

#ifndef ES_PTHREAD_NO_TIMEDLOCK
static void timedwaitTmoCalc(ulong ms, timespec& tmo)
{
  clock_gettime( CLOCK_REALTIME, &tmo );
  ulong lsec = ms/1000;
  tmo.tv_sec += lsec;
  tmo.tv_nsec += (ms % 1000) * 1000;
}
#endif
//---------------------------------------------------------------------------

static int pthreadMutexTimedLock(pthread_mutex_t* mx, ulong ms)
{
  ES_ASSERT(mx);

#ifdef ES_PTHREAD_NO_TIMEDLOCK
  int err = 0;
  while( ms-- )
  {
    err = pthread_mutex_trylock(mx);
    if( 0 == err )
      return 0;
    else if( EINVAL == err )
      return err;

    EsThread::sleep(1);
  }

  if( EBUSY == err )
    err = ETIMEDOUT;

  return err;

#else

  timespec tmo;
  timedwaitTmoCalc(
    ms,
    tmo
  );

  return pthread_mutex_timedlock(
    mx,
    &tmo
  );

#endif
}
//---------------------------------------------------------------------------

EsMutex::Result EsMutex::lock(ulong ms)
{
  if(
    m_type != typeRecursive &&
    0 != m_owningThreadId &&
    m_owningThreadId == EsThread::currentIdGet()
  )
    return resultDeadlock;

  if( !isOk() )
    return resultInvalid;

  int err = 0;
  if( ES_INFINITE == ms )
    err = pthread_mutex_lock(m_mx.get());
  else if( 0 == ms )
    err = pthread_mutex_trylock(m_mx.get());
  else
    err = pthreadMutexTimedLock(m_mx.get(), ms);

  switch( err )
  {
  case EBUSY:
    return resultBusy;
  case ETIMEDOUT:
    return resultTimeout;
  case EDEADLK:
    return resultDeadlock;
  case EAGAIN:
  case EPERM:
    return resultError;
  case EINVAL:
    return resultInvalid;
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

  int err = pthread_mutex_unlock(m_mx.get());
  if( err )
    return resultError;

  return resultOk;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Critical section
//
EsCriticalSection::EsCriticalSection() :
m_cs(EsMutex::typeRecursive)
{}
//---------------------------------------------------------------------------

EsCriticalSection::~EsCriticalSection() ES_NOTHROW
{}
//---------------------------------------------------------------------------

void EsCriticalSection::enter()
{
#ifdef ES_DEBUG
  EsMutex::Result result =
#endif
  m_cs.lock();
#ifdef ES_DEBUG
  ES_DEBUG_TRACE(esT("EsCriticalSection::enter returned: %d"), result);
#endif
}
//---------------------------------------------------------------------------

bool EsCriticalSection::tryEnter()
{
  return EsMutex::resultOk == m_cs.tryLock();
}
//---------------------------------------------------------------------------

void EsCriticalSection::leave()
{
#ifdef ES_DEBUG
  EsMutex::Result result =
#endif
  m_cs.unlock();
#ifdef ES_DEBUG
  ES_DEBUG_TRACE(esT("EsCriticalSection::leave returned: %d"), result);
#endif
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// Semaphore
//

sem_t* EsSemaphore::semaphoreInit(
  ulong initVal
#if ES_OS == ES_OS_MAC
  ,
  const char* fmt,
  void* This,
  char* nameBuff
#endif
  )
{
  sem_t* sem = nullptr;

#if ES_OS == ES_OS_MAC

  snprintf(
    nameBuff,
    EsSemaphore::semNameLen,
    fmt,
    This
  );

  sem = sem_open(
    nameBuff,
    O_CREAT|O_EXCL,
    0700,
    initVal
  );

  if( SEM_FAILED == sem )
  {
    sem = nullptr;
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
  }

#else

  sem = reinterpret_cast<sem_t*>(malloc(sizeof(sem_t)));
  ES_ASSERT(sem);

  if( 0 > sem_init(sem, 0, initVal) )
  {
    free(sem);
    sem = nullptr;
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
  }

#endif

  return sem;
}
//---------------------------------------------------------------------------

void EsSemaphore::semaphoreUninit(
  sem_t* sem
#if ES_OS == ES_OS_MAC
  ,
  const char* name
#endif
  ) ES_NOTHROW
{
  if( sem )
  {
    try
    {
#ifdef ES_DEBUG
      int err =
#endif

#if ES_OS == ES_OS_MAC

      sem_close(sem);

# ifdef ES_DEBUG
      ES_DEBUG_TRACE(esT("sem_close returned: %d"), err);

      err =
# endif

      sem_unlink(name);

# ifdef ES_DEBUG
      ES_DEBUG_TRACE(esT("sem_unlink '%s' returned: %d"), name, err);
# endif

#else  // ES_OS_MAC

      sem_destroy(sem);

# ifdef ES_DEBUG
      ES_DEBUG_TRACE(esT("sem_destroy returned: %d"), err);
# endif

      free(sem);

#endif
    }
    catch(...) //< Do not allow anything to escape uninitializer to comply with ES_NOTHROW
    {}
  }
}
//---------------------------------------------------------------------------

EsSemaphore::EsSemaphore( ulong initialCount /*= 0*/, ulong maxCount /*= 0 */) :
m_mx(EsMutex::typeRecursive),
m_semLock(nullptr),
m_semFree(nullptr),
m_cnt(0),
m_maxCnt(0)
{
  if( maxCount == 0 )
    maxCount = SEM_VALUE_MAX;

  if( initialCount > maxCount )
    initialCount = maxCount;

  m_cnt = initialCount;
  m_maxCnt = maxCount;

  ulong initLockCnt = m_maxCnt-m_cnt;
  ulong initFreeCnt = static_cast<ulong>(SEM_VALUE_MAX)-m_maxCnt+m_cnt;

  // non-shared semaphore (in-process)
#if ES_OS == ES_OS_MAC

  m_semLock = semaphoreInit(
    m_maxCnt-m_cnt,
    "/__seml__%p__",
    this,
    m_nameSemLock
  );

  m_semFree = semaphoreInit(
    m_cnt,
    "/__semf__%p__",
    this,
    m_nameSemFree
  );

#else

  m_semLock = semaphoreInit(m_maxCnt-m_cnt);
  m_semFree = semaphoreInit(m_cnt);

#endif

}
//---------------------------------------------------------------------------

EsSemaphore::~EsSemaphore() ES_NOTHROW
{
  EsMutexLocker lock(m_mx);

#if ES_OS == ES_OS_MAC
  semaphoreUninit(
    m_semLock,
    m_nameSemLock
  );

  semaphoreUninit(
    m_semFree,
    m_nameSemFree
  );
#else
  semaphoreUninit(m_semLock);
  semaphoreUninit(m_semFree);
#endif

  m_semLock = nullptr;
  m_semFree = nullptr;
}
//---------------------------------------------------------------------------

bool EsSemaphore::isOk() const
{
  return (nullptr != m_semLock) && (nullptr != m_semFree);
}
//---------------------------------------------------------------------------

int EsSemaphore::lockDec()
{
  EsMutexLocker lock(m_mx);
  int err = 0;

  if( m_cnt )
  {
    err = sem_post(m_semLock);
    if( 0 == err )
      --m_cnt;
  }

  return err;
}
//---------------------------------------------------------------------------

int EsSemaphore::semaphoreTimedLock(ulong ms)
{
  int err = 0;

#ifdef ES_PTHREAD_NO_TIMEDLOCK

  while( ms-- )
  {
    err = sem_trywait(m_semFree);
    if( 0 == err )
      return lockDec();
    else if( EINVAL == err )
      return err;

    EsThread::sleep(1);
  }

  if( EAGAIN == err )
    err = ETIMEDOUT;

#else

  timespec tmo;
  timedwaitTmoCalc(ms, tmo);

  err = sem_timedwait(m_semFree, &tmo);
  if( 0 == err )
    err = lockDec();

#endif

  return err;
}
//---------------------------------------------------------------------------

EsSemaphore::Result EsSemaphore::wait(ulong ms)
{
  if( !isOk() )
    return resultInvalid;

  int err = 0;
  if( ES_INFINITE == ms )
  {
    err = sem_wait(m_semFree);
    if( 0 == err )
      err = lockDec();
  }
  else if( 0 == ms )
  {
    err = sem_trywait(m_semFree);
    if( -1 != err )
      err = lockDec();
  }
  else
    err = semaphoreTimedLock(ms);

  switch( err )
  {
  case 0:
    return resultOk;
  case EAGAIN:
    return resultBusy;
  case ETIMEDOUT:
    return resultTimeout;
  case EINVAL:
    return resultInvalid;
  }

  return resultError;
}
//---------------------------------------------------------------------------

// increments the semaphore count and signals one of the waiting threads
EsSemaphore::Result EsSemaphore::post()
{
  if( !isOk() )
    return resultInvalid;

  m_mx.lock();
  int err = 0;

  if( m_cnt < m_maxCnt )
  {
    err = sem_post(m_semFree);
    if( 0 == err )
      ++m_cnt;
    m_mx.unlock();
    err = sem_wait(m_semLock);
  }
  else
    m_mx.unlock();

  switch( err )
  {
  case EINVAL:
    return resultInvalid;
  case EOVERFLOW:
    return resultOverflow;
  }

  return resultOk;
}
//---------------------------------------------------------------------------

