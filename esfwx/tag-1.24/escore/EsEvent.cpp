#include "escorepch.h"
#pragma hdrstop
 
#include "EsEvent.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsEvent, NO_CLASS_DESCR)
  // reflected interface methods
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsEvent, EsEventIntf, consume, void_Call, NO_METHOD_DESCR)
  // clas services
  ES_DECL_REFLECTED_CTOR_INFO(EsEvent, EsBaseIntfPtr_ClassCall_cr_EsString_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CTOR_INFO(EsEvent, EsBaseIntfPtr_ClassCall_cr_EsString_ulong_cr_EsVariant, NO_METHOD_DESCR)
  // properties
  ES_DECL_PROP_INFO_RO(  EsEvent, category, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsEvent, id, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(  EsEvent, timestamp, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(    EsEvent, flags, ulong, NO_PROPERTY_LABEL, static_cast<ulong>(EsEvent::flagDefault), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO(    EsEvent, payload, EsVariant, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END

EsEvent::EsEvent(const EsString& category, ulong id, const EsVariant& payload) :
m_ts(EsDateTime::now()),
m_category(category),
m_id(id),
m_flags(flagDefault),
m_payload(payload)
{
  m_dynamic = true;
  if( m_category.empty() )
    m_category = esT("generic");
}

EsEventIntf::Ptr EsEvent::create(const EsString& category, ulong id, const EsVariant& payload /*= EsVariant::s_null*/)
{
  EsEventIntf::Ptr evt(new EsEvent(category, id, payload));
  ES_ASSERT(evt);

  return evt;
}

EsEventIntf::Ptr EsEvent::create(ulong id, const EsVariant& payload /*= EsVariant::s_null*/)
{
  return EsEvent::create(esT("generic"), id, payload);
}

  // reflected CTORs
  //
  // create empty event with category and id
EsBaseIntfPtr  EsEvent::NEW(cr_EsString category, ulong id)
{
  return create(category, id, EsVariant::null());
}

// create event with category, id, and payload
EsBaseIntfPtr EsEvent::NEW(cr_EsString category, ulong id, cr_EsVariant payload)
{
  return create(category, id, payload);
}

// event interface implementation
//
const EsDateTime& EsEvent::timestampGet() const
{
  return m_ts;
}

const EsString& EsEvent::categoryGet() const
{
  return m_category;
}

ulong EsEvent::idGet() const
{
  return m_id;
}

ulong EsEvent::flagsGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_flags;
}

void EsEvent::flagsSet(ulong flags)
{
  EsCriticalSectionLocker lock(m_cs);
  m_flags = flags;
}


void EsEvent::consume()
{
  EsCriticalSectionLocker lock(m_cs);
  m_flags |= flagConsumed;
}

const EsVariant& EsEvent::payloadGet() const
{
  EsCriticalSectionLocker lock(m_cs);
  return m_payload;
}

void EsEvent::payloadSet(const EsVariant& payload)
{
  EsCriticalSectionLocker lock(m_cs);
  m_payload = payload;
}

// reflected properties
//
EsString EsEvent::get_category() const
{
  return categoryGet();
}

ulong EsEvent::get_id() const
{
  return idGet();
}

EsVariant EsEvent::get_timestamp() const
{
  return EsDateTime::toVariant(m_ts);
}

ulong EsEvent::get_flags() const
{
  return flagsGet();
}

void EsEvent::set_flags(cr_ulong flags)
{
  flagsSet(flags);
}

EsVariant EsEvent::get_payload() const
{
  return payloadGet();
}

void EsEvent::set_payload(cr_EsVariant payload)
{
  payloadSet(payload);
}
