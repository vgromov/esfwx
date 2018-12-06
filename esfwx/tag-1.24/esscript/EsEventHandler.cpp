#include "esscriptpch.h"
#pragma hdrstop

#include "EsEventHandler.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

/// Reflected handler implementation
///
/// Internal syncronous event subscriber proxy
///
class EsEventSubscriberSync : public EsEventSubscriber
{
public:
  EsEventSubscriberSync(EsEventHandler& This, const EsString& cats) :
  EsEventSubscriber(cats),
  m_this(This)
  {}

  ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt)
  {
    m_this.onEvent(evt);
  }

protected:
  EsEventHandler& m_this;
};

/// EsEventHandler implementation
///
ES_DECL_BASE_CLASS_INFO_BEGIN(EsEventHandler, NO_CLASS_DESCR)
  // class services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsEventHandler, createSync, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsEventHandler, createAsync, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsEventHandler, createSync, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsEventHandler, createAsync, EsBaseIntfPtr_ClassCall_cr_EsBaseIntfPtr_cr_EsString, NO_METHOD_DESCR)
  // services
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEventHandler, EsEventHandlerIntf, isInterestedIn, bool_CallConst_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEventHandler, EsEventHandlerIntf, categoryAdd, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEventHandler, EsEventHandlerIntf, categoryRemove, void_Call_cr_EsString, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEventHandler, EsEventHandlerIntf, eventProcess, void_Call_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEventHandler, EsEventHandlerIntf, eventProcess, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEventHandler, EsEventHandlerIntf, eventsReset, void_Call, NO_METHOD_DESCR)
  // properties
  ES_DECL_PROP_INFO_RO(  EsEventHandler, isSync, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(    EsEventHandler, categories, EsVariant, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(    EsEventHandler, active, bool, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(  EsEventHandler, handler, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

EsEventHandler::EsEventHandler(bool sync, const EsString& categories /*= EsString::s_null*/) :
m_isSync(sync),
m_sub(nullptr)
{
  m_dynamic = true;
  if( m_isSync )
    m_sub = new EsEventSubscriberSync(*this, categories);
  else
    m_sub = new EsEventSubscriberAsync(categories);
  ES_ASSERT(m_sub);
}

EsEventHandler::~EsEventHandler()
{
  EsCriticalSectionLocker lock(m_cs);
  m_handler.reset();
  if( m_sub )
  {
    m_sub->activeSet(false);
    ES_DELETE(m_sub);
  }
}

EsBaseIntfPtr EsEventHandler::createSync(cr_EsBaseIntfPtr handler)
{
  std::unique_ptr<EsEventHandler> tmp(new EsEventHandler(true));
  ES_ASSERT(tmp);
  tmp->set_handler(handler);
  return tmp.release()->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsEventHandler::createAsync(cr_EsBaseIntfPtr handler)
{
  std::unique_ptr<EsEventHandler> tmp(new EsEventHandler(false));
  ES_ASSERT(tmp);
  tmp->set_handler(handler);
  return tmp.release()->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsEventHandler::createSync(cr_EsBaseIntfPtr handler, cr_EsString cats)
{
  std::unique_ptr<EsEventHandler> tmp(new EsEventHandler(true, cats));
  ES_ASSERT(tmp);
  tmp->set_handler(handler);
  return tmp.release()->asBaseIntfPtrDirect();
}

EsBaseIntfPtr EsEventHandler::createAsync(cr_EsBaseIntfPtr handler, cr_EsString cats)
{
  std::unique_ptr<EsEventHandler> tmp(new EsEventHandler(false, cats));
  ES_ASSERT(tmp);
  tmp->set_handler(handler);
  return tmp.release()->asBaseIntfPtrDirect();
}

bool EsEventHandler::isInterestedIn(cr_EsString category) const
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  return m_sub->isInterestedIn(category);
}

void EsEventHandler::categoryAdd(cr_EsString category)
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  m_sub->categoryAdd(category);
}

void EsEventHandler::categoryRemove(cr_EsString category)
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  m_sub->categoryRemove(category);
}

void EsEventHandler::eventProcess(ulong tmo)
{
  if( !m_isSync )
  {
    EsCriticalSectionLocker lock(m_cs);
    ES_ASSERT(m_sub);
    EsEventIntf::Ptr evt =
      reinterpret_cast<EsEventSubscriberAsync*>(m_sub)->eventReceive(tmo);

    while( evt )
    {
      onEvent(evt);
      evt =  reinterpret_cast<EsEventSubscriberAsync*>(m_sub)->eventReceive(tmo);
    }
  }
}

void EsEventHandler::eventProcess()
{
  eventProcess(0);
}

void EsEventHandler::eventsReset()
{
  if( !m_isSync )
  {
    EsCriticalSectionLocker lock(m_cs);
    ES_ASSERT(m_sub);
    reinterpret_cast<EsEventSubscriberAsync*>(m_sub)->reset();
  }
}

EsStringArray EsEventHandler::categoriesGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  return m_sub->categoriesGet();
}

void EsEventHandler::categoriesSet(const EsStringArray& categories)
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  m_sub->categoriesSet(categories);
}

bool EsEventHandler::isActive() const
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  return m_sub->activeGet();
}

void EsEventHandler::activeSet(bool active)
{
  EsCriticalSectionLocker lock(m_cs);
  ES_ASSERT(m_sub);
  m_sub->activeSet(active);
}

void EsEventHandler::handlerSet(const EsBaseIntfPtr& handler)
{
  if( asBaseIntf() == handler.get() )
    EsException::Throw(_("Could not assign event handler to itself as handler"));

  EsCriticalSectionLocker lock(m_cs);
  // NB! use weak reference to handler object, or else we may provoke
  // deadlock and memory leaks
  EsBaseIntfPtr weakref(handler.get(), false, false);

  m_handler = weakref;
  if( m_handler && !m_handler->hasMethod(EsMethodInfoKeyT(1, esT("onEvent"))) )
    EsException::Throw(_("Could not assign event handler object, no 'onEvent(evt)' service found"));
}

bool EsEventHandler::get_isSync() const
{
  return isSync();
}

EsVariant EsEventHandler::get_categories() const
{
  return categoriesGet();
}

void EsEventHandler::set_categories(const EsVariant& cats)
{
  if( cats.isCollection() || cats.isString() )
    categoriesSet(cats.asStringCollection());
  else
    EsException::Throw( _("Could not assign event categories. Categories parameter is invalid.") );
}

bool EsEventHandler::get_active() const
{
  return isActive();
}

void EsEventHandler::set_active(const bool& active)
{
  activeSet(active);
}

void EsEventHandler::set_handler(const EsBaseIntfPtr& handler)
{
  handlerSet(handler);
}

void EsEventHandler::onEvent(const EsEventIntf::Ptr& evt)
{
  ES_ASSERT(evt);
  // Execute event on local handler copy, to
  // avoid unneeded object locking in MT environment
  EsReflectedClassIntf::Ptr handler = m_handler;
  {
    EsCriticalSectionLocker lock(m_cs);
    handler = m_handler;
  }

  if( handler )
    handler->call(esT("onEvent"), evt);
}

