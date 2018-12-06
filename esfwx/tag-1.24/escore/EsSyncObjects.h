#ifndef _es_sync_objects_h_
#define _es_sync_objects_h_

// multi-thread syncronization primitives and helper classes
//
// thread id type
//
#if ES_OS == ES_OS_WINDOWS
  typedef DWORD EsThreadId;
#  define EsThreadIdNone ((EsThreadId)-1)
# define ES_INFINITE     INFINITE
#elif defined(ES_POSIX_COMPAT)
  typedef llong EsThreadId;
#  define EsThreadIdNone ((EsThreadId)-1)
# define ES_INFINITE     ((ulong)-1)
#endif
//---------------------------------------------------------------------------

// mutex
//
class ESCORE_CLASS EsMutex
{
public:
  enum Type {
    typeDefault,
    typeRecursive
  };

  enum Result {
    resultOk = 0,            // operation completed successfully
    resultInvalid,          // mutex hasn't been initialized
    resultDeadlock,          // mutex is already locked by the calling thread
    resultBusy,              // mutex is already locked by another thread
    resultAlreadyUnlocked,  // attempt to unlock a mutex which is not locked
    resultTimeout,          // lock(ms) has timed out
    resultError              // any other error
  };

public:
  EsMutex(Type type = typeDefault);
  ~EsMutex();

  // validity check
  bool isOk() const;
  Result lock();
  Result lock(ulong ms);
  Result tryLock();
  Result unlock();

private:
  // prohibited functinoality
  EsMutex(const EsMutex&) ES_REMOVEDECL;
  EsMutex& operator=(const EsMutex&) ES_REMOVEDECL;

private:
  Type m_type;
  EsThreadId m_owningThreadId;
#if ES_OS == ES_OS_WINDOWS
  HANDLE m_mx;
#elif defined(ES_POSIX_COMPAT)
  pthread_mutexattr_t m_mxAttrs;
  pthread_mutex_t m_mx;
#endif
};
//---------------------------------------------------------------------------

class ESCORE_CLASS EsMutexLocker
{
public:
  EsMutexLocker(EsMutex& mx);
  ~EsMutexLocker();
  bool isOk() const { return m_ok; }

private:
  // prohibited functionality
  EsMutexLocker() ES_REMOVEDECL;
  EsMutexLocker(const EsMutexLocker&) ES_REMOVEDECL;
  EsMutexLocker& operator=(const EsMutexLocker&) ES_REMOVEDECL;

private:
  EsMutex& m_mx;
  bool m_ok;
};
//---------------------------------------------------------------------------

// critcal section
//
class ESCORE_CLASS EsCriticalSection
{
public:
  typedef std::unique_ptr<EsCriticalSection> Ptr;

public:
  EsCriticalSection();
  ~EsCriticalSection();

  void enter();
  bool tryEnter();
  void leave();

private:
  // prohibited functionality
  EsCriticalSection(const EsCriticalSection&) ES_REMOVEDECL;
  EsCriticalSection& operator=(const EsCriticalSection&) ES_REMOVEDECL;

private:
#if ES_OS == ES_OS_WINDOWS
  CRITICAL_SECTION
#elif defined(ES_POSIX_COMPAT)
  EsMutex
#endif
    m_cs;
};
//---------------------------------------------------------------------------

class ESCORE_CLASS EsCriticalSectionLocker
{
public:
  EsCriticalSectionLocker(EsCriticalSection& obj);
  ~EsCriticalSectionLocker();

private:
  // prohibited functionality
  EsCriticalSectionLocker() ES_REMOVEDECL;
  EsCriticalSectionLocker(const EsCriticalSectionLocker&) ES_REMOVEDECL;
  EsCriticalSectionLocker& operator=(const EsCriticalSectionLocker&) ES_REMOVEDECL;

private:
  EsCriticalSection& m_obj;
};
//---------------------------------------------------------------------------

/// NB! This locker does not have an ownership over critical section pointer object,
/// so be extremely cautious not to reset cs pointer _before_ unlocking it.
/// Use an extra {} scope to assure locker deallocation, and use cs.reset() outside
/// the scope, afterwards
///
class ESCORE_CLASS EsCriticalSectionPtrLocker
{
public:
  EsCriticalSectionPtrLocker( EsCriticalSection::Ptr& obj );
  ~EsCriticalSectionPtrLocker();

private:
  // prohibited functionality
  EsCriticalSectionPtrLocker() ES_REMOVEDECL;
  EsCriticalSectionPtrLocker( const EsCriticalSectionPtrLocker& ) ES_REMOVEDECL;
  EsCriticalSectionPtrLocker& operator=( const EsCriticalSectionPtrLocker& ) ES_REMOVEDECL;

private:
  EsCriticalSection::Ptr& m_obj;
};
//---------------------------------------------------------------------------

// semaphore
//
class ESCORE_CLASS EsSemaphore
{
public:
  enum Result
  {
    resultOk = 0,
    resultInvalid,         // semaphore hasn't been initialized successfully
    resultBusy,            // returned by tryWait() if wait() would block
    resultTimeout,         // returned by waitTimeout()
    resultOverflow,        // post() would increase counter past the max
    resultError
  };

public:
  /// Default constructor.
  /// Specifying a maxcount of 0 actually makes EsSemaphore behave as if there
  /// is no upper limit, if maxcount is 1 the semaphore behaves as a mutex
  ///
  EsSemaphore( ulong initialCount = 0, ulong maxCount = 0 );
  ~EsSemaphore();

  /// Validity check
  bool isOk() const;

  /// Same as wait(), but as a timeout limit, returns wxSEMA_NO_ERROR if the
  /// semaphore was acquired and resultTimeout if the timeout has elapsed
  ///
  Result wait(ulong ms);

  /// Wait indefinitely, until the semaphore count goes beyond 0
  /// and then decrement it and return
  ///
  Result wait();

  /// Same as wait(), but does not block, returns resultOk if
  /// successful and resultBusy if the count is currently zero
  ///
  Result tryWait();

  /// Increments the semaphore count and signals one of the waiting threads
  ///
  Result post();

private:
#if defined(ES_POSIX_COMPAT)
  int semaphoreTimedLock(ulong ms);
  int lockDec();
# if ES_OS == ES_OS_MAC
  static sem_t* semaphoreInit( ulong initVal, const char* fmt, void* This, char* nameBuff );
  static void semaphoreUninit( sem_t* sem, const char* nameBuff );
# else
  static sem_t* semaphoreInit( ulong initVal );
  static void semaphoreUninit( sem_t* sem );
# endif
#endif

private:
#if ES_OS == ES_OS_WINDOWS

  HANDLE m_sem;

#elif defined(ES_POSIX_COMPAT)

# if ES_OS == ES_OS_MAC

  enum { semNameLen = 32 };

  char m_nameSemLock[semNameLen];
  char m_nameSemFree[semNameLen];

# endif

  sem_t* m_semLock;
  sem_t* m_semFree;
  EsMutex m_mx;
  ulong m_cnt;
  ulong m_maxCnt;

#else
#  error Semaphore is not implemented in this OS
#endif

private:
  // Prohibited functionality
  EsSemaphore(const EsSemaphore&) ES_REMOVEDECL;
  EsSemaphore& operator=(const EsSemaphore&) ES_REMOVEDECL;
};
//---------------------------------------------------------------------------

#endif // _es_synch_objects_h_
