#include "escorepch.h"
#pragma hdrstop

#include "EsEventDispatcher.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

// EsEventSubscriber impl
//
// accepts comma-separated list of categories,
// this subscriber is interested in.
// empty string results in 'generic' category being used
EsEventSubscriber::EsEventSubscriber(const EsString& categories, ulong precedence) :
m_precedence(precedence),
m_active(true),
m_subscribed(false)
{
  if( categories.empty() )
    categoriesReset();
  else
    categoriesSet(categories);

  subscribe(precedence);
}

EsEventSubscriber::~EsEventSubscriber()
{
  if( !m_subscribed )
    return;

  unsubscribe();
}

EsEventDispatcher& EsEventDispatcher::instGet()
{
  static EsEventDispatcher s_inst;

  return s_inst;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

const EsString& EsEventSubscriber::all()
{
  static const EsString sc_all(esT("all"), EsString::doHashInit);

  return sc_all;
}

void EsEventSubscriber::subscribe(ulong precedence)
{
  EsCriticalSectionLocker subsLock( EsEventDispatcher::instGet().m_csSubs);
  EsCriticalSectionLocker lock(m_cs);

  if( !m_subscribed )
  {
    m_precedence = precedence;
    EsEventDispatcher::instGet().internalSubscribe(this);
    m_subscribed = true;
  }
}

void EsEventSubscriber::unsubscribe()
{
  EsCriticalSectionLocker subsLock( EsEventDispatcher::instGet().m_csSubs );
  EsCriticalSectionLocker lock(m_cs);

  if( m_subscribed )
  {
    EsEventDispatcher::instGet().internalUnsubscribe(this);
    m_subscribed = false;
  }
}

bool EsEventSubscriber::isSubscribed() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_subscribed;
}

bool EsEventSubscriber::isInterestedIn(const EsString& category) const
{
  // m_categories is thread-safe
  return m_categories.itemExists(category) ||
    m_categories.itemExists( EsEventSubscriber::all() );
}

EsString::Array EsEventSubscriber::categoriesGet() const
{
  // m_categories is thread-safe
  return m_categories.namesGet();
}

void EsEventSubscriber::categoriesSet(const EsString::Array& categories)
{
  m_categories.clear();
  for(size_t idx = 0; idx < categories.size(); ++idx)
    categoryAdd(categories[idx]);
  if(m_categories.isEmpty())
    categoryAdd(esT("generic"));
}

void EsEventSubscriber::categoriesSet(const EsString& categories)
{  
  m_categories.clear();  
  EsStringTokenizer tok(categories, esT(",; \n\r\t"));
  while( tok.get_moreTokens() )
  {
    const EsString& cat = tok.get_nextToken();
    categoryAdd(cat);
  }
  if(m_categories.isEmpty())
    categoryAdd(esT("generic"));
}

void EsEventSubscriber::categoryAdd(const EsString& category)
{
  if( category.empty() )
    EsException::Throw(esT("Subscriber category must not be empty"));
  m_categories.itemAdd(category);
}

void EsEventSubscriber::categoryRemove(const EsString& category)
{
  m_categories.itemDelete(category);
}

void EsEventSubscriber::categoriesReset()
{
  m_categories.clear();
  categoryAdd(esT("generic"));
}

bool EsEventSubscriber::activeGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_active;
}

void EsEventSubscriber::activeSet(bool active)
{
  EsCriticalSectionLocker lock(m_cs);
  m_active = active;
}

// async event subscriber impl
//
EsEventSubscriberAsync::EsEventSubscriberAsync(const EsString& categories /*= EsString::s_null*/) :
EsEventSubscriber(categories),
m_sem(0, queueMaxLength)
{
  ES_ASSERT( m_sem.isOk() );
}

EsEventSubscriberAsync::~EsEventSubscriberAsync()
{
  activeSet(false);
  // clear event queue
  reset();
}

void EsEventSubscriberAsync::reset()
{
  EsCriticalSectionLocker lock(m_cs);
  m_queue.clear();
//  while( EsSemaphore::resultOk == m_sem.tryWait() );
}

// event notification interface
void EsEventSubscriberAsync::notify(const EsEventIntf::Ptr& evt)
{
  ES_ASSERT(evt);
  EsCriticalSectionLocker lock(m_cs);
  while( m_queue.size() >= queueMaxLength )
  {
    ES_DEBUG_TRACE(esT("EsEventSubscriberAsync event queue overflow occurred!"));

    m_queue.pop_front();
  }
  ES_ASSERT(m_queue.size() < queueMaxLength);
  m_queue.push_back(evt);

  // Signal semaphore
  m_sem.post();
}

// async event reception interface
EsEventIntf::Ptr EsEventSubscriberAsync::eventReceive()
{
  return eventReceive(0);
}

EsEventIntf::Ptr EsEventSubscriberAsync::eventReceive(ulong tmo)
{
  if( EsSemaphore::resultOk == m_sem.wait(tmo) )
  {
    EsCriticalSectionLocker lock(m_cs);
    if( !m_queue.empty() )
    {
      EsEventIntf::Ptr evt = m_queue.front();
      m_queue.pop_front();

      return evt;
    }
  }

  return EsEventIntf::Ptr();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Event dispatcher implementation
//
EsEventDispatcher::EsEventDispatcher()
{}

EsEventDispatcher::~EsEventDispatcher()
{
  internalClear();
}

void EsEventDispatcher::internalClear()
{
  EsCriticalSectionLocker lock(m_csSubs);
  while( !m_subs.empty() )
  {
    EsEventSubscriber* sub = m_subs.begin()->second;
    ES_ASSERT(sub);
    internalUnsubscribe(sub);
  }
}

// event processor
bool EsEventDispatcher::internalProcess()
{
  EsEventIntf::Ptr evt;
  {
    EsCriticalSectionLocker lock(m_cs);
    if( !m_queue.empty() )
    {
      evt = m_queue.front();
      m_queue.pop_front();
    }
  }

  if( evt )
  {
    EsCriticalSectionLocker lock(m_csSubs);

    const EsString& cat = evt->categoryGet();
    for( Subscribers::const_iterator cit = m_subs.begin(); cit != m_subs.end(); ++cit )
    {
      EsEventSubscriber* sub = (*cit).second;
      ES_ASSERT(sub);

      if( sub->activeGet() && sub->isInterestedIn(cat) )
        sub->notify(evt);
      // check if consumed event flag is set
      // if so, break further event processing
      if( evt->flagsGet() & EsEvent::flagConsumed )
        break;
    }

    return true;
  }

  return false;
}

bool EsEventDispatcher::internalNoSubscribers() const
{
  EsCriticalSectionLocker lock(m_csSubs);
  return m_subs.empty();
}

void EsEventDispatcher::queueLengthRestrict()
{
  if( m_queue.size() >= queueMaxLength )
    m_queue.pop_front();
  ES_ASSERT(m_queue.size() < queueMaxLength);
}

// internal event searching - merging interface helper
EsEventQueue::iterator EsEventDispatcher::internalEventFind(const EsEventIntf::Ptr& evt)
{
  // todo: think of some way of events comparison. it must include not only category
  // and id, but also payload. And not every payload could be compared...
  return m_queue.end();
}

// event posting interface
void EsEventDispatcher::internalEventPost(const EsEventIntf::Ptr& evt, bool merge)
{
  EsCriticalSectionLocker lock(m_cs);
  queueLengthRestrict();

  if( merge )
  {
    EsEventQueue::iterator it = internalEventFind(evt);
    if( it != m_queue.end() )
    {
      (*it) = evt; // replace old event copy inplace
      return;
    }
  }

  m_queue.push_back(evt);
}

void EsEventDispatcher::internalEventPostUrgent(const EsEventIntf::Ptr& evt, bool merge)
{
  EsCriticalSectionLocker lock(m_cs);
  queueLengthRestrict();
  if( merge )
  {
    EsEventQueue::iterator it = internalEventFind(evt);
    if( it != m_queue.end() )
      m_queue.erase(it); // remove old event
  }

  m_queue.push_front(evt);
}

/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
void EsEventDispatcher::eventPost(const EsString& category, ulong id, const EsVariant& payload /*= EsVariant::s_null*/)
{
  instGet().internalEventPost( EsEvent::create(category, id, payload), false );
}

/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
void EsEventDispatcher::eventPost(ulong id, const EsVariant& payload /*= EsVariant::s_null*/)
{
  instGet().internalEventPost( EsEvent::create(id, payload), false );
}

/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
void EsEventDispatcher::eventPostUrgent(const EsString& category, ulong id, const EsVariant& payload /*= EsVariant::s_null*/)
{
  instGet().internalEventPostUrgent( EsEvent::create(category, id, payload), false );
}

/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
void EsEventDispatcher::eventPostUrgent(ulong id, const EsVariant& payload /*= EsVariant::s_null*/)
{
  instGet().internalEventPostUrgent( EsEvent::create(id, payload), false );
}

// Find subscriber by its instance
EsEventDispatcher::Subscribers::iterator EsEventDispatcher::subscriberFind(EsEventSubscriber* sub)
{
  ES_ASSERT(sub);
  for( Subscribers::iterator cit = m_subs.begin(); cit != m_subs.end(); ++cit )
  {
    if( (*cit).second == sub )
      return cit;
  }

  return m_subs.end();
}

EsEventDispatcher::Subscribers::const_iterator EsEventDispatcher::subscriberFind(EsEventSubscriber* sub) const
{
  ES_ASSERT(sub);
  for( Subscribers::const_iterator cit = m_subs.begin(); cit != m_subs.end(); ++cit )
  {
    if( (*cit).second == sub )
      return cit;
  }

  return m_subs.end();
}

// subscription manipulation
void EsEventDispatcher::internalSubscribe(EsEventSubscriber* sub)
{
  ES_ASSERT(sub);

  Subscribers::const_iterator cit = subscriberFind(sub);
  if( cit == m_subs.end() )
  {
    m_subs.insert(
      Subscribers::value_type(
        sub->precedenceGet(),
        sub
      )
    );
    sub->m_subscribed = true;
  }
}

void EsEventDispatcher::internalUnsubscribe(EsEventSubscriber* sub)
{
  ES_ASSERT(sub);

  Subscribers::iterator cit = subscriberFind(sub);
  if( m_subs.end() != cit )
  {
    m_subs.erase( cit );
    sub->m_subscribed = false;
  }
}

bool EsEventDispatcher::internalIsEmpty() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_queue.empty();
}

// event processor (must be pumped periodically by the main thread)
bool EsEventDispatcher::process()
{
  return instGet().internalProcess();
}

void EsEventDispatcher::clear()
{
  instGet().internalClear();
}

bool EsEventDispatcher::noSubscribers()
{
  return instGet().internalNoSubscribers();
}

// event posting interface
void EsEventDispatcher::eventPost(const EsEventIntf::Ptr& evt)
{
  instGet().internalEventPost(evt, false);
}

void EsEventDispatcher::eventPostUrgent(const EsEventIntf::Ptr& evt)
{
  instGet().internalEventPostUrgent(evt, false);
}

//// subscription manipulation
//void EsEventDispatcher::subscribe(EsEventSubscriber* sub)
//{
//  ES_ASSERT(sub);
//
//  EsCriticalSectionLocker lock(instGet().m_csSubs);
//  instGet().internalSubscribe(sub);
//}
//
//void EsEventDispatcher::unsubscribe(EsEventSubscriber* sub)
//{
//  ES_ASSERT(sub);
//
//  EsCriticalSectionLocker lock(instGet().m_csSubs);
//  instGet().internalUnsubscribe(sub);
//}

/// Event queue emptiness check
bool EsEventDispatcher::isEmpty()
{
  return instGet().internalIsEmpty();
}

