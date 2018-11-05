#ifndef _es_event_dispatcher_h_
#define _es_event_dispatcher_h_

// Forward decl
class EsEventSubscriber;

// Event queue
typedef std::list<EsEventIntf::Ptr> EsEventQueue;

// Event dispatcher singleton class
//
class ESCORE_CLASS EsEventDispatcher
{
private:
	enum { queueMaxLength = 1024 };
	typedef std::multimap<ulong, EsEventSubscriber*> Subscribers;

private:
	EsEventDispatcher();

	// event processor
	bool internalProcess();
  // clear all subscribers
  void internalClear();
  // internal subscribers presence checking
  bool internalNoSubscribers() const;

	// internal event searching - merging interface helper
	EsEventQueue::iterator internalEventFind(const EsEventIntf::Ptr& evt);
	// event posting interface
	void internalEventPost(const EsEventIntf::Ptr& evt, bool merge);
	void internalEventPostUrgent(const EsEventIntf::Ptr& evt, bool merge);
	// subscription manipulation
	void internalSubscribe(EsEventSubscriber* sub);
	void internalUnsubscribe(EsEventSubscriber* sub);
	// helper services
	void queueLengthRestrict();
	bool internalIsEmpty() const;

  // Find subscriber by its instance
  EsEventDispatcher::Subscribers::iterator subscriberFind(EsEventSubscriber* sub);
  EsEventDispatcher::Subscribers::const_iterator subscriberFind(EsEventSubscriber* sub) const;

public:
	~EsEventDispatcher();

	/// Event processor (must be pumped periodically by the main thread)
	static bool process();
  /// Clear all subsbcriptions
  static void clear();
  /// Return true if there are no subscribers
  static bool noSubscribers();

	/// Post event to the message queue
	static void eventPost(const EsEventIntf::Ptr& evt);
	/// Post urgent event to the fromt of the message queue
	static void eventPostUrgent(const EsEventIntf::Ptr& evt);
	/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
	static void eventPost(const EsString& category, ulong id, const EsVariant& payload = EsVariant::null());
	/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
	static void eventPost(ulong id, const EsVariant& payload = EsVariant::null());
	/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
	static void eventPostUrgent(const EsString& category, ulong id, const EsVariant& payload = EsVariant::null());
	/// Simplified event posting service. Internally does call to the corresponding EsEvent::create
	static void eventPostUrgent(ulong id, const EsVariant& payload = EsVariant::null());
//	/// Subscription manipulation
//	static void subscribe(EsEventSubscriber* sub);
//	static void unsubscribe(EsEventSubscriber* sub);
	/// Event queue emptiness check
	static bool isEmpty();

protected:
	static EsEventDispatcher& instGet();

protected:
	// data members
	mutable EsCriticalSection m_cs;
	EsEventQueue m_queue;
	mutable EsCriticalSection m_csSubs;
	Subscribers m_subs;

private:
	// prohibited functionality
	EsEventDispatcher(const EsEventDispatcher&) ES_REMOVEDECL;
	EsEventDispatcher& operator=(const EsEventDispatcher&) ES_REMOVEDECL;

  friend class EsEventSubscriber;
};

// Event subscriber base impl
//
class ESCORE_CLASS EsEventSubscriber
{
public:
  enum {
    precedenceHighest = 0,
    precedenceHigher  = 500,
    precedenceDefault = 1000
  };

protected:
	// accepts comma-separated list of categories,
	// this subscriber is interested in.
	// empty string results in 'generic' category being used
	EsEventSubscriber(const EsString& categories, ulong precedence = precedenceDefault);

public:
	virtual ~EsEventSubscriber();

  void subscribe(ulong precedence = precedenceDefault);
  void unsubscribe();
  bool isSubscribed() const;

  ulong precedenceGet() const { return m_precedence; }
	bool isInterestedIn(const EsString& category) const;
	EsString::Array categoriesGet() const;
	void categoriesSet(const EsString& categories);
	void categoriesSet(const EsString::Array& categories);
	void categoryAdd(const EsString& category);
	void categoryRemove(const EsString& category);
	void categoriesReset();
	bool activeGet() const;
	void activeSet(bool active);
	// event notification interface
	ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt) = 0;

  // Specific category - 'all', subscriber with this category will receive
  // all notifications
  static const EsString& all();

protected:
	EsStringIndexedMap m_categories;
	mutable EsCriticalSection m_cs;
  ulong m_precedence;
	bool m_active;
  bool m_subscribed;

private:
	EsEventSubscriber() ES_REMOVEDECL;
	EsEventSubscriber(const EsEventSubscriber&) ES_REMOVEDECL;
	EsEventSubscriber& operator=(const EsEventSubscriber&) ES_REMOVEDECL;

  friend class EsEventDispatcher;
};

/// Asynchronous event subscriber implementation (which may be used in threads)
/// Asynchronous event subscriber, apart from sync one, may not handle events
/// directly, it rather allows to pass them asynchronously to the handler
/// via eventReceive interface
//
class ESCORE_CLASS EsEventSubscriberAsync : public EsEventSubscriber
{
private:
	enum { queueMaxLength = 512 };

public:
	EsEventSubscriberAsync(const EsString& categories = EsString::null());
	virtual ~EsEventSubscriberAsync();

	/// Event notification interface
	ES_DECL_INTF_METHOD(void, notify)(const EsEventIntf::Ptr& evt);

	/// Async event reception interface. if reception is successful,
	/// event object is returned, otherwise, an empty pointer is returned
	ES_DECL_INTF_METHOD(EsEventIntf::Ptr, eventReceive)();
	ES_DECL_INTF_METHOD(EsEventIntf::Ptr, eventReceive)(ulong tmo);

	/// Reset inbound queue
	ES_DECL_INTF_METHOD(void, reset)();

protected:
	EsSemaphore m_sem;
	EsEventQueue m_queue;

private:
  EsEventSubscriberAsync(const EsEventSubscriberAsync&) ES_REMOVEDECL;
  EsEventSubscriberAsync& operator=(const EsEventSubscriberAsync&) ES_REMOVEDECL;
};

#endif // _es_event_dispatcher_h_
