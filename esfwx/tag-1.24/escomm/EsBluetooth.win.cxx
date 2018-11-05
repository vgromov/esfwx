// Windows-specific bluetooth stuff
// NB! Do not include this file in project, it's got implicitly included from EsBluetooth.cpp
//
#include "EsBluetooth.win.h"

//---------------------------------------------------------------------------

void EsBtLib::load()
{
  if( !m_lib )
    m_lib = EsDynamicLibrary::load(esT("bthprops.cpl"), true);

  ES_ASSERT(m_lib);
  m_pfnBluetoothFindFirstRadio = (pfnBluetoothFindFirstRadio)m_lib->procAddrGet(esT("BluetoothFindFirstRadio"));
  m_pfnBluetoothFindNextRadio = (pfnBluetoothFindNextRadio)m_lib->procAddrGet(esT("BluetoothFindNextRadio"));
  m_pfnBluetoothFindRadioClose = (pfnBluetoothFindRadioClose)m_lib->procAddrGet(esT("BluetoothFindRadioClose"));
  m_pfnBluetoothGetRadioInfo = (pfnBluetoothGetRadioInfo)m_lib->procAddrGet(esT("BluetoothGetRadioInfo"));
  m_pfnBluetoothFindFirstDevice = (pfnBluetoothFindFirstDevice)m_lib->procAddrGet(esT("BluetoothFindFirstDevice"));
  m_pfnBluetoothFindNextDevice = (pfnBluetoothFindNextDevice)m_lib->procAddrGet(esT("BluetoothFindNextDevice"));
  m_pfnBluetoothFindDeviceClose = (pfnBluetoothFindDeviceClose)m_lib->procAddrGet(esT("BluetoothFindDeviceClose"));
  m_pfnBluetoothEnumerateInstalledServices = (pfnBluetoothEnumerateInstalledServices)m_lib->procAddrGet(esT("BluetoothEnumerateInstalledServices"));
}
//---------------------------------------------------------------------------

void EsBtLib::unload()
{
  if( m_lib )
  {
    m_pfnBluetoothFindFirstRadio = 0;
    m_pfnBluetoothFindNextRadio = 0;
    m_pfnBluetoothFindRadioClose = 0;
    m_pfnBluetoothGetRadioInfo = 0;
    m_pfnBluetoothFindFirstDevice = 0;
    m_pfnBluetoothFindNextDevice = 0;
    m_pfnBluetoothFindDeviceClose = 0;
    m_pfnBluetoothEnumerateInstalledServices = 0;
    m_lib->unload();
    m_lib.reset();
  }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#ifdef ES_COMM_USE_BLUETOOTH_LE

void EsBtLEsetupApiLib::load()
{
  if( !m_lib )
    m_lib = EsDynamicLibrary::load(esT("setupapi.dll"), true);

  ES_ASSERT(m_lib);
  m_pfnSetupDiGetClassDevs = (pfnSetupDiGetClassDevs)m_lib->procAddrGetAorW(esT("SetupDiGetClassDevs"));
  m_pfnSetupDiEnumDeviceInterfaces = (pfnSetupDiEnumDeviceInterfaces)m_lib->procAddrGet(esT("SetupDiEnumDeviceInterfaces"));
  m_pfnSetupDiGetDeviceInterfaceDetail = (pfnSetupDiGetDeviceInterfaceDetail)m_lib->procAddrGetAorW(esT("SetupDiGetDeviceInterfaceDetail"));
  m_pfnSetupDiDestroyDeviceInfoList = (pfnSetupDiDestroyDeviceInfoList)m_lib->procAddrGet(esT("SetupDiDestroyDeviceInfoList"));
  m_pfnSetupDiGetDeviceInterfaceProperty = (pfnSetupDiGetDeviceInterfaceProperty)m_lib->procAddrGetAorW(esT("SetupDiGetDeviceInterfaceProperty"));
  m_pfnSetupDiEnumDeviceInfo = (pfnSetupDiEnumDeviceInfo)m_lib->procAddrGet(esT("SetupDiEnumDeviceInfo"));
  m_pfnSetupDiGetDeviceProperty = (pfnSetupDiGetDeviceProperty)m_lib->procAddrGetAorW(esT("SetupDiGetDeviceProperty"));
}
//---------------------------------------------------------------------------

void EsBtLEsetupApiLib::unload()
{
  if( m_lib )
  {
    m_pfnSetupDiGetClassDevs = 0;
    m_pfnSetupDiEnumDeviceInterfaces = 0;
    m_pfnSetupDiGetDeviceInterfaceDetail = 0;
    m_pfnSetupDiDestroyDeviceInfoList = 0;
    m_pfnSetupDiGetDeviceInterfaceProperty = 0;
    m_pfnSetupDiEnumDeviceInfo = 0;
    m_pfnSetupDiGetDeviceProperty = 0;
    m_lib->unload();
    m_lib.reset();
  }
}
#endif //< ES_COMM_USE_BLUETOOTH_LE
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

class EsBluetoothDeviceWin : public EsBluetoothDevice
{
public:
  EsBluetoothDeviceWin(
    ulong flags = static_cast<ulong>(EsBluetoothDeviceFlag::None),
    ulong cod = EsBluetoothDeviceClass::UNKNOWN,
    const EsString& addr = EsString::null(),
    const EsString& name = EsString::null(),
    const EsDateTime& lastSeen = EsDateTime::null(),
    const EsDateTime& lastUsed = EsDateTime::null(),
    const EsBluetoothServiceUUIDs& services = noServices()
  ) :
  EsBluetoothDevice(flags, cod, addr, name, lastSeen, lastUsed, services)
  {}
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// EsBluetoothRadioEnumerator implementation
//
// Common helpers used in radio|device enumeration
static inline bool systemTimeIsEmpty(const SYSTEMTIME& st)
{
  return 0 == st.wYear &&
    0 == st.wMonth &&
    0 == st.wDayOfWeek &&
    0 == st.wDay &&
    0 == st.wHour &&
    0 == st.wMinute &&
    0 == st.wSecond &&
    0 == st.wMilliseconds;
}
//---------------------------------------------------------------------------

static inline void dateTimeFromSystemTime(EsDateTime& dt, const SYSTEMTIME& st)
{
  if( systemTimeIsEmpty(st) )
    dt = EsDateTime::null();
  else
    dt.compose(
      st.wYear,
      st.wMonth,
      st.wDay,
      st.wHour,
      st.wMinute,
      st.wSecond,
      st.wMilliseconds
    );
}
//---------------------------------------------------------------------------

typedef bool (EsBluetoothDeviceEnumerator:: *pfnOnEnumeratedT)(const EsBluetoothDevice::Ptr&);
static void btdevsFind(
  const EsBluetoothRadio::Ptr& pradio,
  ulong flags,
  bool doInquiry,
  ulong inquiryTmo,
  EsBluetoothDevices& devs,
  EsBluetoothDeviceEnumerator* This,
  pfnOnEnumeratedT pfnOnEnumerated
  )
{
  EsBluetoothRadioWin* wradio = dynamic_cast<EsBluetoothRadioWin*>(pradio.get());
  HANDLE hRadio = wradio ? wradio->hRadioGet() : (HANDLE)0;

  BLUETOOTH_DEVICE_SEARCH_PARAMS sp;
  memset(&sp, 0, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));
  sp.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
  if( flags & static_cast<ulong>(EsBluetoothDeviceFlag::Authenticated) )
    sp.fReturnAuthenticated = TRUE;
  else
    sp.fReturnAuthenticated = FALSE;

  if( flags & static_cast<ulong>(EsBluetoothDeviceFlag::Remembered) )
    sp.fReturnRemembered = TRUE;
  else
    sp.fReturnRemembered = FALSE;

  if( flags & static_cast<ulong>(EsBluetoothDeviceFlag::Unknown) )
    sp.fReturnUnknown = TRUE;
  else
    sp.fReturnUnknown = FALSE;

  if( flags & static_cast<ulong>(EsBluetoothDeviceFlag::Connected) )
    sp.fReturnConnected = TRUE;
  else
    sp.fReturnConnected = FALSE;

  sp.fIssueInquiry = doInquiry ? TRUE : FALSE;
  if( doInquiry )
  {
    EsNumericCheck::checkRangeUInteger(1, 48, inquiryTmo, _("Bluetooth Inquery timeout increment"));
    sp.cTimeoutMultiplier = static_cast<UCHAR>(inquiryTmo);
  }
  sp.hRadio = hRadio;

  BLUETOOTH_DEVICE_INFO info;
  info.dwSize = sizeof(info);

  HBLUETOOTH_DEVICE_FIND hfind =
  EsBtLib::inst().BluetoothFindFirstDevice(
    &sp,
    &info
  );

  if( hfind )
  {
    BOOL ok = TRUE;
    EsDateTime lastSeen;
    EsDateTime lastUsed;

    do
    {
      EsBluetoothServiceUUIDs svcs;

      DWORD guidsCnt = 8;
      std::vector<GUID> guids;
      guids.resize(guidsCnt);
      // Enumerate services for found device info
      DWORD result = ERROR_MORE_DATA;
      while( ERROR_MORE_DATA == result )
      {
        result = EsBtLib::inst().BluetoothEnumerateInstalledServices(
          hRadio,
          &info,
          &guidsCnt,
          &guids[0]
        );

        if( ERROR_MORE_DATA == result )
          guids.resize( guidsCnt );
      }

      if( ERROR_SUCCESS == result )
      {
        guids.resize( guidsCnt );

        for(size_t idx = 0; idx < guids.size(); ++idx )
          svcs.push_back( EsBluetoothServiceUUID( guids[idx] ) );
      }

      // Create device info record, and pass it to callback
      flags = static_cast<ulong>(EsBluetoothDeviceFlag::None);
      if( info.fConnected )
        flags |= static_cast<ulong>(EsBluetoothDeviceFlag::Connected);
      if( info.fRemembered )
        flags |= static_cast<ulong>(EsBluetoothDeviceFlag::Remembered);
      if( info.fAuthenticated )
        flags |= static_cast<ulong>(EsBluetoothDeviceFlag::Authenticated);

      dateTimeFromSystemTime(lastSeen, info.stLastSeen);
      dateTimeFromSystemTime(lastUsed, info.stLastUsed);

      EsBluetoothDevice::Ptr dev(
        new EsBluetoothDeviceWin(
          flags,
          info.ulClassofDevice,
          EsBluetooth::addrStringGet(info.Address.ullLong),
          info.szName,
          lastSeen,
          lastUsed,
          svcs
        )
      );

      if( This && pfnOnEnumerated )
      {
        if( !(This->*pfnOnEnumerated)( dev ) )
          break;
      }

      // Append device info to the collection
      devs.push_back( dev );

      // Proceed to the next device info record
      ok = EsBtLib::inst().BluetoothFindNextDevice(
        hfind,
        &info
      );

    } while( ok );

    ok = EsBtLib::inst().BluetoothFindDeviceClose(hfind);
    if( !ok )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );
  }
}
//---------------------------------------------------------------------------

void EsBluetoothRadioEnumerator::execute()
{
  reset();

  BLUETOOTH_RADIO_INFO info;
  info.dwSize = sizeof(info);

  HANDLE hradio = 0;
  BLUETOOTH_FIND_RADIO_PARAMS params;
  params.dwSize = sizeof(params);

  HBLUETOOTH_RADIO_FIND hfind =
  EsBtLib::inst().BluetoothFindFirstRadio(
    &params,
    &hradio
  );

  if( hfind )
  {
    BOOL ok = TRUE;

    do
    {
      if( ERROR_SUCCESS == EsBtLib::inst().BluetoothGetRadioInfo( hradio, &info ) )
      {
        EsBluetoothRadio::Ptr radInfo(
          new EsBluetoothRadioWin(
            hradio,
            EsBluetooth::addrStringGet(info.address.ullLong),
            info.szName,
            info.ulClassofDevice,
            info.lmpSubversion,
            info.manufacturer,
            true
          )
        );

        if( !onRadioEnumerated( radInfo ) )
          break;

        // Append device info to the collection
        m_infos.push_back( radInfo );
      }

      // Proceed to the next device info record
      ok = EsBtLib::inst().BluetoothFindNextRadio(
        hfind,
        &hradio
      );

    } while( ok );

    ok = EsBtLib::inst().BluetoothFindRadioClose(hfind);
    if( !ok )
      EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );
  }
}

//---------------------------------------------------------------------------
// Device enumerator implementation
//

/// Execute Bluetooth device enumeration
void EsBluetoothDeviceEnumerator::execute()
{
  reset();

  btdevsFind(
    m_radio,
    m_flags,
    m_doInquiry,
    m_inquiryTmo,
    m_infos,
    this,
    &EsBluetoothDeviceEnumerator::onDeviceEnumerated
  );
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
/// Bluetooth global services

/// Global initialization
static EsAtomicInteger s_ref = 0;

void EsBluetooth::initialize()
{
  if( 0 == s_ref )
    EsBtLib::inst().load();

  esAtomicInc(s_ref);
}
//---------------------------------------------------------------------------

/// Global initialization check
bool EsBluetooth::initialized()
{
  return EsBtLib::inst().initialized();
}
//---------------------------------------------------------------------------

/// Global uninitialization
void EsBluetooth::uninitialize()
{
  if( s_ref )
    esAtomicDec(s_ref);

  if( 0 == s_ref )
    EsBtLib::inst().unload();
}
//---------------------------------------------------------------------------


