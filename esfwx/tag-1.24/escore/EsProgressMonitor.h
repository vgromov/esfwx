#ifndef _progress_monitor_h_
#define _progress_monitor_h_

/// @file EsProgressMonitor.h
/// Progress monitor interface declaration and basic functionality implementation
///
class EsProgressMonitor;

/// Progress monitor event notification ids. All progress monitor events belong to the
/// "progressMonitor" category
///
#define EVTC_PROGRESS_MONITOR_PFX esT("progressMonitor-")

enum EsProgressMonitorEventId
{
  evtTaskTextChanged,
  evtTaskStateChanged,
  evtTaskProgressChanged,
  evtTaskAdded,
  evtTaskRemoved,
  evtNoMoreTasks
};

/// Progress monitor task interface
///
ES_DECL_INTF_BEGIN( C9D5F372, 2EDF4904, 928FE50A, C4306020, EsProgressMonitorTaskIntf )

protected:
  /// Progress monitor task state.
  ///
  enum
  {
    statePulsing,
    stateNormal,
    stateComplete
  };

public:
  /// Access owning monitor
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, monitorGet)() const = 0;
  /// Access parent task, if any
  ES_DECL_INTF_METHOD(EsBaseIntfPtr, parentGet)() const = 0;
  /// Optional progress text formatter setter. 'progressTextFormatter' argument
  /// should be variant collection, containing exactly 2 items, or empty variant, if
  /// text formatter is re-set. First item is either reflected object, or string-name of
  /// reflected service, containing formatter. Second item is string - reflected method's name
  /// or static formatting service name. Formatting service or method shouls take
  /// single input parameter - the task interface pointer, and produce string on output.
  ES_DECL_INTF_METHOD(void, progressTextFormatterSet)(const EsVariant& progressTextFormatter) = 0;
  /// Access task identifier
  ES_DECL_INTF_METHOD(const EsString&, idGet)() const = 0;
  /// Task orphaned check (Task is considered orphaned, if not assigned to monitor)
  ES_DECL_INTF_METHOD(bool, isOrphaned)() const = 0;
  /// Task complete check (Task is considered complete after end() is called)
  ES_DECL_INTF_METHOD(bool, isComplete)() const = 0;
  /// Task determined state check. Return true if in 'pulse' mode, and completion
  /// status cannot be calculated.
  ES_DECL_INTF_METHOD(bool, isPulsing)() const = 0;
  /// Check if task has sub-tasks
  ES_DECL_INTF_METHOD(bool, hasChildren)() const = 0;
  /// Get task path (All ids up to top, concatenated with '/' separator)
  ES_DECL_INTF_METHOD(EsString, pathGet)() const = 0;
  /// Task text
  ES_DECL_INTF_METHOD(EsString, textGet)() const = 0;
  ES_DECL_INTF_METHOD(void, textSet)(const EsString& text) = 0;
  /// Read-only access to the progress text string
  ES_DECL_INTF_METHOD(EsString, progressTextGet)() const = 0;
  /// Task range (range write may optionally invalidate current position).
  /// Setting range to empty, effectively switches task into pulse
  /// (completion unknown) mode.
  ES_DECL_INTF_METHOD(EsVariant, rangeGet)() const = 0;
  ES_DECL_INTF_METHOD(void, rangeSet)(const EsVariant& range) = 0;
  /// Task value (position). Setting position to empty, effectively switches
  /// task into pulse (completion unknown) mode.
  ES_DECL_INTF_METHOD(EsVariant, valueGet)() const = 0;
  ES_DECL_INTF_METHOD(void, valueSet)(const EsVariant& val) = 0;
  /// Set text, range, value in one call
  ES_DECL_INTF_METHOD(void, initialize)(const EsString& text, const EsVariant& range, const EsVariant& val) = 0;
  /// Pulse task service. In Pulse mode task's completion status is unknown.
  /// It's just the way to indicate some action activity
  ES_DECL_INTF_METHOD(void, pulse)(const EsString& text) = 0;
  /// Calculate total tasks completion. Values 0. .. 1. - indicate completion.
  /// Empty value is treated as undefined, but active status (as if global pulse is in task).
  ES_DECL_INTF_METHOD(EsVariant, completionGet)() const = 0;
  /// End task. Task becomes complete
  ES_DECL_INTF_METHOD(void, end)(const EsString& text) = 0;

  /// Child tasks management
  ///
  /// Attach task (and all its child tasks) to the parent task|progress monitor.
  /// If task with such ID already exists - an exception must be thrown
  ES_DECL_INTF_METHOD(void, attachTo)(const EsBaseIntfPtr& taskOrMonitor) = 0;
  /// Detach task and all its children from parent task and|or progress monitor
  ES_DECL_INTF_METHOD(void, detach)() = 0;

ES_DECL_INTF_END

/// Progress monitor interface
///
ES_DECL_INTF_BEGIN( 99860C17, 23CE478A, BC91CCE4, B706483D, EsProgressMonitorIntf )

  /// Return view event category
  ES_DECL_INTF_METHOD(const EsString&, eventCategoryGet)() const = 0;
  /// Return progress monitor id string
  ES_DECL_INTF_METHOD(const EsString&, idGet)() const = 0;
  /// Entirely resets progress monitor, effectively detaching all tasks from it.
  ES_DECL_INTF_METHOD(void, reset)() = 0;
  /// Post event to the view
  ES_DECL_INTF_METHOD(void, viewEventPost)(ulong id, const EsVariant& payload) const = 0;
  /// Retrieve all linked task paths
  ES_DECL_INTF_METHOD(EsString::Array, taskPathsGet)() const = 0;
  /// Get task by its path
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, taskGetByPath)(const EsString& path) const = 0;

ES_DECL_INTF_END

/// Progress monitor task functionality implementation.
/// All progress task classes should be derived from this base.
///
class ESCORE_CLASS ES_INTF_IMPL2(EsProgressMonitorTask, EsProgressMonitorTaskIntf, EsReflectedClassIntf)

private:
  EsProgressMonitorTask(const EsString& id, const EsString& text,  const EsVariant& range, const EsVariant& val);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsProgressMonitorTask)
  ES_DECL_ATTR_HANDLING_STD

  virtual ~EsProgressMonitorTask();

  static EsProgressMonitorTaskIntf::Ptr create(const EsString& id, const EsString& text = EsString::null(), const EsVariant& range = EsVariant::null(), const EsVariant& val = EsVariant::null());

  /// EsProgressMonitorTaskIntf implementation
  ///
  /// Access owning monitor
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsBaseIntfPtr, monitorGet) ES_OVERRIDE;

  /// Access parent task, if any
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsBaseIntfPtr, parentGet) ES_OVERRIDE;

  /// Optional progress text formatter setter. 'progressTextFormatter' argument
  /// should be variant collection, containing exactly 2 items, or empty variant, if
  /// text formatter is re-set. First item is either reflected object, or string-name of
  /// reflected service, containing formatter. Second item is string - reflected method's name
  /// or static formatting service name. Formatting service or method shouls take
  /// single input parameter - the task interface pointer, and produce string on output.
  ES_DECL_INTF_METHOD(void, progressTextFormatterSet)(const EsVariant& progressTextFormatter) ES_OVERRIDE;

  /// Access task identifier
  ES_DECL_INTF_METHOD(const EsString&, idGet)() const ES_OVERRIDE;

  /// Task orphaned check (Task is considered orphaned, if not assigned to either monitor or other task)
  ES_DECL_INTF_METHOD(bool, isOrphaned)() const ES_OVERRIDE;

  /// Task complete check (Task is considered complete after end() is called)
  ES_DECL_INTF_METHOD(bool, isComplete)() const ES_OVERRIDE;

  /// Task determined state check. Return true if in 'pulse' mode, and completion
  /// status cannot be calculated.
  ///
  ES_DECL_INTF_METHOD(bool, isPulsing)() const ES_OVERRIDE;

  /// Check if task has sub-tasks
  ES_DECL_INTF_METHOD(bool, hasChildren)() const ES_OVERRIDE;

  /// Get task path (All ids up to top, concatenated with '/' separator)
  ES_DECL_INTF_METHOD(EsString, pathGet)() const ES_OVERRIDE;

  /// Task text
  ES_DECL_INTF_METHOD(EsString, textGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, textSet)(const EsString& text) ES_OVERRIDE;

  /// Read-only access to the progress text string
  ES_DECL_INTF_METHOD(EsString, progressTextGet)() const ES_OVERRIDE;

  /// Task range (range write may optionally invalidate current position).
  /// Setting range to empty, effectively switches task into pulse
  /// (completion unknown) mode.
  ///
  ES_DECL_INTF_METHOD(EsVariant, rangeGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, rangeSet)(const EsVariant& range) ES_OVERRIDE;

  /// Task value (position). Setting position to empty, effectively switches
  /// task into pulse (completion unknown) mode.
  ES_DECL_INTF_METHOD(EsVariant, valueGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, valueSet)(const EsVariant& val) ES_OVERRIDE;

  /// Set text, range, value in one call
  ES_DECL_REFLECTED_INTF_METHOD3(void, initialize, cr_EsString, cr_EsVariant, cr_EsVariant) ES_OVERRIDE;

  /// Pulse task service. In Pulse mode task's completion status is unknown.
  /// It's just the way to indicate some action activity
  ES_DECL_REFLECTED_INTF_METHOD1(void, pulse, cr_EsString) ES_OVERRIDE;

  /// Calculate total tasks completion. Values 0. .. 1. - indicate completion.
  /// Empty value is treated as undefined, but active status (as if global pulse is in task).
  ES_DECL_INTF_METHOD(EsVariant, completionGet)() const ES_OVERRIDE;

  /// Signals task as complete.
  ES_DECL_REFLECTED_INTF_METHOD1(void, end, cr_EsString) ES_OVERRIDE;

  /// Child tasks management
  ///
  /// Attach task (and all its child tasks) to the progress monitor.
  /// If task with such ID already exists - an exception must be thrown
  ES_DECL_INTF_METHOD(void, attachTo)(const EsBaseIntf::Ptr& monitorOrTask) ES_OVERRIDE;
  ES_DECL_REFLECTED_METHOD1(void, attachToReflected, cr_EsVariant);

  /// Detach task and all its children from parent task and|or progress monitor
  ES_DECL_REFLECTED_INTF_METHOD0(void, detach) ES_OVERRIDE;

  /// Reflected properties (based on certain interface servies)
  ///
  ES_DECL_PROPERTY_RO(id, EsString)
  ES_DECL_PROPERTY_RO(orphaned, bool)
  ES_DECL_PROPERTY_RO(complete, bool)
  ES_DECL_PROPERTY_RO(pulsing, bool)
  ES_DECL_PROPERTY_RO(hasChildren, bool)
  ES_DECL_PROPERTY_RO(path, EsString)
  ES_DECL_PROPERTY(text, EsString)
  ES_DECL_PROPERTY_RO(progressText, EsString)
  ES_DECL_PROPERTY(range, EsVariant)
  ES_DECL_PROPERTY(position, EsVariant)
  ES_DECL_PROPERTY_RO(completion, EsVariant)
  ES_DECL_PROPERTY_WO(progressTextFormatter, EsVariant)

  /// Reflected constructors
  ///
  /// Create undetermined progress task with id
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsString);

  /// Create undetermined progress task with id and text
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsString, cr_EsString);

  /// Create possibly determined progress task with id, text, range and position
  ES_DECL_REFLECTED_CLASS_CTOR4(EsBaseIntfPtr, cr_EsString, cr_EsString, cr_EsVariant, cr_EsVariant);

protected:
  /// Internal services
  ///
  void internalDetach();
  void internalMonitorSet(const EsProgressMonitorIntf::Ptr& mon);
  /// Overridable - Return current progress text
  virtual EsString internalProgressTextGet() const;
  EsString internalPathGet() const;
  void internalTextSet(const EsString& text);
  void onChildStateChanged(int newState);
  void internalChildAdd(const EsProgressMonitorTaskIntf::Ptr& thisTask, EsProgressMonitorTask* child);
  void internalChildRemove(const EsString& id);
  void internalProgressUpdate(ulong state);
  void internalThisStateSet(ulong state);
  void internalThisStateChangeNotify();
  void internalThisProgressChangeNotify();
  double internalCompletionCalc() const;
  static EsProgressMonitorTask* asTask(const EsProgressMonitorTaskIntf::Ptr& ptask);
  static EsProgressMonitor* asMonitor(const EsProgressMonitorIntf::Ptr& pmon);
  static void idCheck(const EsString& childId);
  static void rangeCheck(const EsVariant& range);

protected:
  mutable EsCriticalSection m_cs;
  EsProgressMonitorIntf::Ptr m_monitor;
  EsProgressMonitorTaskIntf::Ptr m_parent;
  ulong m_state;
  mutable bool m_pathInvalid;
  mutable EsString m_path;
  double m_completion;
  EsString m_id;
  EsString m_text;
  EsVariant m_val;
  EsVariant m_range;
  EsStringIndexedMap m_children;
  EsVariant m_progressTextFormatter;
  bool m_formatterIsSvc;

private:
  // prohibited functionality
  EsProgressMonitorTask() ES_REMOVEDECL;
  EsProgressMonitorTask(const EsProgressMonitorTask&) ES_REMOVEDECL;
  EsProgressMonitorTask& operator=(const EsProgressMonitorTask&) ES_REMOVEDECL;

  friend class EsProgressMonitor;
};

/// Basic progress monitor functionality reflected implementation.
/// All specific progress monitor implementations should be derived from this
/// class.
///
class ESCORE_CLASS ES_INTF_IMPL2(EsProgressMonitor, EsProgressMonitorIntf, EsReflectedClassIntf)

protected:
  EsProgressMonitor(const EsString& id);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsProgressMonitor)
  ES_DECL_ATTR_HANDLING_STD

  static EsProgressMonitorIntf::Ptr create(const EsString& id);

  /// EsProgressMonitorIntf implementation
  ///
  /// Return progress monitor id string
  ES_DECL_INTF_METHOD(const EsString&, idGet)() const ES_OVERRIDE;
  /// Entirely resets progress monitor, effectively detaching all tasks from it.
  ES_DECL_REFLECTED_INTF_METHOD0(void, reset) ES_OVERRIDE;
  /// Return view event category
  ES_DECL_INTF_METHOD(const EsString&, eventCategoryGet)() const ES_NOTHROW ES_OVERRIDE { return m_cat; }
  /// Post event to the view
  ES_DECL_INTF_METHOD(void, viewEventPost)(ulong id, const EsVariant& payload) const ES_OVERRIDE;
  /// Retrieve all linked task paths
  ES_DECL_INTF_METHOD(EsString::Array, taskPathsGet)() const ES_OVERRIDE;
  /// Get task by its path
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, taskGetByPath)(const EsString& path) const ES_OVERRIDE;

  /// Reflected constructor
  ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, cr_EsString);

  /// Reflected properties
  ///
  ES_DECL_PROPERTY_RO(id, EsString)
  ES_DECL_PROPERTY_RO(taskPaths, EsStringArray)

protected:
  void internalChildAdd(const EsProgressMonitorIntf::Ptr& thisMon, EsProgressMonitorTask* child);
  void internalChildRemove(const EsString& childPath);

protected:
  mutable EsCriticalSection m_cs;
  EsString m_id;
  EsString m_cat;
  EsStringIndexedMap m_tasks;

  friend class EsProgressMonitorTask;

private:
  EsProgressMonitor() ES_REMOVEDECL;
  EsProgressMonitor(const EsProgressMonitor&) ES_REMOVEDECL;
  EsProgressMonitor& operator=(const EsProgressMonitor&) ES_REMOVEDECL;
};

#endif // _progress_monitor_h_
