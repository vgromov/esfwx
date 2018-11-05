#ifndef _es_channel_io_bluetooth_client_h_
#define _es_channel_io_bluetooth_client_h_

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH

// Bluetooth client io channel class
//
class ESCOMM_CLASS ES_INTF_IMPL2( EsChannelIoBluetoothClient, EsChannelIoIntf, EsReflectedClassIntf )

protected:
  class Client : public EsSocketClient
  {
  public:
    Client(EsChannelIoBluetoothClient& owner);

  protected:
    /// Overridable callbacks implementation
    virtual void onSendFarewell(EsSocket::Impl& io);
    virtual void onServerFarewellReceived(EsBinBuffer::const_pointer data, ulong len);

  protected:
    EsChannelIoBluetoothClient& m_owner;
  };
  friend class Client;

protected:
	EsChannelIoBluetoothClient();

public:
	virtual ~EsChannelIoBluetoothClient();

  static EsChannelIoIntf::Ptr create() { return NEW(); }

	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoBluetoothClient )
	ES_DECL_ATTR_HANDLING_STD

	// configuraiton properties
	//
	ES_DECL_PROPERTY(deviceAddress, EsString)
	ES_DECL_PROPERTY(deviceName, EsString)
	ES_DECL_PROPERTY(serviceClass, EsString)
	ES_DECL_PROPERTY(serviceName, EsString)
	ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)
	ES_DECL_PROPERTY_WO(breaker, EsBaseIntfPtr)

	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	// EsChannelIoIntf interface
	//
	// channel unique idGet string
	ES_DECL_INTF_METHOD(EsString, idGet)() const;
	// monitor interface access
	ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const;
	ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor);
	// state manipulation
	ES_DECL_REFLECTED_INTF_METHOD0(bool, open);
	ES_DECL_REFLECTED_INTF_METHOD0(void, close);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isOpen);
	// io breaker access
	ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, breakerGet)() const;
	ES_DECL_INTF_METHOD(void, breakerSet)(const EsBaseIntf::Ptr& breaker);
	// byte io
	ES_DECL_INTF_METHOD(esU32, bytesPut)(const esU8* data, esU32 len, esU32 tmo);
	ES_DECL_INTF_METHOD(esU32, bytesGet)(esU8* data, esU32 len, esU32 tmo);
	ES_DECL_REFLECTED_INTF_METHOD2(ulong, bytesPut, cr_EsBinBuffer, ulong);
	ES_DECL_REFLECTED_INTF_METHOD2(EsBinBuffer, bytesGet, ulong, ulong);
	ES_DECL_REFLECTED_INTF_CONST_METHOD1(ulong, txTimeEstimateGet, ulong);
	// port reset & error info access
	ES_DECL_REFLECTED_INTF_METHOD0(void, reset);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(long, errorGet);
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, errorStringGet);
	// initialize C EseChannelIo from existing EsChannelIoIntf
	ES_DECL_INTF_METHOD(void, commChannelInit)(EseChannelIo* chnl);

protected:
	// return unique channel idGet string
	EsString internalIdGet() const;
	// breaking condition check
	bool internalIsBreaking() const;
	// open port using current configuration
	bool internalOpen();
	// close port
	void internalClose();
	// query unread bytes
	esU32 queryUnreadBytes();
	// check for & log last system|communication line error, if there were any
	bool checkLogLastError() const;
	// internal reset method
	void internalReset();
	// internal baud setter
	bool internalSetBaud(const long& val, bool doThrow);
	// internal byte io
	esU32 internalPutBytes(const esU8* data, esU32 len, esU32 tmo);
	esU32 internalGetBytes(esU8* data, esU32 len, esU32 tmo);

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
	static void cResetIo(EseChannelIo* p);
	static esU32 cSendEstimateGet(EseChannelIo* p, esU32 len);
	static esBL cWaitTxEmpty(EseChannelIo* p);
	static int cGetError(EseChannelIo* p);

protected:
	// Data members
	//
  // Bluetooth address
  EsSocketAddr m_addr;
  // Bluetooth socket
  Client m_bt;
  // Channel monitor and breaker
	EsBaseIntf::Ptr m_breaker;
	EsBaseIntf::Ptr m_monitor;
	// multithreading guard
	mutable EsMutex m_mx;
};

#endif // ES_COMM_USE_CHANNEL_BLUETOOTH

#endif // _es_channel_io_bluetooth_client_h_