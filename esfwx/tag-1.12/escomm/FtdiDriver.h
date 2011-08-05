#ifndef _ftdi_driver_h_
#define _ftdi_driver_h_

// ftdi driver wrapper
class EKOSF_COMM_CLASS FtdiDriver
{
public:
	// public types
	typedef ULONG FT_HANDLE;
	typedef ULONG FT_DEVICE;
	// driver call status 
	enum FT_STATUS {
		FT_OK,
		FT_INVALID_HANDLE,
		FT_DEVICE_NOT_FOUND, 
		FT_DEVICE_NOT_OPENED,
		FT_IO_ERROR,
		FT_INSUFFICIENT_RESOURCES,
		FT_INVALID_PARAMETER,
		FT_INVALID_BAUD_RATE,
		FT_DEVICE_NOT_OPENED_FOR_ERASE,
		FT_DEVICE_NOT_OPENED_FOR_WRITE,
		FT_FAILED_TO_WRITE_DEVICE,
		FT_EEPROM_READ_FAILED,
		FT_EEPROM_WRITE_FAILED,
		FT_EEPROM_ERASE_FAILED,
		FT_EEPROM_NOT_PRESENT,
		FT_EEPROM_NOT_PROGRAMMED,
		FT_INVALID_ARGS,
		FT_NOT_SUPPORTED,
		FT_OTHER_ERROR,
	};
	// misc driver constants
	enum {
		// FT_OpenEx Flags
		FT_OPEN_BY_SERIAL_NUMBER			= 1,
		FT_OPEN_BY_DESCRIPTION				= 2,
		// FT_ListDevices Flags (used in conjunction with FT_OpenEx Flags
		FT_LIST_NUMBER_ONLY						= 0x80000000,
		FT_LIST_BY_INDEX							= 0x40000000,
		FT_LIST_ALL										= 0x20000000,
		FT_LIST_MASK									= (FT_LIST_NUMBER_ONLY|FT_LIST_BY_INDEX|FT_LIST_ALL),
		// FT_FLAGS (see FT_DEVICE_LIST_INFO_NODE) 
		FT_FLAGS_OPENED								= 0x00000001,
		FT_FLAGS_HISPEED							= 0x00000002,
		// flow control
		FT_FLOW_NONE									= 0x0000,
		FT_FLOW_RTS_CTS								= 0x0100,
		FT_FLOW_DTR_DSR								= 0x0200,
		FT_FLOW_XON_XOFF							= 0x0400,
		// Purge rx and tx buffers
		FT_PURGE_RX										= 1,
		FT_PURGE_TX										= 2,
		// Events
		FT_EVENT_RXCHAR								= 1,
		FT_EVENT_MODEM_STATUS					= 2,
		FT_NOTIFY_ON_UNPLUG						= 128,
		// Baud Rates
		FT_BAUD_300										= 300,
		FT_BAUD_600										= 600,
		FT_BAUD_1200									= 1200,
		FT_BAUD_2400									= 2400,
		FT_BAUD_4800									= 4800,
		FT_BAUD_9600									= 9600,
		FT_BAUD_14400									= 14400,
		FT_BAUD_19200									= 19200,
		FT_BAUD_38400									= 38400,
		FT_BAUD_57600									= 57600,
		FT_BAUD_115200								= 115200,
		FT_BAUD_230400								= 230400,
		FT_BAUD_460800								= 460800,
		FT_BAUD_921600								= 921600,
		// Word Lengths
		FT_BITS_8											= 8,
		FT_BITS_7											= 7,
		FT_BITS_6											= 6,
		FT_BITS_5											= 5,
		// Stop Bits
		FT_STOP_BITS_1								= 0,
		FT_STOP_BITS_1_5							= 1,
		FT_STOP_BITS_2								= 2,
		// Parity
		FT_PARITY_NONE								= 0,
		FT_PARITY_ODD									= 1,
		FT_PARITY_EVEN								= 2,
		FT_PARITY_MARK								= 3,
		FT_PARITY_SPACE								= 4,
		// Timeouts
		FT_DEFAULT_RX_TIMEOUT					= 300,
		FT_DEFAULT_TX_TIMEOUT					= 300,
		// buffer lengths
		FT_DEFAULT_RX_BUFFER					= 4096,
		FT_DEFAULT_TX_BUFFER					= 4096,
		// device types
		FT_DEVICE_232BM								= 0,
		FT_DEVICE_232AM								= 1, 
		FT_DEVICE_100AX								= 2, 
		FT_DEVICE_UNKNOWN							= 3, 
		FT_DEVICE_2232C								= 4,
		FT_DEVICE_232R								= 5, 
		FT_DEVICE_2232H								= 6, 
		FT_DEVICE_4232H								= 7,
		// bits mode
		FT_BITMODE_RESET							= 0x0,	// Reset 
		FT_BITMODE_ASYNC_BITBANG			= 0x1,	// Asynchronous Bit Bang 
		FT_BITMODE_MPSSE							= 0x2,	// MPSSE (FT4232H, FT2232H and FT2232 devices only) 
		FT_BITMODE_SYNC_BITBANG				= 0x4,	// Synchronous Bit Bang (FT4232H, FT2232H, FT232R, FT245R and FT2232 devices only) 
		FT_BITMODE_MCU_HOSTBUS_EMU		= 0x8,	// MCU Host Bus Emulation Mode (FT4232H, FT2232H, FT232R, FT245R and FT2232 devices only) 
		FT_BITMODE_FAST_OI_SERIAL			= 0x10, // Fast Opto-Isolated Serial Mode (FT4232H, FT2232H, FT232R, FT245R and FT2232 devices only) 
		FT_BITMODE_CBUS								= 0x20, // CBUS Bit Bang Mode (FT232R devices only)  
		FT_BITMODE_SC_SYNC						= 0x40, // Single Channel Synchronous 245 FIFO Mode (FT2232H devices only)
	};
	
	// event callback
	typedef void (*PFT_EVENT_HANDLER)(DWORD,DWORD);

	// driver structures
	//
	struct FT_DEVICE_LIST_INFO_NODE 
	{ 
	public:
		FT_DEVICE_LIST_INFO_NODE();
		inline DWORD getFlags() const { return Flags; }
		inline DWORD getType() const { return Type; }
		inline DWORD getId() const { return ID; }
		inline DWORD getLocId() const { return LocId; }
		inline const char* getSerialNumber() const { return SerialNumber; }
		inline const char* getDescription() const { return Description; }
		EsString getSerialNumberStr() const;
		EsString getDescriptionStr() const;
		// flags checker
		inline bool isOpen() const { return FT_FLAGS_OPENED == (Flags & FT_FLAGS_OPENED); }
		inline bool isHiSpeed() const { return FT_FLAGS_HISPEED == (Flags & FT_FLAGS_HISPEED); }
		
	protected:
		DWORD Flags;  
		DWORD Type;  
		DWORD ID;  
		DWORD LocId;  
		char SerialNumber[16];  
		char Description[64];   
		FT_HANDLE ftHandle;	
	};
	// device infos array
	typedef std::vector<FT_DEVICE_LIST_INFO_NODE> DeviceList;

public:
	FtdiDriver();
	
	// refcounted load|unload
	void load();
	void unload();
	// object check
	bool isOk() const { wxMutexLocker lock(m_mx);  return m_ok; }
	// return list of free (not-opened) devices found.
	DeviceList getDeviceList();
	// generate driver call wrappers
	#define FTDI_DRIVER_ENTRY0(Name) FT_STATUS ft## Name() { wxMutexLocker lock(m_mx);  return internal## Name(); }
	#define FTDI_DRIVER_ENTRY1(Name, Param1) FT_STATUS ft## Name(Param1 p1) { wxMutexLocker lock(m_mx);  return internal## Name(p1); }
	#define FTDI_DRIVER_ENTRY2(Name, Param1, Param2) FT_STATUS ft## Name(Param1 p1, Param2 p2) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2); }
	#define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2, p3); }
	#define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2, p3, p4); }
	#define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2, p3, p4, p5); }
	#define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2, p3, p4, p5, p6); }
	#define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2, p3, p4, p5, p6, p7); }
	#define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) { wxMutexLocker lock(m_mx);  return internal## Name(p1, p2, p3, p4, p5, p6, p7, p8); }
	#include "FtdiDriver.hxx"
	
	// static services
	//
	// return textual representation of ftdi driver entry call status
	static EsString getStatusString(int stat);
	// check status returned from driver entry call, and throw exception if 
	// it's not OK
	static void checkThrowStatus(int stat);
	
protected:
	// internal services
	void internalLoad();
	void internalCheckLoaded();
	void internalUnload();
	// generate internal driver call wrappers
	#define FTDI_DRIVER_ENTRY0(Name) FT_STATUS internal## Name() { internalCheckLoaded(); return m_pfn## Name(); }
	#define FTDI_DRIVER_ENTRY1(Name, Param1) FT_STATUS internal## Name(Param1 p1) { internalCheckLoaded(); return m_pfn## Name(p1); }
	#define FTDI_DRIVER_ENTRY2(Name, Param1, Param2) FT_STATUS internal## Name(Param1 p1, Param2 p2) { internalCheckLoaded(); return m_pfn## Name(p1, p2); }
	#define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3) { internalCheckLoaded(); return m_pfn## Name(p1, p2, p3); }
	#define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4) { internalCheckLoaded(); return m_pfn## Name(p1, p2, p3, p4); }
	#define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { internalCheckLoaded(); return m_pfn## Name(p1, p2, p3, p4, p5); }
	#define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) { internalCheckLoaded(); return m_pfn## Name(p1, p2, p3, p4, p5, p6); }
	#define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) { internalCheckLoaded(); return m_pfn## Name(p1, p2, p3, p4, p5, p6, p7); }
	#define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) { internalCheckLoaded(); return m_pfn## Name(p1, p2, p3, p4, p5, p6, p7, p8); }
	#include "FtdiDriver.hxx"
	
protected:
	// MT access guard
	mutable wxMutex m_mx;
	// library usage refcount. if reached 0 - call unload on driver library
	long m_ref;
	// ok flag
	bool m_ok;
	// ftdi driver library
	wxDynamicLibrary m_drv;
	// generate member variables-pointer to ftdi driver entries
	#define FTDI_DRIVER_ENTRY0(Name) typedef FT_STATUS (WINAPI* Name ##_T)(); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY1(Name, Param1) typedef FT_STATUS (WINAPI* Name ##_T)(Param1); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY2(Name, Param1, Param2) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2, Param3); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2, Param3, Param4); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2, Param3, Param4, Param5); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6, Param7); Name ##_T m_pfn## Name;
	#define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) typedef FT_STATUS (WINAPI* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8); Name ##_T m_pfn## Name;
	#include "FtdiDriver.hxx"
};

// Ftdi channel class
class FtdiIoChannel
{
public:
	FtdiIoChannel();
	virtual ~FtdiIoChannel();
	
	// static services
	//
	// return list of free (not-opened) devices found.
	static inline FtdiDriver::DeviceList getDeviceList() { return s_drv.getDeviceList(); }
	
protected:
	// data members
	//
	static FtdiDriver s_drv;
};

#endif // _ftdi_driver_h_
