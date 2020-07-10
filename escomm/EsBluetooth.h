#ifndef _es_bluetooth_h_
#define _es_bluetooth_h_

#ifdef ES_COMM_USE_BLUETOOTH
/// Forward declarations
///
class EsBluetoothDevice;
class EsBluetoothDeviceEnumerator;

/// Bluetooth device flags
///
enum class EsBluetoothDeviceFlag
{
  None            = 0x0000,
  Unknown         = 0x0001,
  Connected       = 0x0002,
  Remembered      = 0x0004,
  Authenticated   = 0x0008,
  // 'All flags' mask
  All             = EsBluetoothDeviceFlag::Unknown|
                    EsBluetoothDeviceFlag::Connected|
                    EsBluetoothDeviceFlag::Remembered|
                    EsBluetoothDeviceFlag::Authenticated
};

/// Bluetooth device class codec
///
class ESCOMM_CLASS ES_INTF_IMPL1( EsBluetoothDeviceClass, EsReflectedClassIntf )

public:
  /// Bluetooth device class deciphering bits and bit masks
  ///
  enum {
    /// Unknown|undefined COD
    UNKNOWN                       = 0x00001F00,
    /// COD fields offsets
    FORMAT_BIT_OFFSET             = 0,
    MINOR_BIT_OFFSET              = 2,
    MAJOR_BIT_OFFSET              = 8,
    SERVICE_BIT_OFFSET            = 13,
    /// COD fields masks
    FORMAT_MASK                   = 0x00000003,
    MINOR_MASK                    = 0x000000FC,
    MAJOR_MASK                    = 0x00001F00,
    SERVICE_MASK                  = 0x0FFE0000,
    /// COD service bits
    SERVICE_LIMITED               = 0x0001,
    SERVICE_POSITIONING           = 0x0008,
    SERVICE_NETWORKING            = 0x0010,
    SERVICE_RENDERING             = 0x0020,
    SERVICE_CAPTURING             = 0x0040,
    SERVICE_OBJECT_XFER           = 0x0080,
    SERVICE_AUDIO                 = 0x0100,
    SERVICE_TELEPHONY             = 0x0200,
    SERVICE_INFORMATION           = 0x0400,
    /// COD service validation mask
    SERVICE_VALID_MASK            = SERVICE_LIMITED|
                                    SERVICE_POSITIONING|
                                    SERVICE_NETWORKING|
                                    SERVICE_RENDERING|
                                    SERVICE_CAPTURING|
                                    SERVICE_OBJECT_XFER|
                                    SERVICE_AUDIO|
                                    SERVICE_TELEPHONY|
                                    SERVICE_INFORMATION,
    SERVICE_MAX_COUNT             = 9,
    /// COD major device classes
    MAJOR_MISCELLANEOUS           = 0x00,
    MAJOR_COMPUTER                = 0x01,
    MAJOR_PHONE                   = 0x02,
    MAJOR_LAN_ACCESS              = 0x03,
    MAJOR_AUDIO                   = 0x04,
    MAJOR_PERIPHERAL              = 0x05,
    MAJOR_IMAGING                 = 0x06,
    MAJOR_WEARABLE                = 0x07,
    MAJOR_TOY                     = 0x08,
    MAJOR_HEALTH                  = 0x09,
    MAJOR_UNCLASSIFIED            = 0x1F,
    /// COD minor device classes for computer
    COMPUTER_MINOR_UNCLASSIFIED   = 0x00,
    COMPUTER_MINOR_DESKTOP        = 0x01,
    COMPUTER_MINOR_SERVER         = 0x02,
    COMPUTER_MINOR_LAPTOP         = 0x03,
    COMPUTER_MINOR_HANDHELD       = 0x04,
    COMPUTER_MINOR_PALM           = 0x05,
    COMPUTER_MINOR_WEARABLE       = 0x06,
    /// COD minor device classes for phone
    PHONE_MINOR_UNCLASSIFIED      = 0x00,
    PHONE_MINOR_CELLULAR          = 0x01,
    PHONE_MINOR_CORDLESS          = 0x02,
    PHONE_MINOR_SMART             = 0x03,
    PHONE_MINOR_WIRED_MODEM       = 0x04,
    PHONE_MINOR_COMMON_ISDN       = 0x05,
    /// COD minor device classes for audio
    AUDIO_MINOR_UNCLASSIFIED      = 0x00,
    AUDIO_MINOR_HEADSET           = 0x01,
    AUDIO_MINOR_HANDS_FREE        = 0x02,
    AUDIO_MINOR_HEADSET_HANDS_FREE= 0x03,
    AUDIO_MINOR_MICROPHONE        = 0x04,
    AUDIO_MINOR_LOUDSPEAKER       = 0x05,
    AUDIO_MINOR_HEADPHONES        = 0x06,
    AUDIO_MINOR_PORTABLE_AUDIO    = 0x07,
    AUDIO_MINOR_CAR_AUDIO         = 0x08,
    AUDIO_MINOR_SET_TOP_BOX       = 0x09,
    AUDIO_MINOR_HIFI_AUDIO        = 0x0A,
    AUDIO_MINOR_VCR               = 0x0B,
    AUDIO_MINOR_VIDEO_CAMERA      = 0x0C,
    AUDIO_MINOR_CAMCORDER         = 0x0D,
    AUDIO_MINOR_VIDEO_MONITOR     = 0x0E,
    AUDIO_MINOR_VIDEO_DISPLAY_LOUDSPEAKER= 0x0F,
    AUDIO_MINOR_VIDEO_DISPLAY_CONFERENCING= 0x10,
    AUDIO_MINOR_RESERVED          = 0x11,
    AUDIO_MINOR_GAMING_TOY        = 0x12,
    /// COD minor device class capability masks for peripheral
    PERIPHERAL_MINOR_KEYBOARD_MASK= 0x10,
    PERIPHERAL_MINOR_POINTER_MASK = 0x20,
    PERIPHERAL_MINOR_MASK_ALL     = 0x30,
    /// COD minor device classes for peripheral
    PERIPHERAL_MINOR_NO_CATEGORY  = 0x00,
    PERIPHERAL_MINOR_JOYSTICK     = 0x01,
    PERIPHERAL_MINOR_GAMEPAD      = 0x02,
    PERIPHERAL_MINOR_REMOTE_CONTROL= 0x03,
    PERIPHERAL_MINOR_SENSING      = 0x04,
    PERIPHERAL_MINOR_DIGITIZER    = 0x05,
    PERIPHERAL_MINOR_CARD_READER  = 0x06,
    /// COD minor device class capability masks for imaging
    IMAGING_MINOR_DISPLAY_MASK    = 0x04,
    IMAGING_MINOR_CAMERA_MASK     = 0x08,
    IMAGING_MINOR_SCANNER_MASK    = 0x10,
    IMAGING_MINOR_PRINTER_MASK    = 0x20,
    /// COD minor device class masks for LAN. NB! Inside prepared minor byte.
    LAN_ACCESS_BIT_OFFSET         = 3,
    LAN_MINOR_MASK                = 0x07,
    LAN_ACCESS_MASK               = 0x38,
    LAN_MINOR_UNCLASSIFIED        = 0x00,
    /// COD minor device class values for LAN usage
    LAN_ACCESS_0_USED             = 0x00,
    LAN_ACCESS_17_USED            = 0x01,
    LAN_ACCESS_33_USED            = 0x02,
    LAN_ACCESS_50_USED            = 0x03,
    LAN_ACCESS_67_USED            = 0x04,
    LAN_ACCESS_83_USED            = 0x05,
    LAN_ACCESS_99_USED            = 0x06,
    LAN_ACCESS_FULL               = 0x07,
    /// COD minor device class values for wearable
    WEARABLE_MINOR_WRIST_WATCH    = 0x01,
    WEARABLE_MINOR_PAGER          = 0x02,
    WEARABLE_MINOR_JACKET         = 0x03,
    WEARABLE_MINOR_HELMET         = 0x04,
    WEARABLE_MINOR_GLASSES        = 0x05,
    /// COD minor device class values for toy
    TOY_MINOR_ROBOT               = 0x01,
    TOY_MINOR_VEHICLE             = 0x02,
    TOY_MINOR_DOLL                = 0x03,
    TOY_MINOR_CONTROLLER          = 0x04,
    TOY_MINOR_GAME                = 0x05,
    /// COD minor device class values for health
    HEALTH_MINOR_UNCLASSIFIED     = 0x00,
    HEALTH_MINOR_BLOOD_PRESSURE   = 0x01,
    HEALTH_MINOR_THERMOMETER      = 0x02,
    HEALTH_MINOR_WEIGHTING_SCALE  = 0x03,
    HEALTH_MINOR_GLUCOSE_METER    = 0x04,
    HEALTH_MINOR_PULSE_OXIMETER   = 0x05,
    HEALTH_MINOR_PULSE_RATE       = 0x06,
    HEALTH_MINOR_DATA_DISPLAY     = 0x07
  };

public:
  EsBluetoothDeviceClass(ulong code = EsBluetoothDeviceClass::UNKNOWN);

  /// Reflection declaration
  ///
  ES_DECL_REFLECTED_CLASS_BASE( EsBluetoothDeviceClass )
  ES_DECL_ATTR_HANDLING_STD

  ES_DECL_REFLECTED_CLASS_CTOR1( EsVariant, cr_EsVariant );
  
  ES_DECL_PROPERTY(service, ulong)
  ES_DECL_PROPERTY(majorDeviceClass, ulong)
  ES_DECL_PROPERTY(minorDeviceClass, ulong)
  ES_DECL_PROPERTY_RO(format, ulong)
  ES_DECL_PROPERTY_RO(serviceString, EsString)
  ES_DECL_PROPERTY_RO(majorDeviceClassString, EsString)
  ES_DECL_PROPERTY_RO(minorDeviceClassString, EsString)
  ES_DECL_PROPERTY_RO(code, ulong)

  /// Decoding helpers
  ///

  /// Convert code to string representation
  ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString);

  /// Type conversion
  operator ulong () const ES_NOTHROW { return m_code; }

  /// Equality check operators
  bool operator==(const EsBluetoothDeviceClass& other) const ES_NOTHROW { return m_code == other.m_code; }
  bool operator!=(const EsBluetoothDeviceClass& other) const ES_NOTHROW { return m_code != other.m_code; }

  /// Minor class codes string conversion helpers
  /// 
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, computerClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, phoneClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, lanClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, audioClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, peripheralClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, imagingClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, wearableClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, toyClassStringGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, healthClassStringGet, cr_EsVariant);

protected:
  ulong m_code;
};

/// Bluetooth UUID services
///
namespace EsBluetoothUUID
{
  /// Bluetooth base UUID constant
  ESCOMM_FUNC(const GUID&, base() );

  /// Get full UUID from shortened 16|32 UUID
  ESCOMM_FUNC(GUID, fromShortened(ulong uuid) );

  /// UUID validity check. Return true, if uuid 'grows' from the BT base UUID
  ESCOMM_FUNC(bool, isOk(const GUID& uuid) );
}

/// Bluetooth service UUID decoder
///
class ESCOMM_CLASS EsBluetoothServiceUUID
{
public:
  /// Known service UUID16s && UUID32s
  ///
#define UUID16_MAP_BEGIN enum UUID16 {
#define UUID16_MAP_PROT_BEGIN
#define UUID16_MAP_PROT_ENTRY(sym, val, str)  sym = val,
#define UUID16_MAP_PROT_END
#define UUID16_MAP_PSVC_BEGIN
#define UUID16_MAP_PSVC_ENTRY(sym, val, str, isSrv, isProf)  sym = val,
#define UUID16_MAP_PSVC_END
#define UUID16_MAP_END };
#include "EsBluetooth.UUID16defs.h"

public:
  EsBluetoothServiceUUID(const GUID& uuid = GUID_NULL);
  EsBluetoothServiceUUID(ulong uuid);
  EsBluetoothServiceUUID(const EsString& uuid);

  /// UUID validity check
  bool isOk() const { return EsBluetoothUUID::isOk(m_uuid); }

  /// UUID classification
  ///
  /// UUID is protocol
  bool isProtocol() const;

  /// UUID is service class or profile
  bool isService() const;

  /// UUID is service class
  bool isServiceClass() const;

  /// UUID is service profile
  bool isServiceProfile() const;

  /// Return string representation of UUID
  /// If canonical is true, return 'normal' GUID string, with dashes,
  /// otherwise, our default GUID string format is used (continuous HEX string)
  EsString asString(bool canonical = false) const;

  /// Return name of service UUID
  EsString nameGet() const;

  /// Full UUID typecast operator
  operator GUID () const { return m_uuid; }

  /// UUID16|32 typecast operator
  operator ulong () const { return m_uuid.Data1; }

  /// Equality check operators
  bool operator ==(const EsBluetoothServiceUUID& other) const;
  bool operator !=(const EsBluetoothServiceUUID& other) const
  { return !operator==(other); }

  /// Protocol UUID-to-string converter
  static EsString protocolStringGet(ulong uuid);

  /// Service UUID-to-string converter
  static EsString serviceClassStringGet(ulong uuid);

  /// Find protocol|service UUID by its name. If not found - return 0
  static ulong uuidFindByName(const EsString& svcName);

protected:
  GUID m_uuid;
};
typedef std::vector<EsBluetoothServiceUUID> EsBluetoothServiceUUIDs;

/// Bluetooth device information node
///
class ESCOMM_CLASS EsBluetoothDevice
{
public:
  typedef std::shared_ptr<EsBluetoothDevice> Ptr;

protected:
  EsBluetoothDevice(
    ulong flags,
    ulong cod,
    const EsString& addr,
    const EsString& name,
    const EsDateTime& lastSeen,
    const EsDateTime& lastUsed,
    const EsBluetoothServiceUUIDs& services
  );

public:
  virtual ~EsBluetoothDevice() {}

  /// Validity check
  bool isOk() const;

  /// Device flags access
  ///
  /// Device connected check
  bool isConnected() const { return m_flags & static_cast<ulong>(EsBluetoothDeviceFlag::Connected); }
  /// Device remembered check
  bool isRemembered() const { return m_flags & static_cast<ulong>(EsBluetoothDeviceFlag::Remembered); }
  /// Device authenticated check
  bool isAuthenticated() const { return m_flags & static_cast<ulong>(EsBluetoothDeviceFlag::Authenticated); }

  /// Return true if device provides specified BT service
  bool hasService(const EsBluetoothServiceUUID& uuid) const;

  /// Get device class identifier
  const EsBluetoothDeviceClass& classGet() const { return m_class; }

  /// Get device address string
  const EsString& addrGet() const { return m_addr; }

  /// Get device name string
  const EsString& nameGet() const { return m_name; }

  /// Device usage timestampa access
  ///
  /// Get timestamp of the moment device was last seen in PAN
  const EsDateTime& lastSeenGet() const { return m_lastSeen; }

  /// Get timestamp of the moment device was last used in PAN
  const EsDateTime& lastUsedGet() const { return m_lastUsed; }

  /// Found device services for device
  const EsBluetoothServiceUUIDs& servicesGet() const { return m_services; }

protected:
  static const EsBluetoothServiceUUIDs& noServices();

protected:
  ulong m_flags;                      ///< Bluetooth device flags
  EsBluetoothDeviceClass m_class;     ///< Bluetooth device class identifier
  EsString m_addr;                    ///< Device address in form of a string
  EsString m_name;                    ///< Device name
  EsDateTime m_lastSeen;              ///< Timestamp, device was last seen in PAN at
  EsDateTime m_lastUsed;              ///< Timestamp, device was last used in PAN at
  EsBluetoothServiceUUIDs m_services; ///< Enumerated device services
};
typedef std::vector<EsBluetoothDevice::Ptr> EsBluetoothDevices;

/// Bluetooth radio information node
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsBluetoothRadio, EsReflectedClassIntf)

  /// Manufacturers table
  ///
#define MFG_MAP_BEGIN enum {
#define MFG_MAP_ENTRY(sym, val, str) sym = val,
#define MFG_MAP_END };
#include "EsBluetooth.MFGdefs.h"

protected:
  EsBluetoothRadio(
    const EsString& addr,
    const EsString& name,
    ulong cod,
    esU16 sub,
    esU16 mfg,
    bool enabled
  );

public:
  /// Reflection declaration
  ///
  ES_DECL_REFLECTED_CLASS_BASE(EsBluetoothRadio)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected properties
  /// 
  ES_DECL_PROPERTY_RO(isOk, bool)
  ES_DECL_PROPERTY_RO(enabled, bool)
  ES_DECL_PROPERTY_RO(deviceClass, EsVariant)
  ES_DECL_PROPERTY_RO(subversion, ulong)
  ES_DECL_PROPERTY_RO(manufacturer, ulong)
  ES_DECL_PROPERTY_RO(manufacturerString, EsString)
  ES_DECL_PROPERTY_RO(address, EsString)
  ES_DECL_PROPERTY_RO(name, EsString)

  // Non-reflected stuff
  //
    
  /// Get device class identifier
  const EsBluetoothDeviceClass& deviceClassGet() const ES_NOTHROW { return m_class; }

protected:
  EsBluetoothDeviceClass m_class; ///< Radio device class
  esU16 m_subversion;             ///< Mfg. specific device subversion
  esU16 m_mfg;                    ///< Manufacturer information
  EsString m_addr;                ///< Address of radio in string form
  EsString m_name;                ///< Radio name
  bool m_enabled;                 ///< Radio adapter status
};

/// Bluetooth radio enumerator class
///
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsBluetoothRadioEnumerator, ESCOMM_CLASS)

  /// Execute Bluetooth radios enumeration, return a collection of radio objects
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, enumerate);

  // Non-reflected services
  //
  //TODO

ES_DECL_REFLECTED_SERVICES_END

/// Bluetooth device enumerator base class
///
class ESCOMM_CLASS EsBluetoothDeviceEnumerator
{
public:
  /// Enumerator constructor.
  /// flags should be combination of EsBluetoothDeviceFlag values,
  /// and restricts looked-up devices
  ///
  EsBluetoothDeviceEnumerator(const EsBluetoothRadio::Ptr& radio, ulong flags, bool doInquiry = false, ulong inquiryTmo = 0);

  virtual ~EsBluetoothDeviceEnumerator() {}

  /// Execute Bluetooth device enumeration
  void execute();

  /// Reset enumerator to the beginning
  void reset() { m_infos.clear(); }

  /// Retrieve found device information records
  const EsBluetoothDevices& resultGet() const { return m_infos; }

protected:
  /// Overridable enumeration callback
  /// If false is returned, enumeration process is cancelled
  /// default implementation just returns true
  ///
  virtual bool onDeviceEnumerated(const EsBluetoothDevice::Ptr& info);

protected:
  EsBluetoothRadio::Ptr m_radio;
  ulong m_flags;
  bool m_doInquiry;
  ulong m_inquiryTmo;
  /// Enumerated infos
  EsBluetoothDevices m_infos;

private:
  /// Prohibited functionality
  EsBluetoothDeviceEnumerator();
  EsBluetoothDeviceEnumerator(const EsBluetoothDeviceEnumerator&);
  EsBluetoothDeviceEnumerator& operator=(const EsBluetoothDeviceEnumerator&);
};

/// Bluetooth global services
namespace EsBluetooth
{
  /// Global initialization
  ESCOMM_FUNC( void, initialize() );

  /// Global initialization check
  ESCOMM_FUNC( bool, initialized() );

  /// Global uninitialization
  ESCOMM_FUNC( void, uninitialize() );

  /// Return any address string
  ESCOMM_FUNC( const EsString&, anyAddr() );

  /// Retrieve string representation of device CoD
  ESCOMM_FUNC( EsString, codDescriptionGet(ulong cod) );

  /// Retrieve string representation of radio manufacturer
  ESCOMM_FUNC( EsString, mfgStringGet(ulong mfg) );

  /// Retrieve string representation of bluetooth address
  ESCOMM_FUNC( EsString, addrStringGet(ullong addr) );

  /// Try to resolve bluetooth address string to device name
  ESCOMM_FUNC( bool, addrToNameResolve(const EsString& addr, ulong flags, EsString& name, bool doInquiry = false, ulong inquiryTmo = 0) );

  /// Try to resolve bluetooth device name string to device address
  ESCOMM_FUNC( bool, nameToAddrResolve(const EsString& name, ulong flags, EsString& addr, bool doInquiry = false, ulong inquiryTmo = 0) );
}

#endif // ES_COMM_USE_BLUETOOTH

#endif //_es_bluetooth_h_
