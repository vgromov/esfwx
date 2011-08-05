#include <stdafx.h>
#pragma hdrstop

#include "FtdiDriver.h"
#include <algorithm>

FtdiDriver::FT_DEVICE_LIST_INFO_NODE::FT_DEVICE_LIST_INFO_NODE() :
Flags(0),
Type(0),
ID(0),
LocId(0),
ftHandle(0)
{
	memset(SerialNumber, 0, sizeof(SerialNumber));  
	memset(Description, 0, sizeof(Description));   
}

EsString FtdiDriver::FT_DEVICE_LIST_INFO_NODE::getSerialNumberStr() const
{
	return wxString(SerialNumber, *wxConvCurrent, sizeof(SerialNumber));
}

EsString FtdiDriver::FT_DEVICE_LIST_INFO_NODE::getDescriptionStr() const
{
	return wxString(Description, *wxConvCurrent, sizeof(Description));
}

FtdiDriver::FtdiDriver() :
m_ref(0),
m_ok(false)
{}

void FtdiDriver::internalLoad()
{
	if( !m_drv.IsLoaded() )
		m_drv.Load(wxT("ftd2xx.dll"), wxDL_DEFAULT|wxDL_QUIET);
		
	if( !m_drv.IsLoaded() )
		EsException::Throw(0, EsException::Generic, _("FTDI device driver is not found or not installed on this machine."));
	else if( 0 == m_ref )
	{
	// initialize pointers to the ftdi driver entries
	#define FTDI_DRIVER_ENTRY0(Name) \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY1(Name, Param1)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY2(Name, Param1, Param2)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  \
		m_pfn## Name = reinterpret_cast<Name ##_T>(m_drv.GetSymbol(wxT("FT_") wxT(#Name))); \
		if( !m_pfn## Name ) EsException::Throw(0, EsException::Generic, _("Could not find '%s' entry in FTDI driver."), wxT("FT_") wxT(#Name));
	#include "FtdiDriver.hxx"	
	
		++m_ref;
		m_ok = true;
	}
	else
		++m_ref;
}

void FtdiDriver::internalCheckLoaded()
{
	if( !m_ok )
		internalLoad();
}

void FtdiDriver::internalUnload()
{
	if( m_ref )
	{
		if( 0 == --m_ref )
		{
			m_drv.Unload();
			m_ok = false;
		}
	}
}

void FtdiDriver::load()
{
	wxMutexLocker lock(m_mx); 
	
	internalLoad();
}

void FtdiDriver::unload()
{
	wxMutexLocker lock(m_mx); 
	
	internalUnload();
}

// return textual representation of ftdi driver entry call status
EsString FtdiDriver::getStatusString(int stat)
{
	EsString result;
	switch(stat)
	{
	case FT_OK:
		result = _("Success");
		break;
	case FT_INVALID_HANDLE:
		result = _("Invalid handle");
		break;
	case FT_DEVICE_NOT_FOUND:
		result = _("Device is not found");
		break;
	case FT_DEVICE_NOT_OPENED:
		result = _("Device is not open");
    break;
  case FT_IO_ERROR:
		result = _("Driver input-output error");
		break;
	case FT_INSUFFICIENT_RESOURCES:
		result = _("Insufficient resources");
		break;
	case FT_INVALID_PARAMETER:
		result = _("Invalid parameter");
		break;
	case FT_INVALID_BAUD_RATE:
		result = _("Invalid baud rate");
		break;
	case FT_DEVICE_NOT_OPENED_FOR_ERASE:
		result = _("Device not open for erase");
		break;
	case FT_DEVICE_NOT_OPENED_FOR_WRITE:
		result = _("Device not open for write");
		break;
	case FT_FAILED_TO_WRITE_DEVICE:
		result = _("Failed to write device");
		break;
	case FT_EEPROM_READ_FAILED:
		result = _("Device EEPROM read failed");
		break;
	case FT_EEPROM_WRITE_FAILED:
		result = _("Device EEPROM write failed");
		break;
	case FT_EEPROM_ERASE_FAILED:
		result = _("Device EEPROM erase failed");
		break;
	case FT_EEPROM_NOT_PRESENT:
		result = _("EEPROM not present in device");
		break;
	case FT_EEPROM_NOT_PROGRAMMED:
		result = _("Device EEPROM not programmed");
		break;
	case FT_INVALID_ARGS:
		result = _("Invalid arguments");
		break;
	case FT_NOT_SUPPORTED:
		result = _("Operation not supported");
		break;
	case FT_OTHER_ERROR:
		result = _("Unknown FTDI device error");
		break;
	default:
		result = _("Unknown FTDI status.");
	}
	
	return result;
}

// check status returned from driver entry call, and throw exception if 
// it's not OK
void FtdiDriver::checkThrowStatus(int stat)
{
	if( stat != FT_OK )
		EsException::Throw(stat, EsException::Generic, _("FTDI call error occured. Returned status is '%s'."), 
			getStatusString(stat).c_str() );
}

// return list of connected devices
static bool deviceIsOpen(const FtdiDriver::FT_DEVICE_LIST_INFO_NODE& node)
{
	return node.isOpen();
}

FtdiDriver::DeviceList FtdiDriver::getDeviceList()
{
	DeviceList result;
	DWORD devCnt;
	wxMutexLocker lock(m_mx); 
	
	// request device count & create device list
	checkThrowStatus( internalCreateDeviceInfoList(&devCnt) );
	if( devCnt )
	{
		// request device list
		result.resize(devCnt);
		checkThrowStatus( internalGetDeviceInfoList(&result[0], &devCnt) );
		// remove opened devices from list
		DeviceList::iterator new_end = std::remove_if(result.begin(), result.end(), 
			deviceIsOpen );
		result.erase(new_end, result.end());
	}
		
	return result;
}

// ----------------------------------------------------------------------
// FtdiIoChannel implementation
//

FtdiDriver FtdiIoChannel::s_drv;

FtdiIoChannel::FtdiIoChannel()
{
	s_drv.load();
}

FtdiIoChannel::~FtdiIoChannel()
{
	s_drv.unload();
}
