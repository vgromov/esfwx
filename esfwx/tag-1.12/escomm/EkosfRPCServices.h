#ifndef _ekosf_std_services_h_
#define _ekosf_std_services_h_

// RPC calls-related exception class
//
class EKOSF_COMM_CLASS EsRpcException : public EsException
{
protected:
	EsRpcException(RpcReflectionSignature sig, int rpcId, RpcStatus stat, int ioErr = 0);

public:
	static void Throw(RpcReflectionSignature sig, int rpcId, RpcStatus stat, int ioErr = 0);

protected:
	RpcReflectionSignature m_sig;
	int m_rpcId;
};

// standard ekosfera RPC server wrapper class
// when io channel is assigned, it becomes owned by
// this class, all memory management is done automatically
//
class EKOSF_COMM_CLASS EsRpcMaster : public EsReflectedClassIntf
{
public:
	// dynamic ctor tag
	enum objectDynamic { objectDynamicCtor };
	// misc
	enum {
		// delay, in ms after master service activation, before any attempts of connection is made
		activationDelay = 100,
	};

protected:
	EsRpcMaster(objectDynamic);
	
public:
	// dynamic flag should be true for construction on heap 
	EsRpcMaster();

	// declare reflection support
	//
	ES_DECL_REFLECTED_CLASS_BASE(EsRpcMaster)

	// interface support map
	//
	ES_INTF_MAP_BEGIN( EsRpcMaster )
		ES_INTF_SUPPORTS( EsRpcMaster, EsReflectedClassIntf )
	ES_INTF_MAP_END
	{
			// destroy implementation, if was created dynamically
		if( m_dynamic )
			delete this;
	}	

	// channel manipulation
	ES_DECL_REFLECTED_METHOD1(void, setChannel, cr_EsVariant);
	EsIoChannelIntf::Ptr getChannel() const { return m_io; }
	ES_DECL_REFLECTED_CONST_METHOD0(EsVariant, getChannelReflected) { return m_io; }
	ES_DECL_REFLECTED_CONST_METHOD1(bool, hasChannel, cr_EsString);
	ES_DECL_REFLECTED_CONST_METHOD0(bool, isActive);
	ES_DECL_REFLECTED_METHOD0(void, activate);
	ES_DECL_REFLECTED_METHOD0(void, deactivate);
	// try to set current channel from name & configuration string
	ES_DECL_REFLECTED_METHOD3(void, setChannelFromConfig, cr_EsString, cr_EsString, p_wxEvtHandler );
	
	// standard RPC wrappers
	ES_DECL_REFLECTED_METHOD1(bool, ping, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD2(bool, changeAddr, EsBaseIntfPtr, ulong);
	ES_DECL_REFLECTED_METHOD3(EsVariant, enumerateSlaves, EsBaseIntfPtr, ulong, ulong);
	ES_DECL_REFLECTED_METHOD3(bool, setRate, EsBaseIntfPtr, ulong, bool);
	ES_DECL_REFLECTED_METHOD1(EsVariant, getImplementedRPCs, EsBaseIntfPtr);
	ApplianceFirmwareID getFwID(EsIoBreakIntf::Ptr breaker);
	HubFirmwareID getHubFwID(EsIoBreakIntf::Ptr breaker);
	ES_DECL_REFLECTED_METHOD1(EsVariant, getFwIDReflected, EsBaseIntfPtr) { return EsShortHwId::create( getFwID(p1) ); }
	ES_DECL_REFLECTED_METHOD1(EsVariant, getHubFwIDReflected, EsBaseIntfPtr) { return EsLongHwId::create( getHubFwID(p1) ); }
	ES_DECL_REFLECTED_METHOD1(EsVariant, getHwUID, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD1(ulong, getHealthInfo, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD1(void, queueHealthCheck, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD1(EsVariant, getDateTime, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD2(bool, setDateTime, EsBaseIntfPtr, cr_EsVariant);
	ES_DECL_REFLECTED_METHOD1(void, factoryReset, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD1(void, shutdown, EsBaseIntfPtr);
	ES_DECL_REFLECTED_METHOD2(void, queueBootloader, EsBaseIntfPtr, ulong);
	ES_DECL_REFLECTED_METHOD1(EsVariant, getPowerStatus, EsBaseIntfPtr);
	FsResult dirListStart(EsIoBreakIntf::Ptr breaker, const EsString& dir);
	FsResult dirListGetNextItem(EsIoBreakIntf::Ptr breaker, FsItem& item);
	FsResult dirDelete(EsIoBreakIntf::Ptr breaker, const EsString& dir, bool force);
	FsResult fileDelete(EsIoBreakIntf::Ptr breaker, const EsString& file);
	FsResult fileSysFreeSpaceInfoGet(EsIoBreakIntf::Ptr breaker, EsMemSpace& total, EsMemSpace& free);
	FsResult fileReadStart(EsIoBreakIntf::Ptr breaker, const EsString& file, DWORD& chunks);
	ES_DECL_REFLECTED_METHOD1(EsBinBuffer, fileReadNext, EsBaseIntfPtr);
	
	// advanced communication services
	//
	// ping with optional channel settings tune-up (advanced ping)
	// optional EsThread::Worker pointer may be used in threaded execution to check for 
	// thread termination request
	//
	ES_DECL_REFLECTED_METHOD1(bool, pingEx, EsBaseIntfPtr);
	// universal device identification. returned is reflected device hardware id 
	ES_DECL_REFLECTED_METHOD1(EsVariant, getDeviceID, EsBaseIntfPtr);

	// auto-generate C++ wrapper prototypes & reflect methods at the same time
	// NB! only up to 4 parameters RPC calls get reflected, due to rpcId && breaker extra parameters
	//
#define RPC_REFLECTION_BEGIN
#define	RPC_DEF_HANDLER0(ReturnType) \
	ES_DECL_REFLECTED_METHOD2( To_TS_TYPE(ReturnType), GEN_SIG0(ReturnType, Call), UINT16, EsBaseIntfPtr);
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
	ES_DECL_REFLECTED_METHOD3( To_TS_TYPE(ReturnType), GEN_SIG1(ReturnType, Call, Param0Type), UINT16, EsBaseIntfPtr, Param0Type);
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
	ES_DECL_REFLECTED_METHOD4( To_TS_TYPE(ReturnType), GEN_SIG2(ReturnType, Call, Param0Type, Param1Type), UINT16, EsBaseIntfPtr, Param0Type, Param1Type);
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
	ES_DECL_REFLECTED_METHOD5( To_TS_TYPE(ReturnType), GEN_SIG3(ReturnType, Call, Param0Type, Param1Type, Param2Type), UINT16, EsBaseIntfPtr, Param0Type, Param1Type, Param2Type);
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
	ES_DECL_REFLECTED_METHOD6( To_TS_TYPE(ReturnType), GEN_SIG4(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type), UINT16, EsBaseIntfPtr, Param0Type, Param1Type, Param2Type, Param3Type);
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
	To_TS_TYPE(ReturnType) GEN_SIG5(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4);
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
	To_TS_TYPE(ReturnType) GEN_SIG6(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5);
#define	RPC_DEF_HANDLER0_NORETURN() \
	ES_DECL_REFLECTED_METHOD2(void, VOID_Call, UINT16, EsBaseIntfPtr);
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
	ES_DECL_REFLECTED_METHOD3(void, GEN_SIG0(VOID_Call, Param0Type), UINT16, EsBaseIntfPtr, Param0Type);
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
	ES_DECL_REFLECTED_METHOD4(void, GEN_SIG1(VOID_Call, Param0Type, Param1Type), UINT16, EsBaseIntfPtr, Param0Type, Param1Type);
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
	ES_DECL_REFLECTED_METHOD5(void, GEN_SIG2(VOID_Call, Param0Type, Param1Type, Param2Type), UINT16, EsBaseIntfPtr, Param0Type, Param1Type, Param2Type);
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
	ES_DECL_REFLECTED_METHOD6(void, GEN_SIG3(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type), UINT16, EsBaseIntfPtr, Param0Type, Param1Type, Param2Type, Param3Type);
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
	void GEN_SIG4(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4);
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
	void GEN_SIG5(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5);
#define RPC_REFLECTION_END

	#include <common/rpcMap/rpcReflection.cc>

public:
	// reflected properties
	//
	ES_DECL_PROPERTY(clientAddr, ulong)
	ES_DECL_PROPERTY(packetTimeout, ulong)
	ES_DECL_PROPERTY(packetRetries, ulong)
	ES_DECL_PROPERTY_RO(currentRate, ulong)
	ES_DECL_PROPERTY_RO(hasChannel, bool)
	// class services
	//
	ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr) 
	{ 
		EsRpcMaster* svr = new EsRpcMaster(objectDynamicCtor); 
		wxASSERT(svr); 
		return EsBaseIntf::Ptr( static_cast<EsBaseIntf*>(svr) ); 
	}

	// helper services
	// 
	// return string for RpcStatus 
	static EsString getRpcStatusString(RpcStatus stat);
	// return string representation for rpc signature
	static EsString getRpcSignatureString(RpcReflectionSignature sig);
	// return string for communication error 
	static EsString getCommunicationErrorString(int err);
	// check Rpc completion status. throw EsRpcException if it's not ok
	void checkThrowRpcStatus(RpcReflectionSignature sig, int rpcId, RpcStatus stat);

protected:
	// perform channel validity checks
	void checkChannel() const;
	void checkChannelActive() const;

protected:
	// dynamic creation flag
	bool m_dynamic;
	// total rpc master timeout - timeout during which we 
	// wait for response packet. overall rpc response wait time
	// is superposition of packet timeout and this value
	long m_totalTimeout;
#pragma warning(push)
#pragma warning(disable: 4251)
	std::vector<BYTE> m_ioState; // do not expose real StdIoState structure outside the class
	EsIoChannelIntf::Ptr m_io;
#pragma warning(pop)
	CommChannel m_chnl;
	// thread-safety guard
	mutable wxCriticalSection m_cs;
};

#endif // _ekosf_std_services_h_
