#ifndef _es_thread_worker_h_
#define _es_thread_worker_h_

/// Reflected thread worker class.
/// For threaded code, it internally uses m_worker object, if it's set.
/// NB! weak reference is used for m_worker object, to avoid deadlocks due to
/// circular referencing.
/// Worker object may implement three methods:
/// onThreadEnter - executed in thread context, before starting actual worker cycles,
///  onThreadWork - worker cycles go here, loop until non-null result is returned, or
/// thread was externally stopped.
/// onThreadExit - executed in thread context, after all working cycles are over
///
ES_DECL_INTF_BEGIN( E5A769EF, 29FF4FAE, 9CC5C1AE, 567F0F46, EsThreadWorkerIntf )

  /// Control services
  ///

  /// Start thread with default priority, if it's not already running
  ES_DECL_INTF_METHOD(void, start)() = 0;
  /// Start thread with sepcified priority, if it's not already running
  ES_DECL_INTF_METHOD(void, start)(ulong priority) = 0;
  /// Signal thread to stop, do not wait for it to end
  ES_DECL_INTF_METHOD(void, stop)() = 0;
  /// Signal thread to stop, and blocking wait for it to end
  ES_DECL_INTF_METHOD(void, stopAndWait)() = 0;
  /// Append message to thread's error log
  ES_DECL_INTF_METHOD(void, errorLogAppend)(const EsString& msg) = 0;
  /// Access thread worker check for stopping timeout
  ES_DECL_INTF_METHOD(ulong, stoppingCheckTmoGet)() const = 0;
  ES_DECL_INTF_METHOD(void, stoppingCheckTmoSet)(ulong tmo) = 0;

  /// Thread worker state access
  ///

  /// Get thread identification value
  ES_DECL_INTF_METHOD(EsVariant, idGet)() const = 0;
  /// Get thread exit code value
  ES_DECL_INTF_METHOD(EsVariant, exitCodeGet)() const = 0;
  /// Get thread error log
  ES_DECL_INTF_METHOD(EsVariant, errorLogGet)() const = 0;
  /// Get thread state
  ES_DECL_INTF_METHOD(EsVariant, stateGet)() const = 0;
  /// Get thread priority
  ES_DECL_INTF_METHOD(EsVariant, priorityGet)() const = 0;
  /// Get thread breaker object
  ES_DECL_INTF_METHOD(EsVariant, breakerGet)() const = 0;
  /// Set thread worker implementation object
  ES_DECL_INTF_METHOD(void, workerSet)(const EsBaseIntfPtr& worker) = 0;
  /// Get thread worker implementation object
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, workerGet)() const = 0;

ES_DECL_INTF_END

/// EsThreadWorkerIntf implementation
///
class ESSCRIPT_CLASS ES_INTF_IMPL2(EsThreadWorker, EsThreadWorkerIntf, EsReflectedClassIntf)

private:
  EsThreadWorker(bool weakWorkerRef);

public:
  static EsThreadWorkerIntf::Ptr create(bool weakWorkerRef = true, const EsBaseIntfPtr& worker = EsBaseIntfPtr());
  virtual ~EsThreadWorker();

  ES_DECL_REFLECTED_CLASS_BASE(EsThreadWorker)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected constructor
  ///

  /// Default constructor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsVariant);
  /// Create thread worker taking actual worker object as the first parameter
  ES_DECL_REFLECTED_CLASS_CTOR1(EsVariant, cr_EsVariant);
  /// Create thread worker taking actual worker object as the first parameter, and an explicit weakref specifier (true/false) as a second one
  ES_DECL_REFLECTED_CLASS_CTOR2(EsVariant, cr_EsVariant, cr_EsVariant);

  /// EsThreadWorkerIntf reflected services
  ///
  /// Start thread with default priority, if it's not already running
  ES_DECL_REFLECTED_INTF_METHOD0(void, start);
  /// Start thread with sepcified priority, if it's not already running
  ES_DECL_REFLECTED_INTF_METHOD1(void, start, ulong);
  /// Signal thread to stop, do not wait for it to end
  ES_DECL_REFLECTED_INTF_METHOD0(void, stop);
  /// Signal thread to stop, and blocking wait for it to end
  ES_DECL_REFLECTED_INTF_METHOD0(void, stopAndWait);
  /// Append message to thread's error log
  ES_DECL_REFLECTED_INTF_METHOD1(void, errorLogAppend, cr_EsString);
  /// Access check for stopping timeout
  ES_DECL_INTF_METHOD(ulong, stoppingCheckTmoGet)() const;
  ES_DECL_INTF_METHOD(void, stoppingCheckTmoSet)(ulong tmo);

  /// EsThreadWorkerIntf reflected services
  ///
  /// Get thread identification value
  ES_DECL_INTF_METHOD(EsVariant, idGet)() const;
  /// Get thread exit code value
  ES_DECL_INTF_METHOD(EsVariant, exitCodeGet)() const;
  /// Get thread error log
  ES_DECL_INTF_METHOD(EsVariant, errorLogGet)() const;
  /// Get thread state
  ES_DECL_INTF_METHOD(EsVariant, stateGet)() const;
  /// Get thread priority
  ES_DECL_INTF_METHOD(EsVariant, priorityGet)() const;
  /// Get thread breaker object
  ES_DECL_INTF_METHOD(EsVariant, breakerGet)() const;
  /// Set thread worker implementation object
  ES_DECL_INTF_METHOD(void, workerSet)(const EsBaseIntfPtr& worker);
  /// Get thread worker implementation object
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, workerGet)() const;

  /// Reflected properties
  ///
  ES_DECL_PROPERTY_RO(id, EsVariant)
  ES_DECL_PROPERTY_RO(exitCode, EsVariant)
  ES_DECL_PROPERTY_RO(errorLog, EsVariant)
  ES_DECL_PROPERTY_RO(state, EsVariant)
  ES_DECL_PROPERTY_RO(priority, EsVariant)
  ES_DECL_PROPERTY_RO(breaker, EsVariant)
  ES_DECL_PROPERTY(worker, EsBaseIntfPtr)
  ES_DECL_PROPERTY(stoppingWaitTmo, ulong)

  /// Static reflected services
  ///
  ES_DECL_REFLECTED_CLASS_METHOD0(bool, isMain);
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, currentIdGet);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, sleep, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD0(void, yield);

protected:
  mutable EsCriticalSection m_cs;
  EsThread* m_impl;
  EsReflectedClassIntf::Ptr m_worker;
  volatile ulong m_stoppingWaitTmo;
  bool m_weakWorkerRef;

  friend class EsThreadImpl;
};

namespace EsReflection
{
/// Reflected thread state
ES_DECL_ENUMERATION_BEGIN(EsThreadState)
  ES_DECL_ENUMERATION_ITEM_LABELLED(None,      static_cast<ulong>(::EsThreadState::None),       _("Thread is not running"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Suspended,static_cast<ulong>(::EsThreadState::Suspended),  _("Thread is suspended"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Running,  static_cast<ulong>(::EsThreadState::Running),    _("Thread is running"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Stopping,  static_cast<ulong>(::EsThreadState::Stopping),   _("Thread is stopping"))
ES_DECL_ENUMERATION_END

/// Reflected thread priority
ES_DECL_ENUMERATION_BEGIN(EsThreadPriority)
  ES_DECL_ENUMERATION_ITEM_LABELLED(Minimal,  static_cast<ulong>(::EsThreadPriority::Minimal), _("Minimal thread priority"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Lower,    static_cast<ulong>(::EsThreadPriority::Lower),   _("Lower thread priority"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Default,  static_cast<ulong>(::EsThreadPriority::Default), _("Default thread priority"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Higher,    static_cast<ulong>(::EsThreadPriority::Higher),  _("Higher thread priority"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Maximal,  static_cast<ulong>(::EsThreadPriority::Maximal), _("Maximal thread priority"))
ES_DECL_ENUMERATION_END
}

#endif // _es_thread_worker_h_