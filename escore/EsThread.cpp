#include "escorepch.h"
#pragma hdrstop

#include "EsThread.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsThreadId EsThread::s_mainId = EsThread::currentIdGet();
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsThread.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsThread.posix.cxx"
#endif
//---------------------------------------------------------------------------

EsThreadState EsThread::stateGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_state;
}
//---------------------------------------------------------------------------

void EsThread::start(long priority, EsThreadStartState ss /*= EsThreadStartState::Normal*/)
{
  EsThreadState state = stateGet();
  if( EsThreadState::None != state )
  {
    switch( state )
    {
    case EsThreadState::Suspended:
      resume();
      break;
    case EsThreadState::Running:
      EsException::Throw(_("Thread is already executing"));
      break;
    case EsThreadState::Stopping:
      EsException::Throw(_("Thread is in stopping stage and could not be started until after termination"));
      break;
    }
  }
  else
  {
    // check priority value
    EsNumericCheck::checkRangeInteger(
      static_cast<long>(EsThreadPriority::Minimal),
      static_cast<long>(EsThreadPriority::Maximal),
      priority,
      esT("Thread priority")
    );
    m_priority = priority;

    // re-initialize semaphores && misc members
    //
    // reset error
    m_errorCode = 0;
    // reset error log
    m_errorCodes.clear();
    m_errorLog.clear();
    // reset start semaphore, just in case
    m_started.tryWait();

    // cleanup, just in case
    cleanup();

    // mark 'pause on start' condition
    if( EsThreadStartState::Suspended == ss )
      m_pause.tryWait();
    else
      m_pause.post();

    threadCreate();

    // wait until thread is really started
    m_started.wait(ES_INFINITE);
  }
}
//---------------------------------------------------------------------------

void EsThread::stop()
{
  if(
#if ES_OS == ES_OS_WINDOWS
    0 == m_thread
#elif defined(ES_POSIX_COMPAT)
    EsThreadIdNone == m_id
#endif
  )
    return;

  EsCriticalSectionLocker lock(m_cs);
  if( EsThreadState::None != m_state )
  {
    m_state = EsThreadState::Stopping;
    m_pause.post();
    m_stop.post();
  }
}
//---------------------------------------------------------------------------

void EsThread::stopAndWait()
{
  if(
#if ES_OS == ES_OS_WINDOWS
    0 == m_thread
#elif defined(ES_POSIX_COMPAT)
    EsThreadIdNone == m_id
#endif
  )
    return;

  if( EsThreadState::None != stateGet() )
  {
    stoppingSet();
    // wait until thread is stopped
    if( EsSemaphore::resultOk == m_stopped.wait(ES_INFINITE) )
      m_stopped.post(); // enable multiple wait for stop
  }

  cleanup();
}
//---------------------------------------------------------------------------

void EsThread::suspend()
{
  EsCriticalSectionLocker lock(m_cs);
  if( EsThreadState::Running == m_state )
  {
    if( EsSemaphore::resultOk == m_pause.wait() )
      m_state = EsThreadState::Suspended;
  }
}
//---------------------------------------------------------------------------

void EsThread::resume()
{
  EsCriticalSectionLocker lock(m_cs);
  if( EsThreadState::Suspended == m_state )
  {
    m_state = EsThreadState::Running;
    m_pause.post();
  }
}
//---------------------------------------------------------------------------

long EsThread::exitCodeGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_errorCode;
}
//---------------------------------------------------------------------------

const EsString::Array& EsThread::errorLogGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_errorLog;
}
//---------------------------------------------------------------------------

const EsThread::ErrorCodes& EsThread::errorCodesGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_errorCodes;
}
//---------------------------------------------------------------------------

void EsThread::errorLogAppend(long code, const EsString& msg)
{
  EsCriticalSectionLocker lock(m_cs);
  m_errorCode = code;

  if( !msg.empty() ) //< Only append errors with non-empty textual description
  {
    m_errorCodes.push_back(code);
    m_errorLog.push_back(msg);
  }
}
//---------------------------------------------------------------------------

void EsThread::resultSet(long code, const EsString& msg)
{
  errorLogAppend(
    code,
    msg
  );
}
//---------------------------------------------------------------------------

void EsThread::pausedCheck()
{
  // first, suspend on pause condition
  if( EsSemaphore::resultOk == m_pause.wait() )
    m_pause.post(); // allow multiple pause re-query by releasing semaphore
}
//---------------------------------------------------------------------------

bool EsThread::checkForStopping(ulong ms)
{
  pausedCheck();

  if( EsSemaphore::resultOk == m_stop.wait(ms) )
  {
    m_stop.post(); // allow multiple stop re-query by releasing semaphore
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void EsThread::runningSet()
{
  EsCriticalSectionLocker lock(m_cs);
  m_state = EsThreadState::Running;
  m_started.post();
}
//---------------------------------------------------------------------------

void EsThread::stoppingSet()
{
  EsCriticalSectionLocker lock(m_cs);
  m_state = EsThreadState::Stopping;
  m_pause.post();
  m_stop.post();
}
//---------------------------------------------------------------------------

void EsThread::startSuspendedCheck()
{
  if( EsSemaphore::resultOk != m_pause.tryWait() )
  {
    EsCriticalSectionLocker lock(m_cs);
    m_state = EsThreadState::Suspended;
  }
  else
    m_pause.post();
  pausedCheck();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class EsThreadStateProxyCtorAcc : public EsThreadStateProxy
{
public:
  EsThreadStateProxyCtorAcc(EsThread& thread) :
  EsThreadStateProxy(thread)
  {}
};
//---------------------------------------------------------------------------

// EsThreadStateProxy helper implementation
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsThreadStateProxy, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO_STD(EsThreadStateProxy, checkForStop, bool_Call_ulong, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

bool EsThreadStateProxy::checkForStop(ulong tmo)
{
  return m_thread.checkForStopping(tmo);
}
//---------------------------------------------------------------------------

bool EsThreadStateProxy::isBreaking() const
{
  return m_thread.checkForStopping(0);
}
//---------------------------------------------------------------------------

bool EsThreadStateProxy::isBreaking(ulong ms) const
{
  return m_thread.checkForStopping(ms);
}
//---------------------------------------------------------------------------

EsBaseIntf::Ptr EsThread::threadStateProxyGet()
{
  std::unique_ptr<EsThreadStateProxyCtorAcc> proxy = ES_MAKE_UNIQUE(
    EsThreadStateProxyCtorAcc,
    *this
  );
  ES_ASSERT(proxy);
  
  return proxy.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#ifndef ES_THREAD_ENTER
# define ES_THREAD_ENTER
#endif

#ifndef ES_THREAD_EXIT
# define ES_THREAD_EXIT
#endif
//---------------------------------------------------------------------------

EsThread::ResultT ES_THREADCALL EsThread::threadWorker( EsThread::ParamT param )
{
  EsThread* This = (EsThread*)param;
  ES_ASSERT(This);

  long error = 0;

  ES_THREAD_ENTER

  { //< Create explicit local scope start.
    //< To force deallocation of local objects, and unlocking
    //< thread-wide critial section.
    //< This is especially useful for POSIX thread implementation,
    //< because call to pthread_exit do not give a chance for scoped
    //< objects to free their resources.
    //

    EsString errorMsg;
    This->runningSet();
    // auto-set suspended condition
    This->startSuspendedCheck();

    try
    {
      This->onEnterWorker();
    }
    catch( const EsException& ex )
    {
      error = ex.codeGet();
      errorMsg = ex.messageGet();
      This->resultSet(error, errorMsg);
    }
    catch( const std::exception& stdex )
    {
      error = -1;
      errorMsg = EsString::fromUtf8(stdex.what());
      This->resultSet(error, errorMsg);
    }
    catch(...)
    {
      error = -1;
      errorMsg = _("Unknown exception occured on entering worker thread");
      This->resultSet(error, errorMsg);
    }

    if( 0 == error )
    {
      try
      {
        error = This->worker();
      }
      catch( const EsException& ex )
      {
        error = ex.codeGet();
        errorMsg = ex.messageGet();
      }
      catch( const std::exception& stdex )
      {
        error = -1;
        errorMsg = EsString::fromUtf8(stdex.what());
      }
      catch(...)
      {
        error = -1;
        errorMsg = _("Unknown exception occured in worker thread");
      }
      This->resultSet(error, errorMsg);
    }

    try
    {
      This->onExitWorker();
    }
    catch( const EsException& ex )
    {
      error = ex.codeGet();
      errorMsg = ex.messageGet();
      This->resultSet(error, errorMsg);
    }
    catch( const std::exception& stdex )
    {
      error = -1;
      errorMsg = EsString::fromUtf8(stdex.what());
      This->resultSet(error, errorMsg);
    }
    catch(...)
    {
      error = -1;
      errorMsg = _("Unknown exception occured upon exiting worker thread");
      This->resultSet(error, errorMsg);
    }

    EsCriticalSectionLocker lock(This->m_cs);
    This->m_stop.post();
    This->m_pause.post();
    This->m_stopped.post();
    This->m_state = EsThreadState::None;
    This->m_id = EsThreadIdNone;

  } //< Explicit local scope end

  ES_THREAD_EXIT

  return (EsThread::ResultT)error;
}
//---------------------------------------------------------------------------

bool EsThread::isMain()
{
  return !s_mainId ||
    s_mainId == currentIdGet();
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsThreadedTask::EsThreadedTask(WorkerT wFn /*= nullptr*/, PrologueEpilogueT inFn /*= nullptr*/, PrologueEpilogueT outFn /*= nullptr*/, size_t stack /*= 0 */) :
  EsThread(stack),
  m_wFn(wFn),
  m_inFn(inFn),
  m_outFn(outFn)
{}
//--------------------------------------------------------------------------------

void EsThreadedTask::start(WorkerT wFn, PrologueEpilogueT inFn /*= nullptr*/, PrologueEpilogueT outFn /*= nullptr*/, long priority /*= static_cast<long>(EsThreadPriority::Default)*/, EsThreadStartState ss /*= EsThreadStartState::Normal */)
{
  stopAndWait();

  m_wFn = wFn;
  m_inFn = inFn;
  m_outFn = outFn;

  EsThread::start(
    priority,
    ss
  );
}
//--------------------------------------------------------------------------------

long EsThreadedTask::worker()
{
  if( m_wFn )
    return m_wFn(*this);

  return 0;
}
//--------------------------------------------------------------------------------

void EsThreadedTask::onEnterWorker()
{
  if( m_inFn )
    m_inFn(*this);
}
//--------------------------------------------------------------------------------

void EsThreadedTask::onExitWorker()
{
  if( m_outFn )
    m_outFn(*this);
}
//--------------------------------------------------------------------------------
