#ifndef _ekosf_std_services_h_
#define _ekosf_std_services_h_

#ifdef ES_COMM_USE_RPC

// Forward declarations
class EsStdIoStateProxy;
class EsProtocolIoMonitorProxy;

// Protocol IO monitor, along with native implementaion,
// may be any reflected class, implementing the same named services, as ones, declared by the native interface.
// idGet string is _any_ string, uniquely identifying the protocol instance which posts its messages to monitor
//
ES_DECL_INTF_BEGIN( 733DCDE0, 5BB242ab, A9DBDABE, C167FA8A, EsProtocolIoMonitorIntf )
  ES_DECL_INTF_METHOD(void, protocolFrameSent)(const EsString& idGet, const EsBinBuffer& frameData) = 0;
  ES_DECL_INTF_METHOD(void, protocolFrameSendingFailed)(const EsString& idGet, const EsString& error) = 0;
  ES_DECL_INTF_METHOD(void, protocolFrameReceived)(const EsString& idGet, const EsBinBuffer& frameData) = 0;
  ES_DECL_INTF_METHOD(void, protocolFrameReceptionFailed)(const EsString& idGet, const EsString& error) = 0;
ES_DECL_INTF_END

// RPC calls-related exception class
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
class PACKAGE EsRpcException : public EsException
#else
class ESCOMM_CLASS EsRpcException : public EsException
#endif
{
protected:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsRpcException(RpcReflectionSignature sig, long rpcId, RpcStatus stat, long ioErr = 0);

public:
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  __fastcall
#endif
  EsRpcException(const EsRpcException& src, const EsScriptDebugInfoIntf::Ptr& dbg);

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
  virtual __fastcall ~EsRpcException();
#endif

  long signatureGet() const;
  long rpcIdGet() const;
  long rpcStatusGet() const;
  long ioErrorGet() const;

  static void Throw(RpcReflectionSignature sig, int rpcId, RpcStatus stat, int ioErr = 0);
};

// ekosfera rpc master interface definition
//
ES_DECL_INTF_BEGIN( E10F84C7, 3C384C34, 8064F310, BB8B2B51, EsRpcMasterIntf )
  // unique id string
  ES_DECL_INTF_METHOD(EsString, idGet)() const = 0;
  // lock|unlock
  ES_DECL_INTF_METHOD(bool, tryLock)() = 0;
  ES_DECL_INTF_METHOD(void, lock)() = 0;
  ES_DECL_INTF_METHOD(void, unlock)() = 0;
  // protocol stuff
  ES_DECL_INTF_METHOD(void, clientAddrSet)(ulong) = 0;
  ES_DECL_INTF_METHOD(ulong, clientAddrGet)() const = 0;
  ES_DECL_INTF_METHOD(void, packetTimeoutSet)(ulong) = 0;
  ES_DECL_INTF_METHOD(ulong, packetTimeoutGet)() const = 0;
  ES_DECL_INTF_METHOD(void, channelSet)(const EsChannelIoIntf::Ptr&) = 0;
  ES_DECL_INTF_METHOD(EsChannelIoIntf::Ptr, channelGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, currentRateGet)() const = 0;
  ES_DECL_INTF_METHOD(bool, hasChannel)() const = 0;
  // state menipulation
  ES_DECL_INTF_METHOD(bool, isActive)() const = 0;
  ES_DECL_INTF_METHOD(void, activate)() = 0;
  ES_DECL_INTF_METHOD(void, deactivate)() = 0;
  // protocol monitor access
  ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& mon) = 0;
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const = 0;
  // standard RPC wrappers
  ES_DECL_INTF_METHOD(bool, ping)() = 0;
  ES_DECL_INTF_METHOD(bool, changeAddr)(ulong) = 0;
  ES_DECL_INTF_METHOD(bool, rateSet)(ulong, bool) = 0;
  ES_DECL_INTF_METHOD(EsVariant, getImplementedRPCs)() = 0;
  ES_DECL_INTF_METHOD(ulong, getHealthInfo)() = 0;
  ES_DECL_INTF_METHOD(void, queueHealthCheck)() = 0;
  ES_DECL_INTF_METHOD(esDT, getDateTime)() = 0;
  ES_DECL_INTF_METHOD(bool, setDateTime)(esDT) = 0;
  ES_DECL_INTF_METHOD(void, factoryReset)() = 0;
  ES_DECL_INTF_METHOD(void, shutdown)() = 0;
  ES_DECL_INTF_METHOD(void, queueBootloader)(ulong) = 0;
  ES_DECL_INTF_METHOD(bool, pingEx)() = 0;

  // Auto-generate C++ wrapper prototypes & reflect methods at the same time
  // NB! only up to 6 parameters RPC calls get reflected, due to rpcId extra parameter
  //
#define RPC_REFLECTION_BEGIN
#define  RPC_DEF_HANDLER0(ReturnType) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG0(ReturnType, Call))(esU16) = 0;
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG1(ReturnType, Call, Param0Type))(esU16, Param0Type) = 0;
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG2(ReturnType, Call, Param0Type, Param1Type))(esU16, Param0Type, Param1Type) = 0;
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG3(ReturnType, Call, Param0Type, Param1Type, Param2Type))(esU16, Param0Type, Param1Type, Param2Type) = 0;
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG4(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type) = 0;
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG5(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) = 0;
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG6(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type))(esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5) = 0;
#define RPC_DEF_HANDLER7(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type) \
  ES_DECL_INTF_METHOD( To_TS_TYPE(ReturnType), GEN_SIG7(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type))(esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5, Param6Type p6) = 0;
#define  RPC_DEF_HANDLER0_NORETURN() \
  ES_DECL_INTF_METHOD(void, VOID_Call)(esU16) = 0;
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG0(VOID_Call, Param0Type))(esU16, Param0Type) = 0;
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG1(VOID_Call, Param0Type, Param1Type))(esU16, Param0Type, Param1Type) = 0;
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG2(VOID_Call, Param0Type, Param1Type, Param2Type))(esU16, Param0Type, Param1Type, Param2Type) = 0;
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG3(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type) = 0;
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG4(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) = 0;
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG5(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) = 0;
#define RPC_DEF_HANDLER7_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG6(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type) = 0;
#define RPC_REFLECTION_END
#include <esfwxe/rpcMap/rpcReflection.cc>

ES_DECL_INTF_END

// standard ekosfera RPC server wrapper class
// when io channel is assigned, it becomes owned by
// this class, all memory management is done automatically
//
class ESCOMM_CLASS ES_INTF_IMPL2(EsRpcMaster,  EsRpcMasterIntf, EsReflectedClassIntf)

public:
  // misc
  enum {
    // delay, in ms after master service activation, before any attempts of connection is made
    activationDelay = 100,
  };

public:
  EsRpcMaster();
  static EsRpcMasterIntf::Ptr create();
  virtual ~EsRpcMaster();

  // declare reflection support
  //
  ES_DECL_REFLECTED_CLASS_BASE(EsRpcMaster)
  ES_DECL_ATTR_HANDLING_STD

  // EsRpcMasterIntf implementation
  //
  // unique id string
  ES_DECL_INTF_METHOD(EsString, idGet)() const ES_OVERRIDE;
  // lock|unlock
  ES_DECL_REFLECTED_INTF_METHOD0(bool, tryLock) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, lock) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, unlock) ES_OVERRIDE;
  // protocol stuff
  ES_DECL_INTF_METHOD(void, clientAddrSet)(ulong) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(ulong, clientAddrGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, packetTimeoutSet)(ulong) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(ulong, packetTimeoutGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, channelSet)(const EsChannelIoIntf::Ptr&) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(EsChannelIoIntf::Ptr, channelGet)() const ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(ulong, currentRateGet) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, hasChannel) ES_OVERRIDE;
  // state menipulation
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isActive) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, activate) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, deactivate) ES_OVERRIDE;
  // monitor interface access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const ES_OVERRIDE;
  ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor) ES_OVERRIDE;
  // standard RPC wrappers
  ES_DECL_REFLECTED_INTF_METHOD0(bool, ping) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(bool, changeAddr, ulong) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD2(bool, rateSet, ulong, bool) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(EsVariant, getImplementedRPCs) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(ulong, getHealthInfo) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, queueHealthCheck) ES_OVERRIDE;
  ES_DECL_INTF_METHOD(esDT, getDateTime)() ES_OVERRIDE;        //< do not reflect datetime interface methods, as it creates reflection naming clash,
  ES_DECL_INTF_METHOD(bool, setDateTime)(esDT) ES_OVERRIDE;    //< because esDT_Call in this case is both signature and auto-reflcted method name defined in RPC_REFLECTION_BEGIN section
  ES_DECL_REFLECTED_INTF_METHOD0(void, factoryReset) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD0(void, shutdown) ES_OVERRIDE;
  ES_DECL_REFLECTED_INTF_METHOD1(void, queueBootloader, ulong) ES_OVERRIDE;

  // advanced communication services
  //
  // ping with optional channel settings tune-up (advanced ping)
  //
  ES_DECL_REFLECTED_INTF_METHOD0(bool, pingEx) ES_OVERRIDE;

  // Auto-generate C++ wrapper prototypes & reflect methods at the same time
  // NB! only up to 5 parameters RPC calls get reflected, due to rpcId extra parameter
  //
#define RPC_REFLECTION_BEGIN
#define  RPC_DEF_HANDLER0(ReturnType) \
  ES_DECL_REFLECTED_INTF_METHOD1( To_TS_TYPE(ReturnType), GEN_SIG0(ReturnType, Call), esU16) ES_OVERRIDE;
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
  ES_DECL_REFLECTED_INTF_METHOD2( To_TS_TYPE(ReturnType), GEN_SIG1(ReturnType, Call, Param0Type), esU16, Param0Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
  ES_DECL_REFLECTED_INTF_METHOD3( To_TS_TYPE(ReturnType), GEN_SIG2(ReturnType, Call, Param0Type, Param1Type), esU16, Param0Type, Param1Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
  ES_DECL_REFLECTED_INTF_METHOD4( To_TS_TYPE(ReturnType), GEN_SIG3(ReturnType, Call, Param0Type, Param1Type, Param2Type), esU16, Param0Type, Param1Type, Param2Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
  ES_DECL_REFLECTED_INTF_METHOD5( To_TS_TYPE(ReturnType), GEN_SIG4(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type), esU16, Param0Type, Param1Type, Param2Type, Param3Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  ES_DECL_REFLECTED_INTF_METHOD6( To_TS_TYPE(ReturnType), GEN_SIG5(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type), esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  ES_DECL_INTF_METHOD(To_TS_TYPE(ReturnType), GEN_SIG6(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type))(esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5) ES_OVERRIDE;
#define RPC_DEF_HANDLER7(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type) \
  ES_DECL_INTF_METHOD(To_TS_TYPE(ReturnType), GEN_SIG7(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type))(esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5, Param6Type p6) ES_OVERRIDE;
#define  RPC_DEF_HANDLER0_NORETURN() \
  ES_DECL_REFLECTED_INTF_METHOD1(void, VOID_Call, esU16) ES_OVERRIDE;
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
  ES_DECL_REFLECTED_INTF_METHOD2(void, GEN_SIG0(VOID_Call, Param0Type), esU16, Param0Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
  ES_DECL_REFLECTED_INTF_METHOD3(void, GEN_SIG1(VOID_Call, Param0Type, Param1Type), esU16, Param0Type, Param1Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
  ES_DECL_REFLECTED_INTF_METHOD4(void, GEN_SIG2(VOID_Call, Param0Type, Param1Type, Param2Type), esU16, Param0Type, Param1Type, Param2Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
  ES_DECL_REFLECTED_INTF_METHOD5(void, GEN_SIG3(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type), esU16, Param0Type, Param1Type, Param2Type, Param3Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  ES_DECL_REFLECTED_INTF_METHOD6(void, GEN_SIG4(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type), esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG5(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) ES_OVERRIDE;
#define RPC_DEF_HANDLER7_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type) \
  ES_DECL_INTF_METHOD(void, GEN_SIG6(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type))(esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type, Param6Type) ES_OVERRIDE;
#define RPC_REFLECTION_END
#include <esfwxe/rpcMap/rpcReflection.cc>

public:
  // reflected properties
  //
  ES_DECL_PROPERTY_RO(id, EsString)
  ES_DECL_PROPERTY(clientAddr, ulong)
  ES_DECL_PROPERTY(packetTimeout, ulong)
  ES_DECL_PROPERTY(channel, EsBaseIntfPtr)
  ES_DECL_PROPERTY_WO(monitor, EsBaseIntfPtr)

  // reflected methods
  //
  // class services
  //
  ES_DECL_REFLECTED_CLASS_CTOR0(EsBaseIntfPtr)
  {
    return EsRpcMaster::create();
  }

  // helper services
  //
  // return string for RpcStatus
  static EsString getRpcStatusString(RpcStatus stat);
  // return string representation for rpc signature
  static EsString getRpcSignatureString(RpcReflectionSignature sig);
  // return string for protocol error
  static EsString getProtocolErrorString(int err);
  // check Rpc completion status. throw EsRpcException if it's not ok
  void checkThrowRpcStatus(RpcReflectionSignature sig, int rpcId, RpcStatus stat);

private:
  // perform channel validity checks
  void checkChannel() const;
  void checkChannelActive() const;

protected:
  // total rpc master timeout - timeout during which we
  // wait for response packet. overall rpc response wait time
  // is superposition of packet timeout and this value
  long m_totalTimeout;
  EsStdIoStateProxy* m_ioStateProxy; // do not expose real EseStdIoState structure outside the class
  EsChannelIoIntf::Ptr m_chnl;
  EseChannelIo m_cChnl; ///< C channel adapter abstraction
  EsProtocolIoMonitorProxy* m_monitorProxy;
  // thread-safety guard
  mutable EsCriticalSection m_cs;
  // protocol monitor
  EsBaseIntf::Ptr m_monitor;

  friend class EsStdIoStateProxy;
  friend class EsProtocolIoMonitorProxy;

private:
  // prohibited functionality
  EsRpcMaster(const EsRpcMaster&) ES_REMOVEDECL;
  const EsRpcMaster& operator=(const EsRpcMaster&) ES_REMOVEDECL;
};

#endif // ES_COMM_USE_RPC

#endif // _ekosf_std_services_h_
