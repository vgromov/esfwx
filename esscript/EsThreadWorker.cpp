#include "esscriptpch.h"
#pragma hdrstop

#include "EsThreadWorker.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// Reflected Thread worker class
//
// Internal thread worker implementation
class EsThreadImpl : public EsThread
{
public:
  EsThreadImpl(EsThreadWorker& This) :
  EsThread(),
  m_this(This),
  m_breaker(*this)
  {}

  EsBaseIntfPtr breakerGet()
  {
    EsBaseIntf::Ptr result(m_breaker.asBaseIntf(), false, false);
    return result;
  }

protected:
  virtual long worker() ES_OVERRIDE
  {
    long exitCode = 0;

    EsReflectedClassIntf::Ptr wrk;
    {
      EsCriticalSectionLocker lock(m_this.m_cs);
      wrk = m_this.m_worker;
    }

    if(
      !wrk ||
      !wrk->hasMethod(
        EsMethodInfoKeyT(
          0,
          esT("onThreadWork")
        )
      )
    )
      return exitCode;

    EsVariant result;
    do
    {
      result = wrk->call(
        esT("onThreadWork")
      );

    } while(
      result.isEmpty() &&
      !checkForStopping(m_this.m_stoppingWaitTmo)
    );

    if( !result.isEmpty() )
      exitCode = result.asLong();

    return exitCode;
  }

  virtual void onEnterWorker() ES_OVERRIDE
  {
    EsReflectedClassIntf::Ptr wrk;
    {
      EsCriticalSectionLocker lock(m_this.m_cs);
      wrk = m_this.m_worker;
    }

    if( wrk &&
      wrk->hasMethod(EsMethodInfoKeyT(0, esT("onThreadEnter")))
    )
      wrk->call(esT("onThreadEnter"));
  }

  virtual void onExitWorker() ES_OVERRIDE
  {
    EsReflectedClassIntf::Ptr wrk;
    {
      EsCriticalSectionLocker lock(m_this.m_cs);
      wrk = m_this.m_worker;
    }

    if( wrk &&
      wrk->hasMethod(EsMethodInfoKeyT(0, esT("onThreadExit")))
    )
      wrk->call(esT("onThreadExit"));
  }

  virtual bool checkForStopping(ulong ms) ES_OVERRIDE
  {
    EsReflectedClassIntf::Ptr wrk;
    {
      EsCriticalSectionLocker lock(m_this.m_cs);
      wrk = m_this.m_worker;
    }

    bool aborting = false;
    if( 
      wrk &&
      wrk->isScripted()
    )
    {
      EsScriptObjectIntf::Ptr obj = wrk;
      if( obj )
      {
        EsScriptMachine* machine = obj->machineGet();
        if(machine)
          aborting = machine->isAborting();
      }
    }

    return aborting || 
      EsThread::checkForStopping(ms);
  }

protected:
  EsThreadWorker& m_this;

  ES_THREADED_IO_BREAK_DECL_STD( EsThreadImpl )

  friend class EsThreadWorker;
};
//---------------------------------------------------------------------------

namespace EsReflection
{
  ES_DECL_ENUMERATION_INFO(EsThreadState, NO_CLASS_DESCR)
  ES_DECL_ENUMERATION_INFO(EsThreadPriority, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

class EsThreadWorkerCtorAcc : public EsThreadWorker
{
public:
  EsThreadWorkerCtorAcc(bool weakWorkerRef) :
  EsThreadWorker(weakWorkerRef)
  {}
};
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsThreadWorker, NO_CLASS_DESCR)
  /// Reflected properties
  ES_DECL_PROP_INFO_RO(                   EsThreadWorker, id, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsThreadWorker, exitCode, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsThreadWorker, errorLog, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsThreadWorker, state, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsThreadWorker, priority, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(                   EsThreadWorker, breaker, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsThreadWorker, worker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(                      EsThreadWorker, stoppingWaitTmo, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  // Ctors
  ES_DECL_REFLECTED_CTOR_INFO(            EsThreadWorker, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(            EsThreadWorker, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(            EsThreadWorker, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  /// Reflected services
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsThreadWorker, EsThreadWorkerIntf, start, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsThreadWorker, EsThreadWorkerIntf, start, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsThreadWorker, EsThreadWorkerIntf, stop, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsThreadWorker, EsThreadWorkerIntf, stopAndWait, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsThreadWorker, EsThreadWorkerIntf, errorLogAppend, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(     EsThreadWorker, EsThreadWorkerIntf, errorLogAppend, void_Call_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  /// Static reflected services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsThreadWorker, isMain, bool_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsThreadWorker, currentIdGet, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsThreadWorker, sleep, void_ClassCall_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsThreadWorker, yield, void_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

// Worker class mplementation
EsThreadWorker::EsThreadWorker(bool weakWorkerRef) :
m_impl(nullptr),
m_stoppingWaitTmo(0),
m_weakWorkerRef(weakWorkerRef)
{
  m_dynamic = true;
  m_impl = new EsThreadImpl(*this);
  ES_ASSERT(m_impl);
}
//---------------------------------------------------------------------------

EsThreadWorker::~EsThreadWorker()
{
  if( m_impl )
  {
    m_impl->stopAndWait();
    ES_DELETE(m_impl);
  }

  m_worker.reset();
}
//---------------------------------------------------------------------------

EsThreadWorkerIntf::Ptr EsThreadWorker::create(bool weakWorkerRef /*= true*/,
  const EsBaseIntfPtr& worker /*= nullptr*/)
{
  std::unique_ptr<EsThreadWorkerCtorAcc> tmp = ES_MAKE_UNIQUE(
    EsThreadWorkerCtorAcc,
    weakWorkerRef
  );
  ES_ASSERT(tmp);
  
  if( worker )
    tmp->workerSet(worker);

  return tmp.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::NEW()
{
  return create(true);
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::NEW(cr_EsVariant worker)
{
  return create(
    true,
    worker.asObject()
  );
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::NEW(cr_EsVariant worker, cr_EsVariant weakref)
{
  return create(
    weakref.asBool(), 
    worker.asObject()
  );
}
//---------------------------------------------------------------------------

void EsThreadWorker::start()
{
  ES_ASSERT(m_impl);
  m_impl->start();
}
//---------------------------------------------------------------------------

void EsThreadWorker::start(ulong priority)
{
  ES_ASSERT(m_impl);
  m_impl->start( priority );
}
//---------------------------------------------------------------------------

void EsThreadWorker::stop()
{
  ES_ASSERT(m_impl);
  m_impl->stop();
}
//---------------------------------------------------------------------------

void EsThreadWorker::stopAndWait()
{
  ES_ASSERT(m_impl);
  m_impl->stopAndWait();
}
//---------------------------------------------------------------------------

void EsThreadWorker::errorLogAppend(cr_EsString err)
{
  ES_ASSERT(m_impl);
  m_impl->errorLogAppend(
    as_<ulong>(EsScriptExceptionCode::EUnspecified),
    err
  );
}
//---------------------------------------------------------------------------

void EsThreadWorker::errorLogAppend(cr_EsVariant code, cr_EsVariant err)
{
  ES_ASSERT(m_impl);
  m_impl->errorLogAppend(
    code.asULong(),
    err.asString()
  );
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::idGet() const
{
  ES_ASSERT(m_impl);
  return static_cast<ullong>(m_impl->idGet());
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::exitCodeGet() const
{
  ES_ASSERT(m_impl);
  return m_impl->exitCodeGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::errorLogGet() const
{
  ES_ASSERT(m_impl);
  return m_impl->errorLogGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::stateGet() const
{
  ES_ASSERT(m_impl);
  return static_cast<ulong>(m_impl->stateGet());
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::priorityGet() const
{
  ES_ASSERT(m_impl);
  return static_cast<ulong>(m_impl->priorityGet());
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::breakerGet() const
{
  ES_ASSERT(m_impl);
  return reinterpret_cast<EsThreadImpl*>(m_impl)->breakerGet();
}
//---------------------------------------------------------------------------

void EsThreadWorker::workerSet(const EsBaseIntfPtr& worker)
{
  EsCriticalSectionLocker lock(m_cs);
  EsReflectedClassIntf::Ptr rc = worker;

  if( m_weakWorkerRef ) //< Hold weak reference
  {
    if( rc )
    {
      m_worker = rc->asWeakReference();
      return;
    }
  }

  // Otherwise, we own worker impl object, so just assign it
  // It covers the null case as well
  m_worker = rc;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsThreadWorker::workerGet() const
{
  return m_worker;
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::get_id() const
{
  return idGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::get_exitCode() const
{
  return exitCodeGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::get_errorLog() const
{
  return errorLogGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::get_state() const
{
  return stateGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::get_priority() const
{
  return priorityGet();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::get_breaker() const
{
  return breakerGet();
}
//---------------------------------------------------------------------------

void EsThreadWorker::set_worker(const EsBaseIntfPtr& worker)
{
  workerSet(worker);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsThreadWorker::get_worker() const
{
  return workerGet();
}
//---------------------------------------------------------------------------

ulong EsThreadWorker::get_stoppingWaitTmo() const
{
  return m_stoppingWaitTmo;
}
//---------------------------------------------------------------------------

void EsThreadWorker::set_stoppingWaitTmo(const ulong& tmo)
{
  m_stoppingWaitTmo = tmo;
}
//---------------------------------------------------------------------------

ulong EsThreadWorker::stoppingCheckTmoGet() const
{
  return m_stoppingWaitTmo;
}
//---------------------------------------------------------------------------

void EsThreadWorker::stoppingCheckTmoSet(ulong tmo)
{
  m_stoppingWaitTmo = tmo;
}
//---------------------------------------------------------------------------

bool EsThreadWorker::isMain()
{
  return EsThread::isMain();
}
//---------------------------------------------------------------------------

EsVariant EsThreadWorker::currentIdGet()
{
  return static_cast<ullong>(EsThread::currentIdGet());
}
//---------------------------------------------------------------------------

void EsThreadWorker::sleep(ulong ms)
{
  EsThread::sleep(ms);
}
//---------------------------------------------------------------------------

void EsThreadWorker::yield()
{
  EsThread::yield();
}
//---------------------------------------------------------------------------

