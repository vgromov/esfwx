//---------------------------------------------------------------------------
 
#ifndef EsEnumCommPortsH
#define EsEnumCommPortsH

//structure used for port enumeration method
// Struct used when enumerating the available serial ports
// Holds information about an individual serial port.
struct EKOSF_COMM_CLASS EsCommPortInfo
{
	EsCommPortInfo() : m_bModem(false), m_bUSBDevice(false) {}
	EsString m_strDevPath;       	// Device path for use with CreateFile()
	EsString m_strPortName;      	// Simple name (i.e. COM1)
	EsString m_strFriendlyName;  	// Full name to be displayed to a user
	bool m_bModem;							 		// modem is connected to this port
	bool m_bUSBDevice;           		// Provided through a USB connection?

	bool isOk() const 
	{ 
		return !m_strDevPath.empty() && 
			!m_strPortName.empty() &&
			!m_strFriendlyName.empty(); 
	}

	void operator =(const EsCommPortInfo& crefSrc )
	{
		m_strDevPath = crefSrc.m_strDevPath;
		m_strPortName = crefSrc.m_strPortName;
		m_strFriendlyName = crefSrc.m_strFriendlyName;
		m_bUSBDevice = crefSrc.m_bUSBDevice;
	}
};

class EKOSF_COMM_CLASS EsCommPortInfos
{
public:
	enum EsPortAvail {
		EsPaAvail, 
		EsPaNPresent, 
		EsPaInUse, 
		EsPaUnk};

public:
	void enumerate(bool bIgnoreBusyPorts = true);
	
	size_t size() const { return m_infos.size(); }
	const EsCommPortInfo& operator[](size_t idx) const { return m_infos[idx]; }
	const EsCommPortInfo& findPortInfoByDevPath(const EsString& val) const;
	const EsCommPortInfo& findPortInfoByPortName(const EsString& val) const;
	const EsCommPortInfo& findPortInfoByFriendlyName(const EsString& val) const; 

	static EsPortAvail checkPort(const EsString& strDevice);

private:
	// Helpers for EnumSerialPorts
	void EnumPortsWdm(bool bIgnoreBusyPorts);
	void EnumPortsWNt4(bool bIgnoreBusyPorts);
	void EnumPortsW9x(bool bIgnoreBusyPorts);
	void SearchPnpKeyW9x(HKEY hkPnp, bool bUsbDevice, bool bIgnoreBusyPorts);

protected:
	static const EsCommPortInfo s_emptyPortInfo;
	typedef std::vector<EsCommPortInfo> InfosArray;
#pragma warning(push)
#pragma warning(disable: 4251)
	InfosArray m_infos;
#pragma warning(pop)
};

//---------------------------------------------------------------------------
#endif
