//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "EsEnumCommPorts.h"

const EsCommPortInfo EsCommPortInfos::s_emptyPortInfo;

//---------------------------------------------------------------------------
const wxChar* ES_STR_USB = wxT("USB");
const wxChar* ES_STR_MODEM = wxT("Modem");

#define E_CANNOTGETOSVERSION_S1_D2				_("Cannot get OS version in %s, (err=%lx)")
#define CANNOT_READ_FROM_S1								_("Cannot not read from %s")
/*************************************************************************
* Serial port enumeration routines
*
* The EnumSerialPort function will populate an array of EsCommPortInfo structs,
* each of which contains information about one serial port present in
* the system. Note that this code must be linked with setupapi.lib,
* which is included with the Win32 SDK.
*
* by Zach Gorman <gormanjz@hotmail.com>
*
* Copyright (c) 2002 Archetype Auction Software, Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following condition is
* met: Redistributions of source code must retain the above copyright
* notice, this condition and the following disclaimer.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL ARCHETYPE AUCTION SOFTWARE OR ITS
* AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
************************************************************************/

// The next 3 includes are needed for serial port enumeration
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>

//---------------------------------------------------------------
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.
void EsCommPortInfos::enumerate(bool bIgnoreBusyPorts /*= true*/)
{
	// Clear the output array
	m_infos.clear();

	// Use different techniques to enumerate the available serial
	// ports, depending on the OS we're using
	OSVERSIONINFO vi;
	vi.dwOSVersionInfoSize = sizeof(vi);
	if(	!::GetVersionEx(&vi) )
	{
		int err = GetLastError();
		EsException::Throw( err, EsException::Generic,
			E_CANNOTGETOSVERSION_S1_D2,
			wxT("EsEnumCommPorts"), err );
	}

	// Handle windows 9x and NT4 specially
	if(vi.dwMajorVersion < 5)
	{
		if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
			EnumPortsWNt4(bIgnoreBusyPorts);
		else
			EnumPortsW9x(bIgnoreBusyPorts);
	}
	else
	{
		// Win2k and later support a standard API for
		// enumerating hardware devices.
		EnumPortsWdm(bIgnoreBusyPorts);
	}

	InfosArray::iterator it = m_infos.begin();
	while(it != m_infos.end())
	{
		// Come up with a name for the device.
		// If there is no friendly name, use the port name.
		if( it->m_strFriendlyName.empty() )
			it->m_strFriendlyName = it->m_strPortName;

		++it;
	}
}
//---------------------------------------------------------------------------

typedef HKEY			(WINAPI SETUPDIOPENDEVREGKEY)							(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
typedef BOOL			(WINAPI SETUPDICLASSGUIDSFROMNAME)				(LPCTSTR, LPGUID, DWORD, PDWORD);
typedef HDEVINFO	(WINAPI SETUPDIGETCLASSDEVS)							(LPGUID, LPCTSTR, HWND, DWORD);
typedef BOOL			(WINAPI SETUPDIGETDEVICEREGISTRYPROPERTY)	(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);
typedef BOOL			(WINAPI SETUPDIDESTROYDEVICEINFOLIST)			(HDEVINFO);
typedef BOOL			(WINAPI SETUPDIENUMDEVICEINFO)						(HDEVINFO, DWORD, PSP_DEVINFO_DATA);

// Helpers for EnumSerialPorts
void EsCommPortInfos::EnumPortsWdm(bool bIgnoreBusyPorts)
{
	m_infos.clear();
	m_infos.reserve(32);
	wxRegEx rexCom(wxT("COM[0-9]+.*"));	
	wxDynamicLibrary setupapi( wxT("setupapi.dll") );
	
	if( !setupapi.IsLoaded() )
		return;
	
	SETUPDIOPENDEVREGKEY* setupDiOpenDevRegKey = 
		reinterpret_cast<SETUPDIOPENDEVREGKEY*>(setupapi.GetSymbol( wxT("SetupDiOpenDevRegKey") ));
  SETUPDICLASSGUIDSFROMNAME* setupDiClassGuidsFromName = 
		reinterpret_cast<SETUPDICLASSGUIDSFROMNAME*>(setupapi.GetSymbolAorW( wxT("SetupDiClassGuidsFromName") ));
  SETUPDIGETCLASSDEVS* setupDiGetClassDevs = 
		reinterpret_cast<SETUPDIGETCLASSDEVS*>(setupapi.GetSymbolAorW( wxT("SetupDiGetClassDevs") ));
  SETUPDIGETDEVICEREGISTRYPROPERTY* setupDiGetDeviceRegistryProperty = 
		reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(setupapi.GetSymbolAorW( wxT("SetupDiGetDeviceRegistryProperty") ));
  SETUPDIDESTROYDEVICEINFOLIST* setupDiDestroyDeviceInfoList = 
		reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(setupapi.GetSymbol( wxT("SetupDiDestroyDeviceInfoList") ));
  SETUPDIENUMDEVICEINFO* setupDiEnumDeviceInfo = 
		reinterpret_cast<SETUPDIENUMDEVICEINFO*>(setupapi.GetSymbol( wxT("SetupDiEnumDeviceInfo") ));

  if ((setupDiOpenDevRegKey == 0) || (setupDiClassGuidsFromName == 0) || (setupDiDestroyDeviceInfoList == 0) ||
      (setupDiEnumDeviceInfo == 0) || (setupDiGetClassDevs == 0) || (setupDiGetDeviceRegistryProperty == 0))
		return;
  
  //First need to convert the name "Ports" to a GUID using SetupDiClassGuidsFromName
  DWORD dwGuids = 0;
  setupDiClassGuidsFromName(wxT("Ports"), 0, 0, &dwGuids);
  if(dwGuids == 0)
  {
		DWORD err = wxSysErrorCode();
		EsException::Throw(err, EsException::Generic, wxSysErrorMsg(err));
	}	

  //allocate guids array
  std::vector<GUID> guids;
  guids.resize(dwGuids);

  //Call the function again
  if(!setupDiClassGuidsFromName(wxT("Ports"), &guids[0], dwGuids, &dwGuids))
  {
		DWORD err = wxSysErrorCode();
		EsException::Throw(err, EsException::Generic, wxSysErrorMsg(err));
  }

  //Now create a "device information set" which is required to enumerate all the ports
  HDEVINFO hDevInfoSet = setupDiGetClassDevs(&guids[0], 0, 0, DIGCF_PRESENT);
  if(hDevInfoSet == INVALID_HANDLE_VALUE)
  {
		DWORD err = wxSysErrorCode();
		EsException::Throw(err, EsException::Generic, wxSysErrorMsg(err));
  }

  //Finally do the enumeration
  BOOL bMoreItems = TRUE;
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
      EsCommPortInfo info = s_emptyPortInfo;

      //Get the registry key which stores the ports settings
      HKEY hDeviceKey = setupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if(hDeviceKey)
      {
        //Read in the name of the port
        wxString portName;
        DWORD dwSize = 256*sizeof(wxChar);
        DWORD dwType = 0;
  	    if(	RegQueryValueEx(hDeviceKey, wxT("PortName"), NULL, &dwType, 
  				reinterpret_cast<LPBYTE>(
  					static_cast<wxChar*>(wxStringBuffer(portName, 256))), &dwSize) == ERROR_SUCCESS && 
  					dwType == REG_SZ)
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          if( rexCom.IsValid() && rexCom.Matches(portName) )
          {
						info.m_strPortName = portName;
						info.m_strFriendlyName = portName;
						info.m_strDevPath = wxT("\\\\?\\") + portName;
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      //If the port was a serial port, then also try to get its friendly name
      if( info.isOk() )
      {
        wxString portName;
        DWORD dwSize = 256*sizeof(wxChar);
        DWORD dwType = 0;
        if(setupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, 
					reinterpret_cast<PBYTE>(
						static_cast<wxChar*>(wxStringBuffer(portName, 256))), dwSize, &dwSize) && dwType == REG_SZ)
        {
					info.m_strFriendlyName = EsString::format( wxT("%s (%s)"), portName.t_str(), 
						info.m_strFriendlyName.c_str() );
        }
        
        m_infos.push_back(info);
      }
    }

    ++idx;
  }

  //Free up the "device information set" now that we are finished with it
  setupDiDestroyDeviceInfoList(hDevInfoSet);
}

void EsCommPortInfos::EnumPortsWNt4(bool bIgnoreBusyPorts)
{
	// NT4's driver model is totally different, and not that
	// many people use NT4 anymore. Just try all the COM ports
	// between 1 and 16
	EsCommPortInfo si;
	for (int ii=1; ii<=16; ii++)
	{
		EsString strPort = EsString::format( wxT("COM%d"), ii );
		si.m_strDevPath = wxT("\\\\.\\") + strPort;
		si.m_strPortName = strPort;

		if( (bIgnoreBusyPorts && (EsCommPortInfos::checkPort(si.m_strDevPath.c_str()) == EsCommPortInfos::EsPaAvail)) ||
			!bIgnoreBusyPorts )
			m_infos.push_back(si);
	}
}

void EsCommPortInfos::EnumPortsW9x(bool bIgnoreBusyPorts)
{
	// Look at all keys in HKLM\Enum, searching for subkeys named
	// *PNP0500 and *PNP0501. Within these subkeys, search for
	// sub-subkeys containing value entries with the name "PORTNAME"
	// Search all subkeys of HKLM\Enum\USBPORTS for PORTNAME entries.

	// First, open HKLM\Enum
	HKEY hkEnum = NULL;
	HKEY hkSubEnum = NULL;
	HKEY hkSubSubEnum = NULL;

	try
	{
		if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, wxT("Enum"), 0, KEY_READ,	&hkEnum) != ERROR_SUCCESS)
			EsException::Throw( 0, EsException::Generic, CANNOT_READ_FROM_S1, wxT("HKLM\\Enum") );

		// Enumerate the subkeys of HKLM\Enum
		wxChar acSubEnum[128];
		DWORD dwSubEnumIndex = 0;
		DWORD dwSize = sizeof(acSubEnum);
		while(RegEnumKeyEx(hkEnum, dwSubEnumIndex++, acSubEnum, &dwSize,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hkSubEnum = NULL;
			if (RegOpenKeyEx(hkEnum, acSubEnum, 0, KEY_READ,
				&hkSubEnum) != ERROR_SUCCESS)
			{
				EsString strKeyName( wxT("HKLM\\Enum\\") );
				strKeyName += acSubEnum;

				EsException::Throw( 0, EsException::Generic, CANNOT_READ_FROM_S1, strKeyName.c_str() );
			}
			// Enumerate the subkeys of HKLM\Enum\*\, looking for keys
			// named *PNP0500 and *PNP0501 (or anything in USBPORTS)
			bool bUsbDevice = (_tcscmp(acSubEnum, wxT("USBPORTS"))==0);
			wxChar acSubSubEnum[128];
			dwSize = sizeof(acSubSubEnum);  // set the buffer size
			DWORD dwSubSubEnumIndex = 0;
			while (RegEnumKeyEx(hkSubEnum, dwSubSubEnumIndex++, acSubSubEnum,
				&dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
			{
				BOOL bMatch = (_tcscmp(acSubSubEnum, wxT("*PNP0500"))==0 ||
					_tcscmp(acSubSubEnum, wxT("*PNP0501"))==0 ||
					bUsbDevice);
				if (bMatch)
				{
					HKEY hkSubSubEnum = NULL;
					if (RegOpenKeyEx(hkSubEnum, acSubSubEnum, 0, KEY_READ,
						&hkSubSubEnum) != ERROR_SUCCESS)
					{
						EsString strKeyName( wxT("HKLM\\Enum\\") );
						strKeyName = strKeyName + acSubEnum + wxT("\\") + acSubSubEnum;

						EsException::Throw( 0, EsException::Generic, CANNOT_READ_FROM_S1, strKeyName.c_str() );
					}
					SearchPnpKeyW9x(hkSubSubEnum, bUsbDevice, bIgnoreBusyPorts);
					RegCloseKey(hkSubSubEnum);
					hkSubSubEnum = NULL;
				}

				dwSize = sizeof(acSubSubEnum);  // restore the buffer size
			}

			RegCloseKey(hkSubEnum);
			hkSubEnum = NULL;
			dwSize = sizeof(acSubEnum); // restore the buffer size
		}
	}
	catch(...) {}

	if (hkEnum != NULL)
		RegCloseKey(hkEnum);
	if (hkSubEnum != NULL)
		RegCloseKey(hkSubEnum);
	if (hkSubSubEnum != NULL)
		RegCloseKey(hkSubSubEnum);
}

void EsCommPortInfos::SearchPnpKeyW9x(HKEY hkPnp, bool bUsbDevice, bool bIgnoreBusyPorts)
{
	// Enumerate the subkeys of the given PNP key, looking for values with
	// the name "PORTNAME"
	// First, open HKLM\Enum
	HKEY hkSubPnp = NULL;

	try
	{
		// Enumerate the subkeys of HKLM\Enum\*\PNP050[01]
		wxChar acSubPnp[128];
		DWORD dwSubPnpIndex = 0;
		DWORD dwSize = sizeof(acSubPnp);
		while (RegEnumKeyEx(hkPnp, dwSubPnpIndex++, acSubPnp, &dwSize,
			NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		{
			HKEY hkSubPnp = NULL;
			if (RegOpenKeyEx(hkPnp, acSubPnp, 0, KEY_READ,
				&hkSubPnp) != ERROR_SUCCESS)
			{
				EsString strKeyName( wxT("HKLM\\Enum\\..\\") );
				strKeyName += acSubPnp;

				EsException::Throw( 0, EsException::Generic, CANNOT_READ_FROM_S1, strKeyName.c_str() );
			}

			// Look for the PORTNAME value
			wxChar acValue[128];
			dwSize = sizeof(acValue);
			if (RegQueryValueEx(hkSubPnp, wxT("PORTNAME"), NULL, NULL, (BYTE*)acValue,
				&dwSize) == ERROR_SUCCESS)
			{
				EsString strPortName(acValue);

				// Got the portname value. Look for a friendly name.
				EsString strFriendlyName;
				dwSize = sizeof(acValue);
				if (RegQueryValueEx(hkSubPnp, wxT("FRIENDLYNAME"), NULL, NULL, (BYTE*)acValue,
					&dwSize) == ERROR_SUCCESS)
					strFriendlyName = acValue;

				// Prepare an entry for the output array.
				EsCommPortInfo si;
				si.m_strDevPath = wxT("\\\\.\\") + strPortName;
				si.m_strPortName = strPortName;
				si.m_strFriendlyName = strFriendlyName;
				si.m_bUSBDevice = bUsbDevice;

				// Overwrite duplicates.
				BOOL bDup = FALSE;
				InfosArray::iterator it = m_infos.begin();
				while( (it != m_infos.end()) && !bDup )
				{
					if( it->m_strPortName == strPortName)
					{
						bDup = TRUE;
						(*it) = si;
					}
					++it;
				}

				if(!bDup)
				{
					// Add an entry to the array
					if( (bIgnoreBusyPorts && (EsCommPortInfos::checkPort(si.m_strDevPath.c_str()) == EsCommPortInfos::EsPaAvail)) ||
						!bIgnoreBusyPorts )
						m_infos.push_back(si);
				}
			}
 
			RegCloseKey(hkSubPnp);
			hkSubPnp = NULL;
			dwSize = sizeof(acSubPnp);  // restore the buffer size
		}
	}
	catch(...) {}

	if (hkSubPnp != NULL)
		RegCloseKey(hkSubPnp);
}
//---------------------------------------------------------------------------

const EsCommPortInfo& EsCommPortInfos::findPortInfoByDevPath(const EsString& val) const
{
	InfosArray::const_iterator cit = m_infos.begin();
	while( cit != m_infos.end() )
	{
		if( cit->m_strDevPath == val )
			return *cit;
		++cit;
	}

	return s_emptyPortInfo;
}

const EsCommPortInfo& EsCommPortInfos::findPortInfoByPortName(const EsString& val) const
{
	InfosArray::const_iterator cit = m_infos.begin();
	while( cit != m_infos.end() )
	{
		if( cit->m_strPortName == val )
			return *cit;
		++cit;
	}

	return s_emptyPortInfo;
}

const EsCommPortInfo& EsCommPortInfos::findPortInfoByFriendlyName(const EsString& val) const
{
	InfosArray::const_iterator cit = m_infos.begin();
	while( cit != m_infos.end() )
	{
		if( cit->m_strFriendlyName == val )
			return *cit;
		
		++cit;
	}

	return s_emptyPortInfo;
}

EsCommPortInfos::EsPortAvail EsCommPortInfos::checkPort(const EsString& strDevice)
{
	EsPortAvail esPa = EsPaAvail;
	HANDLE hFile = NULL;

	// Try to open the device
	hFile = ::CreateFile(strDevice.c_str(),
						 GENERIC_READ|GENERIC_WRITE,
						 0,
						 0,
						 OPEN_EXISTING,
						 0,
						 0);

	// Check if we could open the device
	if(hFile == INVALID_HANDLE_VALUE)
	{
		// Display error
		switch( ::GetLastError() )
		{
		case ERROR_FILE_NOT_FOUND:
			// The specified COM-port does not exist
			esPa = EsPaNPresent;
			break;

		case ERROR_ACCESS_DENIED:
			// The specified COM-port is in use
			esPa = EsPaInUse;
			break;

		default:
			// Something else is wrong
			esPa = EsPaUnk;
			break;
		}
	}

	// Close handle
	::CloseHandle(hFile);
	
	// Port is available
	return esPa;
}
