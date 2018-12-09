#include "escommpch.h"
#pragma hdrstop

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#ifdef ES_COMM_USE_RPC

#include <esfwxe/remoteProcCall.h>
#include <esfwxe/protocols/proprietary/protocolEkosf.h>
#include "EsRpcServices.h"

#define ERR_RATE_CTL_IS_NOT_SUPPORTED_IN_S1      _("Rate control functionality is not supported in '%s'")

// ----------------------------------------------------------------------
// rpc exception implementation
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsRpcException::EsRpcException(RpcReflectionSignature sig, long rpcId, RpcStatus stat, long ioErr /*= 0*/) :
EsException(
  stat,
  RpcCancelled == stat ?
    EsException::severityWarning :
    EsException::severityGeneric,
  facilityEsRPC,
  EsString::format(
    _("RPC with signature ='%s' and id ='%d' failed with error: '%s'"),
    EsRpcMaster::getRpcSignatureString(sig),
    rpcId,
    EsRpcMaster::getRpcStatusString(stat)
  )
)
{
  m_data.setToNull(EsVariant::VAR_VARIANT_COLLECTION);
  m_data.addToVariantCollection(stat)
    .addToVariantCollection(sig)
    .addToVariantCollection(rpcId)
    .addToVariantCollection(ioErr);

  if( ioErr )
    m_msg += EsString::format(
      _("\nDetails: '%s'"), 
      EsRpcMaster::getProtocolErrorString(ioErr)
    );
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall
#endif
EsRpcException::EsRpcException(const EsRpcException& src, const EsScriptDebugInfoIntf::Ptr& dbg) :
EsException(src, dbg)
{
  logEsException();
}
//---------------------------------------------------------------------------

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
__fastcall EsRpcException::~EsRpcException()
{}
#endif
//---------------------------------------------------------------------------

long EsRpcException::signatureGet() const
{
  return m_data.itemGet(0).asLong();
}
//---------------------------------------------------------------------------

long EsRpcException::rpcIdGet() const
{
  return m_data.itemGet(1).asLong();
}
//---------------------------------------------------------------------------

long EsRpcException::rpcStatusGet() const
{
  return m_data.itemGet(2).asLong();
}
//---------------------------------------------------------------------------

long EsRpcException::ioErrorGet() const
{
  return m_data.itemGet(3).asLong();
}
//---------------------------------------------------------------------------

void EsRpcException::Throw(RpcReflectionSignature sig, int rpcId, RpcStatus stat, int ioErr /*= 0*/)
{
  if( stat != RpcOK )
    throw EsRpcException(sig, rpcId, stat, ioErr);
}
//---------------------------------------------------------------------------
//----------------------------------------------------------------------------------

// Reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsRpcMaster, esT("RPC master services over Ekosfera protocol."))
  // Reflected properties
  ES_DECL_PROP_INFO_RO(EsRpcMaster, id, EsString, _i("Unique Id string"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(EsRpcMaster, monitor, EsBaseIntfPtr, _i("Protocol io monitor"), NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsRpcMaster, clientAddr, ulong, _i("Client address"), ekosfBroadcastPeerAddr, 0, ekosfBroadcastPeerAddr, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_RANGE_PERSISTENT(EsRpcMaster, packetTimeout, ulong, _i("Packet timeout, ms"), ekosfDefPacketTmo, 0, 60000, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_PERSISTENT(EsRpcMaster, channel, EsBaseIntfPtr, _i("Master channel"), EsVariant::nullObject(), NO_PROPERTY_DESCR)
  // Reflected interface services
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, tryLock, bool_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, lock, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, unlock, void_Call, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, currentRateGet, ulong_CallConst, esT("Return currently applied io rate"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, hasChannel, bool_CallConst, esT("Return true if RPC server's channel is set"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, isActive, bool_CallConst, esT("Return true if RPC server's channel is active"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, activate, void_Call, esT("Activate RPC communication"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, deactivate, void_Call, esT("Deactivate RPC communication"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, ping, bool_Call, esT("Peer device ping"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, pingEx, bool_Call, esT("Extended peer ping service"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, changeAddr, bool_Call_ulong, esT("Peer device address set-up"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, rateSet, bool_Call_ulong_bool, esT("Set communication rate, if underlying channel supports it"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, getImplementedRPCs, EsVariant_Call, esT("Return ulong collection of RPC idGet's implemented on client side"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, getHealthInfo, ulong_Call, esT("Return client device health info mask"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, queueHealthCheck, void_Call, esT("Queue client device health check"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, factoryReset, void_Call, esT("Initiate device's factory reset"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, shutdown, void_Call, esT("Initiate device's shutdown"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, queueBootloader, void_Call_ulong, esT("Request device to enter bootloader mode for specified time"))
  // auto-generate RPC interface method infos for RPC C++ wrappers
  //
#define RPC_REFLECTION_BEGIN
#define  RPC_DEF_HANDLER0(ReturnType) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG0(ReturnType, Call), GEN_SIG0(To_TS_TYPE(ReturnType), Call_esU16), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG1(ReturnType, Call, Param0Type), GEN_SIG1(To_TS_TYPE(ReturnType), Call_esU16, Param0Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG2(ReturnType, Call, Param0Type, Param1Type), GEN_SIG2(To_TS_TYPE(ReturnType), Call_esU16, Param0Type, Param1Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG3(ReturnType, Call, Param0Type, Param1Type, Param2Type), GEN_SIG3(To_TS_TYPE(ReturnType), Call_esU16, Param0Type, Param1Type, Param2Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG4(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type), GEN_SIG4(To_TS_TYPE(ReturnType), Call_esU16, Param0Type, Param1Type, Param2Type, Param3Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG5(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type), GEN_SIG5(To_TS_TYPE(ReturnType), Call_esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)
#define  RPC_DEF_HANDLER0_NORETURN() \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, VOID_Call, GEN_SIG0(void, Call_esU16), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG0(VOID_Call, Param0Type), GEN_SIG1(void, Call_esU16, Param0Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG1(VOID_Call, Param0Type, Param1Type), GEN_SIG2(void, Call_esU16, Param0Type, Param1Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG2(VOID_Call, Param0Type, Param1Type, Param2Type), GEN_SIG3(void, Call_esU16, Param0Type, Param1Type, Param2Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG3(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type), GEN_SIG4(void, Call_esU16, Param0Type, Param1Type, Param2Type, Param3Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsRpcMaster, EsRpcMasterIntf, GEN_SIG4(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type), GEN_SIG5(void, Call_esU16, Param0Type, Param1Type, Param2Type, Param3Type, Param3Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)
#define RPC_REFLECTION_END
#include <esfwxe/rpcMap/rpcReflection.cc>
  // Reflected class services
  ES_DECL_REFLECTED_CTOR_INFO(EsRpcMaster, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

// internal EsStdIoStateProxy implementation
//
class EsStdIoStateProxy : public EseStdIoState
{
public:
  EsStdIoStateProxy(EsRpcMaster& master) :
  m_master(master)
  {
    // set-up std rpc master state structure
    ekosfStdMasterIoInit(this, &m_master.m_cChnl);
  }
  
  EsBinBuffer frameGet() const
  {
    size_t endOffs = esMin(ProtocolEkosfHdr_SZE+hdr.dataLen, ESE_RPC_FRAME_SIZE);
    return EsBinBuffer(data, data+endOffs);
  }

  EsRpcMaster& m_master;
};

// Internal protocol monitor proxy implementations
//
class EsProtocolIoMonitorProxy : public EseProtocolIoMonitor
{
public:
  EsProtocolIoMonitorProxy(EsRpcMaster& owner) :
  m_owner(owner)
  {
    ekosfProtocolMonitorInit(this);
    // set-up protocol monitor interface
    protocolFrameSent = &EsProtocolIoMonitorProxy::cProtocolFrameSent;
    protocolFrameSendingFailed = &EsProtocolIoMonitorProxy::cProtocolFrameSendingFailed;
    protocolFrameReceived = &EsProtocolIoMonitorProxy::cProtocolFrameReceived;
    protocolFrameReceptionFailed = &EsProtocolIoMonitorProxy::cProtocolFrameReceptionFailed;    
  }

protected:
  static void cProtocolFrameSent(const EseStdIoState* io)
  {
    ES_ASSERT(io);
    EsProtocolIoMonitorProxy* This = reinterpret_cast<EsProtocolIoMonitorProxy*>(io->mon);
    EsRpcMaster* master = &This->m_owner;
    EsStdIoStateProxy* ioProxy = master->m_ioStateProxy;
    ES_ASSERT(ioProxy);

    ES_DUAL_CALL2_NR(master->m_monitor, EsProtocolIoMonitorIntf, protocolFrameSent, master->idGet(), ioProxy->frameGet());
  }

  static void cProtocolFrameSendingFailed(const EseStdIoState* io)
  {
    ES_ASSERT(io);
    EsProtocolIoMonitorProxy* This = reinterpret_cast<EsProtocolIoMonitorProxy*>(io->mon);
    EsRpcMaster* master = &This->m_owner;
    EsStdIoStateProxy* ioProxy = master->m_ioStateProxy;
    ES_ASSERT(ioProxy);

    EsString error;
    if(master->m_monitor && ioProxy->err)
      error = EsRpcMaster::getProtocolErrorString(ioProxy->err);

    ES_DUAL_CALL2_NR(master->m_monitor, EsProtocolIoMonitorIntf, protocolFrameSendingFailed, master->idGet(), error);
  }

  static void cProtocolFrameReceived(const EseStdIoState* io)
  {
    ES_ASSERT(io);
    EsProtocolIoMonitorProxy* This = reinterpret_cast<EsProtocolIoMonitorProxy*>(io->mon);
    EsRpcMaster* master = &This->m_owner;
    EsStdIoStateProxy* ioProxy = master->m_ioStateProxy;
    ES_ASSERT(ioProxy);

    ES_DUAL_CALL2_NR(master->m_monitor, EsProtocolIoMonitorIntf, protocolFrameReceived, master->idGet(), ioProxy->frameGet());
  }

  static void cProtocolFrameReceptionFailed(const EseStdIoState* io)
  {
    ES_ASSERT(io);
    EsProtocolIoMonitorProxy* This = reinterpret_cast<EsProtocolIoMonitorProxy*>(io->mon);
    EsRpcMaster* master = &This->m_owner;
    EsStdIoStateProxy* ioProxy = master->m_ioStateProxy;
    ES_ASSERT(ioProxy);

    EsString error;
    if(master->m_monitor && ioProxy->err)
      error = EsRpcMaster::getProtocolErrorString(ioProxy->err);

    ES_DUAL_CALL2_NR(master->m_monitor, EsProtocolIoMonitorIntf, protocolFrameReceptionFailed, master->idGet(), error);
  }

protected:
  EsRpcMaster& m_owner;
};

// rpc master methods
EsRpcMaster::EsRpcMaster() :
m_ioStateProxy(nullptr),
m_monitorProxy(nullptr)
{
  m_ioStateProxy = new EsStdIoStateProxy(*this);
  m_monitorProxy = new EsProtocolIoMonitorProxy(*this);
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}

EsRpcMasterIntf::Ptr EsRpcMaster::create()
{
  std::unique_ptr<EsRpcMaster> master( new EsRpcMaster );
  ES_ASSERT( master.get() );
  master->m_dynamic = true;

  return master.release()->asBaseIntfPtr();
}

EsRpcMaster::~EsRpcMaster()
{
  deactivate();

  ES_DELETE(m_monitorProxy);
  ES_DELETE(m_ioStateProxy);
}

// reflected property accessors
//
EsString EsRpcMaster::get_id() const
{
  return idGet();
}

// clientAddr
ulong EsRpcMaster::get_clientAddr() const
{
  return clientAddrGet();
}

void EsRpcMaster::set_clientAddr(const ulong& addr)
{
  clientAddrSet(addr);
}

// packetTimeout
ulong EsRpcMaster::get_packetTimeout() const 
{ 
  return packetTimeoutGet();
}

void EsRpcMaster::set_packetTimeout(const ulong& tmo) 
{ 
  packetTimeoutSet(tmo);
}

// channel
EsBaseIntfPtr EsRpcMaster::get_channel() const 
{ 
  return channelGet();
}

void EsRpcMaster::set_channel(const EsBaseIntfPtr& val) 
{ 
  channelSet(val);
}

void EsRpcMaster::set_monitor(const EsBaseIntfPtr& val) 
{ 
  monitorSet(val);
}

// interface methods
ulong EsRpcMaster::clientAddrGet() const 
{ 
  EsCriticalSectionLocker csLock(m_cs);
  ES_ASSERT(m_ioStateProxy);
  return m_ioStateProxy->addr; 
}

void EsRpcMaster::clientAddrSet(ulong addr) 
{ 
  EsCriticalSectionLocker csLock(m_cs);
  ES_ASSERT(m_ioStateProxy);
  m_ioStateProxy->addr = static_cast<esU8>(addr); 
}

// packetTimeout
ulong EsRpcMaster::packetTimeoutGet() const 
{ 
  EsCriticalSectionLocker csLock(m_cs);
  ES_ASSERT(m_ioStateProxy);
  return m_ioStateProxy->packetTmo; 
}

void EsRpcMaster::packetTimeoutSet(ulong tmo) 
{ 
  EsCriticalSectionLocker csLock(m_cs);
  ES_ASSERT(m_ioStateProxy);
  m_ioStateProxy->packetTmo = static_cast<esU32>(tmo); 
}

// channel
EsChannelIoIntf::Ptr EsRpcMaster::channelGet() const 
{ 
  EsCriticalSectionLocker csLock(m_cs);
  return m_chnl;
}

void EsRpcMaster::channelSet(const EsChannelIoIntf::Ptr& val) 
{ 
  EsCriticalSectionLocker csLock(m_cs);
  if( m_chnl != val )
  {
    // close our existing channel before changing object
    if( m_chnl )
      m_chnl->close();
      
    m_chnl = val;
    // set-up embedded code - compatible channel
    if( m_chnl )
      m_chnl->commChannelInit(&m_cChnl);
  }
}

// io rate
ulong EsRpcMaster::currentRateGet() const 
{
  checkChannel();
  EsRateCtlIntf::Ptr rateCtl = m_chnl.request<EsRateCtlIntf>();
  if( !rateCtl )
    EsException::Throw(
      ERR_RATE_CTL_IS_NOT_SUPPORTED_IN_S1,
      m_chnl->classNameGet()
    );

  return rateCtl->rateGet();  
}

// channel check
bool EsRpcMaster::hasChannel() const 
{ 
  return static_cast<bool>(m_chnl); 
} 

bool EsRpcMaster::rateSet(ulong val, bool doThrow)
{
  checkChannel();
  checkChannelActive();
  EsRateCtlIntf::Ptr rateCtl = m_chnl;
  if( !rateCtl )
  {
    if( doThrow )
      EsException::Throw( ERR_RATE_CTL_IS_NOT_SUPPORTED_IN_S1,
        m_chnl->classNameGet() );
        
    return false;
  }
  if( !ekosfSlaveRateSet(m_ioStateProxy, val) )
  {
    if( doThrow )
      EsException::Throw( _("Set rate query failed"));
    
    return false;
  }
  
  return true;
}

// protocol monitor assignment
ES_IMPL_INTF_METHOD(void, EsRpcMaster::monitorSet)(const EsBaseIntf::Ptr& mon)
{
  ES_ASSERT(m_ioStateProxy);
  EsCriticalSectionLocker csLock(m_cs);
  m_monitor = mon;
  if( m_monitor )
    m_ioStateProxy->mon = m_monitorProxy;
  else
    m_ioStateProxy->mon = 0;
}

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsRpcMaster::monitorGet)() const
{
  EsCriticalSectionLocker csLock(m_cs);
  return m_monitor;
}

// unique protocol identification string
ES_IMPL_INTF_METHOD(EsString, EsRpcMaster::idGet)() const
{
  return EsString::format(
    esT("%s-%p"),
    typeNameGet(),
    EsVariant(
      const_cast<EsRpcMaster*>(this),
      EsVariant::ACCEPT_POINTER
    )
  );
}

// lock|unlock
ES_IMPL_INTF_METHOD(bool, EsRpcMaster::tryLock)()
{
  return m_cs.tryEnter();
}

ES_IMPL_INTF_METHOD(void, EsRpcMaster::lock)()
{
  m_cs.enter();
}

ES_IMPL_INTF_METHOD(void, EsRpcMaster::unlock)()
{
  m_cs.leave();
}

// channel manipulation
//
bool EsRpcMaster::isActive() const 
{ 
  return m_chnl && 
    m_chnl->isOpen(); 
}

void EsRpcMaster::activate() 
{ 
  checkChannel(); 
  m_chnl->open(); 
  EsThread::sleep(activationDelay); 
}

void EsRpcMaster::deactivate()
{ 
  if( m_chnl ) 
    m_chnl->close(); 
}

// perform channel validity checks
void EsRpcMaster::checkChannel() const
{
  if( !hasChannel() )
    EsException::Throw( _("Channel is not set."));
}

void EsRpcMaster::checkChannelActive() const
{
  if( !isActive() )
    EsException::Throw( _("RPC Server is inactive."));
}

// standard RPC wrappers
//
bool EsRpcMaster::ping()
{
  EsCriticalSectionLocker csLock(m_cs);
  checkChannel();
  checkChannelActive();

  ES_ASSERT(m_ioStateProxy);
  bool result = ekosfPingSlave(m_ioStateProxy) == TRUE;
  if( !result )
    m_chnl->reset();
    
  return result;
}

bool EsRpcMaster::changeAddr(ulong newAddr)
{
  EsNumericCheck::checkRangeInteger(0, 254, newAddr, _("Slave address"));
  EsCriticalSectionLocker csLock(m_cs);
  checkChannel();
  checkChannelActive();
  ES_ASSERT(m_ioStateProxy);

  bool result = ekosfSlaveAddrSet(m_ioStateProxy, (esU8)newAddr) == TRUE;

  if( !result )
    m_chnl->reset();
    
  return result;
}

EsVariant EsRpcMaster::getImplementedRPCs()
{
  EsVariant::Array result;

  EsBinBuffer ba = esBA_Call( RPID_STD_CAPS_GET );
  ES_ASSERT( 0 == (ba.size() % 2) );

  result.reserve( ba.size() / 2 );

  esU16* idGet = reinterpret_cast<esU16*>(&ba[0]);
  esU16* end = reinterpret_cast<esU16*>(&ba[0] + ba.size());
  while( idGet < end )
  {
    result.push_back( *idGet );
    ++idGet;
  }

  return result;
}

ulong EsRpcMaster::getHealthInfo()
{
  return esU32_Call( RPID_STD_HEALTH_INFO_GET );
}

void EsRpcMaster::queueHealthCheck()
{
  VOID_Call( RPID_STD_HEALTH_EXEC );
}

esDT EsRpcMaster::getDateTime()
{
  return esDT_Call( RPID_STD_DATETIME_GET );
}

bool EsRpcMaster::setDateTime(esDT dt)
{
  return esBL_Call_esDT( RPID_STD_DATETIME_SET, dt ) == TRUE;
}

void EsRpcMaster::factoryReset()
{
  VOID_Call( RPID_STD_FACTORY_RESET );
}

void EsRpcMaster::shutdown()
{
  VOID_Call( RPID_STD_SHUTDOWN );
}

void EsRpcMaster::queueBootloader( ulong burnTmo)
{
  VOID_Call_esU32( RPID_STD_ENTER_BOOT, burnTmo );
}

// advanced communication services
//
// ping with optional channel settings tune-up (advanced ping)
// optional EsThread::Worker pointer may be used in threaded execution to check for 
// thread termination request
//
bool EsRpcMaster::pingEx()
{
  // first - try to ping device with current channel settings
  bool result = ping();
  
  if( !result && 
      isActive() )
  {
    EsRateCtlIntf::Ptr rateCtl = m_chnl;
    if( rateCtl )
    {
      // preserve current rate (if all ping tries failed - restore current rate)
      long savedRate = rateCtl->rateGet();
      bool proceed = true;
      // prepare rate sweep
      const EsVariant& rates = rateCtl->supportedRatesGet();
      long rate = rateCtl->defaultRateGet();
      // find index of the default rate item in rates collection
      ulong defIdx = 0;
      EsBaseIntf::Ptr brk = m_chnl->breakerGet();
      for(ulong idx = 0; idx < rates.countGet(); ++idx )
        if( rates.itemGet(idx).asLong() == rate )
        {
          defIdx = idx;
          break;
        }

      // ping from default and up
      for( ulong idx = defIdx; idx < rates.countGet() && !result;  ++idx )
      {
        EsVariant breaking = false;
        ES_DUAL_CALL0(breaking, brk, EsBreakIntf, isBreaking);
        if( breaking.asBool() )
        {
          proceed = false;
          break;
        }
        rate = rates.itemGet(idx).asLong();
        deactivate();
        rateCtl->rateSet(rate);
        activate();

        result = ping();
      }

      if( proceed )
      {
        // ping from pre-default and down
        for( ulong idx = defIdx-1; idx >= 0 && !result;  --idx )
        {
          EsVariant breaking = false;
          ES_DUAL_CALL0(breaking, brk, EsBreakIntf, isBreaking);

          if( breaking.asBool()  )
            break;

          rate = rates.itemGet(idx).asLong();
          deactivate();
          rateCtl->rateSet(rate);
          activate();

          result = ping();
        }
      }

      // if result is still false - restore saved rate
      if( !result )
      {
        deactivate();
        rateCtl->rateSet(savedRate);
      }
    }
    
    // finally activate services again
    activate();  
  }

  return result;
}

// auto-generate C++ wrappers around reflected C RPC
//
#define RPC_REFLECTION_BEGIN
#define  RPC_DEF_HANDLER0(ReturnType) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call(esU16 rpcId) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc(m_ioStateProxy, rpcId, &ret ); \
  checkThrowRpcStatus(ReturnType ## _RpcSig, rpcId, stat); return ret; }
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call_ ## Param0Type (esU16 rpcId, Param0Type p0) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type (m_ioStateProxy, rpcId, &ret, p0 ); \
  checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type, rpcId, stat); return ret; }
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type (esU16 rpcId, Param0Type p0, Param1Type p1) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type (m_ioStateProxy, rpcId, &ret, p0, p1); \
  checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type, rpcId, stat); return ret; }
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type (m_ioStateProxy, rpcId, &ret, p0, p1, p2); \
  checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type, rpcId, stat); return ret; }  
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type (m_ioStateProxy, rpcId, &ret, p0, p1, p2, p3); \
  checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type, rpcId, stat); return ret; }  
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type (m_ioStateProxy, rpcId, &ret, p0, p1, p2, p3, p4); \
  checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type, rpcId, stat); return ret; }  
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  To_TS_TYPE(ReturnType) EsRpcMaster:: ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5) { ReturnType ret; checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type (m_ioStateProxy, rpcId, &ret, p0, p1, p2, p3, p4, p5); \
  checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type, rpcId, stat); return ret; }  
#define  RPC_DEF_HANDLER0_NORETURN() \
  void EsRpcMaster::VOID_Call(esU16 rpcId) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc(m_ioStateProxy, rpcId); \
  checkThrowRpcStatus(VOID_RpcSig, rpcId, stat); }
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
  void EsRpcMaster::VOID_Call_## Param0Type (esU16 rpcId, Param0Type p0) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc_## Param0Type (m_ioStateProxy, rpcId, p0); \
  checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type, rpcId, stat); }
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
  void EsRpcMaster::VOID_Call_## Param0Type ## _ ## Param1Type (esU16 rpcId, Param0Type p0, Param1Type p1) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc_## Param0Type ## _ ## Param1Type (m_ioStateProxy, rpcId, p0, p1); \
  checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type, rpcId, stat); }
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
  void EsRpcMaster::VOID_Call_## Param0Type ## _ ## Param1Type ## _ ## Param2Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc_## Param0Type ## _ ## Param1Type ## _ ## Param2Type (m_ioStateProxy, rpcId, p0, p1, p2); \
  checkThrowRpcStatus(VOID_RpcSig_## Param0Type ## _ ## Param1Type ## _ ## Param2Type, rpcId, stat); }
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
  void EsRpcMaster::VOID_Call_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type (m_ioStateProxy, rpcId, p0, p1, p2, p3); \
  checkThrowRpcStatus(VOID_RpcSig_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type, rpcId, stat); }
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  void EsRpcMaster::VOID_Call_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type (m_ioStateProxy, rpcId, p0, p1, p2, p3, p4); \
  checkThrowRpcStatus(VOID_RpcSig_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type, rpcId, stat); }
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  void EsRpcMaster::VOID_Call_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type (esU16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5) { checkChannel(); checkChannelActive(); EsCriticalSectionLocker csLock(m_cs); \
  RpcStatus stat = VOID_Rpc_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type (m_ioStateProxy, rpcId, p0, p1, p2, p3, p4, p5); \
  checkThrowRpcStatus(VOID_RpcSig_## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type, rpcId, stat); }
#define RPC_REFLECTION_END

#include <esfwxe/rpcMap/rpcReflection.cc>

// return string representation for rpc signature
EsString EsRpcMaster::getRpcSignatureString(RpcReflectionSignature sig)
{
#define RPC_REFLECTION_BEGIN \
  EsString msg; \
  switch(sig) { 
#define  RPC_DEF_HANDLER0(RetutnType) \
  case RetutnType ## _RpcSig: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig); \
    break;
#define RPC_DEF_HANDLER1(RetutnType, Param0Type) \
  case RetutnType ## _RpcSig_ ## Param0Type: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig_ ## Param0Type); \
    break;
#define RPC_DEF_HANDLER2(RetutnType, Param0Type, Param1Type) \
  case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type); \
    break;
#define RPC_DEF_HANDLER3(RetutnType, Param0Type, Param1Type, Param2Type) \
  case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type); \
    break;
#define RPC_DEF_HANDLER4(RetutnType, Param0Type, Param1Type, Param2Type, Param3Type) \
  case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type); \
    break;
#define RPC_DEF_HANDLER5(RetutnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type); \
    break;
#define RPC_DEF_HANDLER6(RetutnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type: \
    msg = ES_STRINGIZE(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type); \
    break;
#define  RPC_DEF_HANDLER0_NORETURN() \
  case VOID_RpcSig: \
    msg = esT("VOID_RpcSig"); \
    break;
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
  case VOID_RpcSig_ ## Param0Type: \
    msg = ES_STRINGIZE(VOID_RpcSig_ ## Param0Type); \
    break;
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
  case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type: \
    msg = ES_STRINGIZE(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type); \
    break;
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
  case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type: \
    msg = ES_STRINGIZE(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type); \
    break;
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
  case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type: \
    msg = ES_STRINGIZE(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type); \
    break;
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
  case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type: \
    msg = ES_STRINGIZE(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type); \
    break;
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
  case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type: \
    msg = ES_STRINGIZE(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type); \
    break;
#define RPC_REFLECTION_END \
  default: \
    msg = _("Unknown signature"); \
    break; \
  } return msg;

#include <esfwxe/rpcMap/rpcReflection.cc>
}

EsString EsRpcMaster::getRpcStatusString(RpcStatus stat)
{
  EsString msg;
  switch( stat )
  {
  case RpcNotImplemented:                // rpc with this idGet is not implemented
    msg = _("RPC with this idGet is not implemented in client");
    break;
  case RpcStackCorrupt:                  // rpc stack is corrupt and unusable
    msg = _("RPC stack is corrupt and unusable");
    break;
  case RpcTimedOut:                      // RPC execution was prematurely cancelled due to timeout
    msg = _("RPC execution was prematurely cancelled due to timeout");
    break;
  case RpcStackOverflow:                // too many parameters, rpc stack overflown
    msg = _("RPC has to many parameters, breaking call to prevent stack overflow");
    break;
  case RpcUnknownSignature:              // RPC signature is unknown
    msg = _("RPC signature is unknown");
    break;
  case RpcSignatureMismatch:            // RPC signature of caller and callee do not match each other
    msg = _("RPC signatures of caller and callee do not match");
    break;
  case RpcResponseIdMismatch:            // RPC procedure idGet of response does not match one of request 
    msg = _("RPC idGet of response does not match one of request");
    break;
  case RpcCommunicationError:            // RPC failed due to communication layer
    msg = _("RPC failed due to communication layer");
    break;
  case RpcNotReady:
    msg = _("RPC not ready to respond (requested data not ready and|or device is busy)");
    break;
  case RpcParam0ValueError:
  case RpcParam1ValueError:
  case RpcParam2ValueError:
  case RpcParam3ValueError:
  case RpcParam4ValueError:
  case RpcParam5ValueError:
    msg = EsString::format(_("RPC parameter %d value error"), static_cast<int>(stat - RpcParam0ValueError));
    break;
  case RpcOK:
    msg = _("RPC succeeded");
    break;
  case RpcCancelled:
    msg = _("RPC execution was cancelled");
    break;  
  default:
    msg = _("Unknown RPC status");
  }

  return msg;
}

// return string for communication error 
EsString EsRpcMaster::getProtocolErrorString(int err)
{
  EsString msg;
  switch( err )
  {
  case ES_ERR_DATA_SIZE:
    msg = _("Data size in header does not equal to real data size; data packet may be incomplete.");
    break;
  case ES_ERR_DATA_MISALIGN:
    msg = _("Wrong data size alignment (not multiple of 2).");
    break;
  case ES_ERR_DATA_CRC:
    msg = _("CRC data error.");
    break;
  case ES_ERR_CMD_UNKNOWN:
    msg = _("Unknown command.");
    break;
  case ES_ERR_DATA_TOO_LONG:
    msg = _("Pending data packet is too long.");
    break;
  case ES_ERR_DATA_SIZE_UNEXPECTED:
    msg = _("Actual data size is unexpected for this command.");
    break;
  case ES_ERR_WRONG_RATE:
    msg = _("Requested rate value is not supported.");
    break;
  case ES_ERR_WRONG_ADDR:
    msg = _("Requested device address is out of range.");
    break;
  case ES_ERR_NOTREADY:
    msg = _("Device is not ready.");
    break;
  default:
    msg = EsString::format(_("Unknown error %d."), err);
    break;
  }
  
  return msg;
}

// check Rpc completion status. throw EsRpcException if it's not ok
void EsRpcMaster::checkThrowRpcStatus(RpcReflectionSignature sig, int rpcId, RpcStatus stat) 
{ 
  EsRpcException::Throw(sig, rpcId, stat, m_ioStateProxy->err); 
}

#endif // ES_COMM_USE_RPC

