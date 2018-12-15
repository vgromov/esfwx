#include "escorepch.h"
#pragma hdrstop

#include "EsProgressMonitor.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// EsProgressMonitorTask class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsProgressMonitorTask, NO_CLASS_DESCR)
  // reflected services
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitorTask, EsProgressMonitorTaskIntf, monitorGet, EsBaseIntfPtr_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitorTask, EsProgressMonitorTaskIntf, parentGet, EsBaseIntfPtr_CallConst, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitorTask, EsProgressMonitorTaskIntf, initialize, void_Call_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitorTask, EsProgressMonitorTaskIntf, pulse, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitorTask, EsProgressMonitorTaskIntf, end, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitorTask, EsProgressMonitorTaskIntf, detach, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_METHOD_INFO(EsProgressMonitorTask, attachToReflected, attachTo, void_Call_cr_EsVariant, NO_METHOD_DESCR)
  // reflected properties
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, id, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, orphaned, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, complete, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, pulsing, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, hasChildren, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, path, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsProgressMonitorTask, text, EsString, esT("Descriptive tast text"), NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, progressText, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsProgressMonitorTask, range, EsVariant, esT("Tast range"), NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(EsProgressMonitorTask, position, EsVariant, esT("Task position"), NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitorTask, completion, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(EsProgressMonitorTask, progressTextFormatter, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // constructors
  ES_DECL_REFLECTED_CTOR_INFO(EsProgressMonitorTask, EsBaseIntfPtr_ClassCall_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsProgressMonitorTask, EsBaseIntfPtr_ClassCall_cr_EsString_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsProgressMonitorTask, EsBaseIntfPtr_ClassCall_cr_EsString_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsProgressMonitorTask::EsProgressMonitorTask(const EsString& id, const EsString& text,
  const EsVariant& range, const EsVariant& val) :
m_children(
  EsStringIndexedMap::ContainerUsesInterlock,
  esT("EsProgressMonitorTask")
)
{
  m_dynamic = true;
  m_pathInvalid = true;
  m_state = stateNormal;

  idCheck(id);
  m_id = id;
  m_text = text;

  rangeCheck(range);
  m_range = range;
  m_val = val;

  if( m_range.isEmpty() || m_val.isEmpty() )
    m_state = statePulsing;

  m_completion = 0;
  m_formatterIsSvc = false;
}
//---------------------------------------------------------------------------

EsProgressMonitorTask::~EsProgressMonitorTask()
{
  detach();
}
//---------------------------------------------------------------------------

EsProgressMonitorTaskIntf::Ptr EsProgressMonitorTask::create(const EsString& id, const EsString& text,
  const EsVariant& range, const EsVariant& val)
{
  std::unique_ptr<EsProgressMonitorTask> t( new EsProgressMonitorTask(id, text, range, val) );
  ES_ASSERT(t);

  return t.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsProgressMonitorTask::NEW(cr_EsString id)
{
  return create(id);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsProgressMonitorTask::NEW(cr_EsString id, cr_EsString text)
{
  return create(id, text);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsProgressMonitorTask::NEW(cr_EsString id, cr_EsString text, cr_EsVariant range, cr_EsVariant pos)
{
  return create(id, text, range, pos);
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::idCheck(const EsString& id)
{
  // empty check
  if( id.empty() )
    EsException::Throw(esT("Task id string should not be empty"));

  // id itself must not contain any non-valid EsPath directory chars,
  // or else path processing is ruined
  EsPath::checkPathStringValidity(id, esT("Progress task id"));
}
//---------------------------------------------------------------------------

const EsString& EsProgressMonitorTask::idGet() const
{
  return m_id;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::rangeCheck(const EsVariant& range)
{
  // range may be empty variant, null valiant,
  // numeric, array of 2 numeric elements, or closed range object
  if( !range.isEmpty() && !range.isNumeric() )
  {
    if( EsVariant::VAR_VARIANT_COLLECTION == range.typeGet() )
    {
      const EsVariant::Array& r = range.asVariantCollection();
      if( 2 != r.size() )
        EsException::Throw(esT("Task range collection must contain exactly two elements"));

      if( !r[0].isNumeric() || !r[1].isNumeric() )
        EsException::Throw(esT("Task range collection must contain only numeric elements"));
    }
    else if( range.isObject() )
    {
      EsReflectedClassIntf::Ptr r = range.asExistingObject();
      if( !r->isKindOf(esT("EsRange")) )
        EsException::Throw(esT("Task range object must be of EsRange type"));

      const EsVariant& min = r->propertyGet(esT("min"));
      const EsVariant& max = r->propertyGet(esT("max"));
      if( min.isEmpty() || max.isEmpty() )
        EsException::Throw(esT("Task range object must have defined min and max boundaries"));

      if( !min.isNumeric() || max.isNumeric() )
        EsException::Throw(esT("Task range object boundaries must be of numeric type"));
    }
    else
      EsException::Throw(
        esT("Task range of type '%s' is not supported"),
        EsEnumSymbolGet<EsVariantType>(range.typeGet())
      );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// EsProgressMonitorTaskIntf implementation
///

EsBaseIntfPtr EsProgressMonitorTask::monitorGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_monitor;
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsProgressMonitorTask::parentGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_parent;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::progressTextFormatterSet(const EsVariant& progressTextFormatter)
{
  if(  EsVariant::VAR_VARIANT_COLLECTION == progressTextFormatter.typeGet() &&
      2 == progressTextFormatter.countGet() )
  {
    const EsVariant& objOrStatic = progressTextFormatter.itemGet(0);
    EsReflectedClassIntf::Ptr obj;
    EsString svc;
    if( objOrStatic.isObject() )
      obj = objOrStatic.asExistingObject();
    else if( objOrStatic.isString() )
      svc = objOrStatic.asString();

    if( svc.empty() && !obj )
      EsException::Throw(esT("Progress text formatter must be sttic service or reflected object instance"));

    const EsString& methodName = progressTextFormatter.itemGet(1).asString();
    EsMethodInfoKeyT key(1, methodName);

    if( !svc.empty() )
    {
      const EsClassInfo* info = EsClassInfo::classInfoGet(svc, true);
      if( !info->hasClassMethod(key) )
        EsException::Throw(
          esT("There is no progress text formatter service '%s' with method '%s', taking 1 parameter"),
          svc,
          methodName
        );

      m_formatterIsSvc = true;
    }
    else if( !obj->hasMethod(key) )
      EsException::Throw(
        esT("There is no method '%s', taking 1 parameter, declared in formatter object"),
        svc,
        methodName
      );
    else
      m_formatterIsSvc = false;
  }
  else if( !progressTextFormatter.isEmpty() )
    EsException::Throw(esT("Could not set Progress text formatter"));

  m_progressTextFormatter = progressTextFormatter;
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::isOrphaned() const
{
  EsCriticalSectionLocker lock(m_cs);
  return !m_monitor;
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::isComplete() const
{
  EsCriticalSectionLocker lock(m_cs);
  return stateComplete == m_state;
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::isPulsing() const
{
  EsCriticalSectionLocker lock(m_cs);
  return statePulsing == m_state;
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::hasChildren() const
{
  EsCriticalSectionLocker lock(m_cs);
  return !m_children.isEmpty();
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::internalPathGet() const
{
  if( m_pathInvalid )
  {
    EsString result = esT("/") + m_id;

    EsProgressMonitorTaskIntf::Ptr task = m_parent;
    while( task )
    {
      result = esT("/") + task->idGet() + result;
      task = task->parentGet();
    }

    m_path = result;
    m_pathInvalid = false;
  }

  return m_path;
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::pathGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return internalPathGet();
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::textGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_text;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalTextSet(const EsString& text)
{
  if( m_text != text )
  {
    if( m_monitor )
    {
      EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
      payload.addToVariantCollection( internalPathGet() )
        .addToVariantCollection( m_text )
        .addToVariantCollection( text );

      m_monitor->viewEventPost(evtTaskTextChanged, payload);
    }

    m_text = text;
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::textSet(const EsString& text)
{
  EsCriticalSectionLocker lock(m_cs);
  internalTextSet(text);
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::progressTextGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return internalProgressTextGet();
}
//---------------------------------------------------------------------------

EsProgressMonitorTask* EsProgressMonitorTask::asTask(const EsProgressMonitorTaskIntf::Ptr& ptask)
{
  ES_ASSERT(ptask);

  EsProgressMonitorTask* pimpl = reinterpret_cast<EsProgressMonitorTask*>(
    ptask->implementorGet()
  );
  ES_ASSERT(pimpl);

  return pimpl;
}
//---------------------------------------------------------------------------

EsProgressMonitor* EsProgressMonitorTask::asMonitor(const EsProgressMonitorIntf::Ptr& pmon)
{
  ES_ASSERT(pmon);

  EsProgressMonitor* pimpl = reinterpret_cast<EsProgressMonitor*>(
    pmon->implementorGet()
  );
  ES_ASSERT(pimpl);

  return pimpl;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalThisProgressChangeNotify()
{
  if( m_monitor ) // notify monitor view
  {
    EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
    payload.addToVariantCollection( internalPathGet() )
      .addToVariantCollection( internalProgressTextGet() )
      .addToVariantCollection( m_completion );

    m_monitor->viewEventPost(evtTaskProgressChanged, payload);
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalThisStateChangeNotify()
{
  if( m_monitor ) // notify monitor view
  {
    const EsVariant::Array& payload = {
      internalPathGet(),
      m_state,
      internalProgressTextGet(),
      statePulsing == m_state ? EsVariant::null() : EsVariant(m_completion)
    };

    m_monitor->viewEventPost(
      evtTaskStateChanged,
      payload
    );
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::onChildStateChanged(int newState)
{
  EsCriticalSectionLocker lock(m_cs);

  if( statePulsing == newState )
    internalThisStateSet(statePulsing);
  else
  {
    // do we still need to stay in pulsing state?
    if( statePulsing == m_state )
    {
      bool anyPulsing = false;
      for(ulong idx = 0; idx < m_children.countGet() && !anyPulsing; ++idx)
      {
        EsProgressMonitorTaskIntf::Ptr task = m_children.valueGet(idx).asExistingObject();
        ES_ASSERT(task);

        anyPulsing = task->isPulsing();
      }

      if( !anyPulsing )
        internalThisStateSet(newState);
    }
    else if( stateComplete == m_state && stateNormal == newState )
      internalThisStateSet(stateNormal);
    else if( stateNormal == m_state && stateComplete == newState )
    {
      // should we change ourselves to complete state?
      bool allComplete = true;
      for(ulong idx = 0; idx < m_children.countGet() && allComplete; ++idx)
      {
        EsProgressMonitorTaskIntf::Ptr task = m_children.valueGet(idx).asExistingObject();
        ES_ASSERT(task);

        allComplete = task->isComplete();
      }

      if( allComplete )
        internalThisStateSet(stateComplete);
    }
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalProgressUpdate(ulong state)
{
  double newCompletion = 0;
  if( stateNormal == state )
    newCompletion = internalCompletionCalc();
  else if( stateComplete == state )
    newCompletion = 1;

  if( m_completion != newCompletion )
  {
    m_completion = newCompletion;
    internalThisProgressChangeNotify();
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalThisStateSet(ulong newState)
{
  ES_ASSERT(statePulsing <= newState);
  ES_ASSERT(stateComplete >= newState);

  if( m_state != newState )
  {
    internalProgressUpdate(newState);

    m_state = newState;
    internalThisStateChangeNotify();

    if( m_parent )
    {
      EsProgressMonitorTask* parent = asTask(m_parent);
      ES_ASSERT(parent);
      parent->onChildStateChanged(newState);
    }
  }
}
//---------------------------------------------------------------------------

EsVariant EsProgressMonitorTask::rangeGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Range of task, containing sub-tasks, could not be read"));

  return m_range;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::rangeSet(const EsVariant& range)
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Range of task, containing sub-tasks, could not be explicitly assigned"));

  rangeCheck(range);
  m_range = range;

  if( statePulsing == m_state )
  {
    if( !m_range.isEmpty() && !m_val.isEmpty() )
      internalThisStateSet(stateNormal);
  }
  else if( m_range.isEmpty() )
    internalThisStateSet(statePulsing);
  else if( stateComplete == m_state )
    internalThisStateSet(stateNormal);
  else
    internalProgressUpdate(stateNormal);
}
//---------------------------------------------------------------------------

EsVariant EsProgressMonitorTask::valueGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Value of task, containing sub-tasks, could not be read"));

  return m_val;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::valueSet(const EsVariant& val)
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Value of task, containing sub-tasks, could not be explicitly assigned"));

  if( m_val != val )
  {
    m_val = val;

    if( statePulsing == m_state )
    {
      if( !m_range.isEmpty() && !m_val.isEmpty() )
        internalThisStateSet(stateNormal);
    }
    else if( m_val.isEmpty() )
      internalThisStateSet(statePulsing);
    else if( stateComplete == m_state )
      internalThisStateSet(stateNormal);
    else
      internalProgressUpdate(stateNormal);
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::initialize(const EsString& text, const EsVariant& range, const EsVariant& val)
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Task, containing sub-tasks, could not be explicitly initialized"));

  rangeCheck(range);
  m_range = range;
  if( m_val != val )
    m_val = val;

  if( statePulsing == m_state )
  {
    if( !m_range.isEmpty() && !m_val.isEmpty() )
      internalThisStateSet(stateNormal);
  }
  else if( m_range.isEmpty() || m_val.isEmpty() )
    internalThisStateSet(statePulsing);
  else if( stateComplete == m_state )
    internalThisStateSet(stateNormal);
  else
    internalProgressUpdate(stateNormal);

  internalTextSet(text);
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::pulse(const EsString& text)
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Task, containing sub-tasks, could not be explicitly set to 'pulsing' state"));

  if( statePulsing != m_state )
  {
    m_val = EsVariant::null();
    internalThisStateSet(statePulsing);
  }
  internalTextSet(text);
}
//---------------------------------------------------------------------------

double EsProgressMonitorTask::internalCompletionCalc() const
{
  double tmp;
  double min = 0;
  double max = 0;
  double delta = 0;
  double val = m_val.isEmpty() ? 0 : m_val.asDouble();

  if( m_range.isNumeric() )
  {
    tmp = m_range.asDouble();
    if( 0 > tmp )
      min = tmp;
    else
      max = tmp;
  }
  else if( m_range.isCollection() )
  {
    const EsVariant::Array& r = m_range.asVariantCollection();
    min = r[0].asDouble();
    max = r[1].asDouble();
    if( min > max )
    {
      tmp = max;
      max = min;
      min = tmp;
    }
  }
  else if( !m_range.isEmpty() )
  {
    EsReflectedClassIntf::Ptr r = m_range.asExistingObject();
    min = r->propertyGet(esT("min")).asDouble();
    max = r->propertyGet(esT("max")).asDouble();
    if( min > max )
    {
      tmp = max;
      max = min;
      min = tmp;
    }
  }

  if(val < min)
    return 0;
  else if( val > max )
    return 1.;

  delta = max-min;
  // avoid division by 0
  if( EsUtilities::areEqualFloats(delta, 0.) )
    return 1.;

  val -= min;
  return val/delta;
}
//---------------------------------------------------------------------------

EsVariant EsProgressMonitorTask::completionGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  EsVariant result;

  if( statePulsing != m_state )
  {
    if( !m_children.isEmpty() )
    {
      ulong cnt = m_children.countGet();
      double total = 0;
      for(ulong idx = 0; idx < cnt; ++idx)
      {
        EsProgressMonitorTask* task = reinterpret_cast<EsProgressMonitorTask*>(
          m_children.valueGet(idx).asPointer()
        );
        ES_ASSERT(task);

        total += task->completionGet().asDouble();
      }

      result = total / (double)cnt;
    }
    else
      result = internalCompletionCalc();
  }

  return result;
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::end(const EsString& text)
{
  EsCriticalSectionLocker lock(m_cs);
  if( !m_children.isEmpty() )
    EsException::Throw(esT("Task, containing sub-tasks, could not be explicitly ended"));

  internalThisStateSet(stateComplete);
  internalTextSet(text);
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalMonitorSet(const EsProgressMonitorIntf::Ptr& mon)
{
  const EsString& path = internalPathGet();

  if( m_monitor )
  {
    EsProgressMonitor* directMon = asMonitor(m_monitor);
    directMon->internalChildRemove(path);
    m_monitor.reset();
  }

  if( mon )
  {
    EsProgressMonitor* directMon = asMonitor(mon);
    directMon->internalChildAdd(mon, this);
    m_monitor = mon;
  }

  for( ulong idx = 0; idx < m_children.countGet(); ++idx )
  {
    EsProgressMonitorTask* child = reinterpret_cast<EsProgressMonitorTask*>(
      m_children.valueGet(idx).asPointer()
    );
    ES_ASSERT(child);

    child->internalMonitorSet( mon );
  }
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalChildAdd(
  const EsProgressMonitorTaskIntf::Ptr& thisTask,
  EsProgressMonitorTask* child)
{
  ES_ASSERT(child);
  m_children.itemAdd( child->m_id, EsVariant(child, EsVariant::ACCEPT_POINTER) );
  child->m_parent = thisTask;
  child->internalMonitorSet(m_monitor);
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalChildRemove(const EsString& childId)
{
  ulong idx = m_children.itemFind(childId);
  if(idx != EsStringIndexedMap::npos)
    m_children.itemDelete(idx);
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::attachTo(const EsBaseIntfPtr& taskOrMonitor)
{
  EsCriticalSectionLocker lock(m_cs);

  internalDetach();

  EsProgressMonitorTaskIntf::Ptr task = taskOrMonitor;
  if( task )
  {
    EsProgressMonitorTask* parent = asTask(task);
    parent->internalChildAdd(task, this);
  }
  else
    internalMonitorSet( taskOrMonitor );
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::attachToReflected(const EsVariant& target)
{
  if( !target.isEmpty() )
    attachTo( target.asObject() );
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::detach()
{
  EsCriticalSectionLocker lock(m_cs);

  internalDetach();
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::internalDetach()
{
  internalMonitorSet( EsProgressMonitorIntf::Ptr() );

  if( m_parent )
  {
    EsProgressMonitorTask* parent = asTask(m_parent);
    parent->internalChildRemove(m_id);
    m_parent.reset();
  }

  m_pathInvalid = true;
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::internalProgressTextGet() const
{
  // default functionality
  if( m_progressTextFormatter.isEmpty() )
  {
    if( statePulsing != m_state )
      return EsString::format(esT("%0.1f %%"), m_completion*100.);
  }
  else
  {
    const EsVariant& objOrStatic = m_progressTextFormatter.itemGet(0);
    const EsString& methodName = m_progressTextFormatter.itemGet(1).asString();
    EsVariant result;
    EsBaseIntfPtr This(const_cast<EsBaseIntf*>(asBaseIntf()), true);
    if( m_formatterIsSvc )
    {
      const EsString& svc = objOrStatic.asString();
      result = EsClassInfo::classInfoGet(svc, true)->classCall(methodName, This);
    }
    else
    {
      EsReflectedClassIntf::Ptr obj = objOrStatic.asExistingObject();
      result = obj->call(methodName, This);
    }

    return result.asString();
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::get_id() const
{
  return m_id;
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::get_orphaned() const
{
  return isOrphaned();
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::get_complete() const
{
  return isComplete();
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::get_pulsing() const
{
  return isPulsing();
}
//---------------------------------------------------------------------------

bool EsProgressMonitorTask::get_hasChildren() const
{
  return hasChildren();
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::get_path() const
{
  return pathGet();
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::get_text() const
{
  return textGet();
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::set_text(const EsString& text)
{
  return textSet(text);
}
//---------------------------------------------------------------------------

EsString EsProgressMonitorTask::get_progressText() const
{
  return progressTextGet();
}
//---------------------------------------------------------------------------

EsVariant EsProgressMonitorTask::get_range() const
{
  return rangeGet();
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::set_range(const EsVariant& range)
{
  rangeSet(range);
}
//---------------------------------------------------------------------------

EsVariant EsProgressMonitorTask::get_position() const
{
  return valueGet();
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::set_position(const EsVariant& pos)
{
  valueSet(pos);
}
//---------------------------------------------------------------------------

EsVariant EsProgressMonitorTask::get_completion() const
{
  return completionGet();
}
//---------------------------------------------------------------------------

void EsProgressMonitorTask::set_progressTextFormatter(const EsVariant& formatter)
{
  progressTextFormatterSet(formatter);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Reflection class info declaration
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsProgressMonitor, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsProgressMonitor, EsProgressMonitorIntf, reset, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsProgressMonitor, EsBaseIntfPtr_ClassCall_cr_EsString, NO_METHOD_DESCR)
  // properties
  ES_DECL_PROP_INFO_RO(EsProgressMonitor, id, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsProgressMonitor, taskPaths, EsStringArray, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//---------------------------------------------------------------------------

EsProgressMonitor::EsProgressMonitor(const EsString& id)
{
  EsProgressMonitorTask::idCheck(id);
  m_dynamic = true;
  m_id = id;
  m_cat = EVTC_PROGRESS_MONITOR_PFX+m_id;
}
//---------------------------------------------------------------------------

EsProgressMonitorIntf::Ptr EsProgressMonitor::create(const EsString& id)
{
  std::unique_ptr<EsProgressMonitor> m( new EsProgressMonitor(id) );
  ES_ASSERT(m);

  return m.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsProgressMonitor::NEW(cr_EsString id)
{
  return create(id);
}
//---------------------------------------------------------------------------

void EsProgressMonitor::viewEventPost(ulong id, const EsVariant& payload) const
{
  EsEventDispatcher::eventPost(m_cat, id, payload);
}
//---------------------------------------------------------------------------

const EsString& EsProgressMonitor::idGet() const
{
  return m_id;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(void, EsProgressMonitor::reset)()
{
  EsCriticalSectionLocker lock(m_cs);
  while( !m_tasks.isEmpty() )
  {
    EsProgressMonitorTask* child = reinterpret_cast<EsProgressMonitorTask*>(
        m_tasks.valueGet(0).asPointer()
      );
    ES_ASSERT(child);
    child->detach();
  }
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsString::Array, EsProgressMonitor::taskPathsGet)() const
{
  EsCriticalSectionLocker lock(m_cs);
  EsString::Array result = m_tasks.namesGet();
  std::sort(result.begin(), result.end());

  return result;
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsProgressMonitor::taskGetByPath)(const EsString& path) const
{
  EsCriticalSectionLocker lock(m_cs);
  EsProgressMonitorTask* task = reinterpret_cast<EsProgressMonitorTask*>(
      m_tasks.valueGet(path).asPointer()
    );
  ES_ASSERT(task);
  return EsBaseIntf::Ptr(task->asBaseIntf(), true);
}
//---------------------------------------------------------------------------

void EsProgressMonitor::internalChildAdd(
  const EsProgressMonitorIntf::Ptr& ES_UNUSED(thisMon),
  EsProgressMonitorTask* child)
{
  ES_ASSERT(child);
  const EsString& path = child->pathGet();

  m_tasks.itemAdd( path, EsVariant(child, EsVariant::ACCEPT_POINTER) );

  EsVariant payload(EsVariant::VAR_VARIANT_COLLECTION);
  payload.addToVariantCollection( path )
    .addToVariantCollection( child->m_text )
    .addToVariantCollection( child->m_state )
    .addToVariantCollection( child->internalProgressTextGet() )
    .addToVariantCollection( EsProgressMonitorTask::statePulsing == child->m_state ?
      EsVariant::null() : EsVariant(child->m_completion) );

  viewEventPost(evtTaskAdded, payload);
}
//---------------------------------------------------------------------------

void EsProgressMonitor::internalChildRemove(const EsString& childPath)
{
  ulong idx = m_tasks.itemFind(childPath);
  if(idx != EsStringIndexedMap::npos)
  {
    m_tasks.itemDelete(idx);
    viewEventPost(evtTaskRemoved, childPath);
  }

  if( m_tasks.isEmpty() )
    viewEventPost(evtNoMoreTasks, EsVariant::null());
}
//---------------------------------------------------------------------------

EsString EsProgressMonitor::get_id() const
{
  return m_id;
}
//---------------------------------------------------------------------------

EsStringArray EsProgressMonitor::get_taskPaths() const
{
  return taskPathsGet();
}
//---------------------------------------------------------------------------

