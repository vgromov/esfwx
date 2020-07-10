#ifndef _ekonnect_io_channel_h_
#define _ekonnect_io_channel_h_

#ifdef ES_COMM_USE_CHANNEL_EKONNECT

// Ekonnect non-reflected and reflected enums
//
enum class EsEkonnectIoParity : ulong
{
  None  = EsFtdiDriver::ParityNONE,
  Odd   = EsFtdiDriver::ParityODD,
  Even  = EsFtdiDriver::ParityEVEN,
  Mark  = EsFtdiDriver::ParityMARK,
  Space = EsFtdiDriver::ParitySPACE,
};

namespace EsReflection
{
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCOMM_CLASS, EsEkonnectIoParity)
  ES_DECL_ENUMERATION_ITEM_LABELLED(None,    static_cast<ulong>(::EsEkonnectIoParity::None),   _i("None"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Odd,    static_cast<ulong>(::EsEkonnectIoParity::Odd),    _i("Odd"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Even,    static_cast<ulong>(::EsEkonnectIoParity::Even),   _i("Even"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Mark,    static_cast<ulong>(::EsEkonnectIoParity::Mark),   _i("Mark"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Space,  static_cast<ulong>(::EsEkonnectIoParity::Space),  _i("Space"))
ES_DECL_ENUMERATION_END
}

enum class EsEkonnectIoStopBits : ulong
{
  _1   = EsFtdiDriver::STOP_BITS_1,
  _1_5 = EsFtdiDriver::STOP_BITS_1_5,
  _2   = EsFtdiDriver::STOP_BITS_2
};

namespace EsReflection
{
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCOMM_CLASS, EsEkonnectIoStopBits)
  ES_DECL_ENUMERATION_ITEM_LABELLED(_1,      static_cast<ulong>(::EsEkonnectIoStopBits::_1),   _i("1 Stop Bit"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(_1_5,    static_cast<ulong>(::EsEkonnectIoStopBits::_1_5), _i("1.5 Stop Bits"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(_2,      static_cast<ulong>(::EsEkonnectIoStopBits::_2),   _i("2 Stop Bits"))
ES_DECL_ENUMERATION_END
}

// direct io channel via Ekonnect device (ftdi driver)
//
class ESCOMM_CLASS ES_INTF_IMPL4(EsChannelIoEkonnect, EsReflectedClassIntf, EsChannelIoIntf, EsRateCtlIntf, EsPowerCtlIntf)

protected:
  // internal consts
  enum {
    SLEEP_GRANULARITY = 1,
  };

  EsChannelIoEkonnect();

public:
  virtual ~EsChannelIoEkonnect();

  static EsChannelIoIntf::Ptr create() { return NEW(); }

  // reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoEkonnect )
  ES_DECL_ATTR_HANDLING_STD

  // ekobox configuraiton properties
  //
  ES_DECL_PROPERTY_STD(device, EsString)
  ES_DECL_PROPERTY_STD(baud, long)
  ES_DECL_PROPERTY_STD(bits, long)
  ES_DECL_PROPERTY_STD(parity, long)
  ES_DECL_PROPERTY_STD(stopBits, long)
  ES_DECL_PROPERTY_STD(rxTmo, long)
  ES_DECL_PROPERTY_STD(txTmo, long)
  ES_DECL_PROPERTY_STD(rxBuffLen, long)
  ES_DECL_PROPERTY_STD(txBuffLen, long)
  ES_DECL_PROPERTY_STD(resetOnRxTmo, bool)
  ES_DECL_PROPERTY_STD(useRS232, bool)
  ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)
  ES_DECL_PROPERTY_WO(breaker, EsBaseIntfPtr)
  ES_DECL_PROPERTY_RO(isEkonnect1, bool)

  // class services
  //
  // reflected constructor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
  // EsChannelIoIntf interface
  //
  // channel unique idGet string
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, idGet) ES_OVERRIDE;
  // monitor interface access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor) ES_OVERRIDE;
  // state manipulation
  ES_DECL_REFLECTED_INTF_METHOD0(bool, open) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, close) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen) ES_OVERRIDE;
  // io breaker access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, breakerGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, breakerSet)(const EsBaseIntf::Ptr& breaker) ES_OVERRIDE;
  // byte io
  ES_DECL_INTF_METHOD(esU32, bytesPut)(const esU8* data, esU32 len, esU32 tmo) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(esU32, bytesGet)(esU8* data, esU32 len, esU32 tmo) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(ulong, bytesPut, cr_EsBinBuffer, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, bytesGet, ulong, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, txTimeEstimateGet, ulong) ES_OVERRIDE;
  // port reset & error info access
  ES_DECL_REFLECTED_INTF_METHOD0(void, reset) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, errorGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, errorStringGet) ES_OVERRIDE;
  // initialize C EseChannelIo from existing EsChannelIoIntf
  ES_DECL_INTF_METHOD(void, commChannelInit)(EseChannelIo* chnl) ES_OVERRIDE;
  // EsPowerCtlIntf interface
  // 
  // remove|set device power
  ES_DECL_REFLECTED_INTF_METHOD1(void, devicePowerSet, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, devicePowerGet) ES_OVERRIDE;
  // set|reset device programming mode pin
  ES_DECL_REFLECTED_INTF_METHOD1(void, deviceProgrammingModeSet, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, deviceProgrammingModeGet) ES_OVERRIDE;

  // EsRateCtlIntf interface
  // 
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, isRateSupported, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, supportedRatesGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, defaultRateGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, rateGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(bool, rateSet, ulong) ES_OVERRIDE;

  // The rest of services (non-interfaced)
  //

  // Generate line break condition
  void lineBreak();

  // Get list of applicable io devices found
  static EsFtdiDriver::DeviceList getDeviceList();

  // Format EKONNECT channel ID string
  static EsString idStringFormat(const EsString& deviceIdStr);

  /// Reflected FTDI devices enumeration service - returned is a variant collection, each item
  /// is in turn a collection of port infos, as follows:
  /// [0] - device serial number string
  /// [1] - device description string
  /// [2] - FTDI device is Open (Busy)
  /// [3] - FTDI device is High Speed USB device
  /// [4] - FTDI device flags
  /// [5] - FTDI device type
  /// [6] - FTDI device ID
  /// [7] - FTDI device LocID
  /// [8] - FTDI MPSSE is supported
  /// [9] - Number of FTDI MPSSE channels supported
  ///
  /// Parameters define filtering which applies to FTDI enumeration procedure
  /// @param includeBusyPorts bool -  if true, both busy (Open) and free FTDI devices are included
  ///                                 in enumeration results. If false, only unoccupied ones returned
  ///
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, enumerate, cr_EsVariant);

protected:
  // Internal helpers
  //
  static const EsVariant::Array& baudsGet();
  static const EsVariant::Array& bitsGet();
  // unique idGet string
  EsString internalIdGet() const;
  // breaking condition checker
  bool internalIsBreaking() const;
  // generate line break condition 
  void internalLineBreak();
  // open port using current configuration
  bool internalOpen();
  // close port
  void internalClose();
  // query unread bytes
  esU32 queryUnreadBytes();
  // check for & log ftdi device status from the last call
  bool checkLogStatus() const;
  // internal reset method
  bool internalReset();
  // internal CBUS pins setup
  bool setCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode);
  // internal CBUS pins setup, locked with m_cs
  bool interlockedSetCbusBits(bool useStdSocket, bool supplyAuxPower, bool programmingMode);
  // internal baud setter 
  bool internalSetBaud(const long& val, bool doThrow);
  // internal byte io
  esU32 internalPutBytes(const esU8* data, esU32 len, esU32 tmo);
  esU32 internalGetBytes(esU8* data, esU32 len, esU32 tmo);
  
protected:
  // wrappers for EsChannelIoEkonnect method calls from C code
  static esBL cIsBreaking(EseChannelIo* p);
  static esBL cLock(EseChannelIo* p, esU32 tmo);
  static void cUnlock(EseChannelIo* p);
  static esBL cIsConnected(EseChannelIo* p);
  static esBL cConnect(EseChannelIo* p);
  static void cDisconnect(EseChannelIo* p);
  static esU32 cPutBytes(EseChannelIo* p, const esU8* data, esU32 count);
  static esU32 cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout);
  static void cResetIo(EseChannelIo* p);
  static esBL cWaitTxEmpty(EseChannelIo* p);
  static esBL cIsRateSupported(EseChannelIo* p, esU32 rate);
  static esU32 cGetRate(EseChannelIo* p);
  static esBL cSetRate(EseChannelIo* p, esU32 rate);
  static esU32 cSendEstimateGet(EseChannelIo* p, esU32 len);
  static int cGetError(EseChannelIo* p);

protected:
  // data members
  //
  // open device handle
  EsFtdiDeviceIntf::Ptr m_io;
  // multithreading guards
  mutable EsMutex m_mx;
  mutable EsMutex m_mxBoot;
  // io breaker
  EsBaseIntf::Ptr m_breaker;
  EsBaseIntf::Ptr m_monitor;  
  // modes && aux power state
  bool m_programmingMode;
  bool m_supplyAuxPower;
};

#endif // ES_COMM_USE_EKONNECT

#endif // _ekonnect_io_channel_h_
