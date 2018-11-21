#ifndef _uart_io_h_
#define _uart_io_h_

#ifdef ES_COMM_USE_UART

/// Structure used for port enumeration method
/// Struct used when enumerating the available serial ports
/// Holds information about an individual serial port.
///
class ESCOMM_CLASS EsUartInfo
{
public:
  typedef std::shared_ptr<EsUartInfo> Ptr;

protected:
  EsUartInfo() :
  m_isModem(false),
  m_isUSBdevice(false)
  {}

public:
  bool isEmpty() const
  {
    return m_strDevPath.empty() &&
      m_strName.empty() &&
      m_strFriendlyName.empty() &&
      m_strInstPath.empty();
  }

  bool isModem() const { return m_isModem; }
  bool isUSBdevice() const { return m_isUSBdevice; }

  const EsString& devicePathGet() const { return m_strDevPath; }
  const EsString& nameGet() const { return m_strName; }
  const EsString& friendlyNameGet() const { return m_strFriendlyName; }
  const EsString& instancePathGet() const { return m_strInstPath; }

protected:
  EsString m_strDevPath;        ///< Device path for use with CreateFile()
  EsString m_strName;           ///< Simple name (i.e. COM1)
  EsString m_strFriendlyName;   ///< Full name to be displayed to a user
  EsString m_strInstPath;       ///< Device instance path string (optional)
  bool m_isModem;               ///< Modem is connected to this port
  bool m_isUSBdevice;           ///< Provided through a USB connection

private:
  EsUartInfo(const EsUartInfo&);
  EsUartInfo& operator=(const EsUartInfo&);

  friend class EsUartEnumerator;
};
typedef std::vector<EsUartInfo::Ptr> EsUartInfos;
//---------------------------------------------------------------------------

/// UART devices enumerator interface
///
class ESCOMM_CLASS EsUartEnumerator
{
public:
  // UART Availability flags
  enum EsUartAvail
  {
    uartUnknown = -1,
    uartAvailable,
    uartNotPresent,
    uartInUse
  };

public:
  EsUartEnumerator();

  void enumerate(bool busyPortsInclude = true);

  /// Access enumeration results
  const EsUartInfos& resultGet() const { return m_infos; }

  // Misc lookup routines
  EsUartInfo::Ptr uartInfoFindByDevPath(const EsString& val) const;
  EsUartInfo::Ptr uartInfoFindByName(const EsString& val) const;
  EsUartInfo::Ptr uartInfoFindByFriendlyName(const EsString& val) const;

  // UART availability check
  static EsUartAvail availabilityCheck(const EsUartInfo::Ptr& info);

protected:
  EsUartInfos m_infos;

private:
  EsUartEnumerator(const EsUartEnumerator&);
  EsUartEnumerator& operator=(const EsUartEnumerator&);
};
#endif // ES_COMM_USE_UART

#ifdef ES_COMM_USE_CHANNEL_UART

//---------------------------------------------------------------------------
// UART non-reflected enumerations
//
enum class EsUartIoParity : ulong
{
  None   = 0,
  Odd    = 1,
  Even   = 2,
  Mark   = 3,
  Space  = 4
};

enum class EsUartIoStopBits : ulong
{
  _1     = 0,
  _1_5   = 1,
  _2     = 2
};

//---------------------------------------------------------------------------
// UART reflected enumerations
//
namespace EsReflection
{
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCOMM_CLASS, EsUartIoParity)
  ES_DECL_ENUMERATION_ITEM_LABELLED(None,     static_cast<ulong>(::EsUartIoParity::None),   _i("None"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Odd,      static_cast<ulong>(::EsUartIoParity::Odd),    _i("Odd"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Even,     static_cast<ulong>(::EsUartIoParity::Even),   _i("Even"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Mark,     static_cast<ulong>(::EsUartIoParity::Mark),   _i("Mark"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(Space,    static_cast<ulong>(::EsUartIoParity::Space),  _i("Space"))
ES_DECL_ENUMERATION_END

ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCOMM_CLASS, EsUartIoStopBits)
  ES_DECL_ENUMERATION_ITEM_LABELLED(_1,       static_cast<ulong>(::EsUartIoStopBits::_1),   _i("1 Stop Bit"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(_1_5,     static_cast<ulong>(::EsUartIoStopBits::_1_5), _i("1.5 Stop Bits"))
  ES_DECL_ENUMERATION_ITEM_LABELLED(_2,       static_cast<ulong>(::EsUartIoStopBits::_2),   _i("2 Stop Bits"))
ES_DECL_ENUMERATION_END
}

// UART IO channel class
//
class ESCOMM_CLASS ES_INTF_IMPL3( EsChannelIoUart, EsChannelIoIntf, EsRateCtlIntf, EsReflectedClassIntf )

protected:
  EsChannelIoUart();

public:
  virtual ~EsChannelIoUart();

  static EsChannelIoIntf::Ptr create() { return NEW(); }

  // reflection declaration
  //
  ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoUart )
  ES_DECL_ATTR_HANDLING_STD

  // UART configuraiton properties
  //
  ES_DECL_PROPERTY_STD(port, EsString)
  ES_DECL_PROPERTY_STD(baud, long)
  ES_DECL_PROPERTY_STD(bits, long)
  ES_DECL_PROPERTY_STD(parity, long)
  ES_DECL_PROPERTY_STD(stopBits, long)
  ES_DECL_PROPERTY_STD(rxBuffLen, long)
  ES_DECL_PROPERTY_STD(txBuffLen, long)
  ES_DECL_PROPERTY_STD(resetOnRxTmo, bool)
  ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)
  ES_DECL_PROPERTY_WO(breaker, EsBaseIntfPtr)

  // Reflected constructor
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

  // EsChannelIoIntf interface
  //

  // Channel unique idGet string
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, idGet) ES_OVERRIDE;

  // Monitor interface access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor) ES_OVERRIDE;

  // State manipulation
  ES_DECL_REFLECTED_INTF_METHOD0(bool, open) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, close) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen) ES_OVERRIDE;

  // IO breaker access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, breakerGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, breakerSet)(const EsBaseIntf::Ptr& breaker) ES_OVERRIDE;

  // Byte io
  ES_DECL_INTF_METHOD(esU32, bytesPut)(const esU8* data, esU32 len, esU32 tmo) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(esU32, bytesGet)(esU8* data, esU32 len, esU32 tmo) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(ulong, bytesPut, cr_EsBinBuffer, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, bytesGet, ulong, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, txTimeEstimateGet, ulong) ES_OVERRIDE;

  // UART reset & error info access
  ES_DECL_REFLECTED_INTF_METHOD0(void, reset) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, errorGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, errorStringGet) ES_OVERRIDE;

  // Initialize C EseChannelIo from existing EsChannelIoIntf
  ES_DECL_INTF_METHOD(void, commChannelInit)(EseChannelIo* chnl) ES_OVERRIDE;

  // EsRateCtlIntf interface
  //
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, isRateSupported, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsVariant, supportedRatesGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, defaultRateGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, rateGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(bool, rateSet, ulong) ES_OVERRIDE;

  // The rest of services (non-interfaced)
  //

  void lineBreak();

  // Static services
  //

  static EsString idStringFormat(const EsString& portStr);

  // Return per-byte timeout in ms, calculated for baud, bits, bool parity, stop
  ES_DECL_REFLECTED_CLASS_METHOD4(ulong, getMaxByteTmo, ulong, ulong, bool, ulong);

  /// Reflected UARTs enumeration service - returned is a variant collection, each item
  /// is in turn a collection of port infos, as follows:
  /// [0] - full UART device path string, OS-dependent
  /// [1] - short UART device name
  /// [2] - friendly UART device name, possible, more verbose, than short name, or the same as the one
  /// [3] - UART device instance path, OS-idependent, on non-Windows OSes, Windows form is emulated
  /// [4] - 'UART device is on USB bus' flag
  /// [5] - 'UART device is modem' flag
  ///
  /// Parameters define filtering which applies to UART enumeration procedure
  /// @param[in] includeBusyPorts bool -  if true, both busy and free UART devices are included
  ///                                 in enumeration results. If false, only unoccupied ports
  ///                                 returned
  /// @param[in] usbOnly          bool -  if true, include only UARTS on USB bus in search result
  ///
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, enumerate, cr_EsVariant, cr_EsVariant);

protected:
  // Internal helpers
  //
  static const EsVariant& baudsGet();
  static const EsVariant& bitsGet();

  // Return unique channel idGet string
  EsString internalIdGet() const;

  // Breaking condition check
  bool internalIsBreaking() const;

  // Send line break condition
  void internalLineBreak();

  // Open port using current configuration
  bool internalOpen();

  // Close port
  void internalClose();

  // Reset error & comm state
  void resetError();

  // Query unread bytes
  esU32 queryUnreadBytes();

  // Query unsent bytes
  esU32 queryUnsentBytes();

  // Check for & log last system|communication line error, if there were any
  bool checkLogLastError() const;

  // Decode comm line error into string
  static EsString decodeLineError(esU32 err);

  // Internal reset method
  void internalReset();

  // Internal baud setter
  bool internalSetBaud(const long& val, bool doThrow);

  // Internal byte io
  esU32 internalPutBytes(const esU8* data, esU32 len, esU32 tmo);
  esU32 internalGetBytes(esU8* data, esU32 len, esU32 tmo);

protected:
  // Wrappers for C channel interface
  static esBL cIsBreaking(EseChannelIo* p);
  static esBL cLock(EseChannelIo* p, esU32 tmo);
  static void cUnlock(EseChannelIo* p);
  static esBL cIsConnected(EseChannelIo* p);
  static esBL cConnect(EseChannelIo* p);
  static void cDisconnect(EseChannelIo* p);
  static esU32 cPutBytes(EseChannelIo* p, const esU8* data, esU32 count);
  static esU32 cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout);
  static void cResetIo(EseChannelIo* p);
  static esBL cIsRateSupported(EseChannelIo* p, esU32 rate);
  static esU32 cGetRate(EseChannelIo* p);
  static esBL cSetRate(EseChannelIo* p, esU32 rate);
  static esU32 cSendEstimateGet(EseChannelIo* p, esU32 len);
  static esBL cWaitTxEmpty(EseChannelIo* p);
  static int cGetError(EseChannelIo* p);

protected:
  // Multithreading guard
  mutable EsMutex m_mx;
  esU32 m_sysError;
  esU32 m_lineError;

#if ES_OS == ES_OS_WINDOWS
  HANDLE
#elif defined(ES_POSIX_COMPAT)
  int
#endif
    m_com;

  EsBaseIntf::Ptr m_breaker;
  EsBaseIntf::Ptr m_monitor;
};

#else // ES_COMM_USE_CHANNEL_UART

// Implement time estimate as namespaced code, for compatibility with EsChannelIoEkonnect
namespace EsChannelIoUart
{
  ESCOMM_FUNC( ulong, getMaxByteTmo(ulong, ulong, bool, ulong) );
}
#endif // ES_COMM_USE_CHANNEL_UART

#endif // _uart_io_h_
