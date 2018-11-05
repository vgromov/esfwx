#ifndef _es_event_handler_h_
#define _es_event_handler_h_

/// Event handler interface declaration, EsEventHandlerIntf
///
ES_DECL_INTF_BEGIN2( 8BDC7BDC, 13534891, 94B1BF5D, 8E847C10, EsEventHandlerIntf, EsBaseIntf )

	/// Common services
	///

	/// Return true if handler is interested in specific event category
	ES_DECL_INTF_METHOD(bool, isInterestedIn)(const EsString& category) const = 0;
	/// Append category to the subscription
	ES_DECL_INTF_METHOD(void, categoryAdd)(const EsString& category) = 0;
	/// Remove category from subscription
	ES_DECL_INTF_METHOD(void, categoryRemove)(const EsString& category) = 0;

	/// Async subscriber interface
	///

	/// Try to receive and process event, continue trying while ulong timeout is not expired
	/// Return non-empty EsEventIntf::Ptr, if operation is successful,
	/// empty variant otherwise
	ES_DECL_INTF_METHOD(void, eventProcess)(ulong tmo) = 0;
	/// The same as the previous method called with timeout = 0
	ES_DECL_INTF_METHOD(void, eventProcess)() = 0;
	/// Reset event queue (does nothing for sync subscriber)
	ES_DECL_INTF_METHOD(void, eventsReset)() = 0;

	/// Control services
	///

	/// Query, if this handler is of synchronous or asynchronous type
	ES_DECL_INTF_METHOD(bool, isSync)() const = 0;
	/// Subscription categories access
	ES_DECL_INTF_METHOD(EsStringArray, categoriesGet)() const = 0;
	ES_DECL_INTF_METHOD(void, categoriesSet)(const EsStringArray& categories) = 0;

	/// Subscription activity
	ES_DECL_INTF_METHOD(bool, isActive)() const = 0;
	ES_DECL_INTF_METHOD(void, activeSet)(bool active) = 0;

	/// Actual event handler access
	ES_DECL_INTF_METHOD(void, handlerSet)(const EsBaseIntfPtr& handler) = 0;

ES_DECL_INTF_END

/// Reflected event handler helper class. Implementor of EsEventHandlerIntf.
/// It may use either synchronous or asynchronous subscription interface.
/// It does not handle events directly, but rather calls reflected onEvent service of
/// actual handler class, assigned (if any) in m_handler member. In former case,
/// onEvent is called from the main process synchronously, by dispatcher processing;
/// In the latter case, asynchronous thread should periodically call eventProcess,
/// which internally would cause onEvent to fire, if something was actually received
///
class ES_INTF_IMPL2(EsEventHandler, EsEventHandlerIntf, EsReflectedClassIntf)

private:
	EsEventHandler(bool sync, const EsString& categories = EsString::null());

public:
	virtual ~EsEventHandler();

	ES_DECL_REFLECTED_CLASS_BASE(EsEventHandler)
	ES_DECL_ATTR_HANDLING_STD

	/// Reflected services
	///

	/// Specific constructors
	///

	/// Create synchronous handler, taking event handling object as parameter
	ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, createSync, cr_EsBaseIntfPtr);
	/// Create asynchronous handler, taking event handling object as parameter
	ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, createAsync, cr_EsBaseIntfPtr);
	/// Create synchronous handler, taking event handling object as first parameter,
	/// and subscription categories as second one
	ES_DECL_REFLECTED_CLASS_METHOD2(EsBaseIntfPtr, createSync, cr_EsBaseIntfPtr, cr_EsString);
	/// Create asynchronous handler, taking event handling object as first parameter,
	/// and subscription categories as second one
	ES_DECL_REFLECTED_CLASS_METHOD2(EsBaseIntfPtr, createAsync, cr_EsBaseIntfPtr, cr_EsString);

	/// Common services
	///

	/// Return true if handler is interested in specific event category
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, isInterestedIn, cr_EsString);
	/// Append category to the subscription
	ES_DECL_REFLECTED_INTF_METHOD1(void, categoryAdd, cr_EsString);
	/// Remove category from subscription
	ES_DECL_REFLECTED_INTF_METHOD1(void, categoryRemove, cr_EsString);

	/// Async subscriber interface
	///

	/// Try to receive and process event, continue trying while ulong timeout is not expired
	ES_DECL_REFLECTED_INTF_METHOD1(void, eventProcess, ulong);
	/// The same as the previous method called with timeout = 0
	ES_DECL_REFLECTED_INTF_METHOD0(void, eventProcess);
	/// Reset event queue (does nothing for sync subscriber)
	ES_DECL_REFLECTED_INTF_METHOD0(void, eventsReset);

  /// Non-reflected EsEventHandlerIntf services implementation
  ///

	/// Query, if this handler is of synchronous or asynchronous type
	ES_DECL_INTF_METHOD(bool, isSync)() const { return m_isSync; }
	/// Subscription categories access
	ES_DECL_INTF_METHOD(EsStringArray, categoriesGet)() const;
	ES_DECL_INTF_METHOD(void, categoriesSet)(const EsStringArray& categories);
	/// Subscription activity
	ES_DECL_INTF_METHOD(bool, isActive)() const;
	ES_DECL_INTF_METHOD(void, activeSet)(bool active);

	/// Actual event handler access
	ES_DECL_INTF_METHOD(void, handlerSet)(const EsBaseIntfPtr& handler);

	/// Properties
	///

	/// Query, if this handler is of synchronous or asynchronous type
	ES_DECL_PROPERTY_RO(isSync, bool)
	/// Subscription categories access
	ES_DECL_PROPERTY(categories, EsVariant)
	/// Subscription activity
	ES_DECL_PROPERTY(active, bool)
	/// Actual event handler access
	ES_DECL_PROPERTY_WO(handler, EsBaseIntfPtr)

protected:
	/// Semi-private service. Internally calls actual handler's method
	/// to process inoming event(s)
	void onEvent(const EsEventIntf::Ptr& evt);

protected:
	mutable EsCriticalSection m_cs;
	EsEventSubscriber* m_sub;
	EsReflectedClassIntf::Ptr m_handler;
  bool m_isSync;

	friend class EsEventSubscriberSync;
};

#endif // _es_event_handler_h_