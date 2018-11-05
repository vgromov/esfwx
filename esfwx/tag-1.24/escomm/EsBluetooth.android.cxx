// Android-specific bluetooth stuff
// NB! Do not include this file in project, it's got implicitly included from EsBluetooth.cpp
//
#include <Androidapi.Helpers.hpp>
#include <Androidapi.JNI.Bluetooth.hpp>

//---------------------------------------------------------------------------
// Common helpers
//
static void btdevsEnum(
  const _di_JBluetoothAdapter& bta,
  ulong flags,
  bool ES_UNUSED(doInquery),
  ulong ES_UNUSED(inquiryTmo),
  EsBluetoothDeviceInfos& devs,
  EsBluetoothDeviceEnumerator* This,
  bool (EsBluetoothDeviceEnumerator:: *pfnOnEnumerated)(const EsBluetoothDeviceInfo&)
  )
{
  _di_JSet set;
  if( bta )
  {
    bta->cancelDiscovery();
    set = bta->getBondedDevices();
  }

  if( set )
  {
    _di_JIterator it = set->iterator();

    EsDateTime lastSeen;
    EsDateTime lastUsed;

    while( it->hasNext() )
    {
      _di_ILocalObject obj = it->next();
      ES_ASSERT(obj);
      _di_JBluetoothDevice dev = TJBluetoothDevice::Wrap(obj->GetObjectID());
      ES_ASSERT(dev);

      _di_Jbluetooth_BluetoothClass devCls = dev->getBluetoothClass();
      ES_ASSERT(devCls);
      ulong cod = devCls->getDeviceClass();

      // Create device info record, and pass it to callback
      ulong flags = static_cast<ulong>(EsBluetoothDeviceFlag::None);
      if( TJBluetoothDevice::JavaClass->BOND_BONDED == dev->getBondState() )
        flags = static_cast<ulong>(EsBluetoothDeviceFlag::Unknown);

      EsBluetoothServiceUUIDs svcs;
      TJavaObjectArray__1<_di_JParcelUuid>* uuids = dev->getUuids();
      ES_ASSERT(uuids);
      int cnt = uuids->Length;
      for(int idx = 0; idx < cnt; ++idx )
      {
        JNIObject jniobj = uuids->GetRawItem(idx);
        _di_JParcelUuid juuid = TJParcelUuid::Wrap(jniobj);
        ES_ASSERT(juuid);
        svcs.push_back(
          EsBluetoothServiceUUID( JStringToString(juuid->toString()).c_str() )
        );
      }

      EsBluetoothDeviceInfo devInfo(
        flags,
        cod,
        JStringToString(dev->getAddress()).c_str(),
        JStringToString(dev->getName()).c_str(),
        lastSeen,
        lastUsed,
        svcs
      );

      if( This && pfnOnEnumerated )
      {
        if( !(This->*pfnOnEnumerated)( devInfo ) )
          break;
      }

      // Append device info to the collection
      devs.push_back( devInfo );
    }
  }
}

//---------------------------------------------------------------------------
// EsBluetoothRadioEnumerator implementation
//
void EsBluetoothRadioEnumerator::execute()
{
  reset();

  _di_JBluetoothAdapter bta = TJBluetoothAdapter::JavaClass->getDefaultAdapter();
  if( bta && bta->isEnabled() )
  {
    EsBluetoothDeviceInfos devs;
    btdevsEnum(
      bta,
      0,
      false,
      0,
      devs,
      0,
      0
    );

    EsBluetoothRadioInfo radInfo(
      JStringToString(bta->getAddress()).c_str(),
      JStringToString(bta->getName()).c_str(),
      EsBluetoothDeviceClass::UNKNOWN,
      0,
      0,
      bta->isEnabled(),
      devs
    );

    if( onRadioEnumerated( radInfo ) )
    {
      // Append device info to the collection
      m_infos.push_back( radInfo );
    }
  }
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Execute Bluetooth device enumeration
void EsBluetoothDeviceEnumerator::execute()
{
  reset();
  _di_JBluetoothAdapter bta = TJBluetoothAdapter::JavaClass->getDefaultAdapter();
  if( bta && bta->isEnabled() )
  {
      btdevsEnum(
        bta,
        m_flags,
        m_doInquiry,
        m_inquiryTmo,
        m_infos,
        this,
        &EsBluetoothDeviceEnumerator::onDeviceEnumerated
      );
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Bluetooth global services

/// Global initialization
void EsBluetooth::initialize()
{
}
//---------------------------------------------------------------------------

/// Global initialization check
bool EsBluetooth::initialized()
{
  return true;
}
//---------------------------------------------------------------------------

/// Global uninitialization
void EsBluetooth::uninitialize()
{
}
//---------------------------------------------------------------------------


