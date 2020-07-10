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
  m_pfnBthpGATTCloseSession(nullptr),
  m_pfnBluetoothGATTGetServices(nullptr),
  m_pfnBluetoothGATTGetIncludedServices(nullptr),
  m_pfnBluetoothGATTGetCharacteristics(nullptr),
  m_pfnBluetoothGATTGetDescriptors(nullptr),
  m_pfnBluetoothGATTGetCharacteristicValue(nullptr),
  m_pfnBluetoothGATTGetDescriptorValue(nullptr),
  m_pfnBluetoothGATTBeginReliableWrite(nullptr),
  m_pfnBluetoothGATTEndReliableWrite(nullptr),
  m_pfnBluetoothGATTAbortReliableWrite(nullptr),
  m_pfnBluetoothGATTSetCharacteristicValue(nullptr),
  m_pfnBluetoothGATTSetDescriptorValue(nullptr),
  m_pfnBluetoothGATTRegisterEvent(nullptr),
  m_pfnBluetoothGATTUnregisterEvent(nullptr)
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
    m_pfnBthpGATTCloseSession = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBthpGATTCloseSession>(m_lib->procAddrGet(esT("BthpGATTCloseSession")));
    m_pfnBluetoothGATTGetServices = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTGetServices>(m_lib->procAddrGet(esT("BluetoothGATTGetServices")));
    m_pfnBluetoothGATTGetIncludedServices = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTGetIncludedServices>(m_lib->procAddrGet(esT("BluetoothGATTGetIncludedServices")));
    m_pfnBluetoothGATTGetCharacteristics = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTGetCharacteristics>(m_lib->procAddrGet(esT("BluetoothGATTGetCharacteristics")));
    m_pfnBluetoothGATTGetDescriptors = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTGetDescriptors>(m_lib->procAddrGet(esT("BluetoothGATTGetDescriptors")));
    m_pfnBluetoothGATTGetCharacteristicValue = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTGetCharacteristicValue>(m_lib->procAddrGet(esT("BluetoothGATTGetCharacteristicValue")));
    m_pfnBluetoothGATTGetDescriptorValue = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTGetDescriptorValue>(m_lib->procAddrGet(esT("BluetoothGATTGetDescriptorValue")));
    m_pfnBluetoothGATTBeginReliableWrite = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTBeginReliableWrite>(m_lib->procAddrGet(esT("BluetoothGATTBeginReliableWrite")));
    m_pfnBluetoothGATTEndReliableWrite = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTEndReliableWrite>(m_lib->procAddrGet(esT("BluetoothGATTEndReliableWrite")));
    m_pfnBluetoothGATTAbortReliableWrite = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTAbortReliableWrite>(m_lib->procAddrGet(esT("BluetoothGATTAbortReliableWrite")));
    m_pfnBluetoothGATTSetCharacteristicValue = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTSetCharacteristicValue>(m_lib->procAddrGet(esT("BluetoothGATTSetCharacteristicValue")));
    m_pfnBluetoothGATTSetDescriptorValue = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTSetDescriptorValue>(m_lib->procAddrGet(esT("BluetoothGATTSetDescriptorValue")));
    m_pfnBluetoothGATTRegisterEvent = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTRegisterEvent>(m_lib->procAddrGet(esT("BluetoothGATTRegisterEvent")));
    m_pfnBluetoothGATTUnregisterEvent = esCallCastToCall<EsDynamicLibrary::Pfn, pfnBluetoothGATTUnregisterEvent>(m_lib->procAddrGet(esT("BluetoothGATTUnregisterEvent")));
  }

  void unload()
  {
    if( m_lib )
    {
      m_pfnBthpGATTCloseSession = nullptr;
      m_pfnBluetoothGATTGetServices = nullptr;
      m_pfnBluetoothGATTGetIncludedServices = nullptr;
      m_pfnBluetoothGATTGetCharacteristics = nullptr;
      m_pfnBluetoothGATTGetDescriptors = nullptr;
      m_pfnBluetoothGATTGetCharacteristicValue = nullptr;
      m_pfnBluetoothGATTGetDescriptorValue = nullptr;
      m_pfnBluetoothGATTBeginReliableWrite = nullptr;
      m_pfnBluetoothGATTEndReliableWrite = nullptr;
      m_pfnBluetoothGATTAbortReliableWrite = nullptr;
      m_pfnBluetoothGATTSetCharacteristicValue = nullptr;
      m_pfnBluetoothGATTSetDescriptorValue = nullptr;
      m_pfnBluetoothGATTRegisterEvent = nullptr;
      m_pfnBluetoothGATTUnregisterEvent = nullptr;

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
    extraDataLen = 256,
    fixedBuffLen = dataHeaderLen + extraDataLen
  };

protected:
  EsBluetoothLEdescriptorWin(
    const EsBaseIntfPtr& chx,
    const BTH_LE_GATT_DESCRIPTOR& dcr
  );

public:
  static EsBaseIntfPtr create( const EsBaseIntfPtr& chx, const BTH_LE_GATT_DESCRIPTOR& dcr );

  /// Return current local descriptor value
  virtual EsBinBuffer valueGet() const ES_OVERRIDE;

  /// Set current local descriptor value
  virtual void valueSet(const EsBinBuffer& val) ES_OVERRIDE;

  /// Read remote descriptor value from device, updating local value
  virtual EsBluetoothLEresult read(bool cached = false) ES_OVERRIDE;

  /// Write local descriptor value to device, updating remote side
  virtual EsBluetoothLEresult write() ES_OVERRIDE;

  /// Known descriptor property short-cuts. Validity is depending on descriptor Kind
  ///
  virtual bool isBroadcast() const ES_OVERRIDE;
  virtual bool isIndicate() const ES_OVERRIDE;
  virtual void indicateSet(bool indicate) ES_OVERRIDE;
  virtual bool isNotify() const ES_OVERRIDE;
  virtual void notifySet(bool notify) ES_OVERRIDE;
  virtual bool isReliableWrite() const ES_OVERRIDE;
  virtual bool isWritableAuxiliaries() const ES_OVERRIDE;

  virtual int exponentGet() const ES_OVERRIDE;
  virtual EsBluetoothLEformatType formatTypeGet() const ES_OVERRIDE;
  virtual GUID formatUnitGet() const ES_OVERRIDE;
  virtual GUID formatDescriptionGet() const ES_OVERRIDE;
  virtual EsString userDescriptionGet() const ES_OVERRIDE;

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
    extraDataLen = 64,
    fixedBuffLen = extraDataLen+dataHeaderLen
  };

protected:
  EsBluetoothLEcharacteristicWin(
    const EsBaseIntfPtr& svc,
    const BTH_LE_GATT_CHARACTERISTIC& chx
  );

public:
  static EsBaseIntfPtr create(const EsBaseIntfPtr& svc, const BTH_LE_GATT_CHARACTERISTIC& chx);

  virtual ~EsBluetoothLEcharacteristicWin() ES_OVERRIDE;

  /// Return current local characteristic value
  virtual EsBinBuffer valueGet() const ES_OVERRIDE;

  /// Set current local characteristic value
  virtual void valueSet(const EsBinBuffer& val) ES_OVERRIDE;

  /// Read remote characteristic from device, updating local value
  virtual EsBluetoothLEresult read(bool cached = false) ES_OVERRIDE;

  /// Write local characteristic value to device, updating remote side
  virtual EsBluetoothLEresult write() ES_OVERRIDE;

  /// Subscribe to|unsubscribe from characteristic push notification
  virtual EsBluetoothLEresult subscribe(bool on) ES_OVERRIDE;

  /// Return true if notification subscription is active
  virtual bool isSubscribed() const ES_OVERRIDE
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
  static VOID WINAPI gattEvent( BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID EventContext);

protected:
  BTH_LE_GATT_CHARACTERISTIC m_gattChx;
  BLUETOOTH_GATT_EVENT_HANDLE m_hEvt;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/// Bluetooth LE GATT Service interface
///
class EsBluetoothLEserviceWin : public EsBluetoothLEservice
{
protected:
  EsBluetoothLEserviceWin(
    const EsBaseIntfPtr& dev,
    const BTH_LE_GATT_SERVICE& svc
  );

public:
  static EsBaseIntfPtr create(const EsBaseIntfPtr& dev, const BTH_LE_GATT_SERVICE& svc);
 
  virtual ~EsBluetoothLEserviceWin() ES_OVERRIDE;

  /// Platform-specific services
  ///
  HANDLE hserviceGet() const ES_NOTHROW { return m_hService; }

protected:
  void serviceHandleInit();
  void characteristicsEnumerate();

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
protected:
  EsBluetoothLEdeviceWin(
    const EsBaseIntfPtr& pradio,
    const EsString& name,
    const EsString& path,
    const SP_DEVINFO_DATA& devInfo
  );

public:
  static EsBaseIntfPtr create( const EsBaseIntfPtr& pradio, const EsString& name, const EsString& path, const SP_DEVINFO_DATA& devInfo );

  virtual ~EsBluetoothLEdeviceWin() ES_OVERRIDE;

  /// Generic Bluetooth LE interface
  ///

  /// Return true, if device is connected
  virtual bool isConnected() const ES_OVERRIDE;

  /// Query device RSSI
  virtual void rssiQuery() ES_OVERRIDE {} //< Not supported on Windows

  /// Return platform-dependent device identification string
  virtual const EsString& idGet() const ES_OVERRIDE { return m_path; }

  /// Disconnect device
  virtual void disconnect() ES_OVERRIDE
  {
    if( !m_hDevice )
      return;

    EsBtLEapiLib::inst().BthpGATTCloseSession(
      m_hDevice,
      BLUETOOTH_GATT_FLAG_NONE
    );
  }

  /// Re-discover device services
  virtual void discover() ES_OVERRIDE;

  /// Prepare reliable write batch
  virtual bool reliableWriteBegin() ES_OVERRIDE;

  /// Abort reliable write batch
  virtual void reliableWriteAbort() ES_OVERRIDE;

  /// Execute reliable write batch
  virtual bool reliableWriteExecute() ES_OVERRIDE;

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
  const EsBaseIntfPtr& chx,
  const BTH_LE_GATT_DESCRIPTOR& dcr
) :
EsBluetoothLEdescriptor(chx),
m_dcr(dcr)
{
  m_uuid = BTH_LE_UUIDtoGUID(m_dcr.DescriptorUuid);
  m_kind = kindGetByUUID(m_uuid);
}
//---------------------------------------------------------------------------

EsBaseIntfPtr EsBluetoothLEdescriptorWin::create( const EsBaseIntfPtr& chx, const BTH_LE_GATT_DESCRIPTOR& dcr )
{
  std::unique_ptr<EsBluetoothLEdescriptorWin> ptr = ES_MAKE_UNIQUE(
    EsBluetoothLEdescriptorWin,
    chx,
    dcr
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//--------------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isBroadcast() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::ServerConfiguration == m_kind )
    return FALSE != pval->ServerCharacteristicConfiguration.IsBroadcast;

  throwInvalidPropForKind( esT("isBroadcast"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isIndicate() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if(EsBluetoothLEdescriptorKind::ClientConfiguration == m_kind )
    return FALSE != pval->ClientCharacteristicConfiguration.IsSubscribeToIndication;

  throwInvalidPropForKind( esT("isIndicate"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptorWin::indicateSet(bool indicate)
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if(EsBluetoothLEdescriptorKind::ClientConfiguration == m_kind )
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
  if( EsBluetoothLEdescriptorKind::ClientConfiguration == m_kind )
    return FALSE != pval->ClientCharacteristicConfiguration.IsSubscribeToNotification;

  throwInvalidPropForKind( esT("isNotify"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

void EsBluetoothLEdescriptorWin::notifySet(bool notify)
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::ClientConfiguration == m_kind )
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
  if( EsBluetoothLEdescriptorKind::ExtendedProperties == m_kind )
    return FALSE != pval->CharacteristicExtendedProperties.IsReliableWriteEnabled;

  throwInvalidPropForKind( esT("isReliableWrite"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

bool EsBluetoothLEdescriptorWin::isWritableAuxiliaries() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::ExtendedProperties == m_kind )
    return FALSE != pval->CharacteristicExtendedProperties.IsAuxiliariesWritable;

  throwInvalidPropForKind( esT("isWritableAuxiliaries"), m_kind );
  return false; //< Pacify compilers
}
//---------------------------------------------------------------------------

int EsBluetoothLEdescriptorWin::exponentGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::PresentationFormat == m_kind )
    return pval->CharacteristicFormat.Exponent;

  throwInvalidPropForKind( esT("Exponent"), m_kind );
  return 0; //< Pacify compilers
}
//---------------------------------------------------------------------------

EsBluetoothLEformatType EsBluetoothLEdescriptorWin::formatTypeGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::PresentationFormat == m_kind )
    return static_cast<EsBluetoothLEformatType>(pval->CharacteristicFormat.Format);

  throwInvalidPropForKind( esT("Format type"), m_kind );
  return EsBluetoothLEformatType::Reserved; //< Pacify compilers
}

GUID EsBluetoothLEdescriptorWin::formatUnitGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::PresentationFormat == m_kind )
    return BTH_LE_UUIDtoGUID(pval->CharacteristicFormat.Unit);

  throwInvalidPropForKind( esT("Unit UUID"), m_kind );
  return GUID_NULL; //< Pacify compilers
}

GUID EsBluetoothLEdescriptorWin::formatDescriptionGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::PresentationFormat == m_kind )
    return BTH_LE_UUIDtoGUID(pval->CharacteristicFormat.Description);

  throwInvalidPropForKind( esT("Description UUID"), m_kind );
  return GUID_NULL; //< Pacify compilers
}

EsString EsBluetoothLEdescriptorWin::userDescriptionGet() const
{
  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = const_cast<EsBluetoothLEdescriptorWin*>(this)->valueAccess();
  if( EsBluetoothLEdescriptorKind::UserDescription == m_kind )
  {
    size_t blen = pval->DataSize;
    if( blen % sizeof(EsWideString16::value_type) )
    {
      EsByteString bs( 
        reinterpret_cast<EsByteString::const_pointer>(pval->Data), 
        blen 
      );
      
      return EsString::fromUtf8(bs);
    }
    else
    {
      //NB! On Windows we most probably would have UTF16 here
      EsWideString16 ws(
        reinterpret_cast<EsWideString16::const_pointer>( pval->Data ),
        reinterpret_cast<EsWideString16::const_pointer>( pval->Data ) + (blen / sizeof( EsWideString16::value_type ) )
      );

      return EsString( ws );
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
  pval->DataSize = static_cast<ULONG>(newSize);

  if( newSize )
    memcpy(pval->Data, &val[0], newSize);
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEdescriptorWin::read(bool cached /*= false*/)
{
  if( get_isOrphan() )
    return EsBluetoothLEresult::gattFailure;

  EsCriticalSectionLocker lock(m_cs);

  EsBluetoothLEcharacteristic* chx = characteristicGet();
  ES_ASSERT(chx);

  EsBluetoothLEservice* svc = chx->serviceGet();
  ES_ASSERT(svc);

  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(svc);
  ES_ASSERT(winSvc);

  PBTH_LE_GATT_DESCRIPTOR_VALUE pval = valueAccess();
  USHORT reqCnt = 0;
  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTGetDescriptorValue(
    winSvc->hserviceGet(),
    &m_dcr,
    static_cast<ULONG>(m_val.size()),
    pval,
    &reqCnt,
    cached ?
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
  if( get_isOrphan() )
    return EsBluetoothLEresult::gattFailure;

  EsBluetoothLEcharacteristic* chx = characteristicGet();
  ES_ASSERT( chx );

  EsBluetoothLEservice* svc = chx->serviceGet();
  ES_ASSERT( svc );

  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(svc);
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
  const EsBaseIntfPtr& svc,
  const BTH_LE_GATT_CHARACTERISTIC& chx
) :
EsBluetoothLEcharacteristic(svc),
m_gattChx(chx),
m_hEvt(nullptr)
{
  m_uuid = BTH_LE_UUIDtoGUID( m_gattChx.CharacteristicUuid);

  if( m_gattChx.IsBroadcastable )
    m_props |= EsBluetoothLEcharacteristicProperty::Broadcast;

  if( m_gattChx.IsReadable )
    m_props |= EsBluetoothLEcharacteristicProperty::Read;

  if( m_gattChx.IsWritable )
    m_props |= EsBluetoothLEcharacteristicProperty::Write;

  if( m_gattChx.IsWritableWithoutResponse )
    m_props |= EsBluetoothLEcharacteristicProperty::WriteNoResponse;

  if( m_gattChx.IsSignedWritable )
    m_props |= EsBluetoothLEcharacteristicProperty::SignedWrite;

  if( m_gattChx.IsNotifiable )
    m_props |= EsBluetoothLEcharacteristicProperty::Notify;

  if( m_gattChx.IsIndicatable )
    m_props |= EsBluetoothLEcharacteristicProperty::Indicate;

  if( m_gattChx.HasExtendedProperties )
    m_props |= EsBluetoothLEcharacteristicProperty::ExtendedProps;

  descriptorsEnumerate();
}
//--------------------------------------------------------------------------------

EsBaseIntfPtr EsBluetoothLEcharacteristicWin::create( const EsBaseIntfPtr& svc, const BTH_LE_GATT_CHARACTERISTIC& chx )
{
  std::unique_ptr<EsBluetoothLEcharacteristicWin> ptr = ES_MAKE_UNIQUE(
    EsBluetoothLEcharacteristicWin,
    svc,
    chx
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBluetoothLEcharacteristicWin::~EsBluetoothLEcharacteristicWin()
{
  subscribe(false);
  
  descriptorsCleanup();
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
  pval->DataSize = static_cast<ULONG>(newSize);

  if( newSize )
    memcpy(pval->Data, &val[0], newSize);
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEcharacteristicWin::read(bool cached /*= false*/)
{
  EsCriticalSectionLocker lock(m_cs);

  EsBluetoothLEservice* svc = serviceGet();
  ES_ASSERT(svc);

  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(svc);
  ES_ASSERT(winSvc);

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pval = valueAccess();
  USHORT reqCnt = 0;
  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTGetCharacteristicValue(
    winSvc->hserviceGet(),
    &m_gattChx,
    static_cast<ULONG>(m_val.size()),
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

  if( m_props & EsBluetoothLEcharacteristicProperty::WriteNoResponse )
    flags = BLUETOOTH_GATT_FLAG_WRITE_WITHOUT_RESPONSE;

  EsBluetoothLEservice* svc = serviceGet();
  ES_ASSERT(svc);

  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(svc);
  ES_ASSERT(winSvc);

  EsBluetoothLEdevice* dev = svc->deviceGet();
  ES_ASSERT(dev);

  EsBluetoothLEdeviceWin* winDev = dynamic_cast<EsBluetoothLEdeviceWin*>(dev);
  ES_ASSERT(winDev);

  PBTH_LE_GATT_CHARACTERISTIC_VALUE pval = valueAccess();

  HRESULT hresult = EsBtLEapiLib::inst().BluetoothGATTSetCharacteristicValue(
    winSvc->hserviceGet(),
    &m_gattChx,
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

VOID WINAPI EsBluetoothLEcharacteristicWin::gattEvent( BTH_LE_GATT_EVENT_TYPE EventType, PVOID EventOutParameter, PVOID EventContext)
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
  if( !m_hEvt )
    return false;

  EsBluetoothLEdescriptor* dcr = descriptorFind(
    EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptorKind::ClientConfiguration)
  );

  if( dcr )
    return dcr->isIndicate() || 
      dcr->isNotify();

  return false;
}
//---------------------------------------------------------------------------

EsBluetoothLEresult EsBluetoothLEcharacteristicWin::subscribe(bool on)
{
  EsCriticalSectionLocker lock(m_cs);

  EsBluetoothLEresult result = EsBluetoothLEresult::gattSuccess;
  HRESULT hresult = S_OK;
  if( 
    on && 
    !internalIsSubscribed() && 
    (
      m_props & 
      ( 
        EsBluetoothLEcharacteristicProperty::Notify |
        EsBluetoothLEcharacteristicProperty::Indicate
      )
    ) 
  )
  {
    EsBluetoothLEdescriptor* dcr = descriptorFind(
        EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptorKind::ClientConfiguration)
    );

    if( dcr )
    {
      if(m_props & EsBluetoothLEcharacteristicProperty::Notify)
        dcr->notifySet(on);
      else if(m_props & EsBluetoothLEcharacteristicProperty::Indicate)
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
        if( nullptr == m_hEvt )
        {
          EsBluetoothLEservice* svc = serviceGet();
          ES_ASSERT(svc);

          EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(svc);
          ES_ASSERT(winSvc);

          BLUETOOTH_GATT_VALUE_CHANGED_EVENT_REGISTRATION evtReg;
          evtReg.NumCharacteristics = 1;
          evtReg.Characteristics[0] = m_gattChx;

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
    EsBluetoothLEdescriptor* dcr = descriptorFind(
        EsBluetoothLEdescriptor::uuidGetByKind(EsBluetoothLEdescriptorKind::ClientConfiguration)
    );
    if( dcr )
    {
      if( dcr->isNotify() )
        dcr->notifySet(on);

      if( dcr->isIndicate() )
        dcr->indicateSet(on);

      EsBluetoothLEservice* svc = serviceGet();

      if( svc &&
          !svc->get_isOrphan() &&
          svc->deviceGet()->isConnected()
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
  if( get_isOrphan() )
    return;

  EsBluetoothLEservice* svc = serviceGet();
  ES_ASSERT(svc);

  EsBluetoothLEserviceWin* winSvc = dynamic_cast<EsBluetoothLEserviceWin*>(svc);
  ES_ASSERT(winSvc);

  descriptorsCleanup();

  WORD reqCnt = 0;
  HRESULT result = EsBtLEapiLib::inst().BluetoothGATTGetDescriptors(
    winSvc->hserviceGet(),
    &m_gattChx,
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
      &m_gattChx,
      reqCnt,
      pDcrs,
      &reqCnt,
      BLUETOOTH_GATT_FLAG_NONE
    );

    if( S_OK != HRESULT_CODE(result) )
      EsException::ThrowOsError( 
        EsUtilities::osErrorCodeGet(), 
        _("Could not enumerate characteristic descriptors: %s") 
      );

    for( WORD idx = 0; idx < reqCnt; ++idx )
    {
      EsBaseIntfPtr pdcr = EsBluetoothLEdescriptorWin::create(
        asWeakReference(),
        pDcrs[idx]
      );
      ES_ASSERT(pdcr);

      EsBluetoothLEdescriptor* dcr = ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEdescriptor>(pdcr);
      ES_ASSERT(dcr);
      dcr->read(true); //< Update from cache

      m_dcrs->newValueSet(
        dcr->get_uuid(),
        pdcr
      );
    }
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// Execute Bluetooth LE device enumeration
//

void EsBluetoothLEdeviceEnumerator::enumerate()
{
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
        static_cast<DWORD>(propBuffer.size()),
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

    memset(
      pdiDetailData, 
      static_cast<int>( propBuffer.size() ),
      0
    );
    pdiDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    if( FALSE == EsBtLEsetupApiLib::inst().SetupDiGetDeviceInterfaceDetail(
        hDevInfo,
        &diData,
        pdiDetailData,
        static_cast<DWORD>(propBuffer.size()),
        &reqLen,
        0
      )
    )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet(), ES_ERROR_BLE_DEV_ENUM_S );

    EsString devPath( pdiDetailData->DevicePath );
    EsBaseIntfPtr pdev = EsBluetoothLEdeviceWin::create(
      m_radio,
      devName,
      devPath,
      devInfo
    );
    ES_ASSERT(pdev);

    if( !onDeviceEnumerated(pdev) )
      break;

//      { RSSI not supported on windows }
//      { Advertisment Data not supported on windows }

    ++idx;
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsBluetoothLEdeviceWin::EsBluetoothLEdeviceWin(
  const EsBaseIntfPtr& pradio,
  const EsString& name,
  const EsString& path,
  const SP_DEVINFO_DATA& devInfo
) :
EsBluetoothLEdevice(
  pradio, 
  EsString::null(), 
  name
),
m_path(path),
m_hDevice(nullptr),
m_devInfo(devInfo),
m_wrtCtx(0)
{
  ES_ASSERT( m_radio );

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
    EsException::ThrowOsError( 
      EsUtilities::osErrorCodeGet(), 
      _("Could not open Bluetooth LE device: '%s'") 
    );
}
//--------------------------------------------------------------------------------

EsBaseIntfPtr EsBluetoothLEdeviceWin::create( const EsBaseIntfPtr& pradio, const EsString& name, const EsString& path, const SP_DEVINFO_DATA& devInfo )
{
  std::unique_ptr<EsBluetoothLEdeviceWin> ptr = ES_MAKE_UNIQUE(
    EsBluetoothLEdeviceWin,
    pradio, 
    name, 
    path, 
    devInfo
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBluetoothLEdeviceWin::~EsBluetoothLEdeviceWin()
{
  if( isConnected() )
    reliableWriteAbort();

  servicesCleanup();
  disconnect();

  if( m_hDevice && INVALID_HANDLE_VALUE != m_hDevice )
  {
    CloseHandle(m_hDevice);
    m_hDevice = nullptr;
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

  const EsBluetoothRadio* pradio = radioGet();
  ES_ASSERT(pradio);

  const EsBluetoothRadioWin* winRadio = dynamic_cast<const EsBluetoothRadioWin*>( pradio );
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
    EsException::ThrowOsError( 
      EsUtilities::osErrorCodeGet(), 
      _("Could not query Bluetooth LE device connection state: '%s'") 
    );

  return !(prop & DN_NEEDS_LOCKING/*DN_DEVICE_DISCONNECTED*/);
}
//---------------------------------------------------------------------------

void EsBluetoothLEdeviceWin::discover()
{
  servicesCleanup();

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
    EsException::ThrowOsError( 
      result, 
      _("Could not discover Bluetooth LE service: '%s'") 
    );

  EsBinBuffer svcsBuff( svcCount * sizeof(BTH_LE_GATT_SERVICE), (esU8)0 );

  result = EsBtLEapiLib::inst().BluetoothGATTGetServices(
    m_hDevice,
    svcCount,
    reinterpret_cast<PBTH_LE_GATT_SERVICE>(&svcsBuff[0]),
    &svcCount,
    BLUETOOTH_GATT_FLAG_NONE
  );

  if( S_OK != HRESULT_CODE(result) )
    EsException::ThrowOsError( 
      EsUtilities::osErrorCodeGet(), 
      _("Could not discover Bluetooth LE service: '%s'") 
    );

  PBTH_LE_GATT_SERVICE svcs = reinterpret_cast<PBTH_LE_GATT_SERVICE>(&svcsBuff[0]);
  for( WORD idx = 0; idx < svcCount; ++idx )
  {
    EsBaseIntfPtr psvc = EsBluetoothLEserviceWin::create(
      asWeakReference(),
      svcs[idx]
    );
    ES_ASSERT(psvc);

    m_svcs->newValueSet(
      ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEservice>(psvc)->get_uuid(),
      psvc
    );
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

EsBluetoothLEserviceWin::EsBluetoothLEserviceWin(const EsBaseIntfPtr& dev, const BTH_LE_GATT_SERVICE& svc) :
EsBluetoothLEservice(dev),
m_hService(NULL),
m_svc(svc)
{
  m_uuid = BTH_LE_UUIDtoGUID(svc.ServiceUuid);
  serviceHandleInit();
  characteristicsEnumerate();
}
//--------------------------------------------------------------------------------

EsBaseIntfPtr EsBluetoothLEserviceWin::create( const EsBaseIntfPtr& dev, const BTH_LE_GATT_SERVICE& svc )
{
  std::unique_ptr<EsBluetoothLEserviceWin> ptr = ES_MAKE_UNIQUE(
    EsBluetoothLEserviceWin,
    dev,
    svc
  );
  ES_ASSERT(ptr);

  return ptr.release()->asBaseIntfPtr();
}
//---------------------------------------------------------------------------

EsBluetoothLEserviceWin::~EsBluetoothLEserviceWin()
{
  characteristicsCleanup();
  
  if( m_hService )
    CloseHandle(m_hService);
}
//---------------------------------------------------------------------------

void EsBluetoothLEserviceWin::serviceHandleInit()
{
  EsBluetoothLEdevice* pdev = deviceGet();
  ES_ASSERT(pdev);

  EsBluetoothLEdeviceWin* winDev = dynamic_cast<EsBluetoothLEdeviceWin*>(pdev);
  ES_ASSERT(winDev);

  EsBinBuffer diDetailDataBuff(2048, (esU8)0);
  PSP_DEVICE_INTERFACE_DETAIL_DATA pdiDetailData = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(&diDetailDataBuff[0]);

  HDEVINFO hdevInfo = 0;
  ulong err = 0;

  try
  {
    hdevInfo = EsBtLEsetupApiLib::inst().SetupDiGetClassDevs(
      &m_uuid.uuidGet(),
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
          &m_uuid.uuidGet(),
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
      memset(
        pdiDetailData, 
        static_cast<int>(diDetailDataBuff.size()),
        0
      );
      pdiDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

      if( FALSE == EsBtLEsetupApiLib::inst().SetupDiGetDeviceInterfaceDetail(
          hdevInfo,
          &diData,
          pdiDetailData,
          static_cast<DWORD>(diDetailDataBuff.size()),
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
          EsException::ThrowOsError( 
            EsUtilities::osErrorCodeGet(), 
            ES_ERROR_BLE_COULD_NOT_GET_SVC_HANDLE_S 
          );

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
  EsBluetoothLEdevice* pdev = deviceGet();
  ES_ASSERT(pdev);

  EsBluetoothLEdeviceWin* winDev = dynamic_cast<EsBluetoothLEdeviceWin*>(pdev);
  ES_ASSERT(winDev);

  characteristicsCleanup();

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
      EsException::ThrowOsError( 
        EsUtilities::osErrorCodeGet(), 
        _("Could not enumerate service characteristics: %s") 
      );

    for(WORD idx = 0; idx < reqCnt; ++idx)
    {
      EsBaseIntfPtr pchx = EsBluetoothLEcharacteristicWin::create(
        asWeakReference(),
        pchxs[idx]
      );
      ES_ASSERT(pchx);

      EsBluetoothLEcharacteristic* chx = ES_INTFPTR_TO_OBJECTPTR<EsBluetoothLEcharacteristic>( pchx );
      ES_ASSERT( chx );

      chx->read( true ); //< Update value from cache

      m_chxs->newValueSet(
        chx->get_uuid(),
        pchx
      );
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
