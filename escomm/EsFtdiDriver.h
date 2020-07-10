#ifndef _ftdi_driver_h_
#define _ftdi_driver_h_

#ifdef ES_COMM_USE_FTDI

/// Forward decls
class EsFtdiDriver;

/// FTDI device interface specification
///
ES_DECL_INTF_BEGIN(80FC64D2, 701F41E0, 9C40C863, 9FCE8569, EsFtdiDeviceIntf)

public:
  /// FTDI device handle type
  typedef void* FT_HANDLE;

  /// Device type
  typedef esU32  FT_DEVICE;

  /// Driver call status
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

  // Applicable Latencies
  enum {
    LATENCY_MIN                     = 1,
    LATENCY_MAX                     = 255,
    LATENCY_DEF                     = 16
  };

public:
  /// Object validity check
  ES_DECL_INTF_METHOD(bool, isValid)() const = 0;

  /// Return recent driver call status
  ES_DECL_INTF_METHOD(FT_STATUS, statusGet)() const = 0;

  /// Bound device info access
  ES_DECL_INTF_METHOD(esU32, flagsGet)() const = 0;
  ES_DECL_INTF_METHOD(esU32, typeGet)() const = 0;
  ES_DECL_INTF_METHOD(esU32, idGet)() const = 0;
  ES_DECL_INTF_METHOD(esU32, locIdGet)() const = 0;
  ES_DECL_INTF_METHOD(EsString, serialNumberStrGet)() const = 0;
  ES_DECL_INTF_METHOD(EsString, descriptionStrGet)() const = 0;
  ES_DECL_INTF_METHOD(bool, isOpen)() const = 0;
  ES_DECL_INTF_METHOD(bool, isHiSpeed)() const = 0;
  ES_DECL_INTF_METHOD(bool, isMpsseSupported)() const = 0;
  ES_DECL_INTF_METHOD(esU32, mpsseChannelsCntGet)() const = 0;

  /// Open or close Device
  ES_DECL_INTF_METHOD(bool, open)() = 0;
  ES_DECL_INTF_METHOD(void, close)() = 0;

  /// Generate FTDI generic device services
  #define FTDI_DRIVER_ENTRY0(Name)
  #define FTDI_DRIVER_ENTRY1(Name, Param1)
  #define FTDI_DRIVER_ENTRY2(Name, Param1, Param2)
  #define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3)
  #define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4)
  #define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5)
  #define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6)
  #define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7)
  #define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)
  #define FTDI_DRIVER_DEV_ENTRY1(Name, Param1) ES_DECL_INTF_METHOD(bool, ft## Name)() = 0;
  #define FTDI_DRIVER_DEV_ENTRY2(Name, Param1, Param2) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2) = 0;
  #define FTDI_DRIVER_DEV_ENTRY3(Name, Param1, Param2, Param3) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3) = 0;
  #define FTDI_DRIVER_DEV_ENTRY4(Name, Param1, Param2, Param3, Param4) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4) = 0 ;
  #define FTDI_DRIVER_DEV_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5)  = 0;
  #define FTDI_DRIVER_DEV_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) = 0;
  #define FTDI_DRIVER_DEV_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) = 0;
  #define FTDI_DRIVER_DEV_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) ES_DECL_INTF_METHOD(bool, ft## Name)(Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) = 0;
  #include "EsFtdiDriver.win.hxx"

ES_DECL_INTF_END
//---------------------------------------------------------------------------

/// FTDI MPSSE device interfaces
///

/// MPSSE GPIO Interface
ES_DECL_INTF_BEGIN(4D5C3287, BD344EC6, B36DC77E, 7F1DDD74, EsFtdiMpsseGpioIntf)

  /// Configure direction and initial state of GPIO pins. Must be called at least once
  /// before reading from GPIO
  /// @param[in]  val - GPIO pins state. 0 - reset, 1 - set
  /// @param[in]  dir - GPIO pins direction. 0 - input, 1 - output
  /// @return           true, if operation executed Ok, false otherwise
  ///
  ES_DECL_INTF_METHOD(bool, GPIOwrite)(esU8 val, esU8 dir) = 0;

  /// Read NB! input lines of GPIO, previously configured by GPIOwrite command
  ES_DECL_INTF_METHOD(bool, GPIOread)(esU8& out) = 0;

ES_DECL_INTF_END
//---------------------------------------------------------------------------

/// MPSSE I2C
///
ES_DECL_INTF_BEGIN(18BB3BFD, 27B14C6F, 9D2FC802, 5905B6C3, EsFtdiMpsseI2cIntf)

public:
  // Standard I2C clock rates
  enum {
    CLOCK_STANDARD                  = 100000,    ///< 100kb/sec
    CLOCK_FAST                      = 400000,    ///< 400kb/sec
    CLOCK_FAST_PLUS                 = 1000000,  ///< 1000kb/sec
    CLOCK_HIGH_SPEED                = 3400000    ///< 3.4Mb/sec
  };

  // Transfer options
  enum {
    TRANSFER_OPTIONS_START_BIT      = 0x00000001, ///< Generate start condition before transmitting
    TRANSFER_OPTIONS_STOP_BIT        = 0x00000002, ///< Generate stop condition after transmitting
    TRANSFER_OPTIONS_BREAK_ON_NACK  =  0x00000004, ///< Continue transmitting data in bulk without caring
                                                  /// about Ack or nAck from device if this bit is not set.
                                                  /// If this bit is set then stop transferring the data in the buffer
                                                  /// when the device nACKs
    TRANSFER_OPTIONS_NACK_LAST_BYTE = 0x00000008, ///< MPSSE-I2C generates an ACKs for every byte read. Some I2C slaves require the I2C
                                                  /// master to generate a nACK for the last data byte read.
                                                  /// Setting this bit enables working with  such I2C slaves

    TRANSFER_OPTIONS_FAST_TRANSFER_BYTES=  0x00000010, ///< When the user calls I2C_DeviceWrite or I2C_DeviceRead with this bit set then libMPSSE
                                                  /// packs commands to transfer sizeToTransfer number of bytes, and to read/write
                                                  /// sizeToTransfer number of ack bits. If data is written then the read ack bits are ignored, if
                                                  /// data is being read then an acknowledgement bit(SDA=LOW) is given to the I2C slave
                                                  /// after each byte read

    TRANSFER_OPTIONS_FAST_TRANSFER_BITS= 0x00000020, ///< When the user calls I2C_DeviceWrite or I2C_DeviceRead with this bit set then libMPSSE
                                                  /// packs commands to transfer sizeToTransfer number of bits. There is no ACK phase when
                                                  /// this bit is set

    TRANSFER_OPTIONS_FAST_TRANSFER  =  0x00000030, ///< Fast transfers prepare a buffer containing commands to generate START/STOP/ADDRESS
                                                  /// conditions and commands to read/write data. This buffer is sent to the MPSSE in one shot,
                                                  /// hence delays between different phases of the I2C transfer are eliminated. Fast transfers
                                                  /// can have data length in terms of bits or bytes. The user application should call
                                                  /// I2C_DeviceWrite or I2C_DeviceRead with either
                                                  /// I2C_TRANSFER_OPTIONS_FAST_TRANSFER_BYTES or
                                                  /// I2C_TRANSFER_OPTIONS_FAST_TRANSFER_BITS bit set to perform a fast transfer.
                                                  /// I2C_TRANSFER_OPTIONS_START_BIT and I2C_TRANSFER_OPTIONS_STOP_BIT have
                                                  /// their usual meanings when used in fast transfers, however
                                                  /// I2C_TRANSFER_OPTIONS_BREAK_ON_NACK and
                                                  /// I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE are not applicable in fast transfers


    TRANSFER_OPTIONS_NO_ADDRESS      = 0x00000040  ///< The address parameter is ignored in fast transfers if this bit is set.
                                                  /// This would mean that the address is either a part of the data or this is a special I2C frame that doesn't require
                                                  /// an address. However if this bit is not set then 7bit address and 1bit direction will be
                                                  /// written to the I2C bus each time I2C_DeviceWrite or I2C_DeviceRead is called and a
                                                  /// 1bit acknowledgement will be read after that, which will however be just ignored
  };

public:
  /// Configuration elements access. If channel is currently open, configuration changes
  /// will take place upon next successfull channel opening.
  ///
  ES_DECL_INTF_METHOD(esU32, clockRateGet)() const = 0;
  ES_DECL_INTF_METHOD(void, clockRateSet)(esU32 rate) = 0;

  ES_DECL_INTF_METHOD(esU8, latencyTimerGet)() const = 0;
  ES_DECL_INTF_METHOD(void, latencyTimerSet)(esU8 tmo) = 0;

  ES_DECL_INTF_METHOD(bool, _3phaseDataClockingGet)() const = 0;
  ES_DECL_INTF_METHOD(void, _3phaseDataClockingSet)(bool set) = 0;

  ES_DECL_INTF_METHOD(bool, driveOnlyZeroGet)() const = 0;
  ES_DECL_INTF_METHOD(void, driveOnlyZeroSet)(bool set) = 0;

  ES_DECL_INTF_METHOD(bool, clockStretchingGet)() const = 0;
  ES_DECL_INTF_METHOD(void, clockStretchingSet)(bool set) = 0;

  /// (Re) apply pending configuration changes
  ES_DECL_INTF_METHOD(void, configApply)() = 0;

  /// Try to ping slave at specified address
  ES_DECL_INTF_METHOD(bool, ping)(esU16 devAddr, bool forWrite = false) = 0;

  /// Write data to I2C channel, to slave devAddr, using I2C transfer options - see corresponding enum decl
  /// Returned is count of bytes written
  ///
  ES_DECL_INTF_METHOD(esU32, write)(esU16 devAddr, EsBinBuffer::const_pointer src, esU32 len, esU32 opts) = 0;
  ES_DECL_INTF_METHOD(esU32, write)(esU16 devAddr, const EsBinBuffer& src, esU32 opts) = 0;

  /// Read data from I2C channel, from slave devAddr, using I2C transfer options - see corresponding enum decl
  /// Returned is count of bytes read|acquired data buffer
  ///
  ES_DECL_INTF_METHOD(esU32, read)(esU16 devAddr, EsBinBuffer::pointer dest, esU32 len, esU32 opts) = 0;
  ES_DECL_INTF_METHOD(EsBinBuffer, read)(esU16 devAddr, esU32 len, esU32 opts) = 0;

ES_DECL_INTF_END
//---------------------------------------------------------------------------

/// MPSSE SPI
ES_DECL_INTF_BEGIN(1B289726, 3CA74A3A, B96E0A15, D840639C, EsFtdiMpsseSpiIntf)

public:
  /// SPI data capture and polarity control
  enum CPOL_CPHA {
    MODE0,              ///< data captured on rising edge, propagated on falling
    MODE1,              ///< data captured on falling edge, propagated on rising
    MODE2,              ///< data captured on falling edge, propagated on rising
    MODE3               ///< data captured on rising edge, propagated on falling
  };

  /// SPI chip select control
  enum CS_LINE {
    ABCD_DBUS3,
    ABCD_DBUS4,
    ABCD_DBUS5,
    ABCD_DBUS6,
    ABCD_DBUS7
  };

  /// SPI clock rate
  enum {
    RATE_MIN                    = 1,
    RATE_DEF                    = 1000000,
    RATE_MAX                    = 30000000
  };

  /// SPI transfer control options
  enum {
    TRANSFER_OPTIONS_SIZE_IN_BYTES  = 0,
    TRANSFER_OPTIONS_SIZE_IN_BITS   = 0x01,
    TRANSFER_OPTIONS_CHIPSELECT_ENABLE= 0x02,
    TRANSFER_OPTIONS_CHIPSELECT_DISABLE= 0x04
  };

public:
  /// Configuration fields access
  ///
  ES_DECL_INTF_METHOD(esU32, clockRateGet)() const = 0;
  ES_DECL_INTF_METHOD(void, clockRateSet)(esU32 rate) = 0;

  ES_DECL_INTF_METHOD(esU8, latencyTimerGet)() const = 0;
  ES_DECL_INTF_METHOD(void, latencyTimerSet)(esU8 tmo) = 0;

  /// SPI data capture and polarity control
  ES_DECL_INTF_METHOD(EsFtdiMpsseSpiIntf::CPOL_CPHA, modeGet)() const = 0;
  ES_DECL_INTF_METHOD(void, modeSet)( EsFtdiMpsseSpiIntf::CPOL_CPHA opts ) = 0;

  /// SPI CS line selector control
  ES_DECL_INTF_METHOD(EsFtdiMpsseSpiIntf::CS_LINE, csLineCtlGet)() const = 0;
  ES_DECL_INTF_METHOD(void, csLineCtlSet)(EsFtdiMpsseSpiIntf::CS_LINE ctl) = 0;

  /// (Re) apply pending configuration changes
  ES_DECL_INTF_METHOD(void, configApply)() = 0;

  /// SPI active state (low or high) of CS line
  ES_DECL_INTF_METHOD(bool, csLineActiveHighGet)() const = 0;
  ES_DECL_INTF_METHOD(void, csLineActiveHighSet)(bool high) = 0;

  /// Pins initial direction, initial state, final direction, final state
  /// State of the pins other than ones used for SPI, may be controlled here.
  /// SPI-occupied lines states and configs will be internally preserved for SPI use
  ///
  ES_DECL_INTF_METHOD(esU8, pinsDirInitGet)() const = 0;
  ES_DECL_INTF_METHOD(void, pinsDirInitSet)(esU8 set) = 0;

  ES_DECL_INTF_METHOD(esU8, pinsDirFinalGet)() const = 0;
  ES_DECL_INTF_METHOD(void, pinsDirFinalSet)(esU8 set) = 0;

  ES_DECL_INTF_METHOD(esU8, pinsStateInitGet)() const = 0;
  ES_DECL_INTF_METHOD(void, pinsStateInitSet)(esU8 set) = 0;

  ES_DECL_INTF_METHOD(esU8, pinsStateFinalGet)() const = 0;
  ES_DECL_INTF_METHOD(void, pinsStateFinalSet)(esU8 set) = 0;

  // Pins current direction and state
  ES_DECL_INTF_METHOD(esU8, pinsDirCurrentGet)() const = 0;
  ES_DECL_INTF_METHOD(esU8, pinsStateCurrentGet)() const = 0;
  ES_DECL_INTF_METHOD(bool, pinsStateCurrentSet)(esU8 set) = 0;

  /// Return current MISO line state
  ES_DECL_INTF_METHOD(bool, misoPoll)() = 0;

  /// Write data to SPI channel, using SPI transfer options - see corresponding enum decl
  /// Returned is count of bytes written
  ///
  ES_DECL_INTF_METHOD(esU32, write)(EsBinBuffer::const_pointer src, esU32 len, esU32 opts) = 0;
  ES_DECL_INTF_METHOD(esU32, write)(const EsBinBuffer& src, esU32 opts) = 0;

  /// Read data from SPI channel, using SPI transfer options - see corresponding enum decl
  /// Returned is count of bytes read|acquired data buffer
  ///
  ES_DECL_INTF_METHOD(esU32, read)(EsBinBuffer::pointer dest, esU32 len, esU32 opts) = 0;
  ES_DECL_INTF_METHOD(EsBinBuffer, read)(esU32 len, esU32 opts) = 0;

  /// Perform combined SPI read-write call
  ES_DECL_INTF_METHOD(esU32, readWrite)(EsBinBuffer::const_pointer src, EsBinBuffer::pointer dest, esU32 len, esU32 opts) = 0;
  ES_DECL_INTF_METHOD(EsBinBuffer, readWrite)(const EsBinBuffer& src, esU32 opts) = 0;

  /// Set state of CS line
  ES_DECL_INTF_METHOD(void, csSet)(bool set) = 0;

ES_DECL_INTF_END
//---------------------------------------------------------------------------

/// FTDI device kinds
enum class EsFtdiDeviceKind : ulong {
  Generic,
  MPSSE_I2C,
  MPSSE_SPI
};
//---------------------------------------------------------------------------

/// FTDI driver wrapper singleton object
///
class ESCOMM_CLASS EsFtdiDriver
{
public:
  typedef EsFtdiDeviceIntf::FT_HANDLE FT_HANDLE;

  typedef EsFtdiDeviceIntf::FT_STATUS FT_STATUS;

  /// Misc driver constants
  enum {
    /// FT_OpenEx Flags
    OPEN_BY_SERIAL_NUMBER        = 1,
    OPEN_BY_DESCRIPTION          = 2,
    /// FT_ListDevices Flags (used in conjunction with FT_OpenEx Flags
    LIST_NUMBER_ONLY            = 0x80000000,
    LIST_BY_INDEX                = 0x40000000,
    LIST_ALL                    = 0x20000000,
    LIST_MASK                    = (LIST_NUMBER_ONLY|LIST_BY_INDEX|LIST_ALL),
    /// FT_FLAGS (see FT_DEVICE_LIST_INFO_NODE)
    FLAGS_OPENED                = 0x00000001,
    FLAGS_HISPEED                = 0x00000002,
    /// Flow control
    FLOW_NONE                    = 0x0000,
    FLOW_RTS_CTS                = 0x0100,
    FLOW_DTR_DSR                = 0x0200,
    FLOW_XON_XOFF                = 0x0400,
    /// Purge rx and tx buffers
    PURGE_RX                    = 1,
    PURGE_TX                    = 2,
    /// Events
    EVENT_RXCHAR                = 1,
    EVENT_MODEM_STATUS          = 2,
    NOTIFY_ON_UNPLUG            = 128,
    /// Baud Rates
    _300                         = 300,
    _600                         = 600,
    _1200                         = 1200,
    _2400                         = 2400,
    _4800                         = 4800,
    _9600                        = 9600,
    _14400                       = 14400,
    _19200                       = 19200,
    _38400                       = 38400,
    _57600                       = 57600,
    _115200                      = 115200,
    _230400                      = 230400,
    _460800                      = 460800,
    _921600                      = 921600,
    /// Word Lengths
    BITS_8                      = 8,
    BITS_7                      = 7,
    BITS_6                      = 6,
    BITS_5                      = 5,
    /// Stop Bits
    STOP_BITS_1                  = 0,
    STOP_BITS_1_5                = 1,
    STOP_BITS_2                  = 2,
    /// Parity
    ParityNONE                  = 0,
    ParityODD                    = 1,
    ParityEVEN                  = 2,
    ParityMARK                  = 3,
    ParitySPACE                  = 4,
    /// Timeouts
    DEFAULT_RX_TIMEOUT          = 300,
    DEFAULT_TX_TIMEOUT          = 300,
    /// Buffer lengths
    DEFAULT_RX_BUFFER            = 4096,
    DEFAULT_TX_BUFFER            = 4096,
    /// Device types
    DEVICE_232BM                = 0,
    DEVICE_232AM                = 1,
    DEVICE_100AX                = 2,
    DEVICE_UNKNOWN              = 3,
    DEVICE_2232C                = 4,
    DEVICE_232R                  = 5,
    DEVICE_2232H                = 6,
    DEVICE_4232H                = 7,
    DEVICE_232H                 = 8,
    DEVICE_X_SERIES             = 9,
    /// Bit modes
    BITMODE_RESET                 = 0x0,  // Reset
    BITMODE_ASYNC_BITBANG         = 0x1,  // Asynchronous Bit Bang
    BITMODE_MPSSE                 = 0x2,  // MPSSE (FT4232H, FT232H, FT2232H and FT2232 devices only)
    BITMODE_SYNC_BITBANG        = 0x4,  // Synchronous Bit Bang (FT4232H, FT2232H, FT232R, FT245R and FT2232 devices only)
    BITMODE_MCU_HOSTBUS_EMU      = 0x8,  // MCU Host Bus Emulation Mode (FT4232H, FT2232H, FT232R, FT245R and FT2232 devices only)
    BITMODE_FAST_OI_SERIAL      = 0x10, // Fast Opto-Isolated Serial Mode (FT4232H, FT2232H, FT232R, FT245R and FT2232 devices only)
    BITMODE_CBUS                = 0x20, // CBUS Bit Bang Mode (FT232R devices only)
    BITMODE_SC_SYNC              = 0x40, // Single Channel Synchronous 245 FIFO Mode (FT2232H devices only)
    /// Interface masks
    INTERFACE_MASK_IN            = 0x00,
    INTERFACE_MASK_OUT          = 0x01
  };

  // Device type (see FT_DEVICE_xxx constants above)
  typedef EsFtdiDeviceIntf::FT_DEVICE  FT_DEVICE;

  // Driver Event callback
  typedef void (*PFT_EVENT_HANDLER)(esU32,esU32);

  /// Driver structures
  ///

  /// Driver device node
  struct ESCOMM_CLASS FT_DEVICE_LIST_INFO_NODE
  {
  public:
    FT_DEVICE_LIST_INFO_NODE();
    inline esU32 flagsGet() const { return Flags; }
    inline esU32 typeGet() const { return Type; }
    inline esU32 idGet() const { return ID; }
    inline esU32 locIdGet() const { return LocId; }
    inline const char* serialNumberGet() const { return SerialNumber; }
    inline const char* descriptionGet() const { return Description; }
    inline FT_HANDLE handle() const { return ftHandle; }
    EsString serialNumberStrGet() const;
    EsString descriptionStrGet() const;

    /// Flags checker
    inline bool isOpen() const { return FLAGS_OPENED == (Flags & FLAGS_OPENED); }
    inline bool isHiSpeed() const { return FLAGS_HISPEED == (Flags & FLAGS_HISPEED); }

    /// Functionality checks
    ///
    bool mpsseSupported() const;
    esU32 mpsseChannelsCntGet() const;

    /// Access contents as variant collection
    EsVariant toVariant() const;

  protected:
    esU32 Flags;
    esU32 Type;
    esU32 ID;
    esU32 LocId;
    char SerialNumber[16];
    char Description[64];
    FT_HANDLE ftHandle;

    friend class EsFtdiDriver;
  };

  // Device infos array
  typedef std::vector<FT_DEVICE_LIST_INFO_NODE> DeviceList;

protected:
  EsFtdiDriver();

public:
  static EsFtdiDriver& instanceGet();

  // refcounted load|unload
  bool load(bool doThrow = true);
  void unload();

  // object check
  bool isOk() const;

  // Return recent driver call status
  FT_STATUS statusGet() const;

  // Return list of free (not-opened), or all (if onlyOpened=false), devices found.
  DeviceList deviceListGet(bool onlyOpened = true);

  /// Return currently enumerated device infos list.
  /// This list would contain all currently enumerated devices, regardless of its busy,
  /// or any other state attributes
  ///
  const DeviceList& currentDeviceListGet() const { return m_devInfos; }

  // Create new MPSSE device object
  EsFtdiDeviceIntf::Ptr deviceCreate(EsFtdiDeviceKind kind, const EsString& ser, const EsString& descr);

  // Generate driver call wrappers
  #define FTDI_DRIVER_ENTRY0(Name) FT_STATUS ft## Name() { EsCriticalSectionLocker lock(m_cs);  return internal## Name(); }
  #define FTDI_DRIVER_ENTRY1(Name, Param1) FT_STATUS ft## Name(Param1 p1) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1); }
  #define FTDI_DRIVER_ENTRY2(Name, Param1, Param2) FT_STATUS ft## Name(Param1 p1, Param2 p2) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2); }
  #define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3); }
  #define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4); }
  #define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5); }
  #define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5, p6); }
  #define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5, p6, p7); }
  #define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5, p6, p7, p8); }
  #define FTDI_DRIVER_DEV_ENTRY1(Name, Param1) FT_STATUS ft## Name(Param1 p1) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1); }
  #define FTDI_DRIVER_DEV_ENTRY2(Name, Param1, Param2) FT_STATUS ft## Name(Param1 p1, Param2 p2) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2); }
  #define FTDI_DRIVER_DEV_ENTRY3(Name, Param1, Param2, Param3) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3); }
  #define FTDI_DRIVER_DEV_ENTRY4(Name, Param1, Param2, Param3, Param4) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4); }
  #define FTDI_DRIVER_DEV_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5); }
  #define FTDI_DRIVER_DEV_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5, p6); }
  #define FTDI_DRIVER_DEV_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5, p6, p7); }
  #define FTDI_DRIVER_DEV_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  FT_STATUS ft## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) { EsCriticalSectionLocker lock(m_cs); return internal## Name(p1, p2, p3, p4, p5, p6, p7, p8); }
  #include "EsFtdiDriver.win.hxx"

  /// Static services
  ///

  /// Semi-private services - access DeviceInfo's handle and flag members (used in MPSSE)
  ///
  static inline esU32& nodeFlagsAccess(FT_DEVICE_LIST_INFO_NODE& node) { return node.Flags; }
  static inline FT_HANDLE& nodeHandleAccess(FT_DEVICE_LIST_INFO_NODE& node) { return node.ftHandle; }

  /// Class reflected services
  ///

  ES_USE_AS_EXISTING_REFLECTED_CLASS_SERVICE

  /// Reflected FTDI devices enumeration service
  /// Parameters define filtering which applies to FTDI enumeration procedure
  /// @param 0 includeBusyPorts bool -  if true, both busy (Open) and free FTDI devices are included
  ///                                   in enumeration results. If false, only unoccupied ones returned
  /// @param 1 includeOnlyMPSSE bool -  if true, only MPSSE - supported devices are included
  ///                                   in enumeration results. If false, include all devices
  /// @return                        -  a variant collection, each item
  ///                                   is in turn a collection of port infos, as follows:
  ///                                   [0] - device serial number string
  ///                                   [1] - device description string
  ///                                   [2] - FTDI device is Open (Busy)
  ///                                   [3] - FTDI device is High Speed USB device
  ///                                   [4] - FTDI device flags
  ///                                   [5] - FTDI device type
  ///                                   [6] - FTDI device ID
  ///                                   [7] - FTDI device LocID
  ///                                   [8] - FTDI MPSSE is supported
  ///                                   [9] - Number of FTDI MPSSE channels supported
  ///
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, enumerate, cr_EsVariant, cr_EsVariant);

  /// Load driver.
  /// @param doThrow bool - if true, and driver loading failed, throw an exception
  /// @return bool - return true, if driver is loaded successfully
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, load, cr_EsVariant);

  /// Unload driver
  ES_DECL_REFLECTED_CLASS_METHOD0(void, unloadStatic);

  /// Return driver library version information, as esU32
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, libraryVersionGet);

  /// Return isOK driver status
  ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, isOkStatic);

  /// Return FT status string representation
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, statusStringGet, cr_EsVariant);

  /// Check status returned from driver entry call, throw exception if
  /// it's not OK, and class is allowed to throw exceptions. Otherwise, return true, if
  /// there was no error, and false, otherwise
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, statusCheck, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, statusCheckThrow, cr_EsVariant);

  /// Create or get existing device interfaced object
  ///
  /// @param[in]  EsFtdiDeviceKind enumeration member
  /// @param[in]  EsVariant - Device info (partial info allowed).
  ///                         Parameter should contain either device serial string,
  ///                         or minimal desired device info collection, with
  ///                         at least serial [0], and description [1] members are set.
  /// @return               - created or existing (see above) device object instance
  ///
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, deviceCreate, cr_EsVariant, cr_EsVariant);

protected:
  // Internal services
  bool internalLoad();
  bool internalCheckLoaded();
  void internalUnload();

  EsFtdiDeviceIntf::Ptr deviceCreate( EsFtdiDeviceKind kind, const FT_DEVICE_LIST_INFO_NODE& node );

  // Generate internal driver library enty points calls
  #define FTDI_DRIVER_ENTRY0(Name) FT_STATUS internal## Name() { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(); }
  #define FTDI_DRIVER_ENTRY1(Name, Param1) FT_STATUS internal## Name(Param1 p1) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1); }
  #define FTDI_DRIVER_ENTRY2(Name, Param1, Param2) FT_STATUS internal## Name(Param1 p1, Param2 p2) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2); }
  #define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3); }
  #define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4); }
  #define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5); }
  #define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5, p6); }
  #define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5, p6, p7); }
  #define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5, p6, p7, p8); }
  #define FTDI_DRIVER_DEV_ENTRY1(Name, Param1) FT_STATUS internal## Name(Param1 p1) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1); }
  #define FTDI_DRIVER_DEV_ENTRY2(Name, Param1, Param2) FT_STATUS internal## Name(Param1 p1, Param2 p2) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2); }
  #define FTDI_DRIVER_DEV_ENTRY3(Name, Param1, Param2, Param3) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3); }
  #define FTDI_DRIVER_DEV_ENTRY4(Name, Param1, Param2, Param3, Param4) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4); }
  #define FTDI_DRIVER_DEV_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5); }
  #define FTDI_DRIVER_DEV_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5, p6); }
  #define FTDI_DRIVER_DEV_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5, p6, p7); }
  #define FTDI_DRIVER_DEV_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)  FT_STATUS internal## Name(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) { if( !internalCheckLoaded() ) return EsFtdiDeviceIntf::FT_OTHER_ERROR; return m_pfn## Name(p1, p2, p3, p4, p5, p6, p7, p8); }
  #include "EsFtdiDriver.win.hxx"
  
protected:
  // FTDI driver library
  EsDynamicLibrary::Ptr m_drv;

  // Current device Infos list
  DeviceList m_devInfos;

  // MT access guard
  mutable EsCriticalSection m_cs;

  // Exception enabling flag
  bool m_doThrow;

public:
  // Make mapped driver methods entry points public to provide access to 3-rd party C driver extensions
  // generate member variables-pointer to ftdi driver entries
  //
  #define FTDI_DRIVER_ENTRY0(Name) typedef FT_STATUS (__stdcall* Name ##_T)(); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY1(Name, Param1) typedef FT_STATUS (__stdcall* Name ##_T)(Param1); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY2(Name, Param1, Param2) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY3(Name, Param1, Param2, Param3) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY4(Name, Param1, Param2, Param3, Param4) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6, Param7); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY1(Name, Param1) typedef FT_STATUS (__stdcall* Name ##_T)(Param1); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY2(Name, Param1, Param2) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY3(Name, Param1, Param2, Param3) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY4(Name, Param1, Param2, Param3, Param4) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY5(Name, Param1, Param2, Param3, Param4, Param5) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY6(Name, Param1, Param2, Param3, Param4, Param5, Param6) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY7(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6, Param7); Name ##_T m_pfn## Name;
  #define FTDI_DRIVER_DEV_ENTRY8(Name, Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8) typedef FT_STATUS (__stdcall* Name ##_T)(Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8); Name ##_T m_pfn## Name;
  #include "EsFtdiDriver.win.hxx"

private:
  friend class EsFtdiDevice;

  // Prohibited functionality
  //
  EsFtdiDriver(const EsFtdiDriver&) ES_REMOVEDECL;
  EsFtdiDriver& operator=(const EsFtdiDriver&) ES_REMOVEDECL;
};

/// Reflected FTDI device enumerations
namespace EsReflection
{

ES_DECL_ENUMERATION_BEGIN(EsFtdiDeviceKind)
  ES_DECL_ENUMERATION_ITEM_LABELLED(Generic,      static_cast<ulong>(::EsFtdiDeviceKind::Generic),    esT("Generic FTDI device"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(MPSSE_I2C,    static_cast<ulong>(::EsFtdiDeviceKind::MPSSE_I2C),  esT("I2C over FTDI MPSSE"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(MPSSE_SPI,    static_cast<ulong>(::EsFtdiDeviceKind::MPSSE_SPI),  esT("SPI over FTDI MPSSE"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiDeviceFlag)
  ES_DECL_ENUMERATION_ITEM_LABELLED(OPENED,       static_cast<ulong>(EsFtdiDriver::FLAGS_OPENED),     esT("FTDI device is open"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(HIGHSPEED,    static_cast<ulong>(EsFtdiDriver::FLAGS_HISPEED),    esT("FTDI device supports highspeed IO"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiDeviceFlowCtl)
  ES_DECL_ENUMERATION_ITEM_LABELLED(NONE,         static_cast<ulong>(EsFtdiDriver::FLOW_NONE),        esT("FTDI no flow control"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(RTS_CTS,      static_cast<ulong>(EsFtdiDriver::FLOW_RTS_CTS),     esT("FTDI control RTS CTS"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(DTR_DSR,      static_cast<ulong>(EsFtdiDriver::FLOW_DTR_DSR),     esT("FTDI control DTR DSR"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(XON_XOFF,     static_cast<ulong>(EsFtdiDriver::FLOW_XON_XOFF),    esT("FTDI control XON XOFF"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiDevicePurge)
  ES_DECL_ENUMERATION_ITEM_LABELLED(RX,           static_cast<ulong>(EsFtdiDriver::PURGE_RX),         esT("FTDI purge RX queue"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(TX,           static_cast<ulong>(EsFtdiDriver::PURGE_TX),         esT("FTDI purge TX queue"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiMpsseI2cClock)
  ES_DECL_ENUMERATION_ITEM_LABELLED(STANDARD,     static_cast<ulong>(EsFtdiMpsseI2cIntf::CLOCK_STANDARD),esT("I2C Standard clock: 100kb/sec"))
   ES_DECL_ENUMERATION_ITEM_LABELLED(FAST,        static_cast<ulong>(EsFtdiMpsseI2cIntf::CLOCK_FAST), esT("I2C Fast clock: 400kb/sec"))
   ES_DECL_ENUMERATION_ITEM_LABELLED(FAST_PLUS,   static_cast<ulong>(EsFtdiMpsseI2cIntf::CLOCK_FAST_PLUS),esT("I2C Fast Plus clock: 1000kb/sec"))
   ES_DECL_ENUMERATION_ITEM_LABELLED(HIGH_SPEED,  static_cast<ulong>(EsFtdiMpsseI2cIntf::CLOCK_HIGH_SPEED),esT("I2C High Speed clock: 3.4Mb/sec"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiMpsseI2cIO)
  ES_DECL_ENUMERATION_ITEM_LABELLED(START_BIT,    static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_START_BIT),esT("Generate start condition before transmitting"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(STOP_BIT,     static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_STOP_BIT),esT("Generate stop condition after transmitting"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(BREAK_ON_NACK,static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_BREAK_ON_NACK),esT("Continue transmitting data in bulk without caring about ACK or NACK from device"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(NACK_LAST_BYTE,static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NACK_LAST_BYTE),esT("Generate NACK for the last I2C byte transferred to slave"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(FAST_TRANSFER_BYTES,static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BYTES),esT("Write data buffer as byte stream, ignoring ACKS"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(FAST_TRANSFER_BITS,static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER_BITS),esT("Write data buffer as bit stream, ignoring ACKS"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(FAST_TRANSFER,static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_FAST_TRANSFER),esT("Prepare and handle data transfer in one command"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(NO_ADDRESS,   static_cast<ulong>(EsFtdiMpsseI2cIntf::TRANSFER_OPTIONS_NO_ADDRESS),esT("The address parameter is ignored in fast transfers"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiMpsseSpiCpolCpha)
  ES_DECL_ENUMERATION_ITEM_LABELLED(MODE0,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE0),esT("Data captured on rising edge, propagated on falling"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(MODE1,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE1),esT("Data captured on falling edge, propagated on rising"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(MODE2,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE2),esT("Data captured on falling edge, propagated on rising"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(MODE3,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CPOL_CPHA::MODE3),esT("Data captured on rising edge, propagated on falling"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiMpsseSpiCsLine)
  ES_DECL_ENUMERATION_ITEM_LABELLED(DBUS3,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CS_LINE::ABCD_DBUS3),esT("Chip Select on DBUS3"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(DBUS4,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CS_LINE::ABCD_DBUS4),esT("Chip Select on DBUS4"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(DBUS5,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CS_LINE::ABCD_DBUS5),esT("Chip Select on DBUS5"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(DBUS6,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CS_LINE::ABCD_DBUS6),esT("Chip Select on DBUS6"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(DBUS7,        static_cast<ulong>(EsFtdiMpsseSpiIntf::CS_LINE::ABCD_DBUS7),esT("Chip Select on DBUS7"))
ES_DECL_ENUMERATION_END

ES_DECL_ENUMERATION_BEGIN(EsFtdiMpsseSpiIO)
  ES_DECL_ENUMERATION_ITEM_LABELLED(SIZE_IN_BYTES,static_cast<ulong>(EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_SIZE_IN_BYTES),esT("SPI IO buffer is a stream of bytes"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(SIZE_IN_BITS, static_cast<ulong>(EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_SIZE_IN_BITS),esT("SPI IO buffer is a stream of bits"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(CHIPSELECT_ENABLE,static_cast<ulong>(EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_ENABLE),esT("Enable Chip Select before IO"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(CHIPSELECT_DISABLE,static_cast<ulong>(EsFtdiMpsseSpiIntf::TRANSFER_OPTIONS_CHIPSELECT_DISABLE),esT("Disable Chip Select after IO"))
ES_DECL_ENUMERATION_END

}

#endif // ES_COMM_USE_FTDI

#endif // _ftdi_driver_h_
