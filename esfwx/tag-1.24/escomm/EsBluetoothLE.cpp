#include "escommpch.h"
#pragma hdrstop

#include "EsBluetoothLE.h"
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_BLUETOOTH_LE

// Platform - independent BLE classes implementation part
//
//---------------------------------------------------------------------------

// BLE descriptor
//

#define ES_BLE_DESCRIPTOR_PROPERTY_S_IS_NOT_VALID_FOR_KIND_S \
  _("Descriptor property '%s' is not valid for descriptor kind '%s'")

// Known descriptors database, indexed by descriptor kinds
#define ES_BLE_DESCRIPTOR_BEGIN struct EsBluetoothLEknownDescriptor { \
  EsString::const_pointer m_descr; \
  ulong m_uuid; }; \
  static const EsBluetoothLEknownDescriptor sc_knownDescrs[] = {
#define ES_BLE_DESCRIPTOR_ENTRY(Name, Descr, Uuid) { Descr, Uuid },
#define ES_BLE_DESCRIPTOR_END { 0, 0 } };
#include "EsBluetoothLEdescriptor.defs.h"
//---------------------------------------------------------------------------

static int knownDescriptorRecordIdxFind(ulong uuid)
{
  int idx = 0;
  while(1)
  {
    const EsBluetoothLEknownDescriptor& dcr = sc_knownDescrs[idx];

    if( !dcr.m_descr )
      break;

    if( dcr.m_uuid == uuid )
      return idx;

    ++idx;
  }

  return -1;
}
//---------------------------------------------------------------------------

static int knownDescriptorRecordIdxFind(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return knownDescriptorRecordIdxFind(uuid.Data1);

  return -1;
}
//---------------------------------------------------------------------------

EsBluetoothLEdescriptor::EsBluetoothLEdescriptor() :
m_chx(0),
m_kind(kindUnknown),
m_uuid(GUID_NULL)
{}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::isKnownUUID(ulong uuid)
{
  return -1 != knownDescriptorRecordIdxFind(uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptor::isKnownUUID(const GUID& uuid)
{
  return -1 != knownDescriptorRecordIdxFind(uuid);
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByKind(EsBluetoothLEdescriptor::Kind kind)
{
  size_t idx = static_cast<size_t>(kind);
  if(idx < static_cast<size_t>(EsBluetoothLEdescriptor::kindUnknown))
    return EsStringI18n::translate(sc_knownDescrs[idx].m_descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByUUID(ulong uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return EsStringI18n::translate(sc_knownDescrs[idx].m_descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEdescriptor::kindNameGetByUUID(const GUID& uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return EsStringI18n::translate(sc_knownDescrs[idx].m_descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsBluetoothLEdescriptor::Kind EsBluetoothLEdescriptor::kindGetByUUID(ulong uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return static_cast<EsBluetoothLEdescriptor::Kind>(idx);

  return EsBluetoothLEdescriptor::kindUnknown;
}
//---------------------------------------------------------------------------

EsBluetoothLEdescriptor::Kind EsBluetoothLEdescriptor::kindGetByUUID(const GUID& uuid)
{
  int idx = knownDescriptorRecordIdxFind(uuid);
  if( -1 < idx )
    return static_cast<EsBluetoothLEdescriptor::Kind>(idx);

  return EsBluetoothLEdescriptor::kindUnknown;
}
//---------------------------------------------------------------------------

GUID EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptor::Kind kind)
{
  size_t idx = static_cast<size_t>(kind);
  if(idx < static_cast<size_t>(EsBluetoothLEdescriptor::kindUnknown))
    return EsBluetoothUUID::fromShortened(
      sc_knownDescrs[idx].m_uuid
    );

  return GUID_NULL;
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptor::throwInvalidPropForKind(const EsString& prop, EsBluetoothLEdescriptor::Kind kind)
{
  EsException::Throw( 
    ES_BLE_DESCRIPTOR_PROPERTY_S_IS_NOT_VALID_FOR_KIND_S,
    prop,
    kindNameGetByKind(kind)
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Common characteristic implementation
//
#define ES_BLE_CHARACTERISTIC_BEGIN
#define ES_BLE_CHARACTERISTIC_ENTRY(Uuid, Name) m_data[Uuid] = Name;
#define ES_BLE_CHARACTERISTIC_END
class EsKnownChxMap
{
public:
  typedef std::map<ulong, EsString> DataT;

public:
  EsKnownChxMap()
  {
#include "EsBluetoothLEcharacteristic.defs.h"
  }

  bool has(ulong uuid) const
  {
    return m_data.end() != m_data.find(uuid);
  }

  const EsString& descrGet(ulong uuid) const
  {
    DataT::const_iterator cit = m_data.find(uuid);
    if( cit != m_data.end() )
      return (*cit).second;

    return EsString::null();
  }

  static const EsKnownChxMap& instGet()
  {
    static EsKnownChxMap s_cm;
    return s_cm;
  }

protected:
  DataT m_data;
};
//---------------------------------------------------------------------------

EsBluetoothLEcharacteristic::EsBluetoothLEcharacteristic() :
m_svc(0),
m_props(0),
m_uuid(GUID_NULL)
{}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::nameGet() const
{
  if( isKnown() )
    return knownNameGetByUUID(m_uuid);
  else
  {
    for( size_t idx = 0; idx < m_dcrs.size(); ++idx )
    {
      EsBluetoothLEdescriptor::Ptr dcr = m_dcrs[idx];
      if( EsBluetoothLEdescriptor::kindUserDescription == dcr->kindGet() )
        return dcr->userDescriptionGet();
    }
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsBluetoothLEdescriptor::Ptr EsBluetoothLEcharacteristic::descriptorFind(const GUID& uuid) const
{
  for( size_t idx = 0; idx < m_dcrs.size(); ++idx )
  {
    EsBluetoothLEdescriptor::Ptr dcr = m_dcrs[idx];
    ES_ASSERT(dcr);

    if( uuid == dcr->uuidGet() )
      return dcr;
  }

  return EsBluetoothLEdescriptor::Ptr();
}
//---------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::isKnownUUID(ulong uuid)
{
  return EsKnownChxMap::instGet().has(uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEcharacteristic::isKnownUUID(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return isKnownUUID(uuid.Data1);

  return false;
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::knownNameGetByUUID(ulong uuid)
{
  const EsString& descr = EsKnownChxMap::instGet().descrGet(uuid);
  if( !descr.empty() )
    return EsStringI18n::translate(descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::knownNameGetByUUID(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return knownNameGetByUUID(uuid.Data1);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEcharacteristic::propertiesStringGet(ulong props)
{
  EsString result;

  if( props & EsBluetoothLEcharacteristic::propBroadcast )
    result += _("Broadcastable");

  if( props & EsBluetoothLEcharacteristic::propExtendedProps )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Have extended properties");
  }

  if( props & EsBluetoothLEcharacteristic::propNotify )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Notification");
  }

  if( props & EsBluetoothLEcharacteristic::propIndicate )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Indication");
  }

  if( props & EsBluetoothLEcharacteristic::propRead )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Read");
  }

  if( props & EsBluetoothLEcharacteristic::propWrite )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Write");
  }

  if( props & EsBluetoothLEcharacteristic::propWriteNoResponse )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Write without response");
  }

  if( props & EsBluetoothLEcharacteristic::propSignedWrite )
  {
    if( !result.empty() )
      result += esT(";");
    result += _("Signed write");
  }

  return result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// LE Service base implementation
//
#define ES_BLE_SERVICE_BEGIN
#define ES_BLE_SERVICE_ENTRY(Uuid, Name) m_data[Uuid] = Name;
#define ES_BLE_SERVICE_END
class EsKnownSvcMap
{
public:
  typedef std::map<ulong, EsString> DataT;

public:
  EsKnownSvcMap()
  {
#include "EsBluetoothLEservice.defs.h"
  }

  bool has(ulong uuid) const
  {
    return m_data.end() != m_data.find(uuid);
  }

  const EsString& descrGet(ulong uuid) const
  {
    DataT::const_iterator cit = m_data.find(uuid);
    if( cit != m_data.end() )
      return (*cit).second;

    return EsString::null();
  }

  static const EsKnownSvcMap& instGet()
  {
    static EsKnownSvcMap s_sm;
    return s_sm;
  }

protected:
  DataT m_data;
};

//---------------------------------------------------------------------------

EsBluetoothLEservice::EsBluetoothLEservice() :
m_dev(nullptr),
m_parent(nullptr),
m_uuid(GUID_NULL)
{
}
//---------------------------------------------------------------------------

EsBluetoothLEcharacteristic::Ptr EsBluetoothLEservice::characteristicFind(const GUID& uuid) const
{
  for(size_t idx = 0; idx < m_chxs.size(); ++idx)
  {
    EsBluetoothLEcharacteristic::Ptr chx = m_chxs[idx];
    ES_ASSERT(chx);

    if( uuid == chx->uuidGet() )
      return chx;
  }

  return EsBluetoothLEcharacteristic::Ptr();
}
//---------------------------------------------------------------------------

bool EsBluetoothLEservice::isKnownUUID(ulong uuid)
{
  return EsKnownSvcMap::instGet().has(uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEservice::isKnownUUID(const GUID& uuid)
{
  return EsBluetoothUUID::isOk(uuid) &&
    EsKnownSvcMap::instGet().has(uuid.Data1);
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEservice::knownNameGetByUUID(ulong uuid)
{
  const EsString& descr = EsKnownSvcMap::instGet().descrGet(uuid);
  if( !descr.empty() )
    return EsStringI18n::translate(descr);

  return _("Unknown");
}
//---------------------------------------------------------------------------

EsString EsBluetoothLEservice::knownNameGetByUUID(const GUID& uuid)
{
  if( EsBluetoothUUID::isOk(uuid) )
    return knownNameGetByUUID(uuid.Data1);

  return _("Unknown");
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// BT LE device base implemetation
//
EsBluetoothLEdevice::EsBluetoothLEdevice( EsBluetoothRadio* radio, const EsString& addr, const EsString& name ) :
m_radio(radio),
m_addr(addr),
m_name(name),
m_rssi(0)
{}
//---------------------------------------------------------------------------

EsBluetoothLEservice::Ptr EsBluetoothLEdevice::serviceFind(const GUID& uuid) const
{
  for(size_t idx = 0; idx < m_svcs.size(); ++idx)
  {
    EsBluetoothLEservice::Ptr svc = m_svcs[idx];
    ES_ASSERT(svc);

    if( uuid == svc->uuidGet() )
      return svc;
  }

  return EsBluetoothLEservice::Ptr();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// BT LE devices enumerator
//
EsBluetoothLEdeviceEnumerator::EsBluetoothLEdeviceEnumerator(
  const EsBluetoothRadio::Ptr& radio,
  bool doInquiry,
  ulong inquiryTmo
) :
m_radio(radio),
m_doInquiry(doInquiry),
m_inquiryTmo(inquiryTmo)
{
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdeviceEnumerator::onDeviceEnumerated(const EsBluetoothLEdevice::Ptr& info)
{
  ES_ASSERT(info);
  m_devs.push_back( info );

  return true;
}
//---------------------------------------------------------------------------

EsBluetoothLEdevice::Ptr EsBluetoothLEdeviceEnumerator::deviceFindByName(const EsString& name) const
{
  for(size_t idx = 0; idx < m_devs.size(); ++idx)
  {
    EsBluetoothLEdevice::Ptr dev = m_devs[idx];
    ES_ASSERT(dev);

    if( name == dev->nameGet() )
      return dev;
  }

  return EsBluetoothLEdevice::Ptr();
}
//---------------------------------------------------------------------------

EsBluetoothLEdevice::Ptr EsBluetoothLEdeviceEnumerator::deviceFindByAddress(const EsString& address) const
{
  for(size_t idx = 0; idx < m_devs.size(); ++idx)
  {
    EsBluetoothLEdevice::Ptr dev = m_devs[idx];
    ES_ASSERT(dev);

    if( address == dev->addrGet() )
      return dev;
  }

  return EsBluetoothLEdevice::Ptr();
}
//---------------------------------------------------------------------------

// Platform-specific BLE classes implementations
//
#if ES_OS_WINDOWS == ES_OS
# include "EsBluetoothLE.win.cxx"
#elif ES_OS_ANDROID == ES_OS
# include "EsBluetoothLE.android.cxx"
#endif

//---------------------------------------------------------------------------

EsString EsBluetoothLE::resultCodeStringGet(EsBluetoothLEresult code)
{
  switch(code)
  {
  case EsBluetoothLEresult::gattSuccess:
    return EsString::null();
  case EsBluetoothLEresult::gattReadNotPermitted:
    return _("Read not permitted");
  case EsBluetoothLEresult::gattWriteNotPermitted:
    return _("Write not permitted");
  case EsBluetoothLEresult::gattInsufficientAutentication:
    return _("Insufficient authentication");
  case EsBluetoothLEresult::gattRequestNotSupported:
    return _("Request not supported");
  case EsBluetoothLEresult::gattInvalidOffset:
    return _("Invalid offset");
  case EsBluetoothLEresult::gattInvalidAttributeLength:
    return _("Invalid attribute length");
  case EsBluetoothLEresult::gattInsufficientEncryption:
    return _("Insufficient encryption");
  case EsBluetoothLEresult::gattFailure:
    return _("Generic failure");
  }

  return _("Unknown failure");
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_BLUETOOTH_LE


