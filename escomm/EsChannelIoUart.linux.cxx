// Android superset of POSIX-specific UART channel methods implementation.
// NB! Do not include this file explicitly in project, it's got implicitly
// included in EsChannelIoUart class code.
//
//---------------------------------------------------------------------------

#define ES_USB_ROOT_PATH esT("/sys/bus/usb/devices")

class EsUsbDeviceEnumerator : public EsPathEnumerator
{
protected:
  enum {
    stageUsbDevsFind,
    stageUsbAttrsRead
  };

public:
  EsUsbDeviceEnumerator() :
  EsPathEnumerator(ES_USB_ROOT_PATH),
  m_stage(stageUsbDevsFind),
  m_rootPath(ES_USB_ROOT_PATH),
  m_re(esT("^[0-9\\.a-zA-Z\\-]+$"))
  {
    m_usbs = EsAssocContainer::create();
  }

  EsAssocContainerIntf::Ptr resultGet() const { return m_usbs; }

protected:
  EsVariant valueRead(const EsString& valPath, bool number, int radix = 0)
  {
    EsFile fval(
      valPath,
      static_cast<ulong>(EsFileFlag::Read)|
      static_cast<ulong>(EsFileFlag::Text)
    );

    if( fval.open() )
    {
      const EsString& sval = fval.stringRead();
      fval.close();

      if( number )
        return EsString::toULong(sval, radix);
      else
        return sval;
    }

    return EsVariant::null();
  }

  virtual void onStart()
  {
    if( stageUsbDevsFind == m_stage )
      m_usbs->clear();
  }

  virtual bool onObject(const EsString& curPath, const EsString& name, bool isDir)
  {
    if( stageUsbDevsFind == m_stage &&
        isDir &&
        esT(".") != name &&
        esT("..") != name
    )
    {
      m_re.set_text(name);
      if( m_re.get_matches() )
      {
        EsAssocContainerIntf::Ptr node = EsAssocContainer::create();
        node->newValueSet( esVT("name"), name );

        // Store new node under path to sys bus usb device
        m_usbs->newValueSet(
          curPath + name + esT("/"),
          node
        );
      }
    }
    else if( stageUsbAttrsRead == m_stage && !isDir )
    {
      if( m_usbs->keyExists( curPath ) )
      {
        EsAssocContainerIntf::Ptr node = m_usbs->valueGet(curPath).asExistingObject();
        ES_ASSERT(node);

        EsString valPath = curPath + name;

        if( esT("busnum") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              true,
              10
            )
          );
        }
        else if( esT("devnum") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              true,
              10
            )
          );
        }
        else if( esT("idProduct") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              true,
              16
            )
          );
        }
        else if( esT("idVendor") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              true,
              16
            )
          );
        }
        else if( esT("manufacturer") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              false
            )
          );
        }
        else if( esT("product") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              false
            )
          );
        }
        else if( esT("serial") == name )
        {
          node->newValueSet(
            name,
            valueRead(
              valPath,
              false
            )
          );
        }
      }
    }

    return true;
  }

  virtual void onEnd()
  {
    if( stageUsbDevsFind == m_stage )
    {
      m_stage = stageUsbAttrsRead;
      const EsVariant::Array& keys = m_usbs->allKeysGet().asVariantCollection();
      for(size_t idx = 0; idx < keys.size(); ++idx)
      {
        // Read USB values for each root device found
        m_curPath = EsPath::createFromPath(
          keys[idx].asString()
        );

        execute(); // Re-start in non-recursive mode for each found root USB device
      }
    }
  }

protected:
  ulong m_stage;
  EsString m_rootPath;
  EsRegEx m_re;
  EsAssocContainerIntf::Ptr m_usbs;
};
//---------------------------------------------------------------------------

void EsUartEnumerator::enumerate(bool busyPortsInclude /*= true*/)
{
  // Clear the output array
  m_infos.clear();

  EsUsbDeviceEnumerator enu;
  enu.execute();

  EsAssocContainerIntf::Ptr usbs = enu.resultGet();

  // Finalize enumeration by stuffing USB infos into UART infos
  //
  const EsVariant::Array& keys = usbs->allKeysGet().asVariantCollection();
  m_infos.reserve(keys.size());

  for(size_t idx = 0; idx < keys.size(); ++idx)
  {
    EsAssocContainerIntf::Ptr node = usbs->valueGet( keys[idx] ).asExistingObject();
    ES_ASSERT( node );

    EsUartInfo::Ptr info( new EsUartInfo );
    info->m_strDevPath = EsString::format(
      esT("/dev/bus/usb/%0.3d/%0.3d"),
      node->valueGet(esVT("busnum")).asULong(),
      node->valueGet(esVT("devnum")).asULong()
    );

    if( node->keyExists(esVT("product")) )
      info->m_strName = EsString::format(
        esT("%s (%s)"),
        node->valueGet(esVT("product")).asString(),
        node->valueGet(esVT("name")).asString()
      );
    else
      info->m_strName = node->valueGet(esVT("name")).asString();

    if(
      node->keyExists(esVT("product")) &&
      node->keyExists(esVT("manufacturer"))
    )
      info->m_strFriendlyName = EsString::format(
        esT("%s %s (%s)"),
        node->valueGet(esVT("manufacturer")).asString(),
        node->valueGet(esVT("product")).asString(),
        node->valueGet(esVT("name")).asString()
      );
    else
      info->m_strFriendlyName = info->m_strName;

    info->m_strInstPath = EsString::format(
      esT("USB\\VID_%0.4hX&PID_%0.4hX"),
      node->valueGet(esVT("idVendor")).asULong(),
      node->valueGet(esVT("idProduct")).asULong()
    );

    if( node->keyExists(esVT("serial")) )
      info->m_strInstPath += esT("\\") + node->valueGet(esVT("serial")).asString();

    info->m_isModem = false;
    info->m_isUSBdevice = true;

    m_infos.push_back( info );
  }
}
//---------------------------------------------------------------------------

EsUartEnumerator::EsUartAvail EsUartEnumerator::availabilityCheck(const EsUartInfo::Ptr& info)
{
  EsUartAvail result = uartAvailable;

  ES_ASSERT(info);

  // TODO: Is there check for port availability in xxxX systems?

  // Port is available
  return result;
}
//---------------------------------------------------------------------------
