//---------------------------------------------------------------------------

#if ES_OS == ES_OS_ANDROID
# if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#   include <Androidapi.NativeActivity.hpp>
# endif
#elif (ES_OS == ES_OS_LINUX) || (ES_OS == ES_OS_UNIX)
# include <limits.h> //< PTHREAD_STACK_MIN
#endif
//---------------------------------------------------------------------------

EsThread::EsThread(size_t stack /*= 0*/) :
m_state(EsThreadState::None),
m_started(0, 1),
m_pause(0, 1),
m_stop(0, 1),
m_stopped(1, 1),
m_errorCode(0),
m_stack(stack),
m_id(EsThreadIdNone)
{}
//---------------------------------------------------------------------------

void EsThread::threadCreate()
{
  pthread_attr_t attrs;

  checkPthreadError(
    pthread_attr_init(
      &attrs
    )
  );

  checkPthreadError(
    pthread_attr_setdetachstate(
      &attrs,
      PTHREAD_CREATE_JOINABLE
    )
  );

  if( m_stack )
  {
    if( m_stack < PTHREAD_STACK_MIN )
      m_stack = PTHREAD_STACK_MIN;

    checkPthreadError(
      pthread_attr_setstacksize(
        &attrs,
        m_stack
      )
    );
  }

  ES_ASSERT(EsThreadIdNone == m_id);

  checkPthreadError(
    pthread_create(
      &m_thread,
      &attrs,
      &EsThread::threadWorker,
      (void*)this
    )
  );
  m_id = pthreadIdGet(m_thread);

#ifndef ES_PTHREAD_NO_PRIO
  checkPthreadError(
    pthread_setschedprio(
      m_thread,
      priorityCalc(attrs)
    )
  );
#endif

  pthread_attr_destroy( &attrs );
}
//---------------------------------------------------------------------------

EsThread::~EsThread()
{
  stopAndWait();
}
//---------------------------------------------------------------------------

ES_COMPILE_TIME_ASSERT(sizeof(pthread_t) == sizeof(EsThreadId), _check_pthread_t_fits_EsThreadId);

EsThreadId EsThread::pthreadIdGet(const pthread_t& thread) ES_NOTHROW
{
  return reinterpret_cast<EsThreadId>(thread);
}
//---------------------------------------------------------------------------

void EsThread::checkPthreadError(int err)
{
  if( err )
    EsException::ThrowOsError(err);
}
//---------------------------------------------------------------------------

void EsThread::cleanup()
{
  if( EsThreadIdNone != m_id )
  {
    ResultT res;
#ifdef ES_DEBUG
    int err =
#endif
    pthread_join(m_thread, &res);
#ifdef ES_DEBUG
    ES_ASSERT(0 == err);
#endif
  }

  // reset stop condition, just in case
  m_state = EsThreadState::None;
  m_stop.tryWait();
  m_id = EsThreadIdNone;
}
//---------------------------------------------------------------------------

int EsThread::priorityCalc(pthread_attr_t& attrs) const
{
  int policy;
  int max_prio_for_policy = 0;
  int min_prio_for_policy = 0;

  checkPthreadError(
    pthread_attr_getschedpolicy(
      &attrs,
      &policy
    )
  );
  max_prio_for_policy = sched_get_priority_max(policy);
  min_prio_for_policy = sched_get_priority_min(policy);
  int prioStep = (max_prio_for_policy-min_prio_for_policy)/4;

  // convert generic to native priority
  if(m_priority <= 20)
    return min_prio_for_policy;
  else if (m_priority <= 40)
    return min_prio_for_policy+prioStep;
  else if (m_priority <= 60)
    return min_prio_for_policy+prioStep*2;
  else if (m_priority <= 80)
    return min_prio_for_policy+prioStep*3;
  else
    return max_prio_for_policy;
}
//---------------------------------------------------------------------------

void EsThread::onThreadExit(EsThread::ResultT result)
{
#if ES_OS == ES_OS_ANDROID
# if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  // Detach the NativeActivity virtual machine to ensure the proper relase
  // of JNI context attached to the current thread
  PANativeActivity pna = reinterpret_cast<PANativeActivity>(System::DelphiActivity);
  ES_ASSERT(pna);
  ES_ASSERT(pna->vm);
  pna->vm->DetachCurrentThread();
# else
#   error Not implemented!
# endif
#endif

  pthread_exit(result);
}

#define ES_THREAD_EXIT onThreadExit((EsThread::ResultT)error);
//---------------------------------------------------------------------------

EsThreadId EsThread::currentIdGet()
{
  pthread_t cur = pthread_self();
  return pthreadIdGet(cur);
}
//---------------------------------------------------------------------------

void EsThread::sleep(ulong ms)
{
  usleep(ms*1000);
}
//---------------------------------------------------------------------------

void EsThread::yield()
{
#if defined(ES_PTHREAD_NO_YIELD)
  EsThread::sleep(0);
#else
  pthread_yield();
#endif
}
//---------------------------------------------------------------------------


