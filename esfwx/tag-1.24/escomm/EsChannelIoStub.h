#ifndef _stub_io_channel_h_
#define _stub_io_channel_h_

#ifdef ES_COMM_USE_CHANNEL_STUB

// stub io channel class. does nothing
//
class ESCOMM_CLASS ES_INTF_IMPL2(EsChannelIoStub, EsChannelIoIntf, EsReflectedClassIntf)

protected:
 	EsChannelIoStub();

public:
	virtual ~EsChannelIoStub();

  static EsChannelIoIntf::Ptr create() { return NEW(); }

	// reflection declaration
	//
	ES_DECL_REFLECTED_CLASS_BASE( EsChannelIoStub )
	ES_DECL_ATTR_HANDLING_STD

	// reflected property
	ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)
	ES_DECL_PROPERTY_WO(breaker, EsBaseIntfPtr)

	// reflected constructor
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr);
	
	// EsChannelIoIntf interface
	//
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

protected:
	// internal services
	EsString internalIdGet() const;
	bool internalIsBreaking() const;
	bool internalOpen();
	void internalClose();
	bool internalIsOpen() const;
	esU32 internalPutBytes(const esU8* data, esU32 len, esU32 tmo);
	esU32 internalGetBytes(esU8* data, esU32 len, esU32 tmo);
	void internalReset();
	long internalGetError() const;

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
	static esBL cWaitTxEmpty(EseChannelIo* p);
	static int cGetError(EseChannelIo* p);

protected:
	// data members
	//
	bool m_open;
	// multithreading guard
	mutable EsMutex m_mx;
	EsBaseIntf::Ptr m_breaker;
	EsBaseIntf::Ptr m_monitor;
};

#endif // ES_COMM_USE_CHANNEL_STUB

#endif // _stub_io_channel_h_
