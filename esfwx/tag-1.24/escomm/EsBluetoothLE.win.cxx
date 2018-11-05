// Windows-specific implementation of Bluetooth LE classes
// Do not include explicitly in project. This file is included implicitly
// from EsBluetoothLE.cpp
//
#include "EsBluetooth.win.h"

#if defined(ES_DEBUG) && defined(ES_USE_BLE_TRACE)
# define ES_BLE_TRACE_RESULT(msg, result) ES_DEBUG_TRACE(msg, result);
#else
# define ES_BLE_TRACE_RESULT(msg, result)
#endif
//---------------------------------------------------------------------------

DEFINE_DEVPROPKEY(DEVPKEY_Device_FriendlyName,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);    // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_DevNodeStatus,          0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 2);     // DEVPROP_TYPE_UINT32
//---------------------------------------------------------------------------

#define ES_ERROR_BLE_DEV_ENUM_S \
  _("Error while enumerating Bluetooth LE devices: '%s'")

#define ES_ERROR_BLE_COULD_NOT_GET_SVC_HANDLE_S \
  _("Could not get Bluetooth LE service handle: '%s'")

//---------------------------------------------------------------------------
#include <bluetoothleapis.h>
#include <cfg.h>
//
// Bluetooth Attribute Protocol Warnings
//
//
// MessageId: E_BLUETOOTH_ATT_INVALID_HANDLE
//
// MessageText:
//
// The attribute handle given was not valid on this server.
//
#define E_BLUETOOTH_ATT_INVALID_HANDLE   _HRESULT_TYPEDEF_(0x80650001L)

//
// MessageId: E_BLUETOOTH_ATT_READ_NOT_PERMITTED
//
// MessageText:
//
// The attribute cannot be read.
//
#define E_BLUETOOTH_ATT_READ_NOT_PERMITTED _HRESULT_TYPEDEF_(0x80650002L)

//
// MessageId: E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED
//
// MessageText:
//
// The attribute cannot be written.
//
#define E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED _HRESULT_TYPEDEF_(0x80650003L)

//
// MessageId: E_BLUETOOTH_ATT_INVALID_PDU
//
// MessageText:
//
// The attribute PDU was invalid.
//
#define E_BLUETOOTH_ATT_INVALID_PDU      _HRESULT_TYPEDEF_(0x80650004L)

//
// MessageId: E_BLUETOOTH_ATT_INSUFFICIENT_AUTHENTICATION
//
// MessageText:
//
// The attribute requires authentication before it can be read or written.
//
#define E_BLUETOOTH_ATT_INSUFFICIENT_AUTHENTICATION _HRESULT_TYPEDEF_(0x80650005L)

//
// MessageId: E_BLUETOOTH_ATT_REQUEST_NOT_SUPPORTED
//
// MessageText:
//
// Attribute server does not support the request received from the client.
//
#define E_BLUETOOTH_ATT_REQUEST_NOT_SUPPORTED _HRESULT_TYPEDEF_(0x80650006L)

//
// MessageId: E_BLUETOOTH_ATT_INVALID_OFFSET
//
// MessageText:
//
// Offset specified was past the end of the attribute.
//
#define E_BLUETOOTH_ATT_INVALID_OFFSET   _HRESULT_TYPEDEF_(0x80650007L)

//
// MessageId: E_BLUETOOTH_ATT_INSUFFICIENT_AUTHORIZATION
//
// MessageText:
//
// The attribute requires authorization before it can be read or written.
//
#define E_BLUETOOTH_ATT_INSUFFICIENT_AUTHORIZATION _HRESULT_TYPEDEF_(0x80650008L)

//
// MessageId: E_BLUETOOTH_ATT_PREPARE_QUEUE_FULL
//
// MessageText:
//
// Too many prepare writes have been queued.
//
#define E_BLUETOOTH_ATT_PREPARE_QUEUE_FULL _HRESULT_TYPEDEF_(0x80650009L)

//
// MessageId: E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND
//
// MessageText:
//
// No attribute found within the given attribute handle range.
//
#define E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND _HRESULT_TYPEDEF_(0x8065000AL)

//
// MessageId: E_BLUETOOTH_ATT_ATTRIBUTE_NOT_LONG
//
// MessageText:
//
// The attribute cannot be read or written using the Read Blob Request.
//
#define E_BLUETOOTH_ATT_ATTRIBUTE_NOT_LONG _HRESULT_TYPEDEF_(0x8065000BL)

//
// MessageId: E_BLUETOOTH_ATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE
//
// MessageText:
//
// The Encryption Key Size used for encrypting this link is insufficient.
//
#define E_BLUETOOTH_ATT_INSUFFICIENT_ENCRYPTION_KEY_SIZE _HRESULT_TYPEDEF_(0x8065000CL)

//
// MessageId: E_BLUETOOTH_ATT_INVALID_ATTRIBUTE_VALUE_LENGTH
//
// MessageText:
//
// The attribute value length is invalid for the operation.
//
#define E_BLUETOOTH_ATT_INVALID_ATTRIBUTE_VALUE_LENGTH _HRESULT_TYPEDEF_(0x8065000DL)

//
// MessageId: E_BLUETOOTH_ATT_UNLIKELY
//
// MessageText:
//
// The attribute request that was requested has encountered an error that was unlikely, and therefore could not be completed as requested.
//
#define E_BLUETOOTH_ATT_UNLIKELY         _HRESULT_TYPEDEF_(0x8065000EL)

//
// MessageId: E_BLUETOOTH_ATT_INSUFFICIENT_ENCRYPTION
//
// MessageText:
//
// The attribute requires encryption before it can be read or written.
//
#define E_BLUETOOTH_ATT_INSUFFICIENT_ENCRYPTION _HRESULT_TYPEDEF_(0x8065000FL)

//
// MessageId: E_BLUETOOTH_ATT_UNSUPPORTED_GROUP_TYPE
//
// MessageText:
//
// The attribute type is not a supported grouping attribute as defined by a higher layer specification.
//
#define E_BLUETOOTH_ATT_UNSUPPORTED_GROUP_TYPE _HRESULT_TYPEDEF_(0x80650010L)

//
// MessageId: E_BLUETOOTH_ATT_INSUFFICIENT_RESOURCES
//
// MessageText:
//
// Insufficient Resources to complete the request.
//
#define E_BLUETOOTH_ATT_INSUFFICIENT_RESOURCES _HRESULT_TYPEDEF_(0x80650011L)

//
// MessageId: E_BLUETOOTH_ATT_UNKNOWN_ERROR
//
// MessageText:
//
// An error that lies in the reserved range has been received.
//
#define E_BLUETOOTH_ATT_UNKNOWN_ERROR    _HRESULT_TYPEDEF_(0x80651000L)
//---------------------------------------------------------------------------

// BLE APIs library wrapper
//
class EsBtLEapiLib
{
public:
  typedef VOID (WINAPI *pfnGATTEvent)(
    BTH_LE_GATT_EVENT_TYPE              EventType,
    PVOID                               EventOutParameter,
    PVOID                               Context
  );

protected:
  typedef HRESULT (WINAPI *pfnBthpGATTCloseSession)(
    HANDLE                              hDevice,
    DWORD                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTGetServices)(
    HANDLE                              hDevice,
    USHORT                              ServicesBufferCount,
    PBTH_LE_GATT_SERVICE                ServicesBuffer,
    USHORT*                             ServicesBufferActual,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTGetIncludedServices)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_SERVICE                ParentService,
    USHORT                              IncludedServicesBufferCount,
    PBTH_LE_GATT_SERVICE                IncludedServicesBuffer,
    USHORT*                             IncludedServicesBufferActual,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTGetCharacteristics)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_SERVICE                Service,
    USHORT                              CharacteristicsBufferCount,
    PBTH_LE_GATT_CHARACTERISTIC         CharacteristicsBuffer,
    USHORT*                             CharacteristicsBufferActual,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTGetDescriptors)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_CHARACTERISTIC         Characteristic,
    USHORT                              DescriptorsBufferCount,
    PBTH_LE_GATT_DESCRIPTOR             DescriptorsBuffer,
    USHORT*                             DescriptorsBufferActual,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTGetCharacteristicValue)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_CHARACTERISTIC         Characteristic,
    ULONG                               CharacteristicValueDataSize,
    PBTH_LE_GATT_CHARACTERISTIC_VALUE   CharacteristicValue,
    USHORT*                             CharacteristicValueSizeRequired,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTGetDescriptorValue)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_DESCRIPTOR             Descriptor,
    ULONG                               DescriptorValueDataSize,
    PBTH_LE_GATT_DESCRIPTOR_VALUE       DescriptorValue,
    USHORT*                             DescriptorValueSizeRequired,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTBeginReliableWrite)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_RELIABLE_WRITE_CONTEXT ReliableWriteContext,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTEndReliableWrite)(
    HANDLE                              hDevice,
    BTH_LE_GATT_RELIABLE_WRITE_CONTEXT  ReliableWriteContext,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTAbortReliableWrite)(
    HANDLE                              hDevice,
    BTH_LE_GATT_RELIABLE_WRITE_CONTEXT  ReliableWriteContext,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTSetCharacteristicValue)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_CHARACTERISTIC         Characteristic,
    PBTH_LE_GATT_CHARACTERISTIC_VALUE   CharacteristicValue,
    BTH_LE_GATT_RELIABLE_WRITE_CONTEXT  ReliableWriteContext,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTSetDescriptorValue)(
    HANDLE                              hDevice,
    PBTH_LE_GATT_DESCRIPTOR             Descriptor,
    PBTH_LE_GATT_DESCRIPTOR_VALUE       DescriptorValue,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTRegisterEvent)(
    HANDLE                              hService,
    BTH_LE_GATT_EVENT_TYPE              EventType,
    PVOID                               EventParameterIn,
    pfnGATTEvent                        Callback,
    PVOID                               CallbackContext,
    BLUETOOTH_GATT_EVENT_HANDLE*        pEventHandle,
    ULONG                               Flags
  );

  typedef HRESULT (WINAPI *pfnBluetoothGATTUnregisterEvent)(
    BLUETOOTH_GATT_EVENT_HANDLE         EventHandle,
    ULONG                               Flags
  );

public:
  EsBtLEapiLib() :
  m_pfnBthpGATTCloseSession(0),
  m_pfnBluetoothGATTGetServices(0),
  m_pfnBluetoothGATTGetIncludedServices(0),
  m_pfnBluetoothGATTGetCharacteristics(0),
  m_pfnBluetoothGATTGetDescriptors(0),
  m_pfnBluetoothGATTGetCharacteristicValue(0),
  m_pfnBluetoothGATTGetDescriptorValue(0),
  m_pfnBluetoothGATTBeginReliableWrite(0),
  m_pfnBluetoothGATTEndReliableWrite(0),
  m_pfnBluetoothGATTAbortReliableWrite(0),
  m_pfnBluetoothGATTSetCharacteristicValue(0),
  m_pfnBluetoothGATTSetDescriptorValue(0),
  m_pfnBluetoothGATTRegisterEvent(0),
  m_pfnBluetoothGATTUnregisterEvent(0)
  {}

  ~EsBtLEapiLib()
  {
    unload();
  }

  static EsBtLEapiLib& inst()
  {
    static EsBtLEapiLib s_btlib;
    return s_btlib;
  }

  void initializedCheck()
  {
    if( !m_lib )
      EsException::Throw(_("Bluetooth LE API engine is not initialized"));
  }

  void load()
  {
    if( !m_lib )
      m_lib = EsDynamicLibrary::load(esT("bluetoothapis.dll"), true);

    ES_ASSERT(m_lib);
    m_pfnBthpGATTCloseSession = (pfnBthpGATTCloseSession)m_lib->procAddrGet(esT("BthpGATTCloseSession"));
    m_pfnBluetoothGATTGetServices = (pfnBluetoothGATTGetServices)m_lib->procAddrGet(esT("BluetoothGATTGetServices"));
    m_pfnBluetoothGATTGetIncludedServices = (pfnBluetoothGATTGetIncludedServices)m_lib->procAddrGet(esT("BluetoothGATTGetIncludedServices"));
    m_pfnBluetoothGATTGetCharacteristics = (pfnBluetoothGATTGetCharacteristics)m_lib->procAddrGet(esT("BluetoothGATTGetCharacteristics"));
    m_pfnBluetoothGATTGetDescriptors = (pfnBluetoothGATTGetDescriptors)m_lib->procAddrGet(esT("BluetoothGATTGetDescriptors"));
    m_pfnBluetoothGATTGetCharacteristicValue = (pfnBluetoothGATTGetCharacteristicValue)m_lib->procAddrGet(esT("BluetoothGATTGetCharacteristicValue"));
    m_pfnBluetoothGATTGetDescriptorValue = (pfnBluetoothGATTGetDescriptorValue)m_lib->procAddrGet(esT("BluetoothGATTGetDescriptorValue"));
    m_pfnBluetoothGATTBeginReliableWrite = (pfnBluetoothGATTBeginReliableWrite)m_lib->procAddrGet(esT("BluetoothGATTBeginReliableWrite"));
    m_pfnBluetoothGATTEndReliableWrite = (pfnBluetoothGATTEndReliableWrite)m_lib->procAddrGet(esT("BluetoothGATTEndReliableWrite"));
    m_pfnBluetoothGATTAbortReliableWrite = (pfnBluetoothGATTAbortReliableWrite)m_lib->procAddrGet(esT("BluetoothGATTAbortReliableWrite"));
    m_pfnBluetoothGATTSetCharacteristicValue = (pfnBluetoothGATTSetCharacteristicValue)m_lib->procAddrGet(esT("BluetoothGATTSetCharacteristicValue"));
    m_pfnBluetoothGATTSetDescriptorValue = (pfnBluetoothGATTSetDescriptorValue)m_lib->procAddrGet(esT("BluetoothGATTSetDescriptorValue"));
    m_pfnBluetoothGATTRegisterEvent = (pfnBluetoothGATTRegisterEvent)m_lib->procAddrGet(esT("BluetoothGATTRegisterEvent"));
    m_pfnBluetoothGATTUnregisterEvent = (pfnBluetoothGATTUnregisterEvent)m_lib->procAddrGet(esT("BluetoothGATTUnregisterEvent"));
  }

  void unload()
  {
    if( m_lib )
    {
      m_pfnBthpGATTCloseSession = 0;
      m_pfnBluetoothGATTGetServices = 0;
      m_pfnBluetoothGATTGetIncludedServices = 0;
      m_pfnBluetoothGATTGetCharacteristics = 0;
      m_pfnBluetoothGATTGetDescriptors = 0;
      m_pfnBluetoothGATTGetCharacteristicValue = 0;
      m_pfnBluetoothGATTGetDescriptorValue = 0;
      m_pfnBluetoothGATTBeginReliableWrite = 0;
      m_pfnBluetoothGATTEndReliableWrite = 0;
      m_pfnBluetoothGATTAbortReliableWrite = 0;
      m_pfnBluetoothGATTSetCharacteristicValue = 0;
      m_pfnBluetoothGATTSetDescriptorValue = 0;
      m_pfnBluetoothGATTRegisterEvent = 0;
      m_pfnBluetoothGATTUnregisterEvent = 0;

      m_lib->unload();
      m_lib.reset();
    }
  }

  bool initialized() const { return m_lib && m_lib->isOk(); }

  // Mapped library services
  //
  HRESULT BthpGATTCloseSession(
    HANDLE                              hDevice,
    DWORD                               Flags
  )
  {
    initializedCheck();
    return m_pfnBthpGATTCloseSession(hDevice, Flags);
  }

  HRESULT BluetoothGATTGetServices(
    HANDLE                              hDevice,
    USHORT                              ServicesBufferCount,
    PBTH_LE_GATT_SERVICE                ServicesBuffer,
    USHORT*                             ServicesBufferActual,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTGetServices(hDevice, ServicesBufferCount, ServicesBuffer,
      ServicesBufferActual, Flags);
  }

  HRESULT BluetoothGATTGetIncludedServices(
    HANDLE                              hDevice,
    PBTH_LE_GATT_SERVICE                ParentService,
    USHORT                              IncludedServicesBufferCount,
    PBTH_LE_GATT_SERVICE                IncludedServicesBuffer,
    USHORT*                             IncludedServicesBufferActual,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTGetIncludedServices(hDevice, ParentService,
      IncludedServicesBufferCount, IncludedServicesBuffer, IncludedServicesBufferActual,
      Flags);
  }

  HRESULT BluetoothGATTGetCharacteristics(
    HANDLE                              hDevice,
    PBTH_LE_GATT_SERVICE                Service,
    USHORT                              CharacteristicsBufferCount,
    PBTH_LE_GATT_CHARACTERISTIC         CharacteristicsBuffer,
    USHORT*                             CharacteristicsBufferActual,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTGetCharacteristics(hDevice, Service, CharacteristicsBufferCount,
      CharacteristicsBuffer, CharacteristicsBufferActual, Flags);
  }

  HRESULT BluetoothGATTGetDescriptors(
    HANDLE                              hDevice,
    PBTH_LE_GATT_CHARACTERISTIC         Characteristic,
    USHORT                              DescriptorsBufferCount,
    PBTH_LE_GATT_DESCRIPTOR             DescriptorsBuffer,
    USHORT*                             DescriptorsBufferActual,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTGetDescriptors(hDevice, Characteristic, DescriptorsBufferCount,
      DescriptorsBuffer, DescriptorsBufferActual, Flags);
  }

  HRESULT BluetoothGATTGetCharacteristicValue(
    HANDLE                              hDevice,
    PBTH_LE_GATT_CHARACTERISTIC         Characteristic,
    ULONG                               CharacteristicValueDataSize,
    PBTH_LE_GATT_CHARACTERISTIC_VALUE   CharacteristicValue,
    USHORT*                             CharacteristicValueSizeRequired,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTGetCharacteristicValue(hDevice, Characteristic,
      CharacteristicValueDataSize, CharacteristicValue, CharacteristicValueSizeRequired,
      Flags);
  }

  HRESULT BluetoothGATTGetDescriptorValue(
    HANDLE                              hDevice,
    PBTH_LE_GATT_DESCRIPTOR             Descriptor,
    ULONG                               DescriptorValueDataSize,
    PBTH_LE_GATT_DESCRIPTOR_VALUE       DescriptorValue,
    USHORT*                             DescriptorValueSizeRequired,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTGetDescriptorValue(hDevice, Descriptor, DescriptorValueDataSize,
      DescriptorValue, DescriptorValueSizeRequired, Flags);
  }

  HRESULT BluetoothGATTBeginReliableWrite(
    HANDLE                              hDevice,
    PBTH_LE_GATT_RELIABLE_WRITE_CONTEXT ReliableWriteContext,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTBeginReliableWrite(hDevice, ReliableWriteContext, Flags);
  }

  HRESULT BluetoothGATTEndReliableWrite(
    HANDLE                              hDevice,
    BTH_LE_GATT_RELIABLE_WRITE_CONTEXT  ReliableWriteContext,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTEndReliableWrite(hDevice, ReliableWriteContext, Flags);
  }

  HRESULT BluetoothGATTAbortReliableWrite(
    HANDLE                              hDevice,
    BTH_LE_GATT_RELIABLE_WRITE_CONTEXT  ReliableWriteContext,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTAbortReliableWrite(hDevice, ReliableWriteContext, Flags);
  }

  HRESULT BluetoothGATTSetCharacteristicValue(
    HANDLE                              hDevice,
    PBTH_LE_GATT_CHARACTERISTIC         Characteristic,
    PBTH_LE_GATT_CHARACTERISTIC_VALUE   CharacteristicValue,
    BTH_LE_GATT_RELIABLE_WRITE_CONTEXT  ReliableWriteContext,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTSetCharacteristicValue(hDevice, Characteristic,
      CharacteristicValue, ReliableWriteContext, Flags);
  }

  HRESULT BluetoothGATTSetDescriptorValue(
    HANDLE                              hDevice,
    PBTH_LE_GATT_DESCRIPTOR             Descriptor,
    PBTH_LE_GATT_DESCRIPTOR_VALUE       DescriptorValue,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTSetDescriptorValue(hDevice, Descriptor, DescriptorValue,
      Flags);
  }

  HRESULT BluetoothGATTRegisterEvent(
    HANDLE                              hService,
    BTH_LE_GATT_EVENT_TYPE              EventType,
    PVOID                               EventParameterIn,
    pfnGATTEvent                        Callback,
    PVOID                               CallbackContext,
    BLUETOOTH_GATT_EVENT_HANDLE*        pEventHandle,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTRegisterEvent(hService, EventType, EventParameterIn,
      Callback, CallbackContext, pEventHandle, Flags);
  }

  HRESULT BluetoothGATTUnregisterEvent(
    BLUETOOTH_GATT_EVENT_HANDLE         EventHandle,
    ULONG                               Flags
  )
  {
    initializedCheck();
    return m_pfnBluetoothGATTUnregisterEvent(EventHandle, Flags);
  }

protected:
  EsDynamicLibrary::Ptr m_lib;
  pfnBthpGATTCloseSession m_pfnBthpGATTCloseSession;
  pfnBluetoothGATTGetServices m_pfnBluetoothGATTGetServices;
  pfnBluetoothGATTGetIncludedServices m_pfnBluetoothGATTGetIncludedServices;
  pfnBluetoothGATTGetCharacteristics m_pfnBluetoothGATTGetCharacteristics;
  pfnBluetoothGATTGetDescriptors m_pfnBluetoothGATTGetDescriptors;
  pfnBluetoothGATTGetCharacteristicValue m_pfnBluetoothGATTGetCharacteristicValue;
  pfnBluetoothGATTGetDescriptorValue m_pfnBluetoothGATTGetDescriptorValue;
  pfnBluetoothGATTBeginReliableWrite m_pfnBluetoothGATTBeginReliableWrite;
  pfnBluetoothGATTEndReliableWrite m_pfnBluetoothGATTEndReliableWrite;
  pfnBluetoothGATTAbortReliableWrite m_pfnBluetoothGATTAbortReliableWrite;
  pfnBluetoothGATTSetCharacteristicValue m_pfnBluetoothGATTSetCharacteristicValue;
  pfnBluetoothGATTSetDescriptorValue m_pfnBluetoothGATTSetDescriptorValue;
  pfnBluetoothGATTRegisterEvent m_pfnBluetoothGATTRegisterEvent;
  pfnBluetoothGATTUnregisterEvent m_pfnBluetoothGATTUnregisterEvent;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
static GUID BTH_LE_UUIDtoGUID(const BTH_LE_UUID& uuid)
{
  if( uuid.IsShortUuid )
    return EsBluetoothUUID::fromShortened(uuid.Value.ShortUuid);
  else
    return uuid.Value.LongUuid;
}
//---------------------------------------------------------------------------

static EsBluetoothLEresult hresultToBleResult(HRESULT hresult)
{
  switch( hresult )
  {
  case S_OK:
    return EsBluetoothLEresult::gattSuccess;
  case E_BLUETOOTH_ATT_READ_NOT_PERMITTED:
    return EsBluetoothLEresult::gattReadNotPermitted;
  case E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED:
    return EsBluetoothLEresult::gattWriteNotPermitted;
  case E_BLUETOOTH_ATT_INSUFFICIENT_AUTHENTICATION:
    return EsBluetoothLEresult::gattInsufficientAutentication;
  case E_BLUETOOTH_ATT_ATTRIBUTE_NOT_FOUND:
    return EsBluetoothLEresult::gattRequestNotSupported;
  case E_BLUETOOTH_ATT_INVALID_OFFSET:
    return EsBluetoothLEresult::gattInvalidOffset;
  case E_BLUETOOTH_ATT_INVALID_ATTRIBUTE_VALUE_LENGTH:
    return EsBluetoothLEresult::gattInvalidAttributeLength;
  case E_BLUETOOTH_ATT_INSUFFICIENT_ENCRYPTION:
    return EsBluetoothLEresult::gattInsufficientEncryption;
  }

  return EsBluetoothLEresult::gattFailure;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
class EsBluetoothLEdescriptorWin : public EsBluetoothLEdescriptor
{
public:
  enum {
    dataHeaderLen = sizeof(BTH_LE_GATT_DESCRIPTOR_VALUE),
    fixedBuffLen = 128,
    extraDataLen = fixedBuffLen-dataHeaderLen
  };

public:
  EsBluetoothLEdescriptorWin(
    EsBluetoothLEcharacteristic* chx,
    const BTH_LE_GATT_DESCRIPTOR& dcr
  );

  /// Return current local descriptor value
  virtual EsBinBuffer valueGet() const;

  /// Set current local descriptor value
  virtual void valueSet(const EsBinBuffer& val);

  /// Read remote descriptor value from device, updating local value
  virtual EsBluetoothLEresult read(bool chached = false);

  /// Write local descriptor value to device, updating remote side
  virtual EsBluetoothLEresult write();

  /// Known descriptor property short-cuts. Validity is depending on descriptor Kind
  ///

  virtual bool isBroadcast() const;
  virtual bool isIndicate() const;
  virtual void indicateSet(bool indicate);
  virtual bool isNotify() const;
  virtual void notifySet(bool notify);
  virtual bool isReliableWrite() const;
  virtual bool isWritableAuxiliaries() const;

  virtual int exponentGet() const;
  virtual EsBluetoothLEformatType formatTypeGet() const;
  virtual GUID formatUnitGet() const;
  virtual GUID formatDescriptionGet() const;
  virtual EsString userDescriptionGet() const;

protected:
  /// Platform-specific stuff
  ///
  PBTH_LE_GATT_DESCRIPTOR_VALUE valueAccess();

protected:
  BTH_LE_GATT_DESCRIPTOR m_dcr;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/// Bluetooth LE GATT Characteristic interface
///
class EsBluetoothLEcharacteristicWin : public EsBluetoothLEcharacteristic
{
public:
  enum {
    dataHeaderLen = sizeof(BTH_LE_GATT_CHARACTERISTIC_VALUE),
    fixedBuffLen = 64,
    extraDataLen = fixedBuffLen-dataHeaderLen
  };

public:
  EsBluetoothLEcharacteristicWin(
    EsBluetoothLEservice* svc,
    const BTH_LE_GATT_CHARACTERISTIC& chx
  );

  virtual ~EsBluetoothLEcharacteristicWin();

  /// Return current local characteristic value
  virtual EsBinBuffer valueGet() const;

  /// Set current local characteristic value
  virtual void valueSet(const EsBinBuffer& val);

  /// Read remote characteristic from device, updating local value
  virtual EsBluetoothLEresult read(bool chached = false);

  /// Write local characteristic value to device, updating remote side
  virtual EsBluetoothLEresult write();

  /// Subscribe to|unsubscribe from characteristic push notification
  virtual EsBluetoothLEresult subscribe(bool on);

  /// Return true if notification subscription is active
  virtual bool isSubscribed() const
  {
    EsCriticalSectionLocker lock(m_cs);
    return internalIsSubscribed();
  }

protected:
  /// Platform-specific stuff
  ///
  bool internalIsSubscribed() const;
  void descriptorsEnumerate();
  PBTH_LE_GATT_CHARACTERISTIC_VALUE valueAccess();
  static VOID WINAPI gattEvent( BTH_LE_GATT_EVENT_TYPE EventType,
    PVOID EventOutParameter, PVOID EventContext);

protected:
  BTH_LE_GATT_CHARACTERISTIC m_chx;
  BLUETOOTH_GATT_EVENT_HANDLE m_hEvt;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/// Bluetooth LE GATT Service interface
///
class EsBluetoothLEserviceWin : public EsBluetoothLEservice
{
public:
  EsBluetoothLEserviceWin(
    EsBluetoothLEdevice* dev,
    const BTH_LE_GATT_SERVICE& svc,
    EsBluetoothLEservice* parent = 0
  );

  virtual ~EsBluetoothLEserviceWin();

  /// Platform-specific services
  ///
  HANDLE hserviceGet() const { return m_hService; }

protected:
  void serviceHandleInit();
  void characteristicsEnumerate();
  void subServicesEnumerate();

protected:
  HANDLE m_hService;
  BTH_LE_GATT_SERVICE m_svc;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/// Bluetooth LE device interface
///
class EsBluetoothLEdeviceWin : public EsBluetoothLEdevice
{
public:
  EsBluetoothLEdeviceWin(
    const EsBluetoothRadio::Ptr& pradio,
    const EsString& name,
    const EsString& path,
    const SP_DEVINFO_DATA& devInfo
  );

  virtual ~EsBluetoothLEdeviceWin();

  /// Generic Bluetooth LE interface
  ///

  /// Return true, if device is connected
  virtual bool isConnected() const;

  /// Query device RSSI
  virtual void rssiQuery() {} //< Not supported on Windows

  /// Return platform-dependent device identification string
  virtual const EsString& idGet() const { return m_path; }

  /// Disconnect device
  virtual void disconnect()
  {
    if( m_hDevice )
    {
      EsBtLEapiLib::inst().BthpGATTCloseSession(
        m_hDevice,
        BLUETOOTH_GATT_FLAG_NONE
      );
    }
  }

  /// Re-discover device services
  virtual void discover();

  /// Prepare reliable write batch
  virtual bool reliableWriteBegin();

  /// Abort reliable write batch
  virtual void reliableWriteAbort();

  /// Execute reliable write batch
  virtual bool reliableWriteExecute();

  /// Platform-specific services
  ///

  // Return unformatted mac addess string
  const EsString& addrRawGet() const { return m_addrRaw; }

  // Return device handle
  HANDLE hdeviceGet() const { return m_hDevice; }

  // Reliable write context access
  const BTH_LE_GATT_RELIABLE_WRITE_CONTEXT& reliableWriteCtxGet() const { return m_wrtCtx; }

protected:
  /// Helper services
  ///
  void addrFromPathInit();

protected:
  EsString m_addrRaw;
  HANDLE m_hDevice;
  SP_DEVINFO_DATA m_devInfo;
  EsString m_path;
  BTH_LE_GATT_RELIABLE_WRITE_CONTEXT m_wrtCtx;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Implementations
//
//---------------------------------------------------------------------------

EsBluetoothLEdescriptorWin::EsBluetoothLEdescriptorWin(
  EsBluetoothLEcharacteristic* chx,
  const BTH_LE_GATT_DESCRIPTOR& dcr
) :
m_dcr(dcr)
{
  m_chx = chx;
  ES_ASSERT(m_chx);

  m_uuid = BTH_LE_UUIDtoGUID(m_dcr.DescriptorUuid);
  m_kind = kindGetByUUID(m_uuid);
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isBroadcast() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindServerConfiguration == m_kind )
    return FALSE != pval->ServerCharacteristicConfiguration.IsBroadcast;

  throwInvalidPropForKind( esT("isBroadcast"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isIndicate() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindClientConfiguration == m_kind )
    return FALSE != pval->ClientCharacteristicConfiguration.IsSubscribeToIndication;

  throwInvalidPropForKind( esT("isIndicate"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptorWin::indicateSet(bool indicate)
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindClientConfiguration == m_kind )
    pval->ClientCharacteristicConfiguration.IsSubscribeToIndication =
      indicate ?
        TRUE :
        FALSE;
  else
    throwInvalidPropForKind( esT("indicateSet"), m_kind );
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isNotify() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindClientConfiguration == m_kind )
    return FALSE != pval->ClientCharacteristicConfiguration.IsSubscribeToNotification;

  throwInvalidPropForKind( esT("isNotify"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptorWin::notifySet(bool notify)
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindClientConfiguration == m_kind )
    pval->ClientCharacteristicConfiguration.IsSubscribeToNotification =
      notify ?
        TRUE :
        FALSE;
  else
    throwInvalidPropForKind( esT("notifySet"), m_kind );
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isReliableWrite() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindExtendedProperties == m_kind )
    return FALSE != pval->CharacteristicExtendedProperties.IsReliableWriteEnabled;

  throwInvalidPropForKind( esT("isReliableWrite"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isWritableAuxiliaries() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindExtendedProperties == m_kind )
    return FALSE != pval->CharacteristicExtendedProperties.IsAuxiliariesWritable;

  throwInvalidPropForKind( esT("isWritableAuxiliaries"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

int EsBluetoothLEdescriptorWin::exponentGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindPresentationFormat == m_kind )
    return pval->CharacteristicFormat.Exponent;

  throwInvalidPropForKind( esT("Exponent"), m_kind );
  return 0; //< Pacify compilers
}
//---------------------------------------------------------------------------

EsBluetoothLEformatType EsBluetoothLEdescriptorWin::formatTypeGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindPresentationFormat == m_kind )
    return static_cast<EsBluetoothLEformatType>(pval->CharacteristicFormat.Format);

  throwInvalidPropForKind( esT("Format type"), m_kind );
  return EsBluetoothLEformatType::Reserved; //< Pacify compilers
}

GUID EsBluetoothLEdescriptorWin::formatUnitGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindPresentationFormat == m_kind )
    return BTH_LE_UUIDtoGUID(pval->CharacteristicFormat.Unit);

  throwInvalidPropForKind( esT("Unit UUID"), m_kind );
  return GUID_NULL; //< Pacify compilers
}

GUID EsBluetoothLEdescriptorWin::formatDescriptionGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindPresentationFormat == m_kind )
    return BTH_LE_UUIDtoGUID(pval->CharacteristicFormat.Description);

  throwInvalidPropForKind( esT("Description UUID"), m_kind );
  return GUID_NULL; //< Pacify compilers
}

EsString EsBluetoothLEdescriptorWin::userDescriptionGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( kindUserDescription == m_kind )
  {
    size_t blen = pval->DataSize;
    if( blen % 2 )
    {
      EsByteString bs( reinterpret_cast<EsByteString::const_pointer>(pval->Data), blen );
      return EsString::fromUtf8(bs);
    }
    else
    {
      EsString::const_pointer beg = reinterpret_cast<EsString::const_pointer>(pval->Data);
      EsString::const_pointer end = beg + (blen / sizeof(EsString::value_type));
      return EsString( beg, end );
    }
  }

  throwInvalidPropForKind( esT("Description UUID"), m_kind );
  return EsString::null(); //< Pacify compilers
}
//---------------------------------------------------------------------------

EsBinBuffer EsBluetoothLEdescriptorWin::valueGet() const
{
  EsCriticalSectionLocker lock(m_cs);

  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  EsBinBuffer result(pval->Data, pval->Data+pval->DataSize);

  return result;
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptorWin::valueSet(const EsBinBuffer& val)
{
  EsCriticalSectionLocker lock(m_cs);

  size_t newSize = (val.size() < extraDataLen) ?
    val.size() :
    static_cast<size_t>(extraDataLen);

  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = valueAccess();
  pval->DataSize = newSize;

  if( newSize )
    memcpy(pval->Data, &val[0], newSize);
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEdescriptorWin::read(bool chached /*= false*/)
{
  EsCriticalSectionLocker lock(m_cs);

  EsBluetoothLEservice& svc = m_chx->serviceGet();
  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(&svc);
  ES_ASSERT(winSvc);

  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = valueAccess();
  USHORT reqCnt = 0;
  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTGetDescriptorValue(
    winSvc->hserviceGet(),
    &m_dcr,
    m_val.size(),
    pval,
    &reqCnt,
    chached ?
      BLUETOOTH_GATT_FLAG_NONE :
      BLUETOOTH_GATT_FLAG_FORCE_READ_FROM_DEVICE
  );

  if( pval->DataSize > extraDataLen )
    pval->DataSize = extraDataLen;

  if( S_OK != HRESULT_CODE(hresult) )
  {
    pval->DataSize = 0;
    return hresultToBleResult(hresult);
  }

  return EsBluetoothLEresult::gattSuccess;
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEdescriptorWin::write()
{
  EsBluetoothLEservice& svc = m_chx->serviceGet();
  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(&svc);
  ES_ASSERT(winSvc);

  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = valueAccess();

  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTSetDescriptorValue(
    winSvc->hserviceGet(),
    &m_dcr,
    pval,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( S_OK != HRESULT_CODE(hresult) )
    return hresultToBleResult(hresult);

  return EsBluetoothLEresult::gattSuccess;
}
//---------------------------------------------------------------------------

PBTH_LE_GATT_DESCRIPTOR_VALUE EsBluetoothLEdescriptorWin::valueAccess()
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE result = 0;
  if( m_val.size() < fixedBuffLen )
  {
    m_val.resize( fixedBuffLen );
    result = reinterpret_cast<PBTH_LE_GATT_DESCRIPTOR_VALUE>(&m_val[0]);
    result->DescriptorType = m_dcr.DescriptorType;
    result->DescriptorUuid = m_dcr.DescriptorUuid;
    result->DataSize = 0;
  }
  else
    result = reinterpret_cast<PBTH_LE_GATT_DESCRIPTOR_VALUE>(&m_val[0]);

  return result;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsBluetoothLEcharacteristicWin::EsBluetoothLEcharacteristicWin(
  EsBluetoothLEservice* svc,
  const BTH_LE_GATT_CHARACTERISTIC& chx
) :
EsBluetoothLEcharacteristic(),
m_chx(chx),
m_hEvt(0)
{
  m_svc = svc;
  ES_ASSERT(m_svc);

  m_uuid = BTH_LE_UUIDtoGUID(m_chx.CharacteristicUuid);

  if( m_chx.IsBroadcastable )
    m_props |= propBroadcast;

  if( m_chx.IsReadable )
    m_props |= propRead;

  if( m_chx.IsWritable )
    m_props |= propWrite;

  if( m_chx.IsWritableWithoutResponse )
    m_props |= propWriteNoResponse;

  if( m_chx.IsSignedWritable )
    m_props |= propSignedWrite;

  if( m_chx.IsNotifiable )
    m_props |= propNotify;

  if( m_chx.IsIndicatable )
    m_props |= propIndicate;

  if( m_chx.HasExtendedProperties )
    m_props |= propExtendedProps;

  descriptorsEnumerate();
}
//---------------------------------------------------------------------------

EsBluetoothLEcharacteristicWin::~EsBluetoothLEcharacteristicWin()
{
  subscribe(false);
  m_dcrs.clear();
}
//---------------------------------------------------------------------------

EsBinBuffer EsBluetoothLEcharacteristicWin::valueGet() const
{
  EsCriticalSectionLocker lock(m_cs);

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pval = const_cast<EsBluetoothLEcharacteristicWin*>(this)->valueAccess();
  EsBinBuffer result(pval->Data, pval->Data+pval->DataSize);

  return result;
}
//---------------------------------------------------------------------------

void EsBluetoothLEcharacteristicWin::valueSet(const EsBinBuffer& val)
{
  EsCriticalSectionLocker lock(m_cs);

  size_t newSize = (val.size() < extraDataLen) ?
    val.size() :
    static_cast<size_t>(extraDataLen);

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pval = valueAccess();
  pval->DataSize = newSize;

  if( newSize )
    memcpy(pval->Data, &val[0], newSize);
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEcharacteristicWin::read(bool cached /*= false*/)
{
  EsCriticalSectionLocker lock(m_cs);

  EsBluetoothLEservice& svc = serviceGet();
  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(&svc);
  ES_ASSERT(winSvc);

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pval = valueAccess();
  USHORT reqCnt = 0;
  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTGetCharacteristicValue(
    winSvc->hserviceGet(),
    &m_chx,
    m_val.size(),
    pval,
    &reqCnt,
    cached ?
      BLUETOOTH_GATT_FLAG_NONE :
      BLUETOOTH_GATT_FLAG_FORCE_READ_FROM_DEVICE
  );

  if( pval->DataSize > extraDataLen )
    pval->DataSize = extraDataLen;

  ES_BLE_TRACE_RESULT(esT("GATT chx read returns 0x%X hresult"), (ulong)hresult)

  if( S_OK != HRESULT_CODE(hresult) )
  {
    pval->DataSize = 0;
    return hresultToBleResult(hresult);
  }

  return EsBluetoothLEresult::gattSuccess;
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEcharacteristicWin::write()
{
  EsCriticalSectionLocker lock(m_cs);

  ULONG flags = BLUETOOTH_GATT_FLAG_NONE;

  if( m_props & propWriteNoResponse )
    flags = BLUETOOTH_GATT_FLAG_WRITE_WITHOUT_RESPONSE;

  EsBluetoothLEservice& svc = serviceGet();
  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(&svc);
  ES_ASSERT(winSvc);

  EsBluetoothLEdevice& dev = svc.deviceGet();
  EsBluetoothLEdeviceWin* winDev = dynamic_cast<EsBluetoothLEdeviceWin*>(&dev);
  ES_ASSERT(winDev);

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pval = valueAccess();

  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTSetCharacteristicValue(
    winSvc->hserviceGet(),
    &m_chx,
    pval,
    winDev->reliableWriteCtxGet(),
    flags
  );

  ES_BLE_TRACE_RESULT(esT("GATT chx write returns 0x%X hresult"), (ulong)hresult)

  if( S_OK != HRESULT_CODE(hresult) )
    return hresultToBleResult(hresult);

  return EsBluetoothLEresult::gattSuccess;
}
//---------------------------------------------------------------------------

VOID WINAPI EsBluetoothLEcharacteristicWin::gattEvent( BTH_LE_GATT_EVENT_TYPE EventType,
  PVOID EventOutParameter, PVOID EventContext)
{
  if(CharacteristicValueChangedEvent == EventType)
  {
    EsBluetoothLEcharacteristicWin* This = reinterpret_cast<EsBluetoothLEcharacteristicWin*>(EventContext);
    ES_ASSERT(This);

    PBLUETOOTH_GATT_VALUE_CHANGED_EVENT pevt = reinterpret_cast<PBLUETOOTH_GATT_VALUE_CHANGED_EVENT>(EventOutParameter);
    ES_ASSERT(pevt);

    EsBinBuffer val(pevt->CharacteristicValue->Data, pevt->CharacteristicValue->Data+pevt->CharacteristicValue->DataSize);
    This->valueSet(val);

  }
}
//---------------------------------------------------------------------------

bool EsBluetoothLEcharacteristicWin::internalIsSubscribed() const
{
  if( m_hEvt )
  {
    EsBluetoothLEdescriptor::Ptr dcr = descriptorFind(
        EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptor::kindClientConfiguration)
    );
    if( dcr )
      return dcr->isIndicate() || dcr->isNotify();
  }

  return false;
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEcharacteristicWin::subscribe(bool on)
{
  EsCriticalSectionLocker lock(m_cs);

  EsBluetoothLEresult result = EsBluetoothLEresult::gattSuccess;
  HRESULT hresult = S_OK;
  if( on && !internalIsSubscribed() && (m_props & (propNotify|propIndicate)) )
  {
    EsBluetoothLEdescriptor::Ptr dcr = descriptorFind(
        EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptor::kindClientConfiguration)
    );
    if( dcr )
    {
      if(m_props & propNotify)
        dcr->notifySet(on);

      if(m_props & propIndicate)
        dcr->indicateSet(on);

      result = dcr->write();

      int retries = 10;
      while( retries && EsBluetoothLEresult::gattSuccess != result )
      {
        EsThread::sleep(10);
        result = dcr->write();
        --retries;
      }

      if( EsBluetoothLEresult::gattSuccess == result )
      {
        if( 0 == m_hEvt )
        {
          EsBluetoothLEservice& svc = serviceGet();
          EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(&svc);
          ES_ASSERT(winSvc);

          BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION evtReg;
          evtReg.NumCharacteristics = 1;
          evtReg.Characteristics[0] = m_chx;

          hresult = EsBtLEapiLib::inst().BluetoothGATTRegisterEvent(
            winSvc->hserviceGet(),
            CharacteristicValueChangedEvent,
            &evtReg,
            &gattEvent,
            this,
            &m_hEvt,
            BLUETOOTH_GATT_FLAG_NONE
          );

          result = hresultToBleResult(hresult);
        }
      }
      else
      {
        // Rollback local changes
        dcr->notifySet(false);
        dcr->indicateSet(false);
      }
    }
  }
  else if( !on )
  {
    EsBluetoothLEdescriptor::Ptr dcr = descriptorFind(
        EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptor::kindClientConfiguration)
    );
    if( dcr )
    {
      if( dcr->isNotify() )
        dcr->notifySet(on);

      if( dcr->isIndicate() )
        dcr->indicateSet(on);

      if( m_svc &&
          !m_svc->isOrphan() &&
          m_svc->deviceGet().isConnected()
      )
        dcr->write();
    }

    if( m_hEvt )
    {
      hresult = EsBtLEapiLib::inst().BluetoothGATTUnregisterEvent(
        m_hEvt,
        BLUETOOTH_GATT_FLAG_NONE
      );
      m_hEvt = 0;
      result = hresultToBleResult(hresult);
    }
  }

  return result;
}
//---------------------------------------------------------------------------

PBTH_LE_GATT_CHARACTERISTIC_VALUE EsBluetoothLEcharacteristicWin::valueAccess()
{
  PBTH_LE_GATT_CHARACTERISTIC_VALUE result = 0;

  if( m_val.size() < fixedBuffLen )
  {
    m_val.resize( fixedBuffLen );
    result = reinterpret_cast<PBTH_LE_GATT_CHARACTERISTIC_VALUE>(&m_val[0]);
    result->DataSize = 0;
  }
  else
    result = reinterpret_cast<PBTH_LE_GATT_CHARACTERISTIC_VALUE>(&m_val[0]);

  return result;
}
//---------------------------------------------------------------------------

void EsBluetoothLEcharacteristicWin::descriptorsEnumerate()
{
  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(m_svc);
  ES_ASSERT(winSvc);

  m_dcrs.clear();

  WORD reqCnt = 0;
  HRESULT result = EsBtLEapiLib::inst().BluetoothGATTGetDescriptors(
    winSvc->hserviceGet(),
    &m_chx,
    0,
    0,
    &reqCnt,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( reqCnt )
  {
    EsBinBuffer dcrsBuff( reqCnt * sizeof(BTH_LE_GATT_DESCRIPTOR), (esU8)0 );
    PBTH_LE_GATT_DESCRIPTOR pDcrs = reinterpret_cast<PBTH_LE_GATT_DESCRIPTOR>(&dcrsBuff[0]);

    result = EsBtLEapiLib::inst().BluetoothGATTGetDescriptors(
      winSvc->hserviceGet(),
      &m_chx,
      reqCnt,
      pDcrs,
      &reqCnt,
      BLUETOOTH_GATT_FLAG_NONE
    );

    if( S_OK != HRESULT_CODE(result) )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), _("Could not enumerate characteristic descriptors: %s") );

    for( WORD idx = 0; idx < reqCnt; ++idx )
    {
      EsBluetoothLEdescriptor::Ptr dcr(
        new EsBluetoothLEdescriptorWin(
          this,
          pDcrs[idx]
        )
      );

      ES_ASSERT(dcr);
      dcr->read(true); //< Update from cache

      m_dcrs.push_back(dcr);
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Execute Bluetooth LE device enumeration
//

void EsBluetoothLEdeviceEnumerator::execute()
{
  reset();

  DEVPROPTYPE propType;

  EsBluetoothRadioWin* winRadio = dynamic_cast<EsBluetoothRadioWin*>(m_radio.get());
  ES_ASSERT(winRadio);

  winRadio->hdevInfoRealloc();
  HDEVINFO hDevInfo = winRadio->hdevInfoGet();

  if( INVALID_HANDLE_VALUE == hDevInfo )
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_DEV_ENUM_S );

  EsBinBuffer propBuffer(2048, (esU8)0);
  PSP_DEVICE_INTERFACE_DETAIL_DATA pdiDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(&propBuffer[0]);

  int idx = 0;
  DWORD err = 0;
  DWORD reqLen = 0;

  SP_DEVINFO_DATA devInfo;
  devInfo.cbSize = sizeof(SP_DEVINFO_DATA);

  while(1)
  {
    if( FALSE == EsBtLEsetupApiLib::inst().SetupDiEnumDeviceInfo(
        hDevInfo,
        idx,
        &devInfo
      )
    )
    {
      err = EsUtilities::osErrorCodeGet();
      if( err != ERROR_NO_MORE_ITEMS )
        EsException::ThrowOsError( err, ES_ERROR_BLE_DEV_ENUM_S );
      break;
    }

    if( FALSE == EsBtLEsetupApiLib::inst().SetupDiGetDeviceProperty(
        hDevInfo,
        &devInfo,
        &DEVPKEY_Device_FriendlyName,
        &propType,
        &propBuffer[0],
        propBuffer.size(),
        &reqLen,
        0
      )
    )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_DEV_ENUM_S );

    EsString devName( (EsString::const_pointer)&propBuffer[0], reqLen/sizeof(EsString::value_type) );

    SP_DEVICE_INTERFACE_DATA diData;
    memset(&diData, sizeof(diData), 0);
    diData.cbSize = sizeof(diData);

    if( FALSE == EsBtLEsetupApiLib::inst().SetupDiEnumDeviceInterfaces(
        hDevInfo,
        0,
        &GUID_BLUETOOTHLE_DEVICE_INTERFACE,
        idx,
        &diData
      )
    )
    {
      err = EsUtilities::osErrorCodeGet();
      if( err != ERROR_NO_MORE_ITEMS )
        EsException::ThrowOsError( err, ES_ERROR_BLE_DEV_ENUM_S );
      break;
    }

    memset(pdiDetailData, propBuffer.size(), 0);
    pdiDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if( FALSE == EsBtLEsetupApiLib::inst().SetupDiGetDeviceInterfaceDetail(
        hDevInfo,
        &diData,
        pdiDetailData,
        propBuffer.size(),
        &reqLen,
        0
      )
    )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_DEV_ENUM_S );

     EsString devPath( pdiDetailData->DevicePath );
     EsBluetoothLEdevice::Ptr dev(
        new EsBluetoothLEdeviceWin(
          m_radio,
          devName,
          devPath,
          devInfo
        )
     );

    if( !onDeviceEnumerated(dev) )
      break;

//      { RSSI not supported on windows }
//      { Advertise Data not supported on windows }

    ++idx;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsBluetoothLEdeviceWin::EsBluetoothLEdeviceWin(
  const EsBluetoothRadio::Ptr& pradio,
  const EsString& name,
  const EsString& path,
  const SP_DEVINFO_DATA& devInfo
) :
EsBluetoothLEdevice(pradio.get(), EsString::null(), name),
m_path(path),
m_hDevice(0),
m_devInfo(devInfo),
m_wrtCtx(0)
{
  ES_ASSERT( pradio );

  addrFromPathInit();
  m_hDevice = CreateFile(
    m_path.c_str(),
    GENERIC_READ|GENERIC_WRITE,
    0,
    0,
    OPEN_EXISTING,
    0,
    0
  );

  if( INVALID_HANDLE_VALUE == m_hDevice )
    m_hDevice = CreateFile(
      m_path.c_str(),
      GENERIC_READ,
      0,
      0,
      OPEN_EXISTING,
      0,
      0
    );

  if( INVALID_HANDLE_VALUE == m_hDevice )
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), _("Could not open Bluetooth LE device: '%s'") );
}
//---------------------------------------------------------------------------

EsBluetoothLEdeviceWin::~EsBluetoothLEdeviceWin()
{
  if( isConnected() )
    reliableWriteAbort();

  m_svcs.clear();
  disconnect();

  if( m_hDevice && INVALID_HANDLE_VALUE != m_hDevice )
  {
    CloseHandle(m_hDevice);
    m_hDevice = 0;
  }
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdeviceWin::reliableWriteBegin()
{
  if( m_hDevice && !m_wrtCtx )
  {
    HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTBeginReliableWrite(
      m_hDevice,
      &m_wrtCtx,
      BLUETOOTH_GATT_FLAG_NONE
    );

    return S_OK == HRESULT_CODE(hresult);
  }

  return m_hDevice && m_wrtCtx;
}
//---------------------------------------------------------------------------

void EsBluetoothLEdeviceWin::reliableWriteAbort()
{
  if( m_hDevice && m_wrtCtx )
  {
    EsBtLEapiLib::inst().BluetoothGATTAbortReliableWrite(
      m_hDevice,
      m_wrtCtx,
      BLUETOOTH_GATT_FLAG_NONE
    );

    m_wrtCtx = 0;
  }
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdeviceWin::reliableWriteExecute()
{
  if( m_hDevice && m_wrtCtx )
  {
    HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTEndReliableWrite(
      m_hDevice,
      m_wrtCtx,
      BLUETOOTH_GATT_FLAG_NONE
    );

    m_wrtCtx = 0;

    return S_OK == HRESULT_CODE(hresult);
  }

  return false;
}
//---------------------------------------------------------------------------

void EsBluetoothLEdeviceWin::addrFromPathInit()
{
  const EsString& tmp = EsString::toUpper( m_path );
  size_t pos = tmp.find(esT("#DEV_"));

  if( pos != EsString::npos )
  {
    EsString addr = tmp.substr(pos+5, 12);
    m_addrRaw = addr;

    addr.insert(2, esT(':'));
    addr.insert(5, esT(':'));
    addr.insert(8, esT(':'));
    addr.insert(11,esT(':'));
    addr.insert(14,esT(':'));

    m_addr = addr;
  }
  else
    m_addr = EsBluetooth::anyAddr();
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdeviceWin::isConnected() const
{
  esU32 prop;
  DEVPROPTYPE propType;
  EsBluetoothRadioWin* winRadio = dynamic_cast<EsBluetoothRadioWin*>(m_radio);
  ES_ASSERT(winRadio);

  if( FALSE == EsBtLEsetupApiLib::inst().SetupDiGetDeviceProperty(
      winRadio->hdevInfoGet(),
      const_cast<PSP_DEVINFO_DATA>(&m_devInfo),
      &DEVPKEY_Device_DevNodeStatus,
      &propType,
      (esU8*)&prop,
      4,
      0,
      0
    )
  )
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), _("Could not query Bluetooth LE device connection state: '%s'") );

  return !(prop & DN_NEEDS_LOCKING/*DN_DEVICE_DISCONNECTED*/);
}
//---------------------------------------------------------------------------

void EsBluetoothLEdeviceWin::discover()
{
  m_svcs.clear();

  // Actually, we do not discover services in Windows, 'inbox experience'
  // provides us with fully enumerated services at the time, LE device is known to the system
  //
  WORD svcCount = 0;

  HRESULT result = EsBtLEapiLib::inst().BluetoothGATTGetServices(
    m_hDevice,
    0,
    0,
    &svcCount,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( ERROR_MORE_DATA != HRESULT_CODE(result) )
    EsException::ThrowOsError( result, _("Could not discover Bluetooth LE service: '%s'") );

  EsBinBuffer svcsBuff( svcCount * sizeof(BTH_LE_GATT_SERVICE), (esU8)0 );

  result = EsBtLEapiLib::inst().BluetoothGATTGetServices(
    m_hDevice,
    svcCount,
    reinterpret_cast<PBTH_LE_GATT_SERVICE>(&svcsBuff[0]),
    &svcCount,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( S_OK != HRESULT_CODE(result) )
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), _("Could not discover Bluetooth LE service: '%s'") );

  PBTH_LE_GATT_SERVICE svcs = reinterpret_cast<PBTH_LE_GATT_SERVICE>(&svcsBuff[0]);
  for( WORD idx = 0; idx < svcCount; ++idx )
  {
    EsBluetoothLEservice::Ptr svc(
      new EsBluetoothLEserviceWin(
        this,
        svcs[idx]
      )
    );

    ES_ASSERT(svc);
    m_svcs.push_back(svc);
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsBluetoothLEserviceWin::EsBluetoothLEserviceWin(
    EsBluetoothLEdevice* dev,
    const BTH_LE_GATT_SERVICE& svc,
    EsBluetoothLEservice* parent
) :
m_hService(NULL),
m_svc(svc)
{
  m_dev = dev;
  ES_ASSERT(m_dev);

  m_parent = parent;
  m_uuid = BTH_LE_UUIDtoGUID(svc.ServiceUuid);
  serviceHandleInit();
  characteristicsEnumerate();
  subServicesEnumerate();
}
//---------------------------------------------------------------------------

EsBluetoothLEserviceWin::~EsBluetoothLEserviceWin()
{
  m_svcs.clear();
  m_chxs.clear();
  if( m_hService )
    CloseHandle(m_hService);
}
//---------------------------------------------------------------------------

void EsBluetoothLEserviceWin::serviceHandleInit()
{
  EsBluetoothLEdeviceWin* winDev = dynamic_cast<EsBluetoothLEdeviceWin*>(m_dev);
  ES_ASSERT(winDev);

  EsBinBuffer diDetailDataBuff(2048, (esU8)0);
  PSP_DEVICE_INTERFACE_DETAIL_DATA pdiDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(&diDetailDataBuff[0]);

  HDEVINFO hdevInfo = 0;
  ulong err = 0;

  try
  {
    hdevInfo = EsBtLEsetupApiLib::inst().SetupDiGetClassDevs(
      &m_uuid,
      0,
      0,
      DIGCF_PRESENT|DIGCF_DEVICEINTERFACE
    );

    if( INVALID_HANDLE_VALUE == hdevInfo )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_COULD_NOT_GET_SVC_HANDLE_S );

    DWORD idx = 0;
    while(true)
    {
      SP_DEVICE_INTERFACE_DATA diData;
      memset(&diData, sizeof(SP_DEVICE_INTERFACE_DATA), 0);
      diData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

      if( FALSE == EsBtLEsetupApiLib::inst().SetupDiEnumDeviceInterfaces(
          hdevInfo,
          0,
          &m_uuid,
          idx,
          &diData
        )
      )
      {
        err = EsUtilities::osErrorCodeGet();
        if( err != ERROR_NO_MORE_ITEMS )
          EsException::ThrowOsError(err, ES_ERROR_BLE_COULD_NOT_GET_SVC_HANDLE_S);
        break;
      }

      DWORD reqLen = 0;
      memset(pdiDetailData, diDetailDataBuff.size(), 0);
      pdiDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

      if( FALSE == EsBtLEsetupApiLib::inst().SetupDiGetDeviceInterfaceDetail(
          hdevInfo,
          &diData,
          pdiDetailData,
          diDetailDataBuff.size(),
          &reqLen,
          0
        )
      )
        EsException::ThrowOsError(EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_COULD_NOT_GET_SVC_HANDLE_S);

      EsString strPath(pdiDetailData->DevicePath);

      if( EsString::npos != EsString::toUpper(strPath).find( winDev->addrRawGet() ) )
      {
        HANDLE hsvc = CreateFile(
          strPath.c_str(),
          GENERIC_READ|GENERIC_WRITE,
          FILE_SHARE_READ|FILE_SHARE_WRITE,
          0,
          OPEN_EXISTING,
          0,
          0
        );

        if( INVALID_HANDLE_VALUE == hsvc )
          hsvc = CreateFile(
            strPath.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            0,
            OPEN_EXISTING,
            0,
            0
          );

        if( INVALID_HANDLE_VALUE == hsvc )
          EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_COULD_NOT_GET_SVC_HANDLE_S );

        m_hService = hsvc;
        break;
      }

      ++idx;
    }
  }
  catch(...)
  {
    if( hdevInfo )
      EsBtLEsetupApiLib::inst().SetupDiDestroyDeviceInfoList(hdevInfo);

    throw;
  }

  if( hdevInfo )
    EsBtLEsetupApiLib::inst().SetupDiDestroyDeviceInfoList(hdevInfo);
}
//---------------------------------------------------------------------------

void EsBluetoothLEserviceWin::characteristicsEnumerate()
{
  EsBluetoothLEdeviceWin* winDev = dynamic_cast<EsBluetoothLEdeviceWin*>(m_dev);
  ES_ASSERT(winDev);

  m_chxs.clear();

  WORD reqCnt = 0;
  HRESULT result = EsBtLEapiLib::inst().BluetoothGATTGetCharacteristics(
    winDev->hdeviceGet(),
    &m_svc,
    0,
    0,
    &reqCnt,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( reqCnt )
  {
    EsBinBuffer chxBuff( reqCnt*sizeof(BTH_LE_GATT_CHARACTERISTIC), (esU8)0 );
    PBTH_LE_GATT_CHARACTERISTIC pchxs = reinterpret_cast<PBTH_LE_GATT_CHARACTERISTIC>(&chxBuff[0]);

    result = EsBtLEapiLib::inst().BluetoothGATTGetCharacteristics(
      winDev->hdeviceGet(),
      &m_svc,
      reqCnt,
      pchxs,
      &reqCnt,
      BLUETOOTH_GATT_FLAG_NONE
    );

    if( S_OK != HRESULT_CODE(result) )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), _("Could not enumerate service characteristics: %s") );

    for(WORD idx = 0; idx < reqCnt; ++idx)
    {
      EsBluetoothLEcharacteristic::Ptr chx(
        new EsBluetoothLEcharacteristicWin(
          this,
          pchxs[idx]
        )
      );

      ES_ASSERT(chx);
      chx->read(true); //< Update value from cache

      m_chxs.push_back(chx);
    }
  }
}
//---------------------------------------------------------------------------

void EsBluetoothLEserviceWin::subServicesEnumerate()
{
  m_svcs.clear();

  WORD reqCnt = 0;
  HRESULT result = EsBtLEapiLib::inst().BluetoothGATTGetIncludedServices(
    m_hService,
    &m_svc,
    0,
    0,
    &reqCnt,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( reqCnt )
  {
    EsBinBuffer svcsBuff( reqCnt * sizeof(BTH_LE_GATT_SERVICE), (esU8)0 );
    PBTH_LE_GATT_SERVICE svcs = reinterpret_cast<PBTH_LE_GATT_SERVICE>(&svcsBuff[0]);

    result = EsBtLEapiLib::inst().BluetoothGATTGetIncludedServices(
      m_hService,
      &m_svc,
      reqCnt,
      svcs,
      &reqCnt,
      BLUETOOTH_GATT_FLAG_NONE
    );

    if( S_OK != HRESULT_CODE(result) )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), _("Could not enumerate included services: %s") );

    for(WORD idx = 0; idx < reqCnt; ++idx )
    {
      EsBluetoothLEservice::Ptr svc(
        new EsBluetoothLEserviceWin(
          m_dev,
          svcs[idx],
          this
        )
      );

      ES_ASSERT(svc);
      m_svcs.push_back(svc);
    }
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/// Global initialization
static EsAtomicInteger& bluetoothRef()
{
  static EsAtomicInteger s_ref = 0;
  return s_ref;
}

void EsBluetoothLE::initialize()
{
  EsBluetooth::initialize();

  if( 0 == bluetoothRef() )
  {
    EsBtLEsetupApiLib::inst().load();
    EsBtLEapiLib::inst().load();
  }

  esAtomicInc(
    bluetoothRef()
  );
}
//---------------------------------------------------------------------------

bool EsBluetoothLE::initialized()
{
  return EsBluetooth::initialized() &&
    EsBtLEsetupApiLib::inst().initialized() &&
    EsBtLEapiLib::inst().initialized();
}
//---------------------------------------------------------------------------

void EsBluetoothLE::uninitialize()
{
  if( bluetoothRef() )
    esAtomicDec(
      bluetoothRef()
    );

  if( 0 == bluetoothRef() )
  {
    EsBtLEapiLib::inst().unload();
    EsBtLEsetupApiLib::inst().unload();
  }

  EsBluetooth::uninitialize();
}
//---------------------------------------------------------------------------

