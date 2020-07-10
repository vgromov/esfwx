#ifndef _es_bluetooth_windows_h_
#define _es_bluetooth_windows_h_

#include <BluetoothAPIs.h>

class EsBtLib
{
protected:
  typedef HBLUETOOTH_RADIO_FIND (WINAPI *pfnBluetoothFindFirstRadio)(
    BLUETOOTH_FIND_RADIO_PARAMS* pbtfrp,
    HANDLE* phRadio
  );

  typedef BOOL (WINAPI *pfnBluetoothFindNextRadio)(
    HBLUETOOTH_RADIO_FIND hFind,
    HANDLE* phRadio
  );

  typedef BOOL (WINAPI *pfnBluetoothFindRadioClose)(
    HBLUETOOTH_RADIO_FIND hFind
  );

  typedef DWORD (WINAPI *pfnBluetoothGetRadioInfo)(
    HANDLE hRadio,
    PBLUETOOTH_RADIO_INFO pRadioInfo
  );

  typedef HBLUETOOTH_DEVICE_FIND (WINAPI *pfnBluetoothFindFirstDevice)(
    BLUETOOTH_DEVICE_SEARCH_PARAMS *pbtsp,
    BLUETOOTH_DEVICE_INFO *pbtdi
  );

  typedef BOOL (WINAPI *pfnBluetoothFindNextDevice)(
    HBLUETOOTH_DEVICE_FIND hFind,
    BLUETOOTH_DEVICE_INFO *pbtdi
  );

  typedef BOOL (WINAPI *pfnBluetoothFindDeviceClose)(
    HBLUETOOTH_DEVICE_FIND hFind
  );

  typedef DWORD (WINAPI *pfnBluetoothEnumerateInstalledServices)(
    HANDLE hRadio,
    BLUETOOTH_DEVICE_INFO *pbtdi,
    DWORD *pcServices,
    GUID *pGuidServices
  );

public:
  EsBtLib() :
  m_pfnBluetoothFindFirstRadio( nullptr ),
  m_pfnBluetoothFindNextRadio( nullptr ),
  m_pfnBluetoothFindRadioClose( nullptr ),
  m_pfnBluetoothGetRadioInfo( nullptr ),
  m_pfnBluetoothFindFirstDevice( nullptr ),
  m_pfnBluetoothFindNextDevice( nullptr ),
  m_pfnBluetoothFindDeviceClose( nullptr ),
  m_pfnBluetoothEnumerateInstalledServices( nullptr )
  {
  }

  ~EsBtLib()
  {
    unload();
  }

  static EsBtLib& inst()
  {
    static EsBtLib s_btlib;
    return s_btlib;
  }

  void initializedCheck()
  {
    if( !m_lib )
      EsException::Throw(_("Bluetooth classic engine is not initialized"));
  }

  void load();

  void unload();

  bool initialized() const { return m_lib && m_lib->isOk(); }

  // mapped library services
  //
  HBLUETOOTH_RADIO_FIND BluetoothFindFirstRadio(
    BLUETOOTH_FIND_RADIO_PARAMS* pbtfrp,
    HANDLE* phRadio
  )
  {
    initializedCheck();
    return m_pfnBluetoothFindFirstRadio(pbtfrp, phRadio);
  }

  BOOL BluetoothFindNextRadio(
    HBLUETOOTH_RADIO_FIND hFind,
    HANDLE* phRadio
  )
  {
    initializedCheck();
    return m_pfnBluetoothFindNextRadio(hFind, phRadio);
  }

  BOOL BluetoothFindRadioClose(
    HBLUETOOTH_RADIO_FIND hFind
  )
  {
    initializedCheck();
    return m_pfnBluetoothFindRadioClose(hFind);
  }

  DWORD BluetoothGetRadioInfo(
    HANDLE hRadio,
    PBLUETOOTH_RADIO_INFO pRadioInfo
  )
  {
    initializedCheck();
    return m_pfnBluetoothGetRadioInfo(hRadio, pRadioInfo);
  }

  HBLUETOOTH_DEVICE_FIND BluetoothFindFirstDevice(
    BLUETOOTH_DEVICE_SEARCH_PARAMS *pbtsp,
    BLUETOOTH_DEVICE_INFO *pbtdi
  )
  {
    initializedCheck();
    return m_pfnBluetoothFindFirstDevice(pbtsp, pbtdi);
  }

  BOOL BluetoothFindNextDevice(
    HBLUETOOTH_DEVICE_FIND hFind,
    BLUETOOTH_DEVICE_INFO *pbtdi
  )
  {
    initializedCheck();
    return m_pfnBluetoothFindNextDevice(hFind, pbtdi);
  }

  BOOL BluetoothFindDeviceClose(
    HBLUETOOTH_DEVICE_FIND hFind
  )
  {
    initializedCheck();
    return m_pfnBluetoothFindDeviceClose(hFind);
  }

  DWORD BluetoothEnumerateInstalledServices(
    HANDLE hRadio,
    BLUETOOTH_DEVICE_INFO *pbtdi,
    DWORD *pcServices,
    GUID *pGuidServices
  )
  {
    initializedCheck();
    return m_pfnBluetoothEnumerateInstalledServices(hRadio, pbtdi, pcServices, pGuidServices);
  }

protected:
  EsDynamicLibrary::Ptr m_lib;
  pfnBluetoothFindFirstRadio m_pfnBluetoothFindFirstRadio;
  pfnBluetoothFindNextRadio m_pfnBluetoothFindNextRadio;
  pfnBluetoothFindRadioClose m_pfnBluetoothFindRadioClose;
  pfnBluetoothGetRadioInfo m_pfnBluetoothGetRadioInfo;
  pfnBluetoothFindFirstDevice m_pfnBluetoothFindFirstDevice;
  pfnBluetoothFindNextDevice m_pfnBluetoothFindNextDevice;
  pfnBluetoothFindDeviceClose m_pfnBluetoothFindDeviceClose;
  pfnBluetoothEnumerateInstalledServices m_pfnBluetoothEnumerateInstalledServices;
};
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_BLUETOOTH_LE

// We use SetupAPI to enumerate OS-known LE devices (MS allows only so-called 'inbox
// experience') for LE devices
//
#define INITGUID
#include <setupapi.h>
#include <guiddef.h>
#include <bthledef.h>
#undef INITGUID

class EsBtLEsetupApiLib
{
protected:
  typedef HDEVINFO (WINAPI *pfnSetupDiGetClassDevs)(
    const GUID*                       ClassGuid,
    PCTSTR                            Enumerator,
    HWND                              hwndParent,
    DWORD                             Flags
  );

  typedef BOOL (WINAPI *pfnSetupDiEnumDeviceInterfaces)(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVINFO_DATA                  DeviceInfoData,
    const GUID*                       InterfaceClassGuid,
    DWORD                             MemberIndex,
    PSP_DEVICE_INTERFACE_DATA         DeviceInterfaceData
  );

  typedef BOOL (WINAPI *pfnSetupDiGetDeviceInterfaceDetail)(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVICE_INTERFACE_DATA         DeviceInterfaceData,
    PSP_DEVICE_INTERFACE_DETAIL_DATA  DeviceInterfaceDetailData,
    DWORD                             DeviceInterfaceDetailDataSize,
    PDWORD                            RequiredSize,
    PSP_DEVINFO_DATA                  DeviceInfoData
  );

  typedef BOOL (WINAPI *pfnSetupDiDestroyDeviceInfoList)(
    HDEVINFO                          DeviceInfoSet
  );

  typedef BOOL (WINAPI *pfnSetupDiGetDeviceInterfaceProperty)(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVICE_INTERFACE_DATA         DeviceInterfaceData,
    const DEVPROPKEY*                 PropertyKey,
    DEVPROPTYPE*                      PropertyType,
    PBYTE                             PropertyBuffer,
    DWORD                             PropertyBufferSize,
    PDWORD                            RequiredSize,
    DWORD                             Flags
  );

  typedef BOOL (WINAPI *pfnSetupDiEnumDeviceInfo)(
    HDEVINFO                          DeviceInfoSet,
    DWORD                             MemberIndex,
    PSP_DEVINFO_DATA                  DeviceInfoData
  );

  typedef BOOL (WINAPI *pfnSetupDiGetDeviceProperty)(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVINFO_DATA                  DeviceInfoData,
    const DEVPROPKEY*                 PropertyKey,
    DEVPROPTYPE*                      PropertyType,
    PBYTE                             PropertyBuffer,
    DWORD                             PropertyBufferSize,
    PDWORD                            RequiredSize,
    DWORD                             Flags
  );

public:
  EsBtLEsetupApiLib() :
  m_pfnSetupDiGetClassDevs(0),
  m_pfnSetupDiEnumDeviceInterfaces(0),
  m_pfnSetupDiGetDeviceInterfaceDetail(0),
  m_pfnSetupDiDestroyDeviceInfoList(0),
  m_pfnSetupDiGetDeviceInterfaceProperty(0),
  m_pfnSetupDiEnumDeviceInfo(0),
  m_pfnSetupDiGetDeviceProperty(0)
  {
  }

  ~EsBtLEsetupApiLib()
  {
    unload();
  }

  static EsBtLEsetupApiLib& inst()
  {
    static EsBtLEsetupApiLib s_btlib;
    return s_btlib;
  }

  void initializedCheck()
  {
    if( !m_lib )
      EsException::Throw(_("Bluetooth LE engine is not initialized"));
  }

  void load();
  void unload();

  bool initialized() const { return m_lib && m_lib->isOk(); }

  // Mapped library services
  //
  HDEVINFO SetupDiGetClassDevs(
    const GUID*                       ClassGuid,
    PCTSTR                            Enumerator,
    HWND                              hwndParent,
    DWORD                             Flags
  )
  {
    initializedCheck();
    return m_pfnSetupDiGetClassDevs(ClassGuid, Enumerator, hwndParent, Flags);
  }

  BOOL SetupDiEnumDeviceInterfaces(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVINFO_DATA                  DeviceInfoData,
    const GUID*                       InterfaceClassGuid,
    DWORD                             MemberIndex,
    PSP_DEVICE_INTERFACE_DATA         DeviceInterfaceData
  )
  {
    initializedCheck();
    return m_pfnSetupDiEnumDeviceInterfaces(DeviceInfoSet, DeviceInfoData,
      InterfaceClassGuid, MemberIndex, DeviceInterfaceData);
  }

  BOOL SetupDiGetDeviceInterfaceDetail(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVICE_INTERFACE_DATA         DeviceInterfaceData,
    PSP_DEVICE_INTERFACE_DETAIL_DATA  DeviceInterfaceDetailData,
    DWORD                             DeviceInterfaceDetailDataSize,
    PDWORD                            RequiredSize,
    PSP_DEVINFO_DATA                  DeviceInfoData
  )
  {
    initializedCheck();
    return m_pfnSetupDiGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData,
      DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize,
      DeviceInfoData);
  }

  BOOL SetupDiDestroyDeviceInfoList(
    HDEVINFO                          DeviceInfoSet
  )
  {
    initializedCheck();
    return m_pfnSetupDiDestroyDeviceInfoList(DeviceInfoSet);
  }

  BOOL SetupDiGetDeviceInterfaceProperty(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVICE_INTERFACE_DATA         DeviceInterfaceData,
    const DEVPROPKEY*                 PropertyKey,
    DEVPROPTYPE*                      PropertyType,
    PBYTE                             PropertyBuffer,
    DWORD                             PropertyBufferSize,
    PDWORD                            RequiredSize,
    DWORD                             Flags
  )
  {
    initializedCheck();
    return m_pfnSetupDiGetDeviceInterfaceProperty(DeviceInfoSet, DeviceInterfaceData,
      PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize,
      Flags);
  }

  BOOL SetupDiEnumDeviceInfo(
    HDEVINFO                          DeviceInfoSet,
    DWORD                             MemberIndex,
    PSP_DEVINFO_DATA                  DeviceInfoData
  )
  {
    initializedCheck();
    return m_pfnSetupDiEnumDeviceInfo(DeviceInfoSet, MemberIndex, DeviceInfoData);
  }

  BOOL SetupDiGetDeviceProperty(
    HDEVINFO                          DeviceInfoSet,
    PSP_DEVINFO_DATA                  DeviceInfoData,
    const DEVPROPKEY*                 PropertyKey,
    DEVPROPTYPE*                      PropertyType,
    PBYTE                             PropertyBuffer,
    DWORD                             PropertyBufferSize,
    PDWORD                            RequiredSize,
    DWORD                             Flags
  )
  {
    initializedCheck();
    return m_pfnSetupDiGetDeviceProperty(DeviceInfoSet, DeviceInfoData, PropertyKey,
      PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, Flags);
  }

protected:
  EsDynamicLibrary::Ptr m_lib;
  pfnSetupDiGetClassDevs m_pfnSetupDiGetClassDevs;
  pfnSetupDiEnumDeviceInterfaces m_pfnSetupDiEnumDeviceInterfaces;
  pfnSetupDiGetDeviceInterfaceDetail m_pfnSetupDiGetDeviceInterfaceDetail;
  pfnSetupDiDestroyDeviceInfoList m_pfnSetupDiDestroyDeviceInfoList;
  pfnSetupDiGetDeviceInterfaceProperty m_pfnSetupDiGetDeviceInterfaceProperty;
  pfnSetupDiEnumDeviceInfo m_pfnSetupDiEnumDeviceInfo;
  pfnSetupDiGetDeviceProperty m_pfnSetupDiGetDeviceProperty;
};
//---------------------------------------------------------------------------
#endif // ES_COMM_USE_BLUETOOTH_LE

//---------------------------------------------------------------------------
/// Bluetooth Windows radio information node
///
class EsBluetoothRadioWin : public EsBluetoothRadio
{
protected:
  EsBluetoothRadioWin(
    HANDLE hRadio,
    const EsString& addr,
    const EsString& name,
    ulong cod,
    esU16 sub,
    esU16 mfg,
    bool enabled
  ) :
  EsBluetoothRadio(addr, name, cod, sub, mfg, enabled),
  m_hRadio(hRadio)
#ifdef ES_COMM_USE_BLUETOOTH_LE
  ,m_hDevInfo(nullptr)
#endif
  {}

public:
  static EsBaseIntfPtr create(
    HANDLE hRadio,
    const EsString& addr,
    const EsString& name,
    ulong cod,
    esU16 sub,
    esU16 mfg,
    bool enabled
  )
  {
    std::unique_ptr<EsBluetoothRadioWin> ptr = ES_MAKE_UNIQUE(
      EsBluetoothRadioWin,
      hRadio,
      addr,
      name,
      cod,
      sub,
      mfg,
      enabled
    );
    ES_ASSERT(ptr);

    return ptr.release()->asBaseIntfPtr();
  }

  virtual ~EsBluetoothRadioWin() ES_OVERRIDE
  {
#ifdef ES_COMM_USE_BLUETOOTH_LE
    hdevInfoCleanup();
#endif

    CloseHandle(m_hRadio);
    m_hRadio = nullptr;
  }

  HANDLE hRadioGet() const ES_NOTHROW { return m_hRadio; }

#ifdef ES_COMM_USE_BLUETOOTH_LE
  HDEVINFO hdevInfoGet() const ES_NOTHROW { return m_hDevInfo; }

  void hdevInfoRealloc()
  {
    hdevInfoCleanup();

    m_hDevInfo = EsBtLEsetupApiLib::inst().SetupDiGetClassDevs(
      &GUID_BLUETOOTHLE_DEVICE_INTERFACE,
      0,
      0,
      DIGCF_PRESENT|DIGCF_DEVICEINTERFACE
    );
  }

  void hdevInfoCleanup()
  {
    if(!m_hDevInfo)
      return;

    EsBtLEsetupApiLib::inst().SetupDiDestroyDeviceInfoList(m_hDevInfo);
    m_hDevInfo = nullptr;
  }
#endif

protected:
  HANDLE m_hRadio;
#ifdef ES_COMM_USE_BLUETOOTH_LE
  HDEVINFO m_hDevInfo; //< This is maintained dev info list of LE devices for DiXXXX functions
#endif
};
//---------------------------------------------------------------------------

#endif // _es_bluetooth_windows_h_
