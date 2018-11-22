#ifndef _es_channel_io_bluetooth_le_h_
#define _es_channel_io_bluetooth_le_h_

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE

/// Bluetooth LE byte streaming IO channel over our specific service
/// {04000000-10AE-4C36-9A27-96BCD4C7F835}
/// Two characteristics used there are:
/// {04000100-10AE-4C36-9A27-96BCD4C7F835} - pending incoming bytes count, with notification
/// {04000200-10AE-4C36-9A27-96BCD4C7F835} - byte IO, by chunks with maximum length of 20 bytes each
///
class ESCOMM_CLASS ES_INTF_IMPL2( EsChannelIoBluetoothLE, EsChannelIoIntf, EsReflectedClassIntf )

protected:
	EsChannelIoBluetoothLE();

public:
	virtual ~EsChannelIoBluetoothLE();

  static EsChannelIoIntf::Ptr create() { return NEW(); }

	/// Reflection declaration
	///
	ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoBluetoothLE )
	ES_DECL_ATTR_HANDLING_STD

	/// Configuraiton properties
	///
	ES_DECL_PROPERTY(deviceAddress, EsString)
	ES_DECL_PROPERTY_STD(subscribeToNotifications, bool)
	ES_DECL_PROPERTY_RO(deviceName, EsString)
	ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)
	ES_DECL_PROPERTY_WO(breaker, EsBaseIntfPtr)

	/// Reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);

	/// EsChannelIoIntf interface
	///

	/// Channel unique idGet string
	ES_DECL_INTF_METHOD(EsString, idGet)() const ES_OVERRIDE;

	/// Monitor interface access
	ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const ES_OVERRIDE;
	ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor) ES_OVERRIDE;

	/// State manipulation
	ES_DECL_REFLECTED_INTF_METHOD0(bool, open) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD0(void, close) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen) ES_OVERRIDE;

	/// IO breaker access
	ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, breakerGet)() const ES_OVERRIDE;
	ES_DECL_INTF_METHOD(void, breakerSet)(const EsBaseIntf::Ptr& breaker) ES_OVERRIDE;

	/// Byte IO
	ES_DECL_INTF_METHOD(esU32, bytesPut)(const esU8* data, esU32 len, esU32 tmo) ES_OVERRIDE;
	ES_DECL_INTF_METHOD(esU32, bytesGet)(esU8* data, esU32 len, esU32 tmo) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD2(ulong, bytesPut, cr_EsBinBuffer, ulong) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, bytesGet, ulong, ulong) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, txTimeEstimateGet, ulong) ES_OVERRIDE;

	/// Port reset & error info access
	ES_DECL_REFLECTED_INTF_METHOD0(void, reset) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, errorGet) ES_OVERRIDE;
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, errorStringGet) ES_OVERRIDE;

	/// Initialize C EseChannelIo from existing EsChannelIoIntf
	ES_DECL_INTF_METHOD(void, commChannelInit)(EseChannelIo* chnl) ES_OVERRIDE;

  /// Return byte stream IO service GUID
  static const GUID& byteSvcUUID();

protected:
  // Internal (non-interlocked) services
  //
  void init();
  void uninit();
  bool internalIsOpen() const;
  bool internalIsConnected() const;
  void internalConnect();
  bool internalReconnect();
  void internalReset();
	// Return unique channel idGet string
	EsString internalIdGet() const;
	// Breaking condition check
	bool internalIsBreaking() const;
	// query unread bytes
	esU32 queryUnreadBytes();
	// check for & log recent communication error, if there were any
	void checkLogLastError() const;
	// internal baud setter
	bool internalSetBaud(const long& val, bool doThrow);
	// internal byte io
	esU32 internalBytesPut(const esU8* data, esU32 len, esU32 tmo);
	esU32 internalBytesGet(esU8* data, esU32 len, esU32 tmo);
  // TX batch operation support
  bool txBatchBegin();
  void txBatchEnd(bool ok);
  esU32 txCacheSend();

protected:
	// wrappers for c channel interface
	static esBL cIsBreaking(EseChannelIo* p);
	static esBL cLock(EseChannelIo* p, esU32 tmo);
	static void cUnlock(EseChannelIo* p);
	static esBL cIsConnected(EseChannelIo* p);
	static esBL cConnect(EseChannelIo* p);
	static void cDisconnect(EseChannelIo* p);
	static esU32 cPutBytes(EseChannelIo* p, const esU8* data, esU32 count);
	static esU32 cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout);
  static esBL cTxBatchBegin(EseChannelIo* p);
  static void cTxBatchEnd(EseChannelIo* p, esBL ok);
	static void cResetIo(EseChannelIo* p);
	static esU32 cSendEstimateGet(EseChannelIo* p, esU32 len);
	static esBL cWaitTxEmpty(EseChannelIo* p);
	static int cGetError(EseChannelIo* p);

protected:
	// Multithreading guard
	mutable EsMutex m_mx;
  EsBluetoothRadio::Ptr m_radio;
  EsBluetoothLEdevice::Ptr m_dev;
  EsBluetoothLEcharacteristic::Ptr m_chxPendingBytes;
  EsBluetoothLEcharacteristic::Ptr m_chxIo;
  EsString m_addr;
  EsString m_name;
  EsBluetoothLEresult m_result;
  EsBaseIntf::Ptr m_monitor;
  EsBaseIntf::Ptr m_breaker;
  EsBinBuffer m_cacheRx;
  EsBinBuffer m_cacheTx;
  bool m_inTxBatch;
};
//---------------------------------------------------------------------------

#endif // ES_COMM_USE_CHANNEL_BLUETOOTH_LE

#endif // _es_channel_io_bluetooth_le_h_

