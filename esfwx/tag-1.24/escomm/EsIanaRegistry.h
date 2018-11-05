#ifndef _es_iana_registry_h_
#define _es_iana_registry_h_

#ifdef ES_COMM_USE_SOCKETS

/// IANA registry lookup
///
class ESCOMM_CLASS EsIanaRegistry
{
public:
  /// Protocol lookup masks
  enum {
    ProtocolAny = 0,
    ProtocolTCP = 1,
    ProtocolUDP = 2,
    ProtocolAll = 3
  };

  /// IANA registry node
  class ESCOMM_CLASS Node
  {
  public:
    Node();

    bool isOk() const;

    ulong portGet() const { return m_port; }
    ulong protocolMaskGet() const { return m_mask; }
    const EsString& serviceGet() const { return m_svc; }
    const EsString& serviceAlias1Get() const { return m_alias1; }
    const EsString& serviceAlias2Get() const { return m_alias2; }
    const EsString& descriptionGet() const { return m_descr; }

  protected:
    ulong m_port;
    ulong m_mask;
    EsString m_svc;
    EsString m_alias1;
    EsString m_alias2;
    EsString m_descr;

    friend class EsIanaRegistry;
  };

  /// Lookup registry record by service name or alias, optionally restricting result by
  /// protocol mask
  ///
  static Node lookupByService(const EsString& svc, ulong protoMask = ProtocolAny);

  /// Lookup registry record by port number, optionally restricting result by
  /// protocol mask
  ///
  static Node lookupByPort(ulong port, ulong protoMask = ProtocolAny);

private:
  EsIanaRegistry();
  EsIanaRegistry(const EsIanaRegistry&);
  EsIanaRegistry& operator=(const EsIanaRegistry&);
};

#endif // ES_COMM_USE_SOCKETS

#endif // _es_iana_registry_h_
