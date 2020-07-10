#include "escommpch.h"
#pragma hdrstop

#include "EsBluetooth.h"
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_BLUETOOTH
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN( EsBluetoothDeviceClass, esT( "Bluetooth DeviceClass" ) )
  // Properties
  ES_DECL_PROP_INFO(                        EsBluetoothDeviceClass, service, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                        EsBluetoothDeviceClass, majorDeviceClass, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO(                        EsBluetoothDeviceClass, minorDeviceClass, ulong, NO_PROPERTY_LABEL, NO_DEFAULT_VAL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothDeviceClass, format, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothDeviceClass, serviceString, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothDeviceClass, majorDeviceClassString, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothDeviceClass, minorDeviceClassString, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )
  ES_DECL_PROP_INFO_RO(                     EsBluetoothDeviceClass, code, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR )

  // Constructor
  ES_DECL_REFLECTED_CTOR_INFO(              EsBluetoothDeviceClass, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR )

  // Services
  ES_DECL_REFLECTED_METHOD_INFO_STD(        EsBluetoothDeviceClass, asString, EsString_CallConst, NO_METHOD_DESCR)

  // Static services
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, computerClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, phoneClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, lanClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, audioClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, peripheralClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, imagingClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, wearableClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, toyClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(  EsBluetoothDeviceClass, healthClassStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR )

ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------

// EsBluetoothDeviceClass implementation
//
EsBluetoothDeviceClass::EsBluetoothDeviceClass(ulong code /*= EsBluetoothDeviceClass::UNKNOWN*/) :
m_code(code)
{}
//---------------------------------------------------------------------------

EsVariant EsBluetoothDeviceClass::NEW( cr_EsVariant code )
{
  std::unique_ptr<EsBluetoothDeviceClass> ptr = ES_MAKE_UNIQUE(
    EsBluetoothDeviceClass,
    code.asULong()
  );
  ES_ASSERT(ptr);

  ptr->m_dynamic = true;

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

ulong EsBluetoothDeviceClass::get_code() const
{
  return m_code;
}
//--------------------------------------------------------------------------------

ulong EsBluetoothDeviceClass::get_service() const
{ 
  return ( m_code & SERVICE_MASK ) >> SERVICE_BIT_OFFSET; 
}
//--------------------------------------------------------------------------------

void EsBluetoothDeviceClass::set_service(const ulong& svc) 
{ 
  m_code &= ~SERVICE_MASK; 
  m_code |= ( svc << SERVICE_BIT_OFFSET ) & SERVICE_MASK; 
}
//--------------------------------------------------------------------------------

ulong EsBluetoothDeviceClass::get_majorDeviceClass() const 
{ 
  return ( m_code & MAJOR_MASK ) >> MAJOR_BIT_OFFSET; 
}
//--------------------------------------------------------------------------------

void EsBluetoothDeviceClass::set_majorDeviceClass( const ulong& mdc ) 
{ 
  m_code &= ~MAJOR_MASK; 
  m_code |= ( mdc << MAJOR_BIT_OFFSET ) & MAJOR_MASK; 
}
//--------------------------------------------------------------------------------

ulong EsBluetoothDeviceClass::get_minorDeviceClass() const 
{ 
  return ( m_code & MINOR_MASK ) >> MINOR_BIT_OFFSET; 
}
//--------------------------------------------------------------------------------

void EsBluetoothDeviceClass::set_minorDeviceClass( const ulong& mdc )
{ 
  m_code &= ~MINOR_MASK; 
  m_code |= ( mdc << MINOR_BIT_OFFSET ) & MINOR_MASK; 
}
//--------------------------------------------------------------------------------

ulong EsBluetoothDeviceClass::get_format() const
{ 
  return ( m_code & FORMAT_MASK ) >> FORMAT_BIT_OFFSET; 
}
//--------------------------------------------------------------------------------

static inline void resultAppend(EsString& result, const EsString& sep, const EsString& addendum)
{
  if( !result.empty() )
    result += sep;

  result += addendum;
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::get_serviceString() const
{
  EsString result;
  ulong tmp = get_service();
  if( tmp & SERVICE_LIMITED )
    resultAppend(result, esT("; "), _("Limited discoverable"));
  if( tmp & SERVICE_POSITIONING )
    resultAppend(result, esT("; "), _("Positioning"));
  if( tmp & SERVICE_NETWORKING )
    resultAppend(result, esT("; "), _("Networking"));
  if( tmp & SERVICE_RENDERING )
    resultAppend(result, esT("; "), _("Rendering"));
  if( tmp & SERVICE_CAPTURING )
    resultAppend(result, esT("; "), _("Capturing"));
  if( tmp & SERVICE_OBJECT_XFER )
    resultAppend(result, esT("; "), _("Object transfer"));
  if( tmp & SERVICE_AUDIO )
    resultAppend(result, esT("; "), _("Audio"));
  if( tmp & SERVICE_TELEPHONY )
    resultAppend(result, esT("; "), _("Telephony"));
  if( tmp & SERVICE_INFORMATION )
    resultAppend(result, esT("; "), _("Information"));

  return result;
}
//---------------------------------------------------------------------------

/// Get major device code string representation
EsString EsBluetoothDeviceClass::get_majorDeviceClassString() const
{
  switch( get_majorDeviceClass() )
  {
  case MAJOR_COMPUTER:
    return _("Computer");
  case MAJOR_PHONE:
    return _("Phone");
  case MAJOR_LAN_ACCESS:
    return _("LAN access");
  case MAJOR_AUDIO:
    return _("Audio");
  case MAJOR_PERIPHERAL:
    return _("Peripheral");
  case MAJOR_IMAGING:
    return _("Imaging");
  case MAJOR_WEARABLE:
    return _("Wearable");
  case MAJOR_TOY:
    return _("Toy");
  case MAJOR_HEALTH:
    return _("Health");
  case MAJOR_MISCELLANEOUS:
  case MAJOR_UNCLASSIFIED:
    return _("Unclassified");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::computerClassStringGet( cr_EsVariant minor)
{
  switch( minor.asULong() )
  {
  case COMPUTER_MINOR_UNCLASSIFIED:
    return _("Unclassified");
  case COMPUTER_MINOR_DESKTOP:
    return _("Desktop");
  case COMPUTER_MINOR_SERVER:
    return _("Server");
  case COMPUTER_MINOR_LAPTOP:
    return _("Laptop");
  case COMPUTER_MINOR_HANDHELD:
    return _("Handheld");
  case COMPUTER_MINOR_PALM:
    return _("Palmtop");
  case COMPUTER_MINOR_WEARABLE:
    return _("Wearable");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::phoneClassStringGet( cr_EsVariant minor)
{
  switch( minor.asULong() )
  {
  case PHONE_MINOR_UNCLASSIFIED:
    return _("Unclassified");
  case PHONE_MINOR_CELLULAR:
    return _("Cellular phone");
  case PHONE_MINOR_CORDLESS:
    return _("Cordless phone");
  case PHONE_MINOR_SMART:
    return _("Smartphone");
  case PHONE_MINOR_WIRED_MODEM:
    return _("Wired modem or voice gateway");
  case PHONE_MINOR_COMMON_ISDN:
    return _("Common ISDN Access");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::lanClassStringGet( cr_EsVariant minor)
{
  EsString result;
  switch( (minor.asULong() & LAN_ACCESS_MASK) >> LAN_ACCESS_BIT_OFFSET )
  {
  case LAN_ACCESS_0_USED:
    result = _("Fully available");
    break;
  case LAN_ACCESS_17_USED:
    result = _("1-17% utilized");
    break;
  case LAN_ACCESS_33_USED:
    result = _("17-33% utilized");
    break;
  case LAN_ACCESS_50_USED:
    result = _("33-50% utilized");
    break;
  case LAN_ACCESS_67_USED:
    result = _("50-67% utilized");
    break;
  case LAN_ACCESS_83_USED:
    result = _("67-83% utilized");
    break;
  case LAN_ACCESS_99_USED:
    result = _("83-99% utilized");
    break;
  case LAN_ACCESS_FULL:
    result = _("No service available");
    break;
  }

  if( result.empty() )
    result = _("Uncategorized");

  return result;
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::audioClassStringGet( cr_EsVariant minor)
{
  switch( minor.asULong() )
  {
  case AUDIO_MINOR_UNCLASSIFIED:
    return _("Unclassified");
  case AUDIO_MINOR_HEADSET:
    return _("Headset");
  case AUDIO_MINOR_HANDS_FREE:
    return _("Hands-free");
  case AUDIO_MINOR_HEADSET_HANDS_FREE:
    return _("Headset hands-free");
  case AUDIO_MINOR_MICROPHONE:
    return _("Microphone");
  case AUDIO_MINOR_LOUDSPEAKER:
    return _("Loudspeaker");
  case AUDIO_MINOR_HEADPHONES:
    return _("Headphones");
  case AUDIO_MINOR_PORTABLE_AUDIO:
    return _("Portable audio");
  case AUDIO_MINOR_CAR_AUDIO:
    return _("Car audio");
  case AUDIO_MINOR_SET_TOP_BOX:
    return _("Set top box");
  case AUDIO_MINOR_HIFI_AUDIO:
    return _("HiFi audio");
  case AUDIO_MINOR_VCR:
    return _("VCR");
  case AUDIO_MINOR_VIDEO_CAMERA:
    return _("Video camera");
  case AUDIO_MINOR_CAMCORDER:
    return _("Camcorder");
  case AUDIO_MINOR_VIDEO_MONITOR:
    return _("Video monitor");
  case AUDIO_MINOR_VIDEO_DISPLAY_LOUDSPEAKER:
    return _("Video display and loudspeaker");
  case AUDIO_MINOR_VIDEO_DISPLAY_CONFERENCING:
    return _("Video display conferencing");
  //case AUDIO_MINOR_RESERVED:
  case AUDIO_MINOR_GAMING_TOY:
    return _("Gaming/Toy");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::peripheralClassStringGet( cr_EsVariant minor)
{
  EsString caps;

  ulong tmp = minor.asULong();
  if( tmp & PERIPHERAL_MINOR_KEYBOARD_MASK )
    caps = _("Keyboard");
  if( tmp & PERIPHERAL_MINOR_POINTER_MASK )
    resultAppend(caps, esT("; "), _("Pointing device"));

  if( !caps.empty() )
    caps = esT("[") + caps + esT("]");

  EsString result;
  switch( tmp & PERIPHERAL_MINOR_MASK_ALL)
  {
  case PERIPHERAL_MINOR_NO_CATEGORY:
    result = _("Unclassified");
    break;
  case PERIPHERAL_MINOR_JOYSTICK:
    result = _("Joystick");
    break;
  case PERIPHERAL_MINOR_GAMEPAD:
    result = _("Gamepad");
    break;
  case PERIPHERAL_MINOR_REMOTE_CONTROL:
    result = _("Remote control");
    break;
  case PERIPHERAL_MINOR_SENSING:
    result = _("Sensing device");
    break;
  case PERIPHERAL_MINOR_DIGITIZER:
    result = _("Digitizer tablet");
    break;
  case PERIPHERAL_MINOR_CARD_READER:
    result = _("Card Reader");
    break;
  }

  if( !result.empty() )
    resultAppend(caps, esT(" "), result);

  return caps;
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::imagingClassStringGet( cr_EsVariant minor)
{
  EsString result;
  ulong tmp = minor.asULong();

  if(tmp & IMAGING_MINOR_DISPLAY_MASK)
    resultAppend(result, esT("/"), _("Display"));
  if(tmp & IMAGING_MINOR_CAMERA_MASK)
    resultAppend(result, esT("/"), _("Camera"));
  if(tmp & IMAGING_MINOR_SCANNER_MASK)
    resultAppend(result, esT("/"), _("Scanner"));
  if(tmp & IMAGING_MINOR_PRINTER_MASK)
    resultAppend(result, esT("/"), _("Printer"));

  return result;
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::wearableClassStringGet(cr_EsVariant minor)
{
  switch(minor.asULong())
  {
  case WEARABLE_MINOR_WRIST_WATCH:
    return _("Wrist watch");
  case WEARABLE_MINOR_PAGER:
    return _("Pager");
  case WEARABLE_MINOR_JACKET:
    return _("Jacket");
  case WEARABLE_MINOR_HELMET:
    return _("Helmet");
  case WEARABLE_MINOR_GLASSES:
    return _("Glasses");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::toyClassStringGet(cr_EsVariant minor)
{
  switch(minor.asULong())
  {
  case TOY_MINOR_ROBOT:
    return _("Robot");
  case TOY_MINOR_VEHICLE:
    return _("Vehicle");
  case TOY_MINOR_DOLL:
    return _("Doll/Action figure");
  case TOY_MINOR_CONTROLLER:
    return _("Controller");
  case TOY_MINOR_GAME:
    return _("Game");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::healthClassStringGet(cr_EsVariant minor)
{
  switch(minor.asULong())
  {
  case HEALTH_MINOR_UNCLASSIFIED:
    return _("Unclassified");
  case HEALTH_MINOR_BLOOD_PRESSURE:
    return _("Blood pressure monitor");
  case HEALTH_MINOR_THERMOMETER:
    return _("Thermometer");
  case HEALTH_MINOR_WEIGHTING_SCALE:
    return _("Weighting scale");
  case HEALTH_MINOR_GLUCOSE_METER:
    return _("Glucose meter");
  case HEALTH_MINOR_PULSE_OXIMETER:
    return _("Pulse Oximeter");
  case HEALTH_MINOR_PULSE_RATE:
    return _("Heart/Pulse rate monitor");
  case HEALTH_MINOR_DATA_DISPLAY:
    return _("Health data display");
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

/// Get minor device code string representation
EsString EsBluetoothDeviceClass::get_minorDeviceClassString() const
{
  ulong minor = get_minorDeviceClass();
  switch( get_majorDeviceClass() )
  {
  case MAJOR_COMPUTER:
    return computerClassStringGet(minor);
  case MAJOR_PHONE:
    return phoneClassStringGet(minor);
  case MAJOR_LAN_ACCESS:
    return lanClassStringGet(minor);
  case MAJOR_AUDIO:
    return audioClassStringGet(minor);
  case MAJOR_PERIPHERAL:
    return peripheralClassStringGet(minor);
  case MAJOR_IMAGING:
    return imagingClassStringGet(minor);
  case MAJOR_WEARABLE:
    return wearableClassStringGet(minor);
  case MAJOR_TOY:
    return toyClassStringGet(minor);
  case MAJOR_HEALTH:
    return healthClassStringGet(minor);
  }

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothDeviceClass::asString() const
{
  if( EsBluetoothDeviceClass::UNKNOWN == m_code )
    return _("Unknown device");

  /// Get major device class string
  EsString result = _("Device class: ") + get_majorDeviceClassString();
  /// Get minor device class code string
  result += _("\nDevice details: ") + get_minorDeviceClassString();
  /// Get device service string
  result += _("\nDevice services: ") + get_serviceString();

  return result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Bluetooth UUID helpers

const GUID& EsBluetoothUUID::base()
{
  static const GUID sc_base = {0x00000000, 0x0000, 0x1000, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};
  return sc_base;
}
//---------------------------------------------------------------------------

GUID EsBluetoothUUID::fromShortened(ulong uuid)
{
  GUID fuuid = base();
  fuuid.Data1 = uuid;

  return fuuid;
}
//---------------------------------------------------------------------------

bool EsBluetoothUUID::isOk(const GUID& uuid)
{
  const GUID& _base = base();
  return uuid.Data2 == _base.Data2 &&
    uuid.Data3 == _base.Data3 &&
    0 == memcmp(uuid.Data4, _base.Data4, 8);
}

//---------------------------------------------------------------------------
// EsBluetoothServiceUUID implementation
//
EsBluetoothServiceUUID::EsBluetoothServiceUUID(const GUID& uuid /*= GUID_NULL*/) :
m_uuid(uuid)
{}
//---------------------------------------------------------------------------

EsBluetoothServiceUUID::EsBluetoothServiceUUID(ulong uuid) :
m_uuid(EsBluetoothUUID::fromShortened(uuid))
{
}
//---------------------------------------------------------------------------

EsBluetoothServiceUUID::EsBluetoothServiceUUID(const EsString& uuid) :
m_uuid(GUID_NULL)
{
  m_uuid = EsUtilities::GUIDfromStr(uuid);
}
//---------------------------------------------------------------------------

/// UUID is protocol
bool EsBluetoothServiceUUID::isProtocol() const
{
  return isOk() &&
    0 < m_uuid.Data1 &&
    static_cast<ulong>(UUID16::L2CAP_PROTOCOL) >= m_uuid.Data1;
}
//---------------------------------------------------------------------------

/// UUID is service class or profile
bool EsBluetoothServiceUUID::isService() const
{
  return isOk() &&
    static_cast<ulong>(UUID16::ServiceDiscoveryServer) <= m_uuid.Data1 &&
    static_cast<ulong>(UUID16::HDP_Sink) >= m_uuid.Data1;
}
//---------------------------------------------------------------------------

/// Return string representation of UUID
EsString EsBluetoothServiceUUID::asString(bool canonical /*= false*/) const
{
  return EsUtilities::GUIDtoStr(m_uuid, canonical);
}
//---------------------------------------------------------------------------

/// UUID is service class
bool EsBluetoothServiceUUID::isServiceClass() const
{
  if( isService() )
  {
#define UUID16_MAP_BEGIN switch(m_uuid.Data1) {
#define UUID16_MAP_PROT_BEGIN
#define UUID16_MAP_PROT_ENTRY(sym, val, str)
#define UUID16_MAP_PROT_END
#define UUID16_MAP_PSVC_BEGIN
#define UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrv, isProf) case static_cast<ulong>(UUID16::sym): return (isSrv);
#define UUID16_MAP_PSVC_END
#define UUID16_MAP_END }
#include "EsBluetooth.UUID16defs.h"
  }

  return false;
}
//---------------------------------------------------------------------------

/// UUID is service profile
bool EsBluetoothServiceUUID::isServiceProfile() const
{
  if( isService() )
  {
#define UUID16_MAP_BEGIN switch(m_uuid.Data1) {
#define UUID16_MAP_PROT_BEGIN
#define UUID16_MAP_PROT_ENTRY(sym, val, str)
#define UUID16_MAP_PROT_END
#define UUID16_MAP_PSVC_BEGIN
#define UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrv, isProf) case static_cast<ulong>(UUID16::sym): return (isProf);
#define UUID16_MAP_PSVC_END
#define UUID16_MAP_END }
#include "EsBluetooth.UUID16defs.h"
  }

  return false;
}
//---------------------------------------------------------------------------

EsString EsBluetoothServiceUUID::protocolStringGet( ulong uuid )
{
#define UUID16_MAP_BEGIN switch(uuid) {
#define UUID16_MAP_PROT_BEGIN
#define UUID16_MAP_PROT_ENTRY(sym, val, str) case static_cast<ulong>(UUID16::sym): return str;
#define UUID16_MAP_PROT_END
#define UUID16_MAP_PSVC_BEGIN
#define UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrv, isProf)
#define UUID16_MAP_PSVC_END
#define UUID16_MAP_END }
#include "EsBluetooth.UUID16defs.h"

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetoothServiceUUID::serviceClassStringGet( ulong uuid )
{
#define UUID16_MAP_BEGIN switch(uuid) {
#define UUID16_MAP_PROT_BEGIN
#define UUID16_MAP_PROT_ENTRY(sym, val, str)
#define UUID16_MAP_PROT_END
#define UUID16_MAP_PSVC_BEGIN
#define UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrv, isProf) case static_cast<ulong>(UUID16::sym): return str;
#define UUID16_MAP_PSVC_END
#define UUID16_MAP_END }
#include "EsBluetooth.UUID16defs.h"

  return EsString::null();
}
//---------------------------------------------------------------------------

/// Find protocol|service UUID by its name. If not found - return 0
ulong EsBluetoothServiceUUID::uuidFindByName( const EsString& svcName )
{
#define UUID16_MAP_BEGIN
#define UUID16_MAP_PROT_BEGIN
#define UUID16_MAP_PROT_ENTRY(sym, val, str) if(svcName == str) return val; else
#define UUID16_MAP_PROT_END
#define UUID16_MAP_PSVC_BEGIN
#define UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrv, isProf) if(svcName == str) return val; else
#define UUID16_MAP_PSVC_END
#define UUID16_MAP_END
#include "EsBluetooth.UUID16defs.h"

  return 0;
}
//---------------------------------------------------------------------------

/// Return name of service UUID
EsString EsBluetoothServiceUUID::nameGet() const
{
  if( isProtocol() )
    return protocolStringGet( m_uuid.Data1 );
  else if( isService() )
    return serviceClassStringGet( m_uuid.Data1 );

  return EsString::null();
}
//---------------------------------------------------------------------------

/// Equality check operators
bool EsBluetoothServiceUUID::operator ==(const EsBluetoothServiceUUID& other) const
{
  return 0 == memcmp(&m_uuid, &other.m_uuid, sizeof(GUID));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

ES_DECL_BASE_CLASS_INFO_BEGIN(EsBluetoothRadio, esT("Bluetooth Radio class"))
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, isOk, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, enabled, bool, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, deviceClass, EsVariant, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, subversion, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, manufacturer, ulong, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, manufacturerString, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, address, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RO(EsBluetoothRadio, name, EsString, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
ES_DECL_CLASS_INFO_END
//--------------------------------------------------------------------------------
// EsBluetoothRadio class implementation
//
EsBluetoothRadio::EsBluetoothRadio(
  const EsString& addr,
  const EsString& name,
  ulong cod,
  esU16 sub,
  esU16 mfg,
  bool enabled
) :
m_class(cod),
m_subversion(sub),
m_mfg(mfg),
m_addr(addr),
m_name(name),
m_enabled(enabled)
{
  m_dynamic = true;
}
//---------------------------------------------------------------------------

bool EsBluetoothRadio::get_isOk() const
{
  return !m_addr.empty();
}
//---------------------------------------------------------------------------

bool EsBluetoothRadio::get_enabled() const
{
  return m_enabled;
}
//--------------------------------------------------------------------------------

EsVariant EsBluetoothRadio::get_deviceClass() const
{
  return EsBluetoothDeviceClass::NEW(
    m_class.get_code()
  );
}
//--------------------------------------------------------------------------------

ulong EsBluetoothRadio::get_subversion() const
{
  return m_subversion;
}
//--------------------------------------------------------------------------------

ulong EsBluetoothRadio::get_manufacturer() const
{
  return m_mfg;
}
//--------------------------------------------------------------------------------

EsString EsBluetoothRadio::get_manufacturerString() const
{
  return EsBluetooth::mfgStringGet(m_mfg);
}
//--------------------------------------------------------------------------------

EsString EsBluetoothRadio::get_address() const
{
  return m_addr;
}
//--------------------------------------------------------------------------------

EsString EsBluetoothRadio::get_name() const
{
  return m_name;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// EsBluetoothDevice implementation
//
EsBluetoothDevice::EsBluetoothDevice(
  ulong flags,
  ulong cod,
  const EsString& addr,
  const EsString& name,
  const EsDateTime& lastSeen,
  const EsDateTime& lastUsed,
  const EsBluetoothServiceUUIDs& services
) :
m_flags(flags),
m_class(cod),
m_addr(addr),
m_name(name),
m_lastSeen(lastSeen),
m_lastUsed(lastUsed),
m_services(services)
{
}
//---------------------------------------------------------------------------

bool EsBluetoothDevice::isOk() const
{
  return !(m_flags & static_cast<ulong>(EsBluetoothDeviceFlag::None)) &&
    !m_addr.empty();
}
//---------------------------------------------------------------------------

bool EsBluetoothDevice::hasService(const EsBluetoothServiceUUID& uuid) const
{
  return m_services.end() != std::find(m_services.begin(), m_services.end(), uuid);
}
//---------------------------------------------------------------------------

const EsBluetoothServiceUUIDs& EsBluetoothDevice::noServices()
{
  static const EsBluetoothServiceUUIDs sc_none;
  return sc_none;
}
//---------------------------------------------------------------------------
//--------------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsBluetoothRadioEnumerator, esT("Bluetooth Radio enumerator class"))

  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsBluetoothRadioEnumerator, enumerate, EsVariant_ClassCall, NO_METHOD_DESCR)

ES_DECL_REFLECTED_SERVICES_INFO_END
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

// Platform-dependent stuff
#if ES_OS_WINDOWS == ES_OS
# include "EsBluetooth.win.cxx"
#elif ES_OS_ANDROID == ES_OS
# include "EsBluetooth.android.cxx"
#else
# error Bluetooth functionality is not implemented for this platform!
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/// Enumerator constructor.
/// flags should be combination of EsBluetoothDeviceFlag values,
/// and restricts looked-up devices
///
EsBluetoothDeviceEnumerator::EsBluetoothDeviceEnumerator(
  const EsBluetoothRadio::Ptr& radio,
  ulong flags,
  bool doInquiry /*= false*/,
  ulong inquiryTmo /*= 0*/) :
m_radio(radio),
m_flags(flags),
m_doInquiry(doInquiry),
m_inquiryTmo(inquiryTmo)
{
}
//---------------------------------------------------------------------------

bool EsBluetoothDeviceEnumerator::onDeviceEnumerated(const EsBluetoothDevice::Ptr& info)
{
  return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

const EsString& EsBluetooth::anyAddr()
{
  static const EsString s_any(esT("0:0:0:0:0:0"));
  return s_any;
}
//---------------------------------------------------------------------------

EsString EsBluetooth::codDescriptionGet(ulong cod)
{
  return EsBluetoothDeviceClass(cod).asString();
}
//---------------------------------------------------------------------------

EsString EsBluetooth::mfgStringGet(ulong mfg)
{
#define MFG_MAP_BEGIN switch(mfg) {
#define MFG_MAP_ENTRY(sym, val, str) case EsBluetoothRadio::sym: return str;
#define MFG_MAP_END }
#include "EsBluetooth.MFGdefs.h"

  return EsString::null();
}
//---------------------------------------------------------------------------

EsString EsBluetooth::addrStringGet(ullong addr)
{
  return EsString::format(
      esT("%02X:%02X:%02X:%02X:%02X:%02X"),
      static_cast<int>((addr >> 40) & 0xFF),
      static_cast<int>((addr >> 32) & 0xFF),
      static_cast<int>((addr >> 24) & 0xFF),
      static_cast<int>((addr >> 16) & 0xFF),
      static_cast<int>((addr >> 8 ) & 0xFF),
      static_cast<int>(addr & 0xFF)
    );
}
//---------------------------------------------------------------------------

class EsBtAddrResolver : public EsBluetoothDeviceEnumerator
{
public:
  EsBtAddrResolver(const EsString& addr, ulong flags, bool doInquiry, ulong inquiryTmo) :
  EsBluetoothDeviceEnumerator(EsBluetoothRadio::Ptr(), flags, doInquiry, inquiryTmo),
  m_resolved(false),
  m_addr(addr)
  {}

  bool isResolved() const { return m_resolved; }
  const EsString& nameGet() const { return m_name; }

protected:
  virtual bool onDeviceEnumerated(const EsBluetoothDevice::Ptr& info)
  {
    m_resolved = info->addrGet() == m_addr;
    if( m_resolved )
      m_name = info->nameGet();

    return !m_resolved;
  }

protected:
  bool m_resolved;
  EsString m_addr;
  EsString m_name;
};

bool EsBluetooth::addrToNameResolve(
  const EsString& addr,
  ulong flags,
  EsString& name,
  bool doInquiry /*= false*/,
  ulong inquiryTmo /*= 0*/)
{
  EsBtAddrResolver resolver(addr, flags, doInquiry, inquiryTmo);
  resolver.execute();

  name = resolver.nameGet();

  return resolver.isResolved();
}
//---------------------------------------------------------------------------

class EsBtNameResolver : public EsBluetoothDeviceEnumerator
{
public:
  EsBtNameResolver(const EsString& name, ulong flags, bool doInquiry, ulong inquiryTmo) :
  EsBluetoothDeviceEnumerator(EsBluetoothRadio::Ptr(), flags, doInquiry, inquiryTmo),
  m_resolved(false),
  m_name(name)
  {}

  bool isResolved() const { return m_resolved; }
  const EsString& addrGet() const { return m_addr; }

protected:
  virtual bool onDeviceEnumerated(const EsBluetoothDevice::Ptr& info)
  {
    m_resolved = info->nameGet() == m_name;
    if( m_resolved )
      m_addr = info->addrGet();

    return !m_resolved;
  }

protected:
  bool m_resolved;
  EsString m_addr;
  EsString m_name;
};

bool EsBluetooth::nameToAddrResolve(
  const EsString& name,
  ulong flags,
  EsString& addr,
  bool doInquiry /*= false*/,
  ulong inquiryTmo /*= 0*/)
{
  EsBtNameResolver resolver(name, flags, doInquiry, inquiryTmo);
  resolver.execute();

  addr = resolver.addrGet();

  return resolver.isResolved();
}
//---------------------------------------------------------------------------

#endif //ES_COMM_USE_BLUETOOTH
