#ifndef _es_event_h_
#define _es_event_h_

/// Platform - independent event interface
///
ES_DECL_INTF_BEGIN2(DCE08408, 15314CD7, A0755C21, 6A0185E9, EsEventIntf, EsBaseIntf)
  ES_DECL_INTF_METHOD(const EsDateTime&, timestampGet)() const = 0;
  ES_DECL_INTF_METHOD(const EsString&, categoryGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, idGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const = 0;
  ES_DECL_INTF_METHOD(void, flagsSet)(ulong flags) = 0;
  ES_DECL_INTF_METHOD(void, consume)() = 0;
  ES_DECL_INTF_METHOD(const EsVariant&, payloadGet)() const = 0;
  ES_DECL_INTF_METHOD(void, payloadSet)(const EsVariant& payload) = 0;
ES_DECL_INTF_END

/// Thread-safe event class for in- and -inter thread
/// communication within esfwx framework-based apps
///
class ESCORE_CLASS ES_INTF_IMPL2(EsEvent, EsEventIntf, EsReflectedClassIntf)

public:
  // known event flags
  enum {
    flagDefault    = 0x0000,
    flagConsumed  = 0x0001,
  };

private:
  EsEvent(const EsString& category, ulong id, const EsVariant& payload);

public:
  ES_DECL_REFLECTED_CLASS_BASE(EsEvent)
  ES_DECL_ATTR_HANDLING_STD

  // event creator
  //
  static EsEventIntf::Ptr create(const EsString& category, ulong id, const EsVariant& payload = EsVariant::null());
  static EsEventIntf::Ptr create(ulong id, const EsVariant& payload = EsVariant::null());

  // reflected CTORs
  //
  // create empty event with category and id
  ES_DECL_REFLECTED_CLASS_CTOR2(EsBaseIntfPtr, cr_EsString, ulong);
  // create event with category, id, and payload
  ES_DECL_REFLECTED_CLASS_CTOR3(EsBaseIntfPtr, cr_EsString, ulong, cr_EsVariant);

  // event interface implementation
  //
  ES_DECL_INTF_METHOD(const EsDateTime&, timestampGet)() const;
  ES_DECL_INTF_METHOD(const EsString&, categoryGet)() const;
  ES_DECL_INTF_METHOD(ulong, idGet)() const;
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const;
  ES_DECL_INTF_METHOD(void, flagsSet)(ulong flags);
  ES_DECL_REFLECTED_INTF_METHOD0(void, consume);
  ES_DECL_INTF_METHOD(const EsVariant&, payloadGet)() const;
  ES_DECL_INTF_METHOD(void, payloadSet)(const EsVariant& payload);

  // Reflected properties
  //
  ES_DECL_PROPERTY_RO(category, EsString)
  ES_DECL_PROPERTY_RO(id, ulong)
  ES_DECL_PROPERTY_RO(timestamp, EsVariant)
  ES_DECL_PROPERTY(flags, ulong)
  ES_DECL_PROPERTY(payload, EsVariant)

protected:
  // data members
  mutable EsCriticalSection m_cs;
  EsDateTime m_ts;
  EsString m_category;
  ulong m_id;
  ulong m_flags;
  EsVariant m_payload;
};

/// Helper macro for event categories macros concatenation
#define EVTC_AND   esT(",")

#endif // _es_thread_event_h_
