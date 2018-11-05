// Android specific stuff for Bluetooth sockets
// NB! do not include this file in project directly, it's
// got included from EsSocket.cpp as appropriate
//

// Bluetooth socket address implementation
//
class EsSocketAddrBT : public EsSocketAddr::Impl
{
public:
  enum {
    BT_PORT_ANY = -1
  };

public:
  EsSocketAddrBT()
  {
    m_type = EsSocketAddrType::Bluetooth;
    m_svc = EsUtilities::GUIDtoStr(GUID_NULL);
    m_flags |= flagSvcResolved;

    m_port = BT_PORT_ANY;
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
    return 0;
  }

  virtual size_t nativeSizeGet() const
  {
    return 0;
  }

  virtual void rawSet(const EsString& raw)
  {
    m_error = 0;
    if( m_raw != raw )
    {
      if( !rawParse(raw) )
        EsException::Throw( _("'%s' is not a valid Bluetooth address"), raw.c_str() );

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
      m_flags |= flagPortResolved;

      if( BT_PORT_ANY != port && 0 != port )
      {
        m_svc = EsUtilities::GUIDtoStr(GUID_NULL);
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

  virtual bool isLessThan(const Impl* other) const
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
    m_error = 0;
    m_type = EsSocketAddrType::Bluetooth;
    m_flags |=
      flagRawResolved|
      flagSvcResolved|
      flagPortResolved;

    m_name.clear();
    m_flags &= ~flagNameResolved;
  }

protected:
  // If raw is a valid bluetooth string, return true.
  //
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
    return rex.get_matches();
  }
};
//---------------------------------------------------------------------------