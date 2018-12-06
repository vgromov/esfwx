// Windows pltform-specific UART channel methods implementation.
// NB! Do not include this file explicitly in project, it's got implicitly
// included in EsChannelIoUart class code.
//

#ifdef ES_COMM_USE_CHANNEL_UART

// internal consts
enum {
  ES_UART_READ_SLEEP_GRANULARITY = 1,
};

EsChannelIoUart::EsChannelIoUart() :
m_sysError(0),
m_lineError(0),
m_com(0)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}
//---------------------------------------------------------------------------

const EsVariant& EsChannelIoUart::baudsGet()
{
  static EsVariant sc_bauds =
    EsVariant(EsVariant::VAR_VARIANT_COLLECTION)
    .addToVariantCollection(2400)
    .addToVariantCollection(4800)
    .addToVariantCollection(9600)
    .addToVariantCollection(14400)
    .addToVariantCollection(19200)
    .addToVariantCollection(38400)
    .addToVariantCollection(57600)
    .addToVariantCollection(115200)
    .addToVariantCollection(128000)
    .addToVariantCollection(230400)
    .addToVariantCollection(256000)
    .addToVariantCollection(460800)
    .addToVariantCollection(921600);

  return sc_bauds;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_port(const EsString& val)
{
  EsMutexLocker lock(m_mx);
  if( m_port != val )
  {
    if( 0 != m_com )
      EsException::Throw( _("Cannot change port while channel is active."));
    m_port = val;
  }
}
//---------------------------------------------------------------------------

// internal baud setter service
bool EsChannelIoUart::internalSetBaud(const long& val, bool doThrow)
{
  if( m_baud != val )
  {
    if( 0 != m_com )
    {
      if( doThrow )
        EsException::Throw( _("Cannot change baud rate while channel is active."));
      else
        return false;
    }

    m_baud = val;
  }

  return true;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_bits(const long& val)
{
  EsMutexLocker lock(m_mx);
  if( m_bits != val )
  {
    if( 0 != m_com )
      EsException::Throw( _("Cannot change data bit length while channel is active."));
    m_bits = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_parity(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_parity != val )
  {
    if( 0 != m_com )
      EsException::Throw( _("Cannot change parity while channel is active."));
    m_parity = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_stopBits(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_stopBits != val )
  {
    if( 0 != m_com )
      EsException::Throw( _("Cannot change stop bits while channel is active."));
    m_stopBits = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_rxBuffLen(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_rxBuffLen != val )
  {
    if( 0 != m_com )
      EsException::Throw( _("Cannot change RX buffer length while channel is active."));
    m_rxBuffLen = val;
  }
}
//---------------------------------------------------------------------------

void EsChannelIoUart::set_txBuffLen(const long &val)
{
  EsMutexLocker lock(m_mx);
  if( m_txBuffLen != val )
  {
    if( 0 != m_com )
      EsException::Throw( _("Cannot change TX buffer length while channel is active."));
    m_txBuffLen = val;
  }
}
//---------------------------------------------------------------------------

// decode comm line error into string
EsString EsChannelIoUart::decodeLineError(esU32 err)
{
  EsString result;

  if( err & CE_MODE )
    result += _("The requested mode is not supported, or the file handle is invalid.");
  if( err & CE_BREAK )
  {
    if( !result.empty() )
        result += esT('\n');
      result += _("The hardware detected a break condition.");
  }
  if( err & CE_FRAME )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("The hardware detected a framing error.");
  }
  if( err & CE_IOE )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("An I/O error occurred during communications with the device.");
  }
  if( err & CE_OVERRUN )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("A character-buffer overrun has occurred. The next character is lost.");
  }
  if( err & CE_RXOVER )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.");
  }
  if( err & CE_RXPARITY )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("The hardware detected a parity error.");
  }
  if( err & CE_TXFULL )
  {
    if( !result.empty() )
      result += esT('\n');
    result += _("The application tried to transmit a character, but the output buffer was full.");
  }

  return result;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::internalLineBreak()
{
  SetCommBreak(m_com);
  EsThread::sleep(txTimeEstimateGet(10));
  ClearCommBreak(m_com);
}
//---------------------------------------------------------------------------

void EsChannelIoUart::lineBreak()
{
  EsMutexLocker lock(m_mx);
  if( 0 != m_com )
    internalLineBreak();
}
//---------------------------------------------------------------------------

// reset error & copmm state
void EsChannelIoUart::resetError()
{
  if( 0 != m_com )
  {
    DWORD err = m_lineError;
    ClearCommError(m_com, &err, nullptr);

    m_sysError = m_lineError = 0;
  }
}
//---------------------------------------------------------------------------

// open port using current configuration
bool EsChannelIoUart::internalOpen()
{
  bool result = false;
  m_sysError = m_lineError = 0;
  m_com = CreateFile( m_port.c_str(),
    GENERIC_READ|GENERIC_WRITE|SYNCHRONIZE,
    0, // exclusive - do not share
    0, // do not inherit handle
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL/* | FILE_FLAG_OVERLAPPED*/,
    0);

  if( INVALID_HANDLE_VALUE == m_com )
    m_sysError = EsUtilities::osErrorCodeGet();
  else
  {
    DCB dcb;
    memset(&dcb, 0, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = m_baud;
    dcb.fBinary = TRUE;
    dcb.fParity = m_parity != 0;
    dcb.ByteSize = (esU8)m_bits;
    dcb.Parity = (esU8)m_parity;
    dcb.StopBits = (esU8)m_stopBits;

    resetError();

 #ifdef ES_COMM_USE_ANCIENT_PC_DELAY
    EsThread::sleep(600);   //< On some ancient PC configurations, like P4+P4P800
                            //< this delay is neccessary, otherwise port
                            //< will not be configured in following methods, and any
                            //< access to it would generate error 31 (device is not functioning properly)
#endif

    if( SetCommState(m_com, &dcb) )
    {
      if( SetupComm(m_com, m_rxBuffLen, m_txBuffLen ) )
      {
        COMMTIMEOUTS tms;
        if( GetCommTimeouts(m_com, &tms) )
        {
          tms.ReadTotalTimeoutConstant = 0;
          tms.ReadTotalTimeoutMultiplier = 0;
          tms.ReadIntervalTimeout = MAXDWORD;
          if( SetCommTimeouts(m_com, &tms) && EscapeCommFunction(m_com, SETDTR) )
          {
            result = true;
            internalReset();
            ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelOpened, internalIdGet());
          }
          else
            m_sysError = EsUtilities::osErrorCodeGet();
        }
        else
          m_sysError = EsUtilities::osErrorCodeGet();
      }
      else
        m_sysError = EsUtilities::osErrorCodeGet();
    }
    else
      m_sysError = EsUtilities::osErrorCodeGet();
  }

  if( m_sysError )
  {
    CloseHandle(m_com);
    m_com = 0;

    checkLogLastError();
  }

  return result;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::internalClose()
{
  if( 0 != m_com )
  {
    internalReset();
    EscapeCommFunction(m_com, CLRDTR);
    CloseHandle(m_com);
    m_com = 0;
    ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelClosed, internalIdGet());
  }
}
//---------------------------------------------------------------------------

// port manipulation
ES_IMPL_INTF_METHOD(bool, EsChannelIoUart::open)()
{
  EsMutexLocker lock(m_mx);
  if( 0 != m_com )
    return true;
  else
    return internalOpen();
}
//---------------------------------------------------------------------------

ES_IMPL_INTF_METHOD(bool, EsChannelIoUart::isOpen)() const
{
  EsMutexLocker lock(m_mx);
  return 0 != m_com;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::internalPutBytes(const esU8* data, esU32 len, esU32 tmo)
{
  const esU8* pos = data;
  const esU8* end = data+len;

  if( m_com != 0 )
  {
    resetError();
    while( 
      !checkLogLastError() &&
      !internalIsBreaking() &&
      pos < end 
    )
    {
      DWORD result = 0;
      // EsThread::Yield();
      if( !WriteFile(m_com, data, len, &result, nullptr) )
        m_sysError = EsUtilities::osErrorCodeGet();

      DWORD err = m_lineError;
      ClearCommError(m_com, &err, nullptr);
      m_lineError = err;

      // send "bytes sent" event
      if( result )
          ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesSent, internalIdGet(), EsBinBuffer(data, data+result));

      // move to the next buffer position
      pos += result;
    }

    if( internalIsBreaking() )
      internalLineBreak();
  }

  return static_cast<esU32>(pos-data);
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::internalGetBytes(esU8* data, esU32 len, esU32 tmo)
{
  bool timeout = false;
  esU32 localTmo = tmo;
  esU32 spentTmo = 0;
  esU8* pos = data;
  esU8* end = data+len;

  if( 0 != m_com && len )
  {
    resetError();
    while( !timeout &&
          !checkLogLastError() &&
          pos < end &&
          !internalIsBreaking() )
    {
      esU32 toRead = std::min(
          queryUnreadBytes(), 
          static_cast<esU32>(end-pos)
      );

      if( toRead )
      {
        localTmo = (tmo > spentTmo) ? tmo-spentTmo : 0; // reset local timeout
        DWORD result = 0;
        m_sysError = 0;

        if( !ReadFile(m_com, pos, toRead, &result, nullptr) )
          m_sysError = EsUtilities::osErrorCodeGet();

        // send "bytes received" event
        if( result )
          ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesReceived, internalIdGet(), EsBinBuffer(data, data+result));

        // move to the next buffer position
        pos += result;
      }
      else // enter bytes polling
      {
        if( tmo )
          EsThread::sleep(ES_UART_READ_SLEEP_GRANULARITY);

        if(localTmo > ES_UART_READ_SLEEP_GRANULARITY)
        {
          localTmo -= ES_UART_READ_SLEEP_GRANULARITY;
          spentTmo += ES_UART_READ_SLEEP_GRANULARITY;
        }
        else
        {
          timeout = true;
          if( m_resetOnRxTmo )
          {
            CancelIo(m_com);
            PurgeComm(m_com, PURGE_RXABORT|PURGE_RXCLEAR);
          }
        }
      }
    }
  }

  return static_cast<esU32>(pos-data);
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::queryUnreadBytes()
{
  COMSTAT stat;
  memset(&stat, 0, sizeof(COMSTAT));

  DWORD err = m_lineError;
  if( !ClearCommError(m_com, &err, &stat) )
    m_sysError = EsUtilities::osErrorCodeGet();
  m_lineError = err;

  return stat.cbInQue;
}
//---------------------------------------------------------------------------

esU32 EsChannelIoUart::queryUnsentBytes()
{
  COMSTAT stat;
  memset(&stat, 0, sizeof(COMSTAT));

  DWORD err = m_lineError;
  if( !ClearCommError(m_com, &err, &stat) )
    m_sysError = EsUtilities::osErrorCodeGet();
  m_lineError = err;

  return stat.cbOutQue;
}
//---------------------------------------------------------------------------

void EsChannelIoUart::internalReset()
{
  if( 0 != m_com )
  {
    CancelIo(m_com);
    PurgeComm(m_com, PURGE_RXABORT|PURGE_RXCLEAR|
                      PURGE_TXABORT|PURGE_TXCLEAR);
    resetError();
  }
}
//---------------------------------------------------------------------------
#endif // ES_COMM_USE_CHANNEL_UART

// UART devices enumerator
//

// The next 3 includes are needed for serial port enumeration
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>

//---------------------------------------------------------------
typedef HKEY      (WINAPI SETUPDIOPENDEVREGKEY)             (HDEVINFO, PSP_DEVINFO_DATA, esU32, esU32, esU32, REGSAM);
typedef esBL      (WINAPI SETUPDICLASSGUIDSFROMNAME)        (LPCTSTR, LPGUID, esU32, PDWORD);
typedef HDEVINFO  (WINAPI SETUPDIGETCLASSDEVS)              (LPGUID, LPCTSTR, HWND, esU32);
typedef esBL      (WINAPI SETUPDIGETDEVICEREGISTRYPROPERTY) (HDEVINFO, PSP_DEVINFO_DATA, esU32, PDWORD, PBYTE, esU32, PDWORD);
typedef esBL      (WINAPI SETUPDIDESTROYDEVICEINFOLIST)     (HDEVINFO);
typedef esBL      (WINAPI SETUPDIENUMDEVICEINFO)            (HDEVINFO, esU32, PSP_DEVINFO_DATA);
typedef esBL      (WINAPI SETUPDIGETDEVICEINSTANCEID)       (HDEVINFO, PSP_DEVINFO_DATA, PTSTR, esU32, PDWORD);

EsUartEnumerator::EsUartEnumerator()
{}

void EsUartEnumerator::enumerate(bool busyPortsInclude /*= true*/)
{
  // Clear the output array
  m_infos.clear();
  m_infos.reserve(8);

  static EsRegEx s_rexCom(
    esT("COM[0-9]+.*"),
    static_cast<ulong>(EsRegExCompileFlag::DEFAULT)|
    static_cast<ulong>(EsRegExCompileFlag::NOSUB)
  );

  EsDynamicLibrary::Ptr setupapi = EsDynamicLibrary::load( esT("setupapi.dll") );

  if( !setupapi )
    return;

  SETUPDIOPENDEVREGKEY* setupDiOpenDevRegKey =
    reinterpret_cast<SETUPDIOPENDEVREGKEY*>(setupapi->procAddrGet( esT("SetupDiOpenDevRegKey") ));
  SETUPDICLASSGUIDSFROMNAME* setupDiClassGuidsFromName =
    reinterpret_cast<SETUPDICLASSGUIDSFROMNAME*>(setupapi->procAddrGetAorW( esT("SetupDiClassGuidsFromName") ));
  SETUPDIGETCLASSDEVS* setupDiGetClassDevs =
    reinterpret_cast<SETUPDIGETCLASSDEVS*>(setupapi->procAddrGetAorW( esT("SetupDiGetClassDevs") ));
  SETUPDIGETDEVICEREGISTRYPROPERTY* setupDiGetDeviceRegistryProperty =
    reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(setupapi->procAddrGetAorW( esT("SetupDiGetDeviceRegistryProperty") ));
  SETUPDIDESTROYDEVICEINFOLIST* setupDiDestroyDeviceInfoList =
    reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(setupapi->procAddrGet( esT("SetupDiDestroyDeviceInfoList") ));
  SETUPDIENUMDEVICEINFO* setupDiEnumDeviceInfo =
    reinterpret_cast<SETUPDIENUMDEVICEINFO*>(setupapi->procAddrGet( esT("SetupDiEnumDeviceInfo") ));
  SETUPDIGETDEVICEINSTANCEID* setupDiGetDeviceInstanceId =
    reinterpret_cast<SETUPDIGETDEVICEINSTANCEID*>(setupapi->procAddrGetAorW( esT("SetupDiGetDeviceInstanceId") ));

  if( (setupDiOpenDevRegKey == 0) ||
      (setupDiClassGuidsFromName == 0) ||
      (setupDiDestroyDeviceInfoList == 0) ||
      (setupDiEnumDeviceInfo == 0) ||
      (setupDiGetClassDevs == 0) ||
      (setupDiGetDeviceRegistryProperty == 0)
  )
    return;

  //First need to convert the name "Ports" to a GUID using SetupDiClassGuidsFromName
  DWORD dwGuids = 0;
  setupDiClassGuidsFromName(esT("Ports"), 0, 0, &dwGuids);
  if(dwGuids == 0)
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());

  //allocate guids array
  std::vector<GUID> guids;
  guids.resize(dwGuids);

  //Call the function again
  if( !setupDiClassGuidsFromName(esT("Ports"), &guids[0], dwGuids, &dwGuids) )
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());

  // Now create a "device information set" which is required to enumerate all the ports
  HDEVINFO hDevInfoSet = setupDiGetClassDevs(&guids[0], 0, 0, DIGCF_PRESENT);
  if(hDevInfoSet == INVALID_HANDLE_VALUE)
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());

  // Finally do the enumeration
  esBL bMoreItems = TRUE;
  int idx = 0;
  SP_DEVINFO_DATA devInfo;
  while( bMoreItems )
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    bMoreItems = setupDiEnumDeviceInfo(hDevInfoSet, idx, &devInfo);
    if(bMoreItems)
    {
      // empty info
      EsUartInfo::Ptr info( new EsUartInfo );

      // Get the registry key which stores the ports settings
      HKEY hDeviceKey = setupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if(hDeviceKey)
      {
        //Read in the name of the port
        EsString portName(256, 0);
        DWORD dwSize = static_cast<DWORD>(portName.size()*EsString::value_size);
        DWORD dwType = 0;
        if( 
          RegQueryValueEx(
            hDeviceKey, 
            esT("PortName"), 
            NULL, 
            &dwType,
            reinterpret_cast<LPBYTE>(&portName[0]), 
            &dwSize
          ) == ERROR_SUCCESS &&
          dwType == REG_SZ
        )
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          s_rexCom.set_text(portName.c_str());
          if( s_rexCom.isOk() && s_rexCom.get_matches() )
          {
            info->m_strName = portName.c_str();
            info->m_strFriendlyName = info->m_strName;
            info->m_strDevPath = esT("\\\\?\\") + info->m_strName;
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      // If the port was OK, then also try to get its friendly name &&
      // optional path id
      if( !info->isEmpty() )
      {
        EsString portName(256, 0);
        DWORD dwSize = static_cast<DWORD>(portName.size()*sizeof(EsString::value_type));
        DWORD dwType = 0;
        if(
          setupDiGetDeviceRegistryProperty(
            hDevInfoSet, 
            &devInfo, 
            SPDRP_DEVICEDESC, 
            &dwType,
            reinterpret_cast<PBYTE>(&portName[0]), 
            dwSize, 
            &dwSize
          ) && 
          dwType == REG_SZ
        )
        {
          info->m_strFriendlyName = EsString::format( 
            esT("%s (%s)"), 
            portName,
            info->m_strFriendlyName
          );
        }

        if( 
          setupDiGetDeviceInstanceId &&
          setupDiGetDeviceInstanceId(
            hDevInfoSet, 
            &devInfo, 
            &portName[0], 
            static_cast<esU32>(portName.size()),
            &dwSize
          ) 
        )
        {
          portName.resize(dwSize);
          info->m_strInstPath = portName;
          info->m_isUSBdevice = 0 == portName.find(esT("USB"));
        }

        m_infos.push_back(info);
      }
    }

    ++idx;
  }

  //Free up the "device information set" now that we are finished with it
  setupDiDestroyDeviceInfoList(hDevInfoSet);

  for( size_t idx = 0; idx < m_infos.size(); ++idx )
  {
    EsUartInfo::Ptr info = m_infos[idx];
    ES_ASSERT(info);

    // Come up with a name for the device.
    // If there is no friendly name, use the port name.
    if( info->friendlyNameGet().empty() )
      info->m_strFriendlyName = info->m_strName;
  }
}
//---------------------------------------------------------------------------

EsUartEnumerator::EsUartAvail EsUartEnumerator::availabilityCheck(const EsUartInfo::Ptr& info)
{
  EsUartAvail result = uartAvailable;

  ES_ASSERT(info);

  // Try to open the device
  HANDLE hFile =
    ::CreateFile(info->devicePathGet().c_str(),
       GENERIC_READ|GENERIC_WRITE,
       0,
       0,
       OPEN_EXISTING,
       0,
       0
    );

  // Check if we could open the device
  if(hFile == INVALID_HANDLE_VALUE)
  {
    // Display error
    switch( ::GetLastError() )
    {
    case ERROR_FILE_NOT_FOUND:
      // The specified COM-port does not exist
      result = uartNotPresent;
      break;
    case ERROR_ACCESS_DENIED:
      // The specified COM-port is in use
      result = uartInUse;
      break;
    default:
      // Something else is wrong
      result = uartUnknown;
      break;
    }
  }

  // Close handle
  ::CloseHandle(hFile);

  // Port is available
  return result;
}
//---------------------------------------------------------------------------

