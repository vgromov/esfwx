//---------------------------------------------------------------------------

#if ES_OS == ES_OS_ANDROID
# if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#   include <Androidapi.NativeActivity.hpp>
# endif
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
{
	checkPthreadError(
		pthread_attr_init(&m_threadAttr)
		);

	checkPthreadError(
		pthread_attr_setdetachstate(&m_threadAttr, PTHREAD_CREATE_JOINABLE)
		);

	if( stack )
	{
		if( stack < PTHREAD_STACK_MIN )
			stack = PTHREAD_STACK_MIN;

		checkPthreadError(
			pthread_attr_setstacksize(&m_threadAttr, stack)
			);
	}
}
//---------------------------------------------------------------------------

EsThread::~EsThread()
{
	stopAndWait();

#ifdef ES_DEBUG
	int err =
#endif
	pthread_attr_destroy(&m_threadAttr);
#ifdef ES_DEBUG
	ES_ASSERT(0 == err);
#endif
}
//---------------------------------------------------------------------------

ES_COMPILE_TIME_ASSERT(sizeof(pthread_t) <= sizeof(EsThreadId), checkEsThreadIdSize);

EsThreadId EsThread::pthreadIdGet(pthread_t* thread)
{
  ES_ASSERT(thread);

  EsThreadId id = 0;
  memcpy(&id, thread, sizeof(pthread_t));

	return id;
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

int EsThread::priorityCalc()
{
	int policy;
	int max_prio_for_policy = 0;
	int min_prio_for_policy = 0;
	checkPthreadError(
		pthread_attr_getschedpolicy(&m_threadAttr, &policy)
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

void EsThread::threadCreate()
{
  ES_ASSERT(EsThreadIdNone == m_id);

  checkPthreadError(
    pthread_create(&m_thread, &m_threadAttr, &EsThread::threadWorker, (void*)this)
    );
  m_id = pthreadIdGet(&m_thread);

#ifndef ES_PTHREAD_NO_PRIO
  checkPthreadError(
    pthread_setschedprio(&m_thread, priorityCalc())
    );
#endif
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
	return pthreadIdGet(&cur);
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


