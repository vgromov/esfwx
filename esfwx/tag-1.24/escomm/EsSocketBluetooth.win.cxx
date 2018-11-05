// Windows specific stuff for Bluetooth sockets
// NB! do not include this file in project directly, it's
// got included from EsSocket.cpp as appropriate
//

// Windows Bluetooth socket address implementation
//
class EsSocketAddrBT : public EsSocketAddr::Impl
{
public:
  EsSocketAddrBT()
  {
    m_type = EsSocketAddrType::Bluetooth;

    m_addr.addressFamily = AF_BTH;
    m_addr.btAddr = BTH_ADDR_NULL;
    m_addr.serviceClassId = GUID_NULL;
    m_svc = EsUtilities::GUIDtoStr(m_addr.serviceClassId);
    m_flags |= flagSvcResolved;

    m_port = BT_PORT_ANY;
    m_addr.port = BT_PORT_ANY;
    m_flags |= flagPortResolved;

    rawSet(EsBluetooth::anyAddr());
  }

  EsSocketAddrBT(const EsSocketAddrBT& src)
  {
    m_type = src.m_type;
    m_flags = src.m_flags;
    m_port = src.m_port;
    m_raw = src.m_raw;
    m_name = src.m_name;
    m_svc = src.m_svc;
    memcpy(&m_addr, &src.m_addr, sizeof(m_addr));
  }

  /// Abstract socket address interface implementation
  ///
  virtual const EsString& anyAddrGet() const
  {
    return EsBluetooth::anyAddr();
  }

  virtual EsSocketAddr::Impl* clone() const
  {
    return new EsSocketAddrBT(*this);
  }

  virtual void* nativeGet() const
  {
    return (void*)&m_addr;
  }

  virtual size_t nativeSizeGet() const
  {
    return sizeof(m_addr);
  }

  virtual void rawSet(const EsString& raw)
  {
    m_error = 0;
    if( m_raw != raw )
    {
      if( !rawParse(raw, reinterpret_cast<esU8*>(&m_addr.btAddr)) )
        EsException::Throw( 
          _("'%s' is not a valid Bluetooth address"), 
          raw
        );

      m_raw = raw;
      m_flags |= flagRawResolved;

      m_name.clear();
      m_flags &= ~flagNameResolved;
    }
  }

  virtual void portSet(ulong port)
  {
    m_error = 0;
    if( m_port != port )
    {
      if( BT_PORT_ANY != port )
        EsNumericCheck::checkRangeUInteger(0, 30, port, _("Bluetooth port number"));

      m_port = port;
      m_addr.port = port;
      m_flags |= flagPortResolved;

      if( BT_PORT_ANY != port && 0 != port )
      {
        m_addr.serviceClassId = GUID_NULL;
        m_svc = EsUtilities::GUIDtoStr(m_addr.serviceClassId);
        m_flags |= flagSvcResolved;
      }
    }
  }

  // Svc name is consigered string representation of BT service GUID
  // Setting proper GUID string for service will reset port to BT_PORT_ANY
  virtual void serviceSet(const EsString& svc)
  {
    m_error = 0;
    if( m_svc != svc )
    {
      GUID uuid = EsUtilities::GUIDfromStr(svc);
      // Convert guid to our string representation, just in case
      m_svc = EsUtilities::GUIDtoStr(uuid);
      // Assign to service guid
      m_addr.serviceClassId = uuid;

      m_flags |= flagSvcResolved;
    }
  }

  virtual bool isEqualTo(const EsSocketAddr::Impl* other) const
  {
    if( other )
    {
      if( this == other )
        return true;
      else if( other->typeGet() == m_type &&
               noError() &&
               other->noError() )
      {
        if( rawGet(true) == other->rawGet(true) )
        {
          return m_svc == other->serviceGet()
            && m_port == other->portGet();
        }
      }
    }

    return false;
  }

  virtual bool isLessThan(const EsSocketAddr::Impl* other) const
  {
    if( isEqualTo( other ) )
      return false;

    if( other )
    {
      if( other->typeGet() > m_type )
        return true;

      if( other->typeGet() == m_type &&
        noError() &&
        other->noError() )
      {
        // it's enough to compare port and raw address
        if( portGet() < other->portGet() )
          return true;

        if( rawGet(true) < other->rawGet(true) )
          return true;
      }
    }

    return false;
  }

  virtual bool rawResolve(bool passive)
  {
    m_error = 0;
    if( !(m_flags & flagRawResolved) )
    {
      if( !m_name.empty() )
      {
        if( EsBluetooth::nameToAddrResolve(
            m_name,
            static_cast<ulong>(EsBluetoothDeviceFlag::Connected)|
            static_cast<ulong>(EsBluetoothDeviceFlag::Authenticated),
            m_raw ) )
        {
          m_flags |= flagRawResolved;
        }
        else
        {
          m_error = static_cast<long>(EsSocketError::HostNotFound);
        }
      }
      else
      {
        m_raw = EsBluetooth::anyAddr();
        m_flags |= flagRawResolved;
      }
    }

    return (m_flags & flagRawResolved);
  }

  virtual bool nameResolve()
  {
    m_error = 0;
    if( !(m_flags & flagNameResolved) )
    {
      ES_ASSERT( rawParse(m_raw) );

      if( EsBluetooth::addrToNameResolve(
            m_raw,
            static_cast<ulong>(EsBluetoothDeviceFlag::All),
            m_name ) )
      {
        m_flags |= flagNameResolved;
      }
      else
      {
        m_error = static_cast<long>(EsSocketError::HostNotFound);
      }
    }

    return (m_flags & flagNameResolved);
  }

  // Consider service name always resolved
  virtual bool serviceResolve()
  {
    m_error = 0;
    m_flags |= flagSvcResolved;
    return (m_flags & flagSvcResolved);
  }

  virtual bool portResolve()
  {
    m_error = 0;
    m_flags |= flagPortResolved;
    return (m_flags & flagPortResolved);
  }

  virtual void reconstructFromNative()
  {
    ES_ASSERT( m_addr.addressFamily == AF_BTH );
    m_error = 0;
    m_type = EsSocketAddrType::Bluetooth;

    // Convert addr to host byte order
    m_raw = EsBluetooth::addrStringGet(m_addr.btAddr);
    m_flags |= flagRawResolved;

    // Convert uuid to host byte order
    m_svc = EsUtilities::GUIDtoStr(m_addr.serviceClassId);
    m_flags |= flagSvcResolved;

    m_port = m_addr.port;
    m_flags |= flagPortResolved;

    m_name.clear();
    m_flags &= ~flagNameResolved;
  }

protected:
  // If raw is a valid bluetooth string, return true. If optional out param is not null,
  // fill it with parsed values
  virtual bool rawParse(const EsString& raw, esU8* out = 0) const
  {
    EsRegEx rex(esT("(?:([a-fA-F0-9]{1,2}):")
                esT("([a-fA-F0-9]{1,2}):")
                esT("([a-fA-F0-9]{1,2}):")
                esT("([a-fA-F0-9]{1,2}):")
                esT("([a-fA-F0-9]{1,2}):")
                esT("([a-fA-F0-9]{1,2}))")
    );
    rex.set_text(raw);
    if( rex.get_matches() )
    {
      if( out )
      {
        const EsString& b6 = rex.matchGet(1);
        const EsString& b5 = rex.matchGet(2);
        const EsString& b4 = rex.matchGet(3);
        const EsString& b3 = rex.matchGet(4);
        const EsString& b2 = rex.matchGet(5);
        const EsString& b1 = rex.matchGet(6);

        out[0] = static_cast<esU8>(EsString::toULong(b1, 16));
        out[1] = static_cast<esU8>(EsString::toULong(b2, 16));
        out[2] = static_cast<esU8>(EsString::toULong(b3, 16));
        out[3] = static_cast<esU8>(EsString::toULong(b4, 16));
        out[4] = static_cast<esU8>(EsString::toULong(b5, 16));
        out[5] = static_cast<esU8>(EsString::toULong(b6, 16));
      }

      return true;
    }

    return false;
  }

protected:
  SOCKADDR_BTH m_addr;
};
//---------------------------------------------------------------------------