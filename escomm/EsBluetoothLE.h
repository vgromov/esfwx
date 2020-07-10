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

/// Bluetooth LE operation result
/// 
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

/// Descriptor kinds
/// 
#define ES_BLE_DESCRIPTOR_BEGIN enum class EsBluetoothLEdescriptorKind {
#define ES_BLE_DESCRIPTOR_ENTRY(Name, Descr, Uuid) Name,
#define ES_BLE_DESCRIPTOR_END Unknown };
#include "EsBluetoothLEdescriptor.defs.h"

/// Bluetooth LE GATT Descriptor interface
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsBluetoothLEdescriptor, EsReflectedClassIntf)

protected:
  // NB! a weak reference must be passed here
  EsBluetoothLEdescriptor(const EsBaseIntfPtr& chx);

public:
  // Reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE(EsBluetoothLEdescriptor)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected properties
  /// 
  ES_DECL_PROPERTY_RO(isKnown, bool)
  ES_DECL_PROPERTY_RO(isOrphan, bool)
  ES_DECL_PROPERTY_RO(isBroadcast, bool)
  ES_DECL_PROPERTY(isIndicate, bool)
  ES_DECL_PROPERTY(isNotifying, bool)
  ES_DECL_PROPERTY_RO(isReliableWrite, bool)
  ES_DECL_PROPERTY_RO(isWritableAuxiliaries, bool)
  ES_DECL_PROPERTY_RO(uuidShort, ulong)
  ES_DECL_PROPERTY_RO(uuid, EsVariant)
  ES_DECL_PROPERTY_RO(kind, ulong)
  ES_DECL_PROPERTY_RO(kindName, EsString)
  ES_DECL_PROPERTY(value, EsBinBuffer)
  ES_DECL_PROPERTY_RO(exponent, long)
  ES_DECL_PROPERTY_RO(formatType, ulong)
  ES_DECL_PROPERTY_RO(formatUnitGet, EsVariant)
  ES_DECL_PROPERTY_RO(formatDescriptionGet, EsVariant)
  ES_DECL_PROPERTY_RO(userDescription, EsString)
  ES_DECL_PROPERTY_RO(characteristic, EsVariant)

  /// Reflected services
  ///
  ES_DECL_REFLECTED_METHOD0(ulong, readReflected);  //< Alias of non-reflected read(false)
  ES_DECL_REFLECTED_METHOD0(ulong, readCached);     //< Alias of non-reflected read(true)
  ES_DECL_REFLECTED_METHOD0(ulong, writeReflected); //< Alias of non-reflected write()

  /// Static reflected services
  ///        

  /// @brief  Query, if descriptor short UUID (UUID16 or UUID32), 
  ///         or full UUID is known value.
  ///
  /// @param  uuid    The short or full UUID value to check.
  /// @returns  Return true, if descriptor is of known UUID,
  ///           false otherwise.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isKnownUUID, cr_EsVariant);

  /// @brief  Look-up and return a known descriptor name by its EsBluetoothLEdescriptorKind value.
  ///
  /// @param  kind  The kind of descriptor to look-up the name of.
  ///
  /// @returns  An EsString, descriptor kind name from its EsBluetoothLEdescriptorKind value.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, kindNameGetByKind, cr_EsVariant);

  /// @brief  Return descriptor kind name from its UUID16|UUID32, or full UUID
  ///         
  /// @param  uuid  The short UUID16|UUID32, or full descriptor UUID value.
  ///
  /// @returns  An known UUID name.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, kindNameGetByUUID, cr_EsVariant);

  /// @brief  Return descriptor kind based on its short UUID16|UUID32,
  ///         or full UUID value
  ///
  /// @param  uuid  The short, or full uuid.
  ///
  /// @returns  An EsBluetoothLEdescriptorKind.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, kindGetByUUID, cr_EsVariant);
 
  /// @brief  Return descriptor UUID from its kind
  ///
  /// @param  kind  The descriptor kind.
  ///
  /// @returns  An EsUUID object.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, uuidGetByKind, cr_EsVariant);

  /// Non-reflected stuff
  /// 

  /// Return descriptor UUID
  const EsUUID& uuidGet() const ES_NOTHROW { return m_uuid; }

  /// Return descriptor kind
  EsBluetoothLEdescriptorKind kindGet() const ES_NOTHROW { return m_kind; }

  /// Return kind name, if known, an empty string otherwise
  EsString kindNameGet() const;

  /// Return current local descriptor value
  virtual EsBinBuffer valueGet() const = 0;

  /// Set local descriptor value
  virtual void valueSet(const EsBinBuffer& val) = 0;

  /// Read remote descriptor value from device, updating local value
  virtual EsBluetoothLEresult read(bool cached = false) = 0;

  /// Write local descriptor value to device, updating remote side
  virtual EsBluetoothLEresult write() = 0;

  /// Return characteristic containing this descriptor
  const EsBluetoothLEcharacteristic* characteristicGet() const;
  EsBluetoothLEcharacteristic* characteristicGet();

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
  static bool isKnownUUID(const UUID& uuid);

  /// Return descriptor kind name from its Kind value
  static EsString kindNameGetByKind(EsBluetoothLEdescriptorKind kind);

  /// Return descriptor kind name from its UUID16|UUID32
  static EsString kindNameGetByUUID(ulong uuid);

  /// Return descriptor kind name from its UUID
  static EsString kindNameGetByUUID(const UUID& uuid);

  /// Return descriptor kind based on its UUID16|UUID32 value
  static EsBluetoothLEdescriptorKind kindGetByUUID(ulong uuid);

  /// Return descriptor kind based on its UUID value
  static EsBluetoothLEdescriptorKind kindGetByUUID(const UUID& uuid);

  /// Return descriptor UUID from its kind
  static GUID uuidGetByKind(EsBluetoothLEdescriptorKind kind);

protected:
  static void throwInvalidPropForKind(const EsString& prop, EsBluetoothLEdescriptorKind kind);

protected:
  mutable EsCriticalSection m_cs;
  EsBaseIntfPtr m_chx; //< NB! weak reference to the owning characteristic
  EsBluetoothLEdescriptorKind m_kind;
  EsUUID m_uuid;
  EsBinBuffer m_val;

  ES_NO_DEFAULT_CTR(EsBluetoothLEdescriptor);
  ES_NON_COPYABLE(EsBluetoothLEdescriptor);

  friend class EsBluetoothLEcharacteristic;
};

/// Bluetooth LE characteristic properties
///
enum EsBluetoothLEcharacteristicProperty
{
  Broadcast           = 0x00000001,   ///< The characteristic is broadcastable.
  ExtendedProps       = 0x00000002,   ///< The characteristic has extended properties.
  Notify              = 0x00000004,   ///< The characteristic supports notification.
  Indicate            = 0x00000008,   ///< The characteristic supports indication.
  Read                = 0x00000010,   ///< The characteristic is readable.
  Write               = 0x00000020,   ///< The characteristic can be written.
  WriteNoResponse     = 0x00000040,   ///< The characteristic can be written without response.
  SignedWrite         = 0x00000080    ///< The characteristic supports writing with signature.
};

/// Bluetooth LE GATT Characteristic interface
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsBluetoothLEcharacteristic, EsReflectedClassIntf)

protected:
  EsBluetoothLEcharacteristic(const EsBaseIntfPtr& svc);

public:
  // Reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE(EsBluetoothLEcharacteristic)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected properties
  /// 
  ES_DECL_PROPERTY_RO(isKnown, bool)
  ES_DECL_PROPERTY_RO(isOrphan, bool)
  ES_DECL_PROPERTY_RO(knownName, EsString)
  ES_DECL_PROPERTY_RO(name, EsString)
  ES_DECL_PROPERTY_RO(properties, ulong)
  ES_DECL_PROPERTY_RO(propertiesStr, EsString)
  ES_DECL_PROPERTY_RO(canRead, bool)
  ES_DECL_PROPERTY_RO(canWrite, bool)
  ES_DECL_PROPERTY_RO(canNotify, bool)
  ES_DECL_PROPERTY_RO(canIndicate, bool)
  ES_DECL_PROPERTY_RO(subscribed, bool)
  ES_DECL_PROPERTY_RO(uuidShort, ulong)
  ES_DECL_PROPERTY_RO(uuid, EsVariant)
  ES_DECL_PROPERTY_RO(service, EsVariant)
  ES_DECL_PROPERTY_RO(descriptorsUUIDs, EsVariant)
  ES_DECL_PROPERTY_RO(descriptors, EsVariant)
  ES_DECL_PROPERTY(value, EsBinBuffer)

  /// Reflected services
  /// 

  /// @brief  Find characteristic's descriptor object by its UUID.
  ///         Return null, if not found 
  ///
  /// @param  uuid  A descriptor UUID to look-up.
  /// 
  /// @return A descriptor object, if found, null, if not.
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, descriptorFind, cr_EsVariant);

  /// @brief  Get characteristic's sescriptor by its UUID
  ///
  /// @param  uuid  A descriptor UUID to look-up.
  ///
  /// @returns  A descriptor object, if found. If not- 
  ///           throws an exception.
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, descriptorGet, cr_EsVariant);

  /// @brief  Subscribe to|unsubscribe from characteristic notification or indication
  ///
  /// @param  on  True to subscribe, false to unsubscribe.
  ///
  /// @returns  A subscription operation result EsBluetoothLEresult.
  ///
  ES_DECL_REFLECTED_METHOD1(EsVariant, subscribe, cr_EsVariant);

  ES_DECL_REFLECTED_METHOD0(ulong, readReflected);  //< Alias of non-reflected read(false)
  ES_DECL_REFLECTED_METHOD0(ulong, readCached);     //< Alias of non-reflected read(true)
  ES_DECL_REFLECTED_METHOD0(ulong, writeReflected); //< Alias of non-reflected write()

  /// Reflected static servcies
  /// 

  /// @brief      Check if UUID is of known characteristic one
  ///           
  /// @param uuid Either full UUID, or UUID16|UUID32
  ///           
  /// @return     Return true if characteristic full UUID, or UUID16|UUID32 is known
  ///           
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isKnownUUID, cr_EsVariant);

  /// @brief      Return known characteristic name by its full UUID, 
  ///             or UUID16|UUID32
  ///
  /// @param  uuid  Either full UUID, or UUID16|UUID32
  ///
  /// @returns  An known characteristic name, or an empty string.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, knownNameGetByUUID, cr_EsVariant);

  /// @brief  Get characteristic's properties string representation
  ///
  /// @param  props The properties bit mask.
  ///
  /// @returns  An string representing properties, or an empty string.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, propertiesStringGet, cr_EsVariant);

  // Non-reflected stuff
  // 

  /// Return current local characteristic value
  virtual EsBinBuffer valueGet() const = 0;

  /// Set local characteristic value
  virtual void valueSet(const EsBinBuffer& val) = 0;

  /// Read remote characteristic from device, updating local value
  virtual EsBluetoothLEresult read(bool cached = false) = 0;

  /// Write local characteristic value to device, updating remote side
  virtual EsBluetoothLEresult write() = 0;

  /// Subscribe to|unsubscribe from characteristic push notification
  virtual EsBluetoothLEresult subscribe(bool on) = 0;

  /// Return true if notification subscription is active
  virtual bool isSubscribed() const = 0;

  /// Find characteristic descriptor by its UUID. Return an empty smartptr, if not found
  EsBluetoothLEdescriptor* descriptorFind(const GUID& uuid) const;

  /// Access service, this characteristic is assigned to
  const EsBluetoothLEservice* serviceGet() const;
  EsBluetoothLEservice* serviceGet();

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

  /// Return string representation of characteristic properties bit mask
  static EsString propertiesStringGet(ulong props);

protected:
  // Internal descriptors cleanup, setting orphan explicitly
  virtual void descriptorsCleanup();

protected:
  mutable EsCriticalSection m_cs;
  EsBaseIntfPtr m_svc; //< NB! weak reference to the owning service
  ulong m_props;
  EsUUID m_uuid;
  EsBinBuffer m_val;
  EsAssocContainerIntf::Ptr m_dcrs; //< Descriptor objects, indexed by their UUIDs

  ES_NO_DEFAULT_CTR(EsBluetoothLEcharacteristic);
  ES_NON_COPYABLE(EsBluetoothLEcharacteristic);

  friend class EsBluetoothLEservice;
};

/// Bluetooth LE GATT Service interface
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsBluetoothLEservice, EsReflectedClassIntf)

protected:
  EsBluetoothLEservice(const EsBaseIntfPtr& dev);

public:
  // Reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE(EsBluetoothLEservice)
  ES_DECL_ATTR_HANDLING_STD

  /// Relected properties
  ///
  ES_DECL_PROPERTY_RO(isKnown, bool)
  ES_DECL_PROPERTY_RO(isOrphan, bool)
  ES_DECL_PROPERTY_RO(uuidShort, ulong)
  ES_DECL_PROPERTY_RO(uuid, EsVariant)
  ES_DECL_PROPERTY_RO(knownName, EsString)
  ES_DECL_PROPERTY_RO(characteristicsUUIDs, EsVariant)
  ES_DECL_PROPERTY_RO(characteristics, EsVariant)
  ES_DECL_PROPERTY_RO(device, EsVariant)

  /// Reflected services
  ///
  
  /// @brief Find characteristic by full UUID, or UUID16|UUID32
  ///
  /// @param  uuid  Full characteristic UUID, or UUID16|UUID32
  ///
  /// @return Found characteristic object, or an null, if not found
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, characteristicFind, cr_EsVariant);

  /// @brief Get characteristic by full UUID, or UUID16|UUID32
  ///
  /// @param  uuid  Full characteristic UUID, or UUID16|UUID32
  ///
  /// @return       Characteristic object. If not found, throw an exception
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, characteristicGet, cr_EsVariant);

  /// Reflected static services
  ///
  
  /// @brief  Return true if service full UUID, or UUID16|UUID32 is known
  ///
  /// @param  uuid  The full UUID, or UUID16|UUID32.
  ///
  /// @returns  True if known uuid, false if not.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isKnownUUID, cr_EsVariant);

  /// @brief  Return known service name by its full UUID,
  ///         or UUID16|UUID32
  ///
  /// @param  uuid  A full UUID, or UUID16|UUID32.
  ///
  /// @returns  A known service name, of empty string.
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, knownNameGetByUUID, cr_EsVariant);

  // Non-reflected stuff
  // 

  /// Find and return service characteristic by its UUID, or null ptr, if not found
  EsBluetoothLEcharacteristic* characteristicFind(const GUID& uuid) const;

  /// Get service characteristic by its UUID, throw exception, if not found
  EsBluetoothLEcharacteristic* characteristicGet(const GUID& uuid) const;

  /// Access LE device, this service is assigned to
  const EsBluetoothLEdevice* deviceGet() const;
  EsBluetoothLEdevice* deviceGet();

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
  virtual void characteristicsCleanup();

protected:
  EsUUID m_uuid;
  EsBaseIntfPtr m_dev;
  EsAssocContainerIntf::Ptr m_chxs;

  ES_NO_DEFAULT_CTR(EsBluetoothLEservice);
  ES_NON_COPYABLE(EsBluetoothLEservice);

  friend class EsBluetoothLEdevice;
};

/// Bluetooth LE device interface
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsBluetoothLEdevice, EsReflectedClassIntf)

protected:
  EsBluetoothLEdevice( 
    const EsBaseIntfPtr& radio,
    const EsString& addr,
    const EsString& name 
  );

public:
  // Reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE(EsBluetoothLEdevice)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected properties
  /// 
  ES_DECL_PROPERTY_RO(radio, EsVariant)
  ES_DECL_PROPERTY_RO(address, EsString)
  ES_DECL_PROPERTY_RO(name, EsString)
  ES_DECL_PROPERTY_RO(id, EsString)
  ES_DECL_PROPERTY_RO(recentRssi, long)
  ES_DECL_PROPERTY_RO(connected, bool)
  ES_DECL_PROPERTY_RO(servicesUUIDs, EsVariant)
  ES_DECL_PROPERTY_RO(services, EsVariant)

  /// Reflected services
  ///

  /// @brief  Return service by its full UUID, or null, if not found
  ///
  /// @param  uuid  The service UUID to look up to
  ///               
  /// @return Service object, if found, null, if not
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, serviceFind, cr_EsVariant);

  /// @brief  Return service by its full UUID, or throw an exception, if not found
  ///
  /// @param  uuid  The service UUID to look up to
  ///               
  /// @return Service object, if found. An exception is thrown
  ///         if service is not found in device.
  ///
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, serviceGet, cr_EsVariant);

  /// Query device RSSI
  ES_DECL_REFLECTED_METHOD0(void, rssiQueryReflected); //< Reflected alias of rssiQuery

  /// Disconnect device
  ES_DECL_REFLECTED_METHOD0(void, disconnectReflected); //< Reflected alias of disconnect

  /// Re-discover device services
  ES_DECL_REFLECTED_METHOD0(void, discoverReflected); //< Reflected alias of discover

  /// Prepare reliable write batch
  ES_DECL_REFLECTED_METHOD0(bool, reliableWriteBeginReflected); //< Reflected alias of reliableWriteBegin

  /// Abort reliable write batch
  ES_DECL_REFLECTED_METHOD0(void, reliableWriteAbortReflected); //< Reflected alias of reliableWriteAbort

  /// Execute reliable write batch
  ES_DECL_REFLECTED_METHOD0(bool, reliableWriteExecuteReflected); //< Reflected alias of reliableWriteExecute

  // Non-reflected stuff
  // 

  /// Return Bluetooth radio this LE device is connected to
  const EsBluetoothRadio* radioGet() const;  
  EsBluetoothRadio* radioGet();

  /// Return service by its full UUID, or null ptr, if not found
  EsBluetoothLEservice* serviceFind(const GUID& uuid) const;

  /// Return service by its full UUID, throw an exception, if not found
  EsBluetoothLEservice* serviceGet(const GUID& uuid) const;

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
  virtual void servicesCleanup();

protected:
  EsBaseIntfPtr m_radio;
  EsString m_addr;                    ///< Device address in form of a string
  EsString m_name;                    ///< Device name
  long m_rssi;                        ///< Recently acquired RSSI value
  EsAssocContainerIntf::Ptr m_svcs;

  ES_NO_DEFAULT_CTR(EsBluetoothLEdevice);
  ES_NON_COPYABLE(EsBluetoothLEdevice);
};

/// Bluetooth LE device enumerator base class
///
class ESCOMM_CLASS ES_INTF_IMPL1(EsBluetoothLEdeviceEnumerator, EsReflectedClassIntf)

public:
  /// Enumerator constructor.
  /// flags should be combination of EsBluetoothDeviceFlag values,
  /// and restricts looked-up devices
  ///
  EsBluetoothLEdeviceEnumerator( const EsBaseIntfPtr& radio );

  /// Reflection declaration
  ///
  ES_DECL_REFLECTED_CLASS_BASE(EsBluetoothLEdeviceEnumerator)
  ES_DECL_ATTR_HANDLING_STD

  /// Reflected constructor
  /// 
  ES_DECL_REFLECTED_CLASS_CTOR1(EsVariant, cr_EsVariant);

  /// Reflected properties
  /// 
  ES_DECL_PROPERTY_RO(radio, EsVariant)
  ES_DECL_PROPERTY_RO(devicesIDs, EsVariant)
  ES_DECL_PROPERTY_RO(devices, EsVariant)

  /// Reflected services
  /// 

  /// @brief Execute Bluetooth LE device enumeration, optionally breaking one, currently active.
  /// 
  /// @param doInquiry    True|false - perform device services inquiry upon enumeration
  /// @param inquiryTmo   Inquiry timeout (if requested) in milliseconds
  ///                     
  ES_DECL_REFLECTED_METHOD2(void, enumerateReflected, cr_EsVariant, cr_EsVariant);

  /// @brief Stop an ungoing enumeration, if any.
  ///        
  ES_DECL_REFLECTED_METHOD0(void, stop);

  /// @brief Stop an ungoing enumeration, if any, and reset already enumerated devices collection
  ///        
  ES_DECL_REFLECTED_METHOD0(void, reset);

  /// @brief Find enumerated device by its name string.
  ///
  /// @param name   A name string to find device by.
  ///
  /// @return Found device object, null if not found.
  ///         
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, deviceFindByNameReflected, cr_EsString);

  /// @brief Find enumerated device by its platform-specific ID string.
  ///
  /// @param name   An ID string to find device by.
  ///
  /// @return Found device object, null if not found.
  ///         
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, deviceFindByIDreflected, cr_EsString);

  /// @brief Find enumerated device by its platform-specific ID string.
  ///
  /// @param name   An address string to find device by.
  ///
  /// @return Found device object, null if not found.
  ///         
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, deviceFindByAddressReflected, cr_EsString);

  /// Non-reflected stuff
  ///
  void enumerate(bool doInquiry, ulong inquiryTmo);

  /// Find enumerated device by its name string. Return null if not found
  EsBluetoothLEdevice* deviceFindByName(const EsString& name) const;

  /// Find enumerated device by its platform-specific ID string. Return null if not found
  EsBluetoothLEdevice* deviceFindByID(const EsString& id) const;

  /// Find enumerated device by its address string. Return null if not found
  EsBluetoothLEdevice* deviceFindByAddress(const EsString& address) const;

protected:
  /// To be implemented in platform-specific code
  /// 
  void enumerate();

  /// Device enumeration callback
  /// If false is returned, enumeration process is cancelled
  ///
  bool onDeviceEnumerated(const EsReflectedClassIntf::Ptr& pdev);

protected:
  /// Data members
  ///
  EsBaseIntfPtr m_radio;
  ulong m_flags;
  bool m_doInquiry;
  ulong m_inquiryTmo;
  /// Enumerated devices
  EsAssocContainerIntf::Ptr m_devs;

  ES_NO_DEFAULT_CTR(EsBluetoothLEdeviceEnumerator);
  ES_NON_COPYABLE(EsBluetoothLEdeviceEnumerator);
};

/// Bluetooth LE static services
///
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsBluetoothLE, ESCOMM_CLASS)

  /// Global initialization
  ES_DECL_REFLECTED_CLASS_METHOD0( void, initialize );

  /// Global initialization check
  ES_DECL_REFLECTED_CLASS_METHOD0( bool, initialized );

  /// Global uninitialization
  ES_DECL_REFLECTED_CLASS_METHOD0( void, uninitialize );

  /// Return Bluetooth LE result code string by code value
  ES_DECL_REFLECTED_CLASS_METHOD1( EsString, resultCodeStringGetReflected, cr_EsVariant ); //< Reflected alias of resultCodeStringGet

  /// Return list of enumerated BLE radios (Alias of EsBluetoothRadioEnumerator::enumerate, filtered by LE support)
  ES_DECL_REFLECTED_CLASS_METHOD0( EsVariant, radiosGet );

  // Non-reflected stuff
  static EsString resultCodeStringGet(EsBluetoothLEresult code);

ES_DECL_REFLECTED_SERVICES_END

/// Reflected enumerations
///
namespace EsReflection
{

ES_DECL_ENUMERATION_BEGIN(EsBluetoothLEformatType)
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Reserved,                               static_cast<ulong>(::EsBluetoothLEformatType::Reserved),                                    _i("Reserved") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Boolean,                                static_cast<ulong>(::EsBluetoothLEformatType::Boolean),                                     _i("Boolean") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned2bitInteger,                    static_cast<ulong>(::EsBluetoothLEformatType::Unsigned2bitInteger),                         _i("Unsigned 2bit Integer") )         
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned4bitInteger,                    static_cast<ulong>(::EsBluetoothLEformatType::Unsigned4bitInteger),                         _i("Unsigned 4bit Integer") )         
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned8bitInteger,                    static_cast<ulong>(::EsBluetoothLEformatType::Unsigned8bitInteger),                         _i("Unsigned 8bit Integer") )         
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned12bitInteger,                   static_cast<ulong>(::EsBluetoothLEformatType::Unsigned12bitInteger),                        _i("Unsigned 12bit Integer") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned16bitInteger,                   static_cast<ulong>(::EsBluetoothLEformatType::Unsigned16bitInteger),                        _i("Unsigned 16bit Integer") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned24bitInteger,                   static_cast<ulong>(::EsBluetoothLEformatType::Unsigned24bitInteger),                        _i("Unsigned 24bit Integer") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned32bitInteger,                   static_cast<ulong>(::EsBluetoothLEformatType::Unsigned32bitInteger),                        _i("Unsigned 32bit Integer") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned48bitInteger,                   static_cast<ulong>(::EsBluetoothLEformatType::Unsigned48bitInteger),                        _i("Unsigned 48bit Integer") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned64bitInteger,                   static_cast<ulong>(::EsBluetoothLEformatType::Unsigned64bitInteger),                        _i("Unsigned 64bit Integer") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Unsigned128bitInteger,                  static_cast<ulong>(::EsBluetoothLEformatType::Unsigned128bitInteger),                       _i("Unsigned 128bit Integer") )       
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed8bitInteger,                      static_cast<ulong>(::EsBluetoothLEformatType::Signed8bitInteger),                           _i("Signed 8bit Integer") )           
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed12bitInteger,                     static_cast<ulong>(::EsBluetoothLEformatType::Signed12bitInteger),                          _i("Signed 12bit Integer") )          
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed16bitInteger,                     static_cast<ulong>(::EsBluetoothLEformatType::Signed16bitInteger),                          _i("Signed 16bit Integer") )          
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed24bitInteger,                     static_cast<ulong>(::EsBluetoothLEformatType::Signed24bitInteger),                          _i("Signed 24bit Integer") )          
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed32bitInteger,                     static_cast<ulong>(::EsBluetoothLEformatType::Signed32bitInteger),                          _i("Signed 32bit Integer") )          
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed48bitInteger,                     static_cast<ulong>(::EsBluetoothLEformatType::Signed48bitInteger),                          _i("Signed 48bit Integer") )          
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed64bitInteger,                     static_cast<ulong>(::EsBluetoothLEformatType::Signed64bitInteger),                          _i("Signed 64bit Integer") )          
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Signed128bitInteger,                    static_cast<ulong>(::EsBluetoothLEformatType::Signed128bitInteger),                         _i("Signed 128bit Integer") )         
  ES_DECL_ENUMERATION_ITEM_LABELLED(  IEEE754_32bit_floating_point,           static_cast<ulong>(::EsBluetoothLEformatType::IEEE754_32bit_floating_point),                _i("IEEE754 32bit Floating point") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  IEEE754_64bit_floating_point,           static_cast<ulong>(::EsBluetoothLEformatType::IEEE754_64bit_floating_point),                _i("IEEE754 64bit Floating point") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  IEEE11073_16bitSFLOAT,                  static_cast<ulong>(::EsBluetoothLEformatType::IEEE11073_16bitSFLOAT),                       _i("IEEE11073 16bit Short Floating point") )       
  ES_DECL_ENUMERATION_ITEM_LABELLED(  IEEE11073_32bitFLOAT,                   static_cast<ulong>(::EsBluetoothLEformatType::IEEE11073_32bitFLOAT),                        _i("IEEE11073 32bit Floating point") )        
  ES_DECL_ENUMERATION_ITEM_LABELLED(  IEEE20601Format,                        static_cast<ulong>(::EsBluetoothLEformatType::IEEE20601Format),                             _i("IEEE20601 Format") )             
  ES_DECL_ENUMERATION_ITEM_LABELLED(  UTF8String,                             static_cast<ulong>(::EsBluetoothLEformatType::UTF8String),                                  _i("UTF8 String") )                  
  ES_DECL_ENUMERATION_ITEM_LABELLED(  UTF16String,                            static_cast<ulong>(::EsBluetoothLEformatType::UTF16String),                                 _i("UTF16 String") )                 
  ES_DECL_ENUMERATION_ITEM_LABELLED(  OpaqueStructure,                        static_cast<ulong>(::EsBluetoothLEformatType::OpaqueStructure),                             _i("Opaque Structure") )              
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsBluetoothLEscanResponseKey)
  ES_DECL_ENUMERATION_ITEM(  Flags,                                           static_cast<ulong>(::EsBluetoothLEscanResponseKey::Flags) )
  ES_DECL_ENUMERATION_ITEM(  IncompleteList16SCUUID,                          static_cast<ulong>(::EsBluetoothLEscanResponseKey::IncompleteList16SCUUID) )
  ES_DECL_ENUMERATION_ITEM(  CompleteList16SCUUID,                            static_cast<ulong>(::EsBluetoothLEscanResponseKey::CompleteList16SCUUID) )
  ES_DECL_ENUMERATION_ITEM(  IncompleteList32SCUUID,                          static_cast<ulong>(::EsBluetoothLEscanResponseKey::IncompleteList32SCUUID) )
  ES_DECL_ENUMERATION_ITEM(  CompleteList32SCUUID,                            static_cast<ulong>(::EsBluetoothLEscanResponseKey::CompleteList32SCUUID) )
  ES_DECL_ENUMERATION_ITEM(  IncompleteList128SCUUID,                         static_cast<ulong>(::EsBluetoothLEscanResponseKey::IncompleteList128SCUUID) )
  ES_DECL_ENUMERATION_ITEM(  CompleteList128SCUUID,                           static_cast<ulong>(::EsBluetoothLEscanResponseKey::CompleteList128SCUUID) )
  ES_DECL_ENUMERATION_ITEM(  ShortenedLocalName,                              static_cast<ulong>(::EsBluetoothLEscanResponseKey::ShortenedLocalName) )
  ES_DECL_ENUMERATION_ITEM(  CompleteLocalName,                               static_cast<ulong>(::EsBluetoothLEscanResponseKey::CompleteLocalName) )
  ES_DECL_ENUMERATION_ITEM(  TxPowerLevel,                                    static_cast<ulong>(::EsBluetoothLEscanResponseKey::TxPowerLevel) )
  ES_DECL_ENUMERATION_ITEM(  ClassOfDevice,                                   static_cast<ulong>(::EsBluetoothLEscanResponseKey::ClassOfDevice) )
  ES_DECL_ENUMERATION_ITEM(  SimplePairingHashC,                              static_cast<ulong>(::EsBluetoothLEscanResponseKey::SimplePairingHashC) )
  ES_DECL_ENUMERATION_ITEM(  SimplePairingRAndomizerR,                        static_cast<ulong>(::EsBluetoothLEscanResponseKey::SimplePairingRAndomizerR) )
  ES_DECL_ENUMERATION_ITEM(  DeviceID,                                        static_cast<ulong>(::EsBluetoothLEscanResponseKey::DeviceID) )
  ES_DECL_ENUMERATION_ITEM(  SecurityManagerOutOfBandFlags,                   static_cast<ulong>(::EsBluetoothLEscanResponseKey::SecurityManagerOutOfBandFlags) )
  ES_DECL_ENUMERATION_ITEM(  SlaveConnectionIntervalRange,                    static_cast<ulong>(::EsBluetoothLEscanResponseKey::SlaveConnectionIntervalRange) )
  ES_DECL_ENUMERATION_ITEM(  List16bServiceSolicitationUUIDs,                 static_cast<ulong>(::EsBluetoothLEscanResponseKey::List16bServiceSolicitationUUIDs) )
  ES_DECL_ENUMERATION_ITEM(  List32bServiceSolicitationUUIDs,                 static_cast<ulong>(::EsBluetoothLEscanResponseKey::List32bServiceSolicitationUUIDs) )
  ES_DECL_ENUMERATION_ITEM(  List128bServiceSolicitationUUIDs,                static_cast<ulong>(::EsBluetoothLEscanResponseKey::List128bServiceSolicitationUUIDs) )
  ES_DECL_ENUMERATION_ITEM(  ServiceData,                                     static_cast<ulong>(::EsBluetoothLEscanResponseKey::ServiceData) )
  ES_DECL_ENUMERATION_ITEM(  ServiceData16b,                                  static_cast<ulong>(::EsBluetoothLEscanResponseKey::ServiceData16b) )
  ES_DECL_ENUMERATION_ITEM(  ServiceData32b,                                  static_cast<ulong>(::EsBluetoothLEscanResponseKey::ServiceData32b) )
  ES_DECL_ENUMERATION_ITEM(  ServiceData128b,                                 static_cast<ulong>(::EsBluetoothLEscanResponseKey::ServiceData128b) )
  ES_DECL_ENUMERATION_ITEM(  SecureConnectionsConfirmationVal,                static_cast<ulong>(::EsBluetoothLEscanResponseKey::SecureConnectionsConfirmationVal) )
  ES_DECL_ENUMERATION_ITEM(  LEsecureConnectionRandomVal,                     static_cast<ulong>(::EsBluetoothLEscanResponseKey::LEsecureConnectionRandomVal) )
  ES_DECL_ENUMERATION_ITEM(  PublicTargetAddress,                             static_cast<ulong>(::EsBluetoothLEscanResponseKey::PublicTargetAddress) )
  ES_DECL_ENUMERATION_ITEM(  RandomTargetAddress,                             static_cast<ulong>(::EsBluetoothLEscanResponseKey::RandomTargetAddress) )
  ES_DECL_ENUMERATION_ITEM(  Appearance,                                      static_cast<ulong>(::EsBluetoothLEscanResponseKey::Appearance) )
  ES_DECL_ENUMERATION_ITEM(  AdvertisingInterval,                             static_cast<ulong>(::EsBluetoothLEscanResponseKey::AdvertisingInterval) )
  ES_DECL_ENUMERATION_ITEM(  LEBluetoothDeviceAddress,                        static_cast<ulong>(::EsBluetoothLEscanResponseKey::LEBluetoothDeviceAddress) )
  ES_DECL_ENUMERATION_ITEM(  LERole,                                          static_cast<ulong>(::EsBluetoothLEscanResponseKey::LERole) )
  ES_DECL_ENUMERATION_ITEM(  SimplePairingHashc256,                           static_cast<ulong>(::EsBluetoothLEscanResponseKey::SimplePairingHashc256) )
  ES_DECL_ENUMERATION_ITEM(  SimplePairingRAndomizerR256,                     static_cast<ulong>(::EsBluetoothLEscanResponseKey::SimplePairingRAndomizerR256) )
  ES_DECL_ENUMERATION_ITEM(  InformationData3D,                               static_cast<ulong>(::EsBluetoothLEscanResponseKey::InformationData3D) )
  ES_DECL_ENUMERATION_ITEM(  ManufacturerSpecificData,                        static_cast<ulong>(::EsBluetoothLEscanResponseKey::ManufacturerSpecificData) )
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsBluetoothLEresult)
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Success,                                static_cast<ulong>(::EsBluetoothLEresult::gattSuccess),                                      _i("Success") )                  
  ES_DECL_ENUMERATION_ITEM_LABELLED(  ReadNotPermitted,                       static_cast<ulong>(::EsBluetoothLEresult::gattReadNotPermitted),                             _i("Read Not Permitted") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  WriteNotPermitted,                      static_cast<ulong>(::EsBluetoothLEresult::gattWriteNotPermitted),                            _i("Write Not Permitted") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  InsufficientAutentication,              static_cast<ulong>(::EsBluetoothLEresult::gattInsufficientAutentication),                    _i("Insufficient Autentication") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  RequestNotSupported,                    static_cast<ulong>(::EsBluetoothLEresult::gattRequestNotSupported),                          _i("Request Not Supported") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  InvalidOffset,                          static_cast<ulong>(::EsBluetoothLEresult::gattInvalidOffset),                                _i("Invalid Offset") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  InvalidAttributeLength,                 static_cast<ulong>(::EsBluetoothLEresult::gattInvalidAttributeLength),                       _i("Invalid Attribute Length") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  InsufficientEncryption,                 static_cast<ulong>(::EsBluetoothLEresult::gattInsufficientEncryption),                       _i("Insufficient Encryption") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Failure,                                static_cast<ulong>(::EsBluetoothLEresult::gattFailure),                                      _i("Failure") )             
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsBluetoothLEdescriptorKind)
# define ES_BLE_DESCRIPTOR_BEGIN
# define ES_BLE_DESCRIPTOR_ENTRY(Name, Descr, Uuid) ES_DECL_ENUMERATION_ITEM( Name, static_cast<ulong>(::EsBluetoothLEdescriptorKind::## Name) )
# define ES_BLE_DESCRIPTOR_END                      ES_DECL_ENUMERATION_ITEM( Unknown,  static_cast<ulong>(::EsBluetoothLEdescriptorKind::Unknown) )
# include "EsBluetoothLEdescriptor.defs.h"
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsBluetoothLEcharacteristicProperty)
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Broadcast,                              static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::Broadcast),                        _i("The characteristic is broadcastable") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  ExtendedProps,                          static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::ExtendedProps),                    _i("The characteristic has extended properties") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Notify,                                 static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::Notify),                           _i("The characteristic supports notification") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Indicate,                               static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::Indicate),                         _i("The characteristic supports indication") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Read,                                   static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::Read),                             _i("The characteristic is readable") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  Write,                                  static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::Write),                            _i("The characteristic can be written") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  WriteNoResponse,                        static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::WriteNoResponse),                  _i("The characteristic can be written without response") )
  ES_DECL_ENUMERATION_ITEM_LABELLED(  SignedWrite,                            static_cast<ulong>(::EsBluetoothLEcharacteristicProperty::SignedWrite),                      _i("The characteristic supports writing with signature") )
ES_DECL_ENUMERATION_END

}

#endif // ES_COMM_USE_BLUETOOTH_LE

#endif // _es_bluetooth_le_h_
