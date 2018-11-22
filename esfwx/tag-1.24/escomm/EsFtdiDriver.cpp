#include "escommpch.h"
#pragma hdrstop

#include "EsFtdiDriver.h"
#include "EsFtdiDevice.h"
#include "EsFtdiDeviceMpsseI2c.h"
#include "EsFtdiDeviceMpsseSpi.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_FTDI

namespace EsReflection
{
ES_DECL_ENUMERATION_INFO(EsFtdiDeviceKind, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiDeviceFlag, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiDeviceFlowCtl, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiDevicePurge, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiMpsseI2cClock, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiMpsseI2cIO, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiMpsseSpiCpolCpha, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiMpsseSpiCsLine, NO_CLASS_DESCR)
ES_DECL_ENUMERATION_INFO(EsFtdiMpsseSpiIO, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsFtdiDriver, NO_CLASS_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, enumerate, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, load, EsVariant_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(    EsFtdiDriver, unloadStatic, unload, void_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, libraryVersionGet, EsVariant_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(    EsFtdiDriver, isOkStatic, isOk, EsVariant_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, statusStringGet, EsString_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, statusCheck, bool_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, statusCheckThrow, void_ClassCall_cr_EsVariant, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsFtdiDriver, deviceCreate, EsVariant_ClassCall_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END

//---------------------------------------------------------------------------
#include <algorithm>

EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE::FT_DEVICE_LIST_INFO_NODE() :
Flags(0),
Type(0),
ID(0),
LocId(0),
ftHandle(0)
{
	memset(SerialNumber, 0, sizeof(SerialNumber));
	memset(Description, 0, sizeof(Description));
}
//---------------------------------------------------------------------------

EsString EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE::serialNumberStrGet() const
{
	EsByteString bs(SerialNumber, esBstrnlen(SerialNumber, sizeof(SerialNumber)) );
	return EsString::fromUtf8(bs);
}
//---------------------------------------------------------------------------

EsString EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE::descriptionStrGet() const
{
	EsByteString bs(Description, esBstrnlen(Description, sizeof(Description)) );
	return EsString::fromUtf8(bs);
}
//---------------------------------------------------------------------------

bool EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE::mpsseSupported() const
{
	switch(Type)
	{
  case DEVICE_2232C:
    if((LocId & 0xf)==1)
      return true;
  case DEVICE_2232H:
    if(((LocId & 0xf)==1)|| ((LocId & 0xf)==2))
      return true;
  case DEVICE_4232H:
    if(((LocId & 0xf)==1)|| ((LocId & 0xf)==2))
      return true;
  case DEVICE_232H:
    return true;
	}

  return false;
}
//---------------------------------------------------------------------------

esU32 EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE::mpsseChannelsCntGet() const
{
  if( !mpsseSupported() )
    return 0;

	switch(Type)
	{
  case DEVICE_2232C:
    return 1;
  case DEVICE_2232H:
    return 2;
  case DEVICE_4232H:
    return 2;
  case DEVICE_232H:
    return 1;
	}

  return 0;
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE::toVariant() const
{
  const EsVariant::Array& info = {
    serialNumberStrGet(),
    descriptionStrGet(),
    isOpen(),
    isHiSpeed(),
    flagsGet(),
    typeGet(),
    idGet(),
    locIdGet(),
    mpsseSupported(),
    mpsseChannelsCntGet()
  };

  return info;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsFtdiDriver::EsFtdiDriver() :
m_doThrow(false)
{}
//---------------------------------------------------------------------------

EsFtdiDriver& EsFtdiDriver::instanceGet()
{
  static EsFtdiDriver s_drv;

  return s_drv;
}
//---------------------------------------------------------------------------

bool EsFtdiDriver::isOk() const
{
	EsCriticalSectionLocker lock(m_cs);
	return m_drv && m_drv->isOk();
}
//---------------------------------------------------------------------------

bool EsFtdiDriver::internalLoad()
{
	if( !m_drv )
  {
		m_drv =
#if ES_OS == ES_OS_WINDOWS
    EsDynamicLibrary::load(esT("ftd2xx.dll"), false);
#elif ES_OS == ES_OS_MAC
    EsDynamicLibrary::load(esT("libftd2xx.1.2.2.dylib"), false);
#endif

    if( m_drv )
    {
    // initialize pointers to the ftdi driver entries
    #define FTDI_DRIVER_ENTRY0(Name) \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY1(Name, Param1)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY2(Name, Param1, Param2)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY1(Name, Param1)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY2(Name, Param1, Param2)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY3(Name, Param1, Param2, Param3)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY4(Name, Param1, Param2, Param3, Param4)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #define FTDI_DRIVER_DEV_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  \
      m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv->procAddrGet(esT("FT_") esT(#Name)));
    #include "EsFtdiDriver.win.hxx"
    }
  }

	if( !m_drv )
	{
		if( m_doThrow )
			EsException::Throw(
        _("FTDI device driver is not found or not installed on this device")
      );

    return false;
	}

  return true;
}
//---------------------------------------------------------------------------

EsFtdiDeviceIntf::Ptr EsFtdiDriver::deviceCreate( EsFtdiDeviceKind kind, const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node )
{
  EsFtdiDeviceIntf::Ptr result;
  const EsVariant::Array& info = {
    node.serialNumberStrGet(),
    node.descriptionStrGet()
  };

  switch(kind)
  {
  case EsFtdiDeviceKind::Generic:
    result = EsFtdiDevice::create(*this, node);
    break;
  case EsFtdiDeviceKind::MPSSE_I2C:
    result = EsFtdiDeviceMpsseI2c::create(*this, node);
    break;
  case EsFtdiDeviceKind::MPSSE_SPI:
    result = EsFtdiDeviceMpsseSpi::create(*this, node);
    break;
  }

  return result;
}
//---------------------------------------------------------------------------

bool EsFtdiDriver::internalCheckLoaded()
{
	if( !m_drv )
		return internalLoad();

  return true;
}
//---------------------------------------------------------------------------

void EsFtdiDriver::internalUnload()
{
  if( m_drv )
  {
    //TODO: Signal existing FTDI device objects, that driver is shut down
  }
	m_drv.reset();
}
//---------------------------------------------------------------------------

bool EsFtdiDriver::load(bool doThrow /*= true*/)
{
	EsCriticalSectionLocker lock(m_cs);
	m_doThrow = doThrow;

	return internalLoad();
}
//---------------------------------------------------------------------------

void EsFtdiDriver::unload()
{
	EsCriticalSectionLocker lock(m_cs);

	internalUnload();
}
//---------------------------------------------------------------------------

EsString EsFtdiDriver::statusStringGet( cr_EsVariant stat )
{
	EsString result;
	switch( stat.asInt() )
	{
	case EsFtdiDeviceIntf::FT_OK:
		result = _("Success");
		break;
	case EsFtdiDeviceIntf::FT_INVALID_HANDLE:
		result = _("Invalid handle");
		break;
	case EsFtdiDeviceIntf::FT_DEVICE_NOT_FOUND:
		result = _("Device is not found");
		break;
	case EsFtdiDeviceIntf::FT_DEVICE_NOT_OPENED:
		result = _("Device is not open");
    break;
  case EsFtdiDeviceIntf::FT_IO_ERROR:
		result = _("Driver input-output error");
		break;
	case EsFtdiDeviceIntf::FT_INSUFFICIENT_RESOURCES:
		result = _("Insufficient resources");
		break;
	case EsFtdiDeviceIntf::FT_INVALID_PARAMETER:
		result = _("Invalid parameter");
		break;
	case EsFtdiDeviceIntf::FT_INVALID_BAUD_RATE:
		result = _("Invalid baud rate");
		break;
	case EsFtdiDeviceIntf::FT_DEVICE_NOT_OPENED_FOR_ERASE:
		result = _("Device not open for erase");
		break;
	case EsFtdiDeviceIntf::FT_DEVICE_NOT_OPENED_FOR_WRITE:
		result = _("Device not open for write");
		break;
	case EsFtdiDeviceIntf::FT_FAILED_TO_WRITE_DEVICE:
		result = _("Failed to write device");
		break;
	case EsFtdiDeviceIntf::FT_EEPROM_READ_FAILED:
		result = _("Device EEPROM read failed");
		break;
	case EsFtdiDeviceIntf::FT_EEPROM_WRITE_FAILED:
		result = _("Device EEPROM write failed");
		break;
	case EsFtdiDeviceIntf::FT_EEPROM_ERASE_FAILED:
		result = _("Device EEPROM erase failed");
		break;
	case EsFtdiDeviceIntf::FT_EEPROM_NOT_PRESENT:
		result = _("EEPROM not present in device");
		break;
	case EsFtdiDeviceIntf::FT_EEPROM_NOT_PROGRAMMED:
		result = _("Device EEPROM not programmed");
		break;
	case EsFtdiDeviceIntf::FT_INVALID_ARGS:
		result = _("Invalid arguments");
		break;
	case EsFtdiDeviceIntf::FT_NOT_SUPPORTED:
		result = _("Operation not supported");
		break;
	case EsFtdiDeviceIntf::FT_OTHER_ERROR:
		result = _("Unknown FTDI device error");
		break;
	default:
		result = _("Unknown FTDI status.");
	}

	return result;
}
//---------------------------------------------------------------------------

static void ftdiErrorThrow(int error)
{
  EsException::Throw(
    error,
    EsException::severityGeneric,
    EsException::facilityEsOS,
    _("FTDI call error occured. Returned status is '%s'."),
    EsFtdiDriver::statusStringGet(
      error
    )
  );
}
//---------------------------------------------------------------------------

EsFtdiDriver::DeviceList EsFtdiDriver::deviceListGet(bool onlyOpened /*= true*/)
{
	DeviceList result;
	esU32 devCnt = 0;
	EsCriticalSectionLocker lock(m_cs);

	// Request device count & create device list
	if(
    statusCheck(
      internalCreateDeviceInfoList(&devCnt)
    ) &&
    devCnt
  )
	{
		// Request device list
		result.resize(devCnt);
		statusCheck(
      internalGetDeviceInfoList(
        &result[0],
        &devCnt
      )
    );

    m_devInfos = result;

		// Remove opened devices from list
    if( onlyOpened )
		{
      auto new_end = std::remove_if(
        result.begin(),
        result.end(),
        [](const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& node) -> bool {
          return node.isOpen();
        }
      );
      result.erase(new_end, result.end());
    }
	}

	return result;
}
//---------------------------------------------------------------------------

EsFtdiDeviceIntf::Ptr EsFtdiDriver::deviceCreate(EsFtdiDeviceKind kind, const EsString& ser, const EsString& descr)
{
  EsCriticalSectionLocker lock(m_cs);

  // Look-up device info which matches ser and descr
  for( auto const& v: m_devInfos )
  {
    if( v.serialNumberStrGet() == ser )
    {
      if( !descr.empty() &&
          v.descriptionStrGet() != descr
      )
        continue;

      EsVariant::Array info = {
        v.serialNumberStrGet(),
        v.descriptionStrGet()
      };

      return deviceCreate(
        kind,
        v
      );
    }
  }

  /// Pacify compilers
  return nullptr;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

EsVariant EsFtdiDriver::enumerate(cr_EsVariant includeBusyPorts, cr_EsVariant includeOnlyMPSSE)
{
  const EsFtdiDriver::DeviceList& devs = EsFtdiDriver::instanceGet().deviceListGet(
    !includeBusyPorts.asBool()
  );

  EsVariant::Array result;
  result.reserve( devs.size() );

  for(size_t idx = 0; idx < devs.size(); ++idx)
  {
    const EsFtdiDriver::FT_DEVICE_LIST_INFO_NODE& dev = devs[idx];
    if( !includeBusyPorts.asBool() &&
      dev.isOpen()
    )
      continue;

    if( includeOnlyMPSSE.asBool() &&
      !dev.mpsseSupported()
    )
      continue;

    result.push_back(
      dev.toVariant()
    );
  }

  return result;
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDriver::load(cr_EsVariant doThrow)
{
  return instanceGet().load(
    doThrow.asBool()
  );
}
//---------------------------------------------------------------------------

void EsFtdiDriver::unloadStatic()
{
  instanceGet().unload();
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDriver::deviceCreate(cr_EsVariant kind, cr_EsVariant info)
{
  ulong ukind = kind.asULong();

  EsNumericCheck::checkRangeUInteger(
    static_cast<ulong>(EsFtdiDeviceKind::Generic),
    static_cast<ulong>(EsFtdiDeviceKind::MPSSE_SPI),
    ukind,
    esT("EsFtdiDeviceKind")
  );

  EsString ser, descr;
  if( info.isCollection() )
  {
    ser = info.itemGet(0).asString();

    if( info.countGet() > 1 )
      descr = info.itemGet(1).asString();
  }
  else
    ser = info.asString();

  return EsFtdiDriver::instanceGet().deviceCreate(
    static_cast<EsFtdiDeviceKind>(ukind),
    ser,
    descr
  );
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDriver::libraryVersionGet()
{
  esU32 ver = 0;

  EsFtdiDriver& drv = instanceGet();

  if( drv.isOk() )
  {
    drv.statusCheck(
      drv.ftGetLibraryVersion(
        &ver
      )
    );
  }
  else if( drv.m_doThrow )
    EsException::Throw(
      _("FTDI Driver was not found on this device, or was not loaded")
    );

  return ver;
}
//---------------------------------------------------------------------------

EsVariant EsFtdiDriver::isOkStatic()
{
  return instanceGet().isOk();
}
//---------------------------------------------------------------------------

bool EsFtdiDriver::statusCheck(cr_EsVariant stat)
{
	if( stat.asInt() != EsFtdiDeviceIntf::FT_OK )
  {
    if( instanceGet().m_doThrow )
      statusCheckThrow(stat);

    return false;
  }

  return true;
}
//---------------------------------------------------------------------------

void EsFtdiDriver::statusCheckThrow(cr_EsVariant stat)
{
	if( stat.asInt() != EsFtdiDeviceIntf::FT_OK )
    ftdiErrorThrow(
      stat.asInt()
    );
}
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_FTDI
