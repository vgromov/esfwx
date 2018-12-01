#ifndef _es_thread_h_
#define _es_thread_h_

/// Non-reflected thread state
enum class EsThreadState : ulong
{
  None,
  Suspended,
  Running,
  Stopping
};

enum class EsThreadPriority : ulong
{
  Minimal  = 0,
  Lower    = 30,
  Default  = 50,
  Higher   = 70,
  Maximal  = 100
};

enum class EsThreadStartState : ulong
{
  Normal,
  Suspended
};

// Controlled detached thread wrapper class
//
class ESCORE_CLASS EsThread
{
public:
  EsThread(size_t stack = 0);
  virtual ~EsThread();

  EsThreadId idGet() const { return m_id; }

  /// Thread control services
  ///

  /// Request thread state
  EsThreadState stateGet() const;

  /// Request thread priority
  long priorityGet() const { return m_priority; }

  /// Create and start thread if it's not already created
  void start(long priority = static_cast<long>(EsThreadPriority::Default),
    EsThreadStartState ss = EsThreadStartState::Normal );

  /// Suspend-resume functionality
  void suspend();
  void resume();

  /// Signal stop, do not wait for it to end
  void stop();

  /// Signal stop and wait for thread to end
  void stopAndWait();

  /// Access the thread error conditions
  ///
  long exitCodeGet() const;
  const EsString::Array& errorLogGet() const;
  void errorLogAppend(const EsString& msg);

  /// Thread static services
  ///
  static bool isMain();
  static EsThreadId currentIdGet();
  static void sleep(ulong ms);
  static void yield();
#if defined(ES_POSIX_COMPAT)
  static void checkPthreadError(int err);
#endif

protected:
  // Thread worker body. to be implemented in derived classes
  virtual long worker() = 0;
  virtual void onEnterWorker() {}
  virtual void onExitWorker() {}

  // internal services
  void cleanup();
  void resultSet(long error, const EsString& msg);
  void runningSet();
  void stoppingSet();
  void pausedCheck();
  void startSuspendedCheck();

  /// Internal check for pause|stop condition, to be called from worker thread execution code
  /// ms is time in milliseconds to wait for stop condition to be set from elsewhere
  ///
  bool checkForStopping(ulong ms = 0);

  /// Return helper thread state proxy reflected object
  /// NB! should only be used locally in thread workers which use
  /// reflected 'check for stop' service
  ///
  EsBaseIntf::Ptr threadStateProxyGet();

private:
  // Prohibited functionality
  EsThread(const EsThread&) ES_REMOVEDECL;
  EsThread& operator= (const EsThread&) ES_REMOVEDECL;

private:
#if ES_OS == ES_OS_WINDOWS
# define ES_THREADCALL  __stdcall
  typedef esU32 ResultT;
  typedef LPVOID ParamT;
#elif defined(ES_POSIX_COMPAT)
# define ES_THREADCALL
  typedef void* ResultT;
  typedef void* ParamT;
  static EsThreadId pthreadIdGet(pthread_t* thread);
  static void onThreadExit(EsThread::ResultT result);
#endif

  void threadCreate();

  /// Actual thread worker
  static ResultT ES_THREADCALL threadWorker( EsThread::ParamT param );

  /// Thread priority calculation helper
  int priorityCalc();

protected:
  // recently executed thread error log
  EsString::Array m_errorLog;

  // MT access guard
  mutable EsCriticalSection m_cs;

  // Thread control
  //
  EsSemaphore m_started;
  EsSemaphore m_pause;
  EsSemaphore m_stop;
  EsSemaphore m_stopped;

  static EsThreadId s_mainId;
  // thread state
  EsThreadState m_state;
  // recently executed thread error status
  long m_errorCode;
  // requested stack size
  size_t m_stack;
  long m_priority;

  EsThreadId m_id;
#if ES_OS == ES_OS_WINDOWS
  HANDLE m_thread;
#elif defined(ES_POSIX_COMPAT)
  pthread_attr_t m_threadAttr;
  pthread_t m_thread;
#endif

  friend class EsThreadStateProxy;
};

/// Helper reflected proxy for thread stop cheking in scriptlets.
/// May be used as io breaker in threaded channels as well.
///
class ES_INTF_IMPL2(EsThreadStateProxy, EsBreakIntf, EsReflectedClassIntf)

protected:
  EsThreadStateProxy(EsThread& thread) :
  m_thread(thread)
  { m_dynamic = true; }

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsThreadStateProxy)
  ES_DECL_ATTR_HANDLING_STD

  // reflected services
  ES_DECL_REFLECTED_METHOD1(bool, checkForStop, ulong);
  ES_DECL_INTF_METHOD(bool, isBreaking)() const;
  ES_DECL_INTF_METHOD(bool, isBreaking)(ulong ms) const;

protected:
  EsThread& m_thread;
  friend class EsThread;
};

#endif // _es_thread_h_
