#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptDebug.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#if defined(ESSCRIPT_USE_DEBUG_LOG_TRACE)
# define ES_SCRIPT_DEBUG_LOG_TRACE      ES_DEBUG_TRACE
#else
# define ES_SCRIPT_DEBUG_LOG_TRACE(...) ((void)0)
#endif
//---------------------------------------------------------------------------

// Debugger sesstion execution thread
//
EsScriptDebug::Runner::Runner(EsScriptDebug& owner) :
EsThread(),
m_owner(owner)
{
}
//---------------------------------------------------------------------------

void EsScriptDebug::Runner::onEnterWorker()
{
  // Set debugger pointer each time
  m_owner.vmDebuggerSet();

  EsEventDispatcher::eventPost(
    ES_EVTC_SCRIPT_DEBUG_RSP,
    as_<ulong>(EsScriptDebugEvt::Started)
  );
}
//---------------------------------------------------------------------------

void EsScriptDebug::Runner::onExitWorker()
{
  // Reset debugger pointer each time
  m_owner.vmDebuggerReset();

  if( m_owner.m_script->vm().isAborting() )
    EsEventDispatcher::eventPost( 
      ES_EVTC_SCRIPT_DEBUG_RSP, 
      as_<ulong>(EsScriptDebugEvt::Aborted)
    );
  else
    EsEventDispatcher::eventPost( 
      ES_EVTC_SCRIPT_DEBUG_RSP, 
      as_<ulong>(EsScriptDebugEvt::Finished)
    );
}
//---------------------------------------------------------------------------

long EsScriptDebug::Runner::worker()
{
  m_owner.m_script->exec();

  return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Debugger requests subscriber
//
EsScriptDebug::EvtHandler::EvtHandler(EsScriptDebug& owner) :
EsEventSubscriber(
  ES_EVTC_SCRIPT_DEBUG_REQ ES_EVTC_AND
  ES_EVTC_SCRIPT_DEBUG_RSP,
  precedenceHighest //< The highest handling precedence
),
m_owner(owner)
{
}
//---------------------------------------------------------------------------

void EsScriptDebug::EvtHandler::notify(const EsEventIntf::Ptr& evt)
{
  ES_ASSERT(evt);

  const EsString& cat = evt->categoryGet();
  ulong id = evt->idGet();
  const EsVariant& payload = evt->payloadGet();

  if( ES_EVTC_SCRIPT_DEBUG_REQ == cat )
  {
    switch( as_<EsScriptDebugEvt>(id) )
    {
    case EsScriptDebugEvt::ScriptAttach:       //< [script object|null]
      if( payload.isEmpty() )
        m_owner.scriptSet( nullptr );
      else
        m_owner.scriptSet( payload.asObject() );
      break;
    case EsScriptDebugEvt::BreakpointSet:      //< [file, [line,(true/false)]]
      m_owner.breakpointSet(
        payload[0].asString(),
        payload[1][0].asULong(),
        payload[1][1].asBool()
      );
      break;
    case EsScriptDebugEvt::BreakpointsSet:      //< [file, data, sender]
      m_owner.breakpointsSet(
        payload[0].asString(),                 //< file
        payload[1].asExistingObject(),         //< lines to state map
        payload[2].asPointer()                 //< sender
      );
      break;
    case EsScriptDebugEvt::BreakpointDelete:   //< [file, line]
      m_owner.breakpointDelete(
        payload[0].asString(),
        payload[1].asULong()
      );
      break;
    case EsScriptDebugEvt::Pause:              //< Request debugger pause script being debugged
      m_owner.pause();
      break;
    case EsScriptDebugEvt::Step:               //< [stepType (next line|next instruction)]
      m_owner.step();
      break;
    case EsScriptDebugEvt::Run:                //< Execute (resume execution) script in debugger
      m_owner.run();
      break;
    case EsScriptDebugEvt::Abort:              //< abortExecution script execution
      m_owner.abortExecution();
      break;
    }
  }
  else if( ES_EVTC_SCRIPT_DEBUG_RSP == cat )
  {
    switch( as_<EsScriptDebugEvt>(id) )
    {
    case EsScriptDebugEvt::Paused:
      m_owner.onPaused();
      break;
    case EsScriptDebugEvt::Aborted:
    case EsScriptDebugEvt::Finished:
      m_owner.onStopped();
      break;
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsScriptDebug, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsScriptDebug, log, void_ClassCall_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

//---------------------------------------------------------------------------

EsScriptDebug::EsScriptDebug() :
m_evtHandler(*this),
m_stepLock(0, 1),
m_mode(Idle),
m_runner(*this)
{
  prevInstrLineReset();
  m_breakpoints = EsAssocContainer::create();
}
//---------------------------------------------------------------------------

EsScriptDebug::~EsScriptDebug()
{
  m_runner.stopAndWait();
}
//---------------------------------------------------------------------------

EsScriptDebug::RunnerMode EsScriptDebug::runnerModeGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_mode;
}
//---------------------------------------------------------------------------

const EsVariant& EsScriptDebug::breakpointsGet(const EsString& file) const
{
  if( m_breakpoints->keyExists(file) )
    return m_breakpoints->valueGet(file);

  return EsVariant::null();
}
//---------------------------------------------------------------------------

void EsScriptDebug::scriptSet(const EsScriptIntf::Ptr& script)
{
  if( m_script != script )
  {
    m_script = script;

    breakpointsValidate();

    // Notify of attached script change
    EsEventDispatcher::eventPost(
      ES_EVTC_SCRIPT_DEBUG_RSP,
      static_cast<ulong>(EsScriptDebugEvt::ScriptChanged),
      m_script
    );
  }
}
//---------------------------------------------------------------------------

void EsScriptDebug::vmDebuggerSet()
{
  ES_ASSERT(m_script);
  m_script->vm().debuggerSet( this );
}
//---------------------------------------------------------------------------

void EsScriptDebug::vmDebuggerReset()
{
  ES_ASSERT(m_script);
  m_script->vm().debuggerSet( nullptr );
}
//---------------------------------------------------------------------------

void EsScriptDebug::brkChangedEvtPost(const EsString& file, const EsVariant& oldState, const EsVariant& newState)
{
  const EsVariant::Array& payload = {
    m_script,
    file,
    oldState,
    newState
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_SCRIPT_DEBUG_RSP,
    as_<ulong>(EsScriptDebugEvt::BreakpointChanged),
    payload
  );
}
//---------------------------------------------------------------------------

void EsScriptDebug::breakpointSet(const EsString& file, ulong line, bool state)
{
  if( !isScriptAttached() )
    return;

  const EsVariant& execLines = m_script->sourceModuleExecutableLinesGet(file);
  if( !execLines.has(line) )
    return;

  EsCriticalSectionLocker lock(m_cs);

  EsAssocContainerIntf::Ptr brks;
  if( !m_breakpoints->keyExists(file) )
  {
    brks = EsAssocContainer::create();
    ES_ASSERT(brks);

    m_breakpoints->newValueSet(
      file,
      brks
    );
  }
  else
    brks = m_breakpoints->valueGet(file).asExistingObject();

  const EsVariant& val = brks->valueGetDef(
    line,
    EsVariant::null()
  );

  if( val.isBool() && val.asBool() == state )
    return;

  const EsVariant::Array& oldval = {
    line,
    val
  };
  const EsVariant::Array& newval = {
    line,
    state
  };

  brks->newValueSet(
    line,
    newval
  );

  brkChangedEvtPost(
    file,
    oldval, 
    newval
  );
}
//---------------------------------------------------------------------------

void EsScriptDebug::breakpointDelete(const EsString& file, ulong line)
{
  EsCriticalSectionLocker lock(m_cs);

  if( !m_breakpoints->keyExists(file) )
    return;

  EsAssocContainerIntf::Ptr brks = m_breakpoints->valueGet(file).asExistingObject();
  ES_ASSERT(brks);

  if( !brks->keyExists(line) )
    return;

  const EsVariant::Array& oldval = {
    line,
    brks->valueGet(line).asBool()
  };

  brks->valueDelete(line);

  const EsVariant::Array& newval = {
    line,
    EsVariant::null()
  };
  
  brkChangedEvtPost(
    file,
    oldval,
    newval
  );
}
//---------------------------------------------------------------------------

void EsScriptDebug::brkChangedEvtPost(const void* sender) const
{
  const EsVariant::Array& payload = {
    m_script,
    m_breakpoints,
    EsVariant(
      const_cast<void*>(sender),
      EsVariant::ACCEPT_POINTER
    )
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_SCRIPT_DEBUG_RSP,
    as_<ulong>(EsScriptDebugEvt::BreakpointsChanged),
    payload
  );
}
//--------------------------------------------------------------------------------

void EsScriptDebug::breakpointsSet(const EsString& file, const EsAssocContainerIntf::Ptr& data, const void* sender /*= nullptr*/)
{
  if( !m_script )
    return;

  if( !m_script->hasSourceModule(file) )
    return;

  EsCriticalSectionLocker lock(m_cs);

  if( data )
  {
    if( !m_breakpoints->keyExists(file) )
      m_breakpoints->newValueSet(
        file,
        data
      );
    else
      m_breakpoints->valueSet(
        file,
        data
      );

    breakpointsValidate();

    brkChangedEvtPost(sender);
  }
  else if( m_breakpoints->keyExists(file) )
  {
    m_breakpoints->valueDelete(file);

    breakpointsValidate();    
    
    brkChangedEvtPost(sender);
  }
}
//--------------------------------------------------------------------------------

void EsScriptDebug::breakpointsSet(const EsAssocContainerIntf::Ptr& bp, const void* sender /*= nullptr*/)
{
  ES_ASSERT(bp);

  EsCriticalSectionLocker lock(m_cs);
  if( m_breakpoints != bp )
  {
    m_breakpoints->clear();
    const EsVariant::Array& keys = bp->allKeysGet().asVariantCollection();
    for( auto const& key : keys )
    {
      const EsString& path = key.asString();
      if( m_script->hasSourceModule(path) )
        m_breakpoints->newValueSet(
          path,
          bp->valueGet(key)
        );
    }

    breakpointsValidate();

    brkChangedEvtPost(sender);
  }
}
//--------------------------------------------------------------------------------

void EsScriptDebug::pause()
{
  EsCriticalSectionLocker lock(m_cs);
  if( Continuous == m_mode )
  {
    m_mode = Stepped;
    m_stepLock.wait();
  }
}
//---------------------------------------------------------------------------

void EsScriptDebug::step()
{
  if( Idle == m_mode )
    run( Stepped );
  else if( Stepped == m_mode )
    m_stepLock.post();
}
//---------------------------------------------------------------------------

void EsScriptDebug::run(RunnerMode mode /*= Continuous*/)
{
  EsCriticalSectionLocker lock(m_cs);

  if( Idle < mode && m_mode != mode )
  {
    RunnerMode prevMode = m_mode;
    m_mode = mode;

    if( Stepped == prevMode && Continuous == mode )
      m_stepLock.post();
    else if( Idle == prevMode )
    {
      // Just in case, unlock step
      m_stepLock.post();

      // Pre-clean prev instruction file && line holders
      m_prevInstrFile.clear();
      prevInstrLineReset();

      // Execute code being debugged in debugger internal thread
      m_runner.start();
    }
  }
}
//---------------------------------------------------------------------------

void EsScriptDebug::abortExecution()
{
  if( Idle != m_mode )
  {
    m_script->execAbort();
    m_stepLock.post();
    m_runner.stopAndWait();
  }
}
//---------------------------------------------------------------------------

void EsScriptDebug::prevInstrLineReset()
{
  m_prevInstrLine = static_cast<ulong>(-1);
}
//---------------------------------------------------------------------------

bool EsScriptDebug::breakpointTriggerCheck(const EsString& file, ulong line)
{
  EsCriticalSectionLocker lock(m_cs);

  if( 
    (
      (m_prevInstrFile != file) || 
      (m_prevInstrLine != line)
    ) &&
    m_breakpoints->keyExists(file) 
  )
  {
    EsAssocContainerIntf::Ptr brks = m_breakpoints->valueGet(file).asExistingObject();
    ES_ASSERT(brks);

    if(!brks->keyExists(line))
      return false;

    bool state = brks->valueGet(line).asBool();
    if(!state)
      return false;

    // [file, line, isBreakpoint]
    const EsVariant::Array& payload = {
      file,
      line,
      true
    };

    EsEventDispatcher::eventPost(
      ES_EVTC_SCRIPT_DEBUG_RSP,
      static_cast<ulong>(EsScriptDebugEvt::Paused),
      payload
    );

    // Possibly obtain step semaphore, so the next call to
    // m_stepLock.wait() would lock until post() is called
    // from controlling thread
    //
    m_stepLock.tryWait();
    return true;
  }

  return false;
}
//---------------------------------------------------------------------------

void EsScriptDebug::instructionSteppedNotify(const EsString& file, ulong line)
{
  // [file, line, isBreakpoint]
  const EsVariant::Array& payload = {
    file,
    line,
    false 
  };

  EsEventDispatcher::eventPost(
    ES_EVTC_SCRIPT_DEBUG_RSP,
    static_cast<ulong>(EsScriptDebugEvt::Paused),
    payload
  );
}
//---------------------------------------------------------------------------

// NB! this callback is executed in the context of debugger thread (at best)
void EsScriptDebug::onInstruction(EsScriptInstructions::const_iterator instr)
{
  if( m_script->vm().isAborting() )
    return;

  EsScriptDebugInfoIntf::Ptr dbg = (*instr).debugInfoGet();
  if( dbg )
  {
    const EsString& file = dbg->fileGet();
    ulong line = dbg->lineGet();
    ES_ASSERT(line > 0);
    --line;

    if( !breakpointTriggerCheck(file, line) )
    {
      if( Stepped == runnerModeGet() )
      {
        instructionSteppedNotify(
          file, 
          line
        );

        // Wait for manual unlock
        m_stepLock.wait();

        EsEventDispatcher::eventPost(
          ES_EVTC_SCRIPT_DEBUG_RSP,
          as_<long>(EsScriptDebugEvt::Continued)
        );
      }
    }
    else // Wait for manual unlock
    {
      m_stepLock.wait();

      EsEventDispatcher::eventPost(
        ES_EVTC_SCRIPT_DEBUG_RSP,
        as_<long>(EsScriptDebugEvt::Continued)
      );
    }

    m_prevInstrFile = file;
    m_prevInstrLine = line;
  }
}
//---------------------------------------------------------------------------

void EsScriptDebug::onPaused()
{
  EsCriticalSectionLocker lock(m_cs);
  m_mode = Stepped;
}
//---------------------------------------------------------------------------

void EsScriptDebug::onStopped()
{
  EsCriticalSectionLocker lock(m_cs);
  m_mode = Idle;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

void EsScriptDebug::breakpointsValidate()
{
  if( m_script )
  {
    ES_ASSERT(m_script->hasSourceModulesInfo());

    const EsString::Array& keys = m_breakpoints->allKeysGet().asStringCollection();
    for( auto const& key : keys )
    {
      if( m_script->hasSourceModule(key) )
      {
        const EsVariant& exelines = m_script->sourceModuleExecutableLinesGet(key);
        if(exelines.isEmpty())
        {
          m_breakpoints->valueDelete(key);
          return;
        }
        
        const EsVariant& ranges = exelines.asVariantCollection();
        
        EsAssocContainerIntf::Ptr brks = m_breakpoints->valueGet(key).asExistingObject();
        ES_ASSERT(brks);

        const EsVariant::Array& lines = brks->allKeysGet().asVariantCollection();

        for( auto const& line : lines )
        {
          if( !ranges.has(line) )
            brks->valueDelete(line);
        }
      }
      else
        m_breakpoints->valueDelete(key);
    }
  }
  else
    m_breakpoints->clear();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Static debug logging|tracing services

static void logDebug(const EsString& msg)
{
  ES_SCRIPT_DEBUG_LOG_TRACE( esT("## %s"), msg );

  EsEventDispatcher::eventPost(
    ES_EVTC_SCRIPT_DEBUG_RSP,
    as_<ulong>(EsScriptDebugEvt::Log),
    msg
  );
}
//---------------------------------------------------------------------------

void EsScriptDebug::log(cr_EsVariant p1)
{
  if( p1.isCollection() || p1.isObject() || p1.isEmpty() )
    logDebug( EsScriptMachine::traceVariant(p1) );
  else
    logDebug( p1.asString() );
}
//---------------------------------------------------------------------------

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2)
{
  logDebug( EsString::format(p1.c_str(), p2) );
}
//---------------------------------------------------------------------------

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3)
{
  logDebug( EsString::format(p1.c_str(), p2, p3) );
}
//---------------------------------------------------------------------------

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4)
{
  logDebug( EsString::format(p1.c_str(), p2, p3, p4) );
}
//---------------------------------------------------------------------------

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5)
{
  logDebug( EsString::format(p1.c_str(), p2, p3, p4, p5) );
}
//---------------------------------------------------------------------------

void EsScriptDebug::log(cr_EsString p1, cr_EsVariant p2, cr_EsVariant p3, cr_EsVariant p4, cr_EsVariant p5, cr_EsVariant p6)
{
  logDebug( EsString::format(p1.c_str(), p2, p3, p4, p5, p6) );
}
//---------------------------------------------------------------------------

