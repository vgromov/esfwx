#ifndef _es_script_debug_h_
#define _es_script_debug_h_

/// @file EsScriptDebug.h
/// Reflected scripting debug services declaration.
///

/// Script debug events
///
#define ES_EVTC_SCRIPT_DEBUG_REQ  esT("ess_debug_request")
#define ES_EVTC_SCRIPT_DEBUG_RSP  esT("ess_debug_response")

enum class EsScriptDebugEvt : ulong {
  /// Requests       Payload
  ScriptAttach,      ///< [script object|null]
  BreakpointSet,     ///< Optional sender, [file, [line, (true/false)]]
  BreakpointDelete,  ///< Optional sender, [file, line]
  BreakpointsSet,    ///< Replace all breakpoints information for specific file. 
                     ///< [file, states, optional sender], where states is a assoc container of [line, (true/false)]
  Pause,             ///< Request debugger pause script being debugged
  Step,              ///< Request debugger to step source code
                     ///< [stepType (next line|next instruction)]
  Run,               ///< Execute (resume execution) script in debugger
  Abort,             ///< Abort script execution

  /// Responses
  Started,           ///< Script debugger execution is started
  Continued,         ///< Script execution continued after a pause|breakpoint
  ScriptChanged,     ///< Attached script object is changed (either attached, re-attached, or reset )
                     ///< [script object|null]
  BreakpointChanged, ///< Breakpoint state was changed.
                     ///< [script object, [old state], [new state]], where state is [file, [line, null|(true/false)]]
                     ///< If new breakpoint was created, old state is null
                     ///< If existing breakpoint was removed, new state is null
  BreakpointsChanged,///< All breakpoints data were changed, payload is a new breakpoint data instance. Payload: [script object, breakpoints, (optional sender, or nullptr, if broadcasted)]
  Paused,            ///< Script was paused, either after Pause event,
                     ///< breakpoint triggered, or after misc Step events
                     ///< [file, line, isBreakpoint]
  Aborted,           ///< Debugger session was aborted
  Finished,          ///< Debugger session was complete
                     ///< [optional execution error log|null if OK]
  Log                ///< Debug logger event
                     ///< [log message]
};

/// Script debug class
///
class ESSCRIPT_CLASS ES_INTF_IMPL1(EsScriptDebug, EsReflectedClassIntf)

public:
  /// Debugger runner modes
  ///
  enum RunnerMode {
    Idle,                 ///< Runner is not active (stopped|aborted)
    Continuous,           ///< Runner is working in continuous mode, i.e. it
                          ///< automatically commits each instruction, unless
                          ///< breakpoint is triggered, or pause request is made
    Stepped,              ///< Runner works in stepped mode, i.e. each instruction
                          ///< is executed upon manual commit from external controller
  };


protected:
  /// Script debugger execution thread
  class Runner : public EsThread
  {
  public:
    Runner(EsScriptDebug& owner);

  protected:
    virtual void onEnterWorker();
    virtual void onExitWorker();
    virtual long worker();

  protected:
    EsScriptDebug& m_owner;
  };
  friend class Runner;

  /// Main process requests handler
  class EvtHandler : public EsEventSubscriber
  {
  public:
    EvtHandler(EsScriptDebug& owner);

    virtual void notify(const EsEventIntf::Ptr& evt);

  protected:
    EsScriptDebug& m_owner;
  };
  friend class Subuscriber;

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsScriptDebug)
  ES_DECL_ATTR_HANDLING_STD

  EsScriptDebug();
  virtual ~EsScriptDebug();

  /// Return currently active runner mode
  RunnerMode runnerModeGet() const;

  /// Attach|detach script object.
  /// If empty script is passed, detach from any existing one
  /// @param script [in] Script object to attach to debugger, or null,
  ///                     if we need to detah from it.
  ///
  void scriptSet(const EsScriptIntf::Ptr& script);

  /// Return true, if script object is attached, false otherwise
  bool isScriptAttached() const { return m_script; }

  /// Breakpoints manipulation
  ///

  /// Add|set breakpoint. Breakpoint file and line must be in
  /// breakable lines collection. If not - an exception is thrown.
  /// @param    file    [in] script source file name
  /// @param    line    [in] source file line number (0-based)
  /// @param    state   [in] breakpoint activity state
  ///
  void breakpointSet(const EsString& file, ulong line, bool state);

  /// Delete breakpoint. Breakpoint must exist, or exception is thrown.
  /// @param    file    [in] script source file name
  /// @param    line    [in] source file line number (0-based)
  ///
  void breakpointDelete(const EsString& file, ulong line);

  /// Get breakpoints collection
  EsAssocContainerIntf::Ptr breakpointsGet() const ES_NOTHROW { return m_breakpoints; }

  /// Get breakpoints collection for specific file (full path is expected)
  /// If file does not have registered breakpoints, an empty variant is returned
  ///
  const EsVariant& breakpointsGet(const EsString& file) const;

  /// Assign source file-specific breakpoints data
  /// @param   data   [in] Breakpoints data - state (true/false), uniquely mapped to source lines
  /// 
  void breakpointsSet(const EsString& file, const EsAssocContainerIntf::Ptr& data, const void* sender = nullptr);

  /// Assign breakpoints data
  void breakpointsSet(const EsAssocContainerIntf::Ptr& bp, const void* sender = nullptr);

  /// Debugger runner control interface
  ///

  /// Pause debugger runner, effectively switching it to stepped execution state.
  /// If runner is already in idle|stepped state, do nothing
  ///
  void pause();

  /// Advance script one instruction forward.
  /// If debugger runner is in Idle state, start debugger runner in stepped mode.
  /// If debugger is running in continuous mode, do nothing
  ///
  void step();

  /// Execute debugger runner in specified mode.
  /// @param mode [in] Requested execution mode. If mode is Idle, do nothing.
  ///                  If runner is in stepped mode, and Continuous mode is
  ///                  requested, change mode and proceed. Otherwise, do nothing.
  ///
  void run(RunnerMode mode = Continuous);

  /// Abort current debugger session. If there is no active debugger session,
  /// this call does nothing.
  ///
  void abortExecution();

  /// Static Script debug logging services.
  /// Debug logging goes to IDE console output (if supported) in DEBUG C++ code.
  /// In addition, trace information is being sent to the standard applog with message level == debug
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(void, log, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, log, cr_EsString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD3(void, log, cr_EsString, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD4(void, log, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD5(void, log, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD6(void, log, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

protected:
  // Set-up|reset debugger for script virtual machine
  void vmDebuggerSet();
  void vmDebuggerReset();
  // Reset prev breakpoint line
  void prevInstrLineReset();
  // Breakpoint tripping checker
  bool breakpointTriggerCheck(const EsString& file, ulong line);
  // Post event notification on instruction step
  static void instructionSteppedNotify(const EsString& file, ulong line);
  // Handler to be called from virtual machine on instruction change
  void onInstruction(EsScriptInstructions::const_iterator instr);
  // Post breakpoint change events
  void brkChangedEvtPost(const EsString& file, const EsVariant& oldState, const EsVariant& newState);
  void brkChangedEvtPost(const void* sender) const;
  // Validate existing breakpoints data against script value and executable lines data
  void breakpointsValidate();

  // Application event handlers
  //
  void onPaused();
  void onStopped();

protected:
  /// Data locker
  mutable EsCriticalSection m_cs;

  /// Main process debug requests handler
  EvtHandler m_evtHandler;

  /// Script debug execution thread sync
  EsSemaphore m_stepLock;

  /// Attached script
  EsScriptIntf::Ptr m_script;

  /// Breakpoints collection. Each breakpoint information is variant collection
  /// [file line, active state]
  ///
  EsAssocContainerIntf::Ptr m_breakpoints;

  /// Debugger execution thread
  Runner m_runner;

  /// File of previously triggered breakpoint
  EsString m_prevInstrFile;

  /// Runner mode
  RunnerMode m_mode;

  /// Line of previously triggered breakpoint
  ulong m_prevInstrLine;

private:
  EsScriptDebug(const EsScriptDebug&) ES_REMOVEDECL;
  EsScriptDebug& operator=(const EsScriptDebug&) ES_REMOVEDECL;

  friend class EsScriptMachine;
};

#endif // _es_script_debug_h_
