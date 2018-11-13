#ifndef _es_bluetooth_le_h_
#define _es_bluetooth_le_h_

#ifdef ES_COMM_USE_BLUETOOTH_LE

/// @file   EsBluetoothLE.h
/// Bluetooth Low Energy cross-platform implementation
///

/// Forward declarations
class EsBluetoothLEcharacteristic;
class EsBluetoothLEservice;
class EsBluetoothLEdevice;

/// Bluetooth LE value fiormat types
///
enum class EsBluetoothLEformatType : ulong
{
  Reserved,
  Boolean,
  Unsigned2bitInteger,
  Unsigned4bitInteger,
  Unsigned8bitInteger,
  Unsigned12bitInteger,
  Unsigned16bitInteger,
  Unsigned24bitInteger,
  Unsigned32bitInteger,
  Unsigned48bitInteger,
  Unsigned64bitInteger,
  Unsigned128bitInteger,
  Signed8bitInteger,
  Signed12bitInteger,
  Signed16bitInteger,
  Signed24bitInteger,
  Signed32bitInteger,
  Signed48bitInteger,
  Signed64bitInteger,
  Signed128bitInteger,
  IEEE754_32bit_floating_point,
  IEEE754_64bit_floating_point,
  IEEE11073_16bitSFLOAT,
  IEEE11073_32bitFLOAT,
  IEEE20601Format,
  UTF8String,
  UTF16String,
  OpaqueStructure
};

/// Bluetooth LE scan response keys
///
enum class EsBluetoothLEscanResponseKey : esU8
{
  Flags                             = 0x01,
  IncompleteList16SCUUID            = 0x02,
  CompleteList16SCUUID              = 0x03,
  IncompleteList32SCUUID            = 0x04,
  CompleteList32SCUUID              = 0x05,
  IncompleteList128SCUUID           = 0x06,
  CompleteList128SCUUID             = 0x07,
  ShortenedLocalName                = 0x08,
  CompleteLocalName                 = 0x09,
  TxPowerLevel                      = 0x0A,
  ClassOfDevice                     = 0x0D,
  SimplePairingHashC                = 0x0E,
  SimplePairingRAndomizerR          = 0x0F,
  DeviceID                          = 0x10,
  SecurityManagerOutOfBandFlags     = 0x11,
  SlaveConnectionIntervalRange      = 0x12,
  List16bServiceSolicitationUUIDs   = 0x14,
  List32bServiceSolicitationUUIDs   = 0x1F,
  List128bServiceSolicitationUUIDs  = 0x15,
  ServiceData                       = 0x16,
  ServiceData16b                    = 0x16,
  ServiceData32b                    = 0x20,
  ServiceData128b                   = 0x21,
  SecureConnectionsConfirmationVal  = 0x22,
  LEsecureConnectionRandomVal       = 0x23,
  PublicTargetAddress               = 0x17,
  RandomTargetAddress               = 0x18,
  Appearance                        = 0x19,
  AdvertisingInterval               = 0x1A,
  LEBluetoothDeviceAddress          = 0x1B,
  LERole                            = 0x1C,
  SimplePairingHashc256             = 0x1D,
  SimplePairingRAndomizerR256       = 0x1E,
  InformationData3D                 = 0x3D,
  ManufacturerSpecificData          = 0xFF
};

/// Bluetooth LE scan response data, indexed by keys, declared above
///
typedef std::map<EsBluetoothLEscanResponseKey, EsBinBufferPtr> EsBluetoothLEscanResponseData;

/// Bluetooth LE operation result
enum class EsBluetoothLEresult : ulong
{
  gattSuccess,
  gattReadNotPermitted = 2,
  gattWriteNotPermitted,
  gattInsufficientAutentication = 5,
  gattRequestNotSupported,
  gattInvalidOffset,
  gattInvalidAttributeLength = 13,
  gattInsufficientEncryption = 15,
  gattFailure = 257
};

/// Bluetooth LE GATT Descriptor interface
///
class ESCOMM_CLASS EsBluetoothLEdescriptor
{
public:
  typedef std::shared_ptr<EsBluetoothLEdescriptor> Ptr;

  /// Descriptor kinds
#define ES_BLE_DESCRIPTOR_BEGIN enum Kind {
#define ES_BLE_DESCRIPTOR_ENTRY(Name, Descr, Uuid) kind##Name,
#define ES_BLE_DESCRIPTOR_END kindUnknown };
#include "EsBluetoothLEdescriptor.defs.h"

protected:
  EsBluetoothLEdescriptor();

public:
  virtual ~EsBluetoothLEdescriptor() {}

  /// Return true if descriptor is known
  bool isKnown() const { return isKnownUUID(m_uuid); }

  /// Return descriptor UUID16|UUID32
  ulong uuidShortGet() const { return m_uuid.Data1; }

  /// Return descriptor UUID
  const GUID& uuidGet() const { return m_uuid; }

  /// Return descriptor kind
  EsBluetoothLEdescriptor::Kind kindGet() const { return m_kind; }

  /// Return kind name, if known, an empty string otherwise
  EsString kindNameGet() const { return isKnown() ? kindNameGetByKind(m_kind) : EsString::null(); }

  /// Return current local descriptor value
  virtual EsBinBuffer valueGet() const = 0;

  /// Set local descriptor value
  virtual void valueSet(const EsBinBuffer& val) = 0;

  /// Read remote descriptor value from device, updating local value
  virtual EsBluetoothLEresult read(bool chached = false) = 0;

  /// Write local descriptor value to device, updating remote side
  virtual EsBluetoothLEresult write() = 0;

  /// Return false if this descriptor is assigned to characteristic, true, if it's an orphan
  bool isOrphan() const { return 0 == m_chx; }

  /// Return characteristic containing this descriptor
  const EsBluetoothLEcharacteristic& characteristicGet() const { return *m_chx; }
  EsBluetoothLEcharacteristic& characteristicGet() { return *m_chx; }

  /// Known descriptor property short-cuts. Validity is depending on descriptor Kind
  ///

  virtual bool isBroadcast() const = 0;
  virtual bool isIndicate() const = 0;
  virtual void indicateSet(bool indicate) = 0;
  virtual bool isNotify() const = 0;
  virtual void notifySet(bool notify) = 0;
  virtual bool isReliableWrite() const = 0;
  virtual bool isWritableAuxiliaries() const = 0;

  virtual int exponentGet() const = 0;
  virtual EsBluetoothLEformatType formatTypeGet() const = 0;
  virtual GUID formatUnitGet() const = 0;
  virtual GUID formatDescriptionGet() const = 0;
  virtual EsString userDescriptionGet() const = 0;

  /// Static services
  ///

  /// Return true, if descriptor is of known UUID16|UUID32
  static bool isKnownUUID(ulong uuid);

  /// Return true, if descriptor is of known UUID
  static bool isKnownUUID(const GUID& uuid);

  /// Return descriptor kind name from its Kind value
  static EsString kindNameGetByKind(EsBluetoothLEdescriptor::Kind kind);

  /// Return descriptor kind name from its UUID16|UUID32
  static EsString kindNameGetByUUID(ulong uuid);

  /// Return descriptor kind name from its UUID
  static EsString kindNameGetByUUID(const GUID& uuid);

  /// Return descriptor kind based on its UUID16|UUID32 value
  static EsBluetoothLEdescriptor::Kind kindGetByUUID(ulong uuid);

  /// Return descriptor kind based on its UUID value
  static EsBluetoothLEdescriptor::Kind kindGetByUUID(const GUID& uuid);

  /// Return descriptor UUID from its kind
  static GUID uuidGetByKind(EsBluetoothLEdescriptor::Kind kind);

protected:
  static void throwInvalidPropForKind(const EsString& prop, EsBluetoothLEdescriptor::Kind kind);

protected:
  mutable EsCriticalSection m_cs;
  EsBluetoothLEcharacteristic* m_chx;
  Kind m_kind;
  GUID m_uuid;
  EsBinBuffer m_val;

private:
  EsBluetoothLEdescriptor(const EsBluetoothLEdescriptor&);
  EsBluetoothLEdescriptor& operator=(const EsBluetoothLEdescriptor&);
};
typedef std::vector<EsBluetoothLEdescriptor::Ptr> EsBluetoothLEdescriptors;

/// Bluetooth LE GATT Characteristic interface
///
class ESCOMM_CLASS EsBluetoothLEcharacteristic
{
public:
  typedef std::shared_ptr<EsBluetoothLEcharacteristic> Ptr;

  /// Characteristic properties
  enum {
    propBroadcast       = 0x00000001,   ///< The characteristic is broadcastable.
    propExtendedProps   = 0x00000002,   ///< The characteristic has extended properties.
    propNotify          = 0x00000004,   ///< The characteristic supports notification.
    propIndicate        = 0x00000008,   ///< The characteristic supports indication.
    propRead            = 0x00000010,   ///< The characteristic is readable.
    propWrite           = 0x00000020,   ///< The characteristic can be written.
    propWriteNoResponse = 0x00000040,   ///< The characteristic can be written without response.
    propSignedWrite     = 0x00000080    ///< The characteristic supports writing with signature.
  };

protected:
  EsBluetoothLEcharacteristic();

public:
  virtual ~EsBluetoothLEcharacteristic() {}

  /// Return false if this characteristic is assigned to service, true, if it's an orphan
  bool isOrphan() const { return 0 == m_svc; }

  /// Return true if characteristic is known
  bool isKnown() const { return isKnownUUID(m_uuid); }

  /// Return known charateristic name string, or empty string otherwise
  EsString knownNameGet() const { return isKnown() ? knownNameGetByUUID(m_uuid) : EsString::null(); }

  /// Return known name, or, if characteristic is unknown, try to extract name from user description, if any
  EsString nameGet() const;

  /// Return current characteristic properties
  ulong propsGet() const { return m_props; }

  /// Basic capabilities
  ///
  bool canRead() const { return (m_props & propRead); }
  bool canWrite() const { return (m_props & propWrite); }
  bool canNotify() const { return (m_props & propNotify); }
  bool canIndicate() const { return (m_props & propIndicate); }

  /// Return properties string
  EsString propertiesStringGet() const { return propertiesStringGet(m_props); }

  /// Return characteristic UUID16|UUID32
  ulong uuidShortGet() const { return m_uuid.Data1; }

  /// Return characteristic UUID
  const GUID& uuidGet() const { return m_uuid; }

  /// Return current local characteristic value
  virtual EsBinBuffer valueGet() const = 0;

  /// Set local characteristic value
  virtual void valueSet(const EsBinBuffer& val) = 0;

  /// Read remote characteristic from device, updating local value
  virtual EsBluetoothLEresult read(bool chached = false) = 0;

  /// Write local characteristic value to device, updating remote side
  virtual EsBluetoothLEresult write() = 0;

  /// Subscribe to|unsubscribe from characteristic push notification
  virtual EsBluetoothLEresult subscribe(bool on) = 0;

  /// Return true if notification subscription is active
  virtual bool isSubscribed() const = 0;

  /// Return descriptors of this characteristic
  const EsBluetoothLEdescriptors& descriptorsGet() const { return m_dcrs; }

  /// Find characteristic descriptor by its UUID. Return an empty smartptr, if not found
  EsBluetoothLEdescriptor::Ptr descriptorFind(const GUID& uuid) const;

  /// Access service, this characteristic is assigned to
  const EsBluetoothLEservice& serviceGet() const { return *m_svc; }
  EsBluetoothLEservice& serviceGet() { return *m_svc; }

  /// Static services
  ///

  /// Return true if characteristic UUID16|UUID32 is known
  static bool isKnownUUID(ulong uuid);

  /// Return true if characteristic UUID is known
  static bool isKnownUUID(const GUID& uuid);

  /// Return known characteristic name by its UUID16|UUID32
  static EsString knownNameGetByUUID(ulong uuid);

  /// Return known characteristic name by its UUID
  static EsString knownNameGetByUUID(const GUID& uuid);

  /// Return properties string
  static EsString propertiesStringGet(ulong props);

protected:
  mutable EsCriticalSection m_cs;
  EsBluetoothLEservice* m_svc;
  ulong m_props;
  GUID m_uuid;
  EsBinBuffer m_val;
  EsBluetoothLEdescriptors m_dcrs;

private:
  EsBluetoothLEcharacteristic(const EsBluetoothLEcharacteristic&);
  EsBluetoothLEcharacteristic& operator=(const EsBluetoothLEcharacteristic&);
};
typedef std::vector<EsBluetoothLEcharacteristic::Ptr> EsBluetoothLEcharacteristics;

/// Bluetooth LE GATT Service interface
///
class ESCOMM_CLASS EsBluetoothLEservice
{
public:
  typedef std::shared_ptr<EsBluetoothLEservice> Ptr;
  typedef std::vector<Ptr> VectorT;

protected:
  EsBluetoothLEservice();

public:
  virtual ~EsBluetoothLEservice() {}

  /// Return false if this service is assigned to device, true, if it's an orphan
  bool isOrphan() const { return 0 == m_dev; }

  /// Return true, if this is primary service (parent service is 0)
  bool isPrimary() const { return nullptr == m_parent; }

  /// Return true, if services is a known LE service
  bool isKnown() const { return isKnownUUID(m_uuid); }

  /// Return service UUID16|UUID32
  ulong uuidShortGet() const { return m_uuid.Data1; }

  /// Return service UUID
  const GUID& uuidGet() const { return m_uuid; }

  /// Return known service name string, or empty string otherwise
  EsString knownNameGet() const { return isKnown() ? knownNameGetByUUID(m_uuid) : EsString::null(); }

  /// Return list of sub-services
  const EsBluetoothLEservice::VectorT& includedServicesGet() const { return m_svcs; }

  /// Return list of service's characteristics
  const EsBluetoothLEcharacteristics& characteristicsGet() const { return m_chxs; }

  /// Find and return servcie characteristic by its UUID, or null ptr, if not found
  EsBluetoothLEcharacteristic::Ptr characteristicFind(const GUID& uuid) const;

  /// Access LE device, this service is assigned to
  const EsBluetoothLEdevice& deviceGet() const { return *m_dev; }
  EsBluetoothLEdevice& deviceGet() { return *m_dev; }

  /// Static services
  ///

  /// Return true if service UUID16|UUID32 is known
  static bool isKnownUUID(ulong uuid);

  /// Return true if service UUID is known
  static bool isKnownUUID(const GUID& uuid);

  /// Return service name by its UUID16|UUID32
  static EsString knownNameGetByUUID(ulong uuid);

  /// Return service name by its UUID
  static EsString knownNameGetByUUID(const GUID& uuid);

protected:
  EsBluetoothLEdevice* m_dev;
  EsBluetoothLEservice* m_parent;
  GUID m_uuid;
  EsBluetoothLEcharacteristics m_chxs;
  VectorT m_svcs;

private:
  EsBluetoothLEservice(const EsBluetoothLEservice&);
  EsBluetoothLEservice& operator=(const EsBluetoothLEservice&);
};
typedef EsBluetoothLEservice::VectorT EsBluetoothLEservices;

/// Bluetooth LE device interface
///
class ESCOMM_CLASS EsBluetoothLEdevice
{
public:
  typedef std::shared_ptr<EsBluetoothLEdevice> Ptr;

protected:
  EsBluetoothLEdevice( EsBluetoothRadio* radio,
                       const EsString& addr,
                       const EsString& name );

public:
  virtual ~EsBluetoothLEdevice() {}

  /// Return Bluetooth radio this LE device is connected to
  EsBluetoothRadio& radioGet() { return *m_radio; }
  const EsBluetoothRadio& radioGet() const { return *m_radio; }

  /// Return Bluetooth device LE MAC address string
  const EsString& addrGet() const { return m_addr; }

  /// Return Bluetooth device LE name
  const EsString& nameGet() const { return m_name; }

  /// Return recent RSSI value (after the last RSSI query)
  long recentRssiGet() const { return m_rssi; }

  /// Return service by its full UUID, or null ptr, if not found
  EsBluetoothLEservice::Ptr serviceFind(const GUID& uuid) const;

  /// Return discovered device services
  const EsBluetoothLEservices& servicesGet() const { return m_svcs; }

  /// Generic Bluetooth LE interface
  ///

  /// Return true, if device is connected
  virtual bool isConnected() const = 0;

  /// Query device RSSI
  virtual void rssiQuery() = 0;

  /// Return platform-dependent device identification string
  virtual const EsString& idGet() const = 0;

  /// Disconnect device
  virtual void disconnect() = 0;

  /// Re-discover device services
  virtual void discover() = 0;

  /// Prepare reliable write batch
  virtual bool reliableWriteBegin() = 0;

  /// Abort reliable write batch
  virtual void reliableWriteAbort() = 0;

  /// Execute reliable write batch
  virtual bool reliableWriteExecute() = 0;

protected:
  EsBluetoothRadio* m_radio;
  EsString m_addr;                    ///< Device address in form of a string
  EsString m_name;                    ///< Device name
  long m_rssi;                        ///< Recently acquired RSSI value
  EsBluetoothLEservices m_svcs;

private:
  EsBluetoothLEdevice();
  EsBluetoothLEdevice(const EsBluetoothLEdevice&);
  EsBluetoothLEdevice& operator=(const EsBluetoothLEdevice&);
};
typedef std::vector< EsBluetoothLEdevice::Ptr > EsBluetoothLEdevices;

/// Bluetooth LE device enumerator base class
///
class ESCOMM_CLASS EsBluetoothLEdeviceEnumerator
{
public:
  /// Enumerator constructor.
  /// flags should be combination of EsBluetoothDeviceFlag values,
  /// and restricts looked-up devices
  ///
  EsBluetoothLEdeviceEnumerator(const EsBluetoothRadio::Ptr& radio, bool doInquiry = false, ulong inquiryTmo = 0);

  virtual ~EsBluetoothLEdeviceEnumerator() {}

  /// Execute Bluetooth LE device enumeration
  void execute();

  /// Reset enumerator to the beginning
  void reset() { m_devs.clear(); }

  /// Retrieve found device information records
  const EsBluetoothLEdevices& resultGet() const { return m_devs; }

  /// Find enumerated device by its name string. Return an empty ptr if not found
  EsBluetoothLEdevice::Ptr deviceFindByName(const EsString& name) const;

  /// Find enumerated device by its address string. Return an empty ptr if not found
  EsBluetoothLEdevice::Ptr deviceFindByAddress(const EsString& address) const;

protected:
  /// Overridable enumeration callback
  /// If false is returned, enumeration process is cancelled
  /// default implementation just returns true
  ///
  virtual bool onDeviceEnumerated(const EsBluetoothLEdevice::Ptr& device);

protected:
  /// Data members
  ///
  EsBluetoothRadio::Ptr m_radio;
  ulong m_flags;
  bool m_doInquiry;
  ulong m_inquiryTmo;
  /// Enumerated infos
  EsBluetoothLEdevices m_devs;

private:
  /// Prohibited functionality
  EsBluetoothLEdeviceEnumerator();
  EsBluetoothLEdeviceEnumerator(const EsBluetoothLEdeviceEnumerator&);
  EsBluetoothLEdeviceEnumerator& operator=(const EsBluetoothLEdeviceEnumerator&);
};

/// Bluetooth LE globals
///
namespace EsBluetoothLE
{
  /// Global initialization
  ESCOMM_FUNC( void, initialize() );

  /// Global initialization check
  ESCOMM_FUNC( bool, initialized() );

  /// Global uninitialization
  ESCOMM_FUNC( void, uninitialize() );

  /// Return Bluetooth LE result code string by code value
  ESCOMM_FUNC( EsString, resultCodeStringGet(EsBluetoothLEresult code) );
}

#endif // ES_COMM_USE_BLUETOOTH_LE

#endif // _es_bluetooth_le_h_
