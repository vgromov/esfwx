#include "escommpch.h"
#pragma hdrstop

#include "EsIanaRegistry.h"
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_COMM_USE_SOCKETS

/// IANA registry record
struct EsIanaRegistryRec
{
  ulong port;
  ulong protoMask;
  char svc[16];
  char alias1[16];
  char alias2[16];
  char descr[64];
};

#define IANA_REGISTRY_BEGIN \
  static const EsIanaRegistryRec sc_ianaRegistry[] = {
#define IANA_REGISTRY_ENTRY(port, protMask, svc, svcAlias1, svcAlias2, descr) \
  {port, protMask, svc, svcAlias1, svcAlias2, descr},
#define IANA_REGISTRY_END \
  { 0, 0, 0, 0, 0, 0 } };
#include "EsIanaRegistryDefs.h"

//---------------------------------------------------------------------------
// IANA registry class implementation
//
static inline bool ianaRecIsEmpty(const EsIanaRegistryRec* rec)
{
  return 0 == rec->port &&
        0 == rec->protoMask &&
        0 == rec->svc[0] &&
        0 == rec->alias1[0] &&
        0 == rec->alias2[0] &&
        0 == rec->descr[0];
}
//---------------------------------------------------------------------------

/// Find service registry node by service name
static const EsIanaRegistryRec* ianaRecFind(const char* svc, ulong protoMask)
{
  const EsIanaRegistryRec* rec = sc_ianaRegistry;
  while( !ianaRecIsEmpty(rec) )
  {
    if( ( 0 == strncmp(svc, rec->svc, 16) ||
          0 == strncmp(svc, rec->alias1, 16) ||
          0 == strncmp(svc, rec->alias2, 16) ) &&
        protoMask == (rec->protoMask & protoMask) )
      return rec;

    ++rec;
  }

  return 0;
}
//---------------------------------------------------------------------------

/// Find service registry node by port value
static const EsIanaRegistryRec* ianaRecFind(ulong port, ulong protoMask)
{
  const EsIanaRegistryRec* rec = sc_ianaRegistry;
  while( !ianaRecIsEmpty(rec) )
  {
    if( port == rec->port &&
        protoMask == (rec->protoMask & protoMask) )
      return rec;

    ++rec;
  }

  return 0;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Node default constructor
EsIanaRegistry::Node::Node() :
m_port(0),
m_mask(0)
{}
//---------------------------------------------------------------------------

// Node emptiness check
bool EsIanaRegistry::Node::isOk() const
{
  return 0 != m_port &&
    0 != m_mask &&
    !m_svc.empty();
}
//---------------------------------------------------------------------------

/// Lookup registry record by service name or alias, optionally restricting result by
/// protocol mask
///
EsIanaRegistry::Node EsIanaRegistry::lookupByService(const EsString& svc, ulong protoMask /*= ProtocolAny*/)
{
  const EsByteString& cstr = EsString::toAscii( EsString::toLower(svc) );
  const EsIanaRegistryRec* rec = ianaRecFind(cstr.c_str(), protoMask);
  EsIanaRegistry::Node result;

  if( rec )
  {
    result.m_port = rec->port;
    result.m_mask = rec->protoMask;
    result.m_svc = EsString::fromAscii(rec->svc);
    result.m_alias1 = EsString::fromAscii(rec->alias1);
    result.m_alias2 = EsString::fromAscii(rec->alias2);
    result.m_descr = EsString::fromAscii(rec->descr);
  }

  return result;
}
//---------------------------------------------------------------------------

/// Lookup registry record by port number, optionally restricting result by
/// protocol mask
///
EsIanaRegistry::Node EsIanaRegistry::lookupByPort(ulong port, ulong protoMask /*= ProtocolAny*/)
{
  const EsIanaRegistryRec* rec = ianaRecFind(port, protoMask);
  EsIanaRegistry::Node result;
  if( rec )
  {
    result.m_port = rec->port;
    result.m_mask = rec->protoMask;
    result.m_svc = EsString::fromAscii(rec->svc);
    result.m_alias1 = EsString::fromAscii(rec->alias1);
    result.m_alias2 = EsString::fromAscii(rec->alias2);
    result.m_descr = EsString::fromAscii(rec->descr);
  }

  return result;
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_SOCKETS
