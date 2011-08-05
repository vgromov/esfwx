#include "stdafx.h"
#pragma hdrstop

#include <common/remoteProcCall.h>
#include <common/protocols/proprietary/protocolEkosf.h>

#include "EkosfRPCServices.h"

#define WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2	_("Wrong data size. Expected %d bytes, %d bytes received.")
#define ERR_RATE_CTL_IS_NOT_SUPPORTED_IN_S1			_("Rate control functionality is not supported in '%s'")

// ----------------------------------------------------------------------
// rpc exception implementation
//
EsRpcException::EsRpcException(RpcReflectionSignature sig, int rpcId, RpcStatus stat, int ioErr /*= 0*/) : 
EsException( stat, EsException::Generic, EsString::format( _("RPC with sig='%s' and id='%d' failed with error: '%s'"), 
						EsRpcMaster::getRpcSignatureString(sig).c_str(), rpcId,
						EsRpcMaster::getRpcStatusString(stat).c_str()) ),
m_sig(sig),
m_rpcId(rpcId)
{
	if( ioErr )
		m_msg += EsString::format(_("\nDetails: '%s'"), EsRpcMaster::getCommunicationErrorString(ioErr).c_str());
}

void EsRpcException::Throw(RpcReflectionSignature sig, int rpcId, RpcStatus stat, int ioErr /*= 0*/)
{
	if( stat != RpcOK )
	{
		EsRpcException ex(sig, rpcId, stat, ioErr);
		EsUtilities::logError(ex.getMsg());
				
		throw ex;
	}
}

//----------------------------------------------------------------------------------
// std server wait implementation
extern "C" {

	void ekosfWait(DWORD tmo)
	{
		wxThread::Sleep(tmo);
	}

}

// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsRpcMaster, wxT("RPC master services over Ekosfera protocol."))
	// reflected properties
	ES_DECL_PROP_INFO_RANGE(EsRpcMaster, clientAddr, ulong, _i("Client address"), ekosfBroadcastPeerAddr, 0, ekosfBroadcastPeerAddr)
	ES_DECL_PROP_INFO_RANGE(EsRpcMaster, packetTimeout, ulong, _i("Packet timeout, ms"), ekosfDefPacketTmo, 0, 60000)
	ES_DECL_PROP_INFO_RANGE(EsRpcMaster, packetRetries, ulong, _i("Packet retries"), ekosfDefMasterPacketRetries, 1, 10)
	ES_DECL_PROP_INFO_RO(EsRpcMaster, currentRate, ulong, _i("Current I/O rate"))
	ES_DECL_PROP_INFO_RO(EsRpcMaster, hasChannel, bool, wxT("Is channel object set")) 
	// reflected services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, setChannel, void_Call_cr_EsVariant, wxT("Set communication channel object"))
	ES_DECL_REFLECTED_METHOD_INFO(EsRpcMaster, getChannelReflected, getChannel, EsVariant_CallConst, wxT("Return currently set channel object"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, hasChannel, bool_CallConst_cr_EsString, wxT("Return true if currently set channel class name matches argument"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, isActive, bool_CallConst, wxT("Return true if RPC server's channel is active"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, activate, void_Call, wxT("Activate RPC communication"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, deactivate, void_Call, wxT("Deactivate RPC communication"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, setChannelFromConfig, void_Call_cr_EsString_cr_EsString_p_wxEvtHandler, wxT("Set RPC communication channel from configuration"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, ping, bool_Call_EsBaseIntfPtr, wxT("Peer device ping"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, pingEx, bool_Call_EsBaseIntfPtr, wxT("Extended peer ping service"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, changeAddr, bool_Call_EsBaseIntfPtr_ulong, wxT("Peer device address set-up"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, enumerateSlaves, EsVariant_Call_EsBaseIntfPtr_ulong_ulong, wxT("Enumerate slave devices available on the network"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, setRate, bool_Call_EsBaseIntfPtr_ulong_bool, wxT("Set communication rate, if underlying channel supports it"))	
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, getImplementedRPCs, EsVariant_Call_EsBaseIntfPtr, wxT("Return ulong collection of RPC id's implemented on client side"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, getHealthInfo, ulong_Call_EsBaseIntfPtr, wxT("Return client device health info mask"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, queueHealthCheck, void_Call_EsBaseIntfPtr, wxT("Queue client device health check"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, getDateTime, EsVariant_Call_EsBaseIntfPtr, wxT("Return device's date and time"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, setDateTime, bool_Call_EsBaseIntfPtr_cr_EsVariant, wxT("Set device's date and time"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, factoryReset, void_Call_EsBaseIntfPtr, wxT("Initiate device's factory reset"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, shutdown, void_Call_EsBaseIntfPtr, wxT("Initiate device's shutdown"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, queueBootloader, void_Call_EsBaseIntfPtr_ulong, wxT("Request device to enter bootloader mode for specified time"))
	ES_DECL_REFLECTED_METHOD_INFO(EsRpcMaster, getFwIDReflected, getFwID, EsVariant_Call_EsBaseIntfPtr, wxT("Request short device ID"))
	ES_DECL_REFLECTED_METHOD_INFO(EsRpcMaster, getHubFwIDReflected, getHubFwID, EsVariant_Call_EsBaseIntfPtr, wxT("Request long device ID"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, getHwUID, EsVariant_Call_EsBaseIntfPtr, wxT("Request device hardware UID"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, getPowerStatus, EsVariant_Call_EsBaseIntfPtr, wxT("Return device power status"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, fileReadNext, EsBinBuffer_Call_EsBaseIntfPtr, wxT("Read next chunk of file data"))
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, getDeviceID, EsVariant_Call_EsBaseIntfPtr, wxT("Universal request for device ID"))
	// auto-generate RPC method infos for RPC C++ wrappers
	//
#define RPC_REFLECTION_BEGIN
#define	RPC_DEF_HANDLER0(ReturnType) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG0(ReturnType, Call), GEN_SIG0(To_TS_TYPE(ReturnType), Call_UINT16_EsBaseIntfPtr), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG1(ReturnType, Call, Param0Type), GEN_SIG1(To_TS_TYPE(ReturnType), Call_UINT16_EsBaseIntfPtr, Param0Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG2(ReturnType, Call, Param0Type, Param1Type), GEN_SIG2(To_TS_TYPE(ReturnType), Call_UINT16_EsBaseIntfPtr, Param0Type, Param1Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG3(ReturnType, Call, Param0Type, Param1Type, Param2Type), GEN_SIG3(To_TS_TYPE(ReturnType), Call_UINT16_EsBaseIntfPtr, Param0Type, Param1Type, Param2Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG4(ReturnType, Call, Param0Type, Param1Type, Param2Type, Param3Type), GEN_SIG4(To_TS_TYPE(ReturnType), Call_UINT16_EsBaseIntfPtr, Param0Type, Param1Type, Param2Type, Param3Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type)
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)
#define	RPC_DEF_HANDLER0_NORETURN() \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, VOID_Call, GEN_SIG0(void, Call_UINT16_EsBaseIntfPtr), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG0(VOID_Call, Param0Type), GEN_SIG1(void, Call_UINT16_EsBaseIntfPtr, Param0Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG1(VOID_Call, Param0Type, Param1Type), GEN_SIG2(void, Call_UINT16_EsBaseIntfPtr, Param0Type, Param1Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG2(VOID_Call, Param0Type, Param1Type, Param2Type), GEN_SIG3(void, Call_UINT16_EsBaseIntfPtr, Param0Type, Param1Type, Param2Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsRpcMaster, GEN_SIG3(VOID_Call, Param0Type, Param1Type, Param2Type, Param3Type), GEN_SIG4(void, Call_UINT16_EsBaseIntfPtr, Param0Type, Param1Type, Param2Type, Param3Type), NO_METHOD_DESCR)
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type)
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type)
#define RPC_REFLECTION_END

#include <common/rpcMap/rpcReflection.cc>

	// reflected class services
	ES_DECL_REFLECTED_CTOR_INFO(EsRpcMaster, EsBaseIntfPtr_ClassCall, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

#define CAST_IO_STATE reinterpret_cast<EkosfStdIoState*>(&m_ioState[0])
#define CAST_CONST_IO_STATE reinterpret_cast<const EkosfStdIoState*>(&m_ioState[0])

EsRpcMaster::EsRpcMaster() :
m_ioState(EkosfStdIoState_SZE, 0),
m_dynamic(false)
{
	// set-up std rpc master state structure
	ekosfStdMasterIoInit(CAST_IO_STATE, &m_chnl);
	
	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);	
}

EsRpcMaster::EsRpcMaster(objectDynamic) :
m_ioState(EkosfStdIoState_SZE, 0),
m_dynamic(true)
{
	// set-up std rpc master state structure
	ekosfStdMasterIoInit(CAST_IO_STATE, &m_chnl);

	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);
}

EsRpcMaster::~EsRpcMaster() 
{
	deactivate();
}

// channel manipulation
void EsRpcMaster::setChannel(const EsVariant& io)
{
	wxCriticalSectionLocker lock(m_cs);
	if( m_io != io.asObject() )
	{
		// close channel before changing object
		if( m_io )
			m_io->close();
			
		m_io = io.asObject();
		if( m_io )
			// set-up embedded code - compatible channel
			m_io->initCommChannel(&m_chnl);
	}
}

// try to set current channel from name & configuration string
void EsRpcMaster::setChannelFromConfig( const EsString& channelName, const EsString& config, wxEvtHandler* sink)
{
	wxASSERT( !isActive() );
	if( !hasChannel( channelName ) )
		setChannel( IoChannelFactory::createChannel(channelName, config, sink) );
	else
	{
		EsReflectedClassIntf::Ptr ref = m_io;
		if( ref )
			ref->setConfigString(config, true);
	}
}

// reflected property accessors
//
// clientAddr
ulong EsRpcMaster::get_clientAddr() const 
{ 
	wxCriticalSectionLocker csLock(m_cs);
	return CAST_CONST_IO_STATE->addr; 
}

void EsRpcMaster::set_clientAddr(const ulong& addr) 
{ 
	wxCriticalSectionLocker csLock(m_cs);
	CAST_IO_STATE->addr = static_cast<BYTE>(addr); 
}

// packetTimeout
ulong EsRpcMaster::get_packetTimeout() const 
{ 
	wxCriticalSectionLocker csLock(m_cs);
	return CAST_CONST_IO_STATE->packetTmo; 
}

void EsRpcMaster::set_packetTimeout(const ulong& tmo) 
{ 
	wxCriticalSectionLocker csLock(m_cs);
	CAST_IO_STATE->packetTmo = static_cast<DWORD>(tmo); 
}

// packetRetries
ulong EsRpcMaster::get_packetRetries() const 
{ 
	wxCriticalSectionLocker csLock(m_cs);
	return CAST_CONST_IO_STATE->retries; 
}

void EsRpcMaster::set_packetRetries(const ulong& val) 
{ 
	wxCriticalSectionLocker csLock(m_cs);
	CAST_IO_STATE->retries = static_cast<BYTE>(val); 
}

// io rate
ulong EsRpcMaster::get_currentRate() const 
{
	checkChannel();
	EsRateCtlIntf::Ptr rateCtl = m_io.request<EsRateCtlIntf>();
	if( !rateCtl )
		EsException::Throw(0, EsException::Generic, ERR_RATE_CTL_IS_NOT_SUPPORTED_IN_S1,
			m_io->getClassName().c_str() );

	return rateCtl->getRate();	
}

// channel check
bool EsRpcMaster::get_hasChannel() const 
{ 
	return m_io; 
} 

bool EsRpcMaster::setRate(EsBaseIntfPtr breaker, ulong val, bool doThrow)
{
	checkChannel();
	checkChannelActive();
	EsRateCtlIntf::Ptr rateCtl = m_io.request<EsRateCtlIntf>();
	if( !rateCtl )
	{
		if( doThrow )
			EsException::Throw(0, EsException::Generic, ERR_RATE_CTL_IS_NOT_SUPPORTED_IN_S1,
				m_io->getClassName().c_str() );
				
		return false;
	}
	EsCommChannelBreakImpl brk(breaker);
	if( !ekosfSlaveRateSet(CAST_IO_STATE, val, brk.getCommChannelBreakPtr()) )
	{
		if( doThrow )
			EsException::Throw(0, EsException::Generic, _("Set rate query failed"));
		
		return false;
	}
	
	return true;
}

// channel manipulation
//
bool EsRpcMaster::hasChannel(cr_EsString p1) const
{ 
	return m_io && 
		m_io->is(p1); 
}

bool EsRpcMaster::isActive() const 
{ 
	return m_io && 
		m_io->isOpen(); 
}

void EsRpcMaster::activate() 
{ 
	checkChannel(); 
	m_io->open(); 
	wxThread::Sleep(activationDelay); 
}

void EsRpcMaster::deactivate()
{ 
	if( m_io ) 
		m_io->close(); 
}

// perform channel validity checks
void EsRpcMaster::checkChannel() const
{
	if( !get_hasChannel() )
		EsException::Throw(0, EsException::Generic, _("Channel is not set."));
}

void EsRpcMaster::checkChannelActive() const
{
	if( !isActive() )
		EsException::Throw(0, EsException::Generic, _("RPC Server is inactive."));
}

// standard RPC wrappers
//
bool EsRpcMaster::ping(EsBaseIntfPtr breaker)
{
	wxCriticalSectionLocker csLock(m_cs);
	checkChannel();
	checkChannelActive();
	EsCommChannelBreakImpl brk(breaker);
	bool result = ekosfPingSlave(CAST_IO_STATE, brk.getCommChannelBreakPtr()) == TRUE;
	if( !result )
		m_io->reset();
		
	return result;
}

bool EsRpcMaster::changeAddr(EsBaseIntfPtr breaker, ulong newAddr)
{
	EsNumericCheck::checkRangeInteger(0, 254, newAddr, _("Slave address"));
	wxCriticalSectionLocker csLock(m_cs);
	checkChannel();
	checkChannelActive();
	EsCommChannelBreakImpl brk(breaker);
	bool result = ekosfSlaveAddrSet(CAST_IO_STATE, newAddr, brk.getCommChannelBreakPtr()) == TRUE;	
	if( !result )
		m_io->reset();
		
	return result;
}

// internal slave enumerator functor
struct EsEnumSlave
{
	EsEnumSlave(EsRpcMaster& master, const EsBaseIntfPtr& breaker) : 
	m_breaker(breaker), m_master(master), m_slaves(EsVariant::VAR_VARIANT_COLLECTION) 
	{}
	
	static BOOL onEnum(EkosfStdIoState* io, BYTE addr, void* data)
	{
		EsEnumSlave* This = reinterpret_cast<EsEnumSlave*>(data);
		wxASSERT(This);
		EsVariant id;
		try
		{
			id = This->m_master.getDeviceID(This->m_breaker);
			This->m_slaves.addToVariantCollection( EsVariant(EsVariant::VAR_VARIANT_COLLECTION).
				addToVariantCollection(addr).
				addToVariantCollection(id) );
		}
		catch(...)
		{
			return FALSE;
		}
		return TRUE;
	}
	
	const EsBaseIntfPtr& m_breaker;
	EsRpcMaster& m_master;
	EsVariant m_slaves;
};

EsVariant EsRpcMaster::enumerateSlaves(EsBaseIntfPtr breaker, ulong startAddr, ulong endAddr)
{
	if( startAddr > endAddr  )
		EsException::Throw(0, EsException::Generic, _("Start slave address value must not be greater than end address"));
	EsNumericCheck::checkRangeInteger(0, 254, startAddr, _("Slave address start value"));
	EsNumericCheck::checkRangeInteger(0, 254, endAddr, _("Slave address end value"));
	wxCriticalSectionLocker csLock(m_cs);
	checkChannel();
	checkChannelActive();

	// perform slave devices enumeration
	EsEnumSlave enumSlave(*this, breaker);
	EsCommChannelBreakImpl brk(breaker);
	ekosfSlaveEnumerate(CAST_IO_STATE, static_cast<BYTE>(startAddr), static_cast<BYTE>(endAddr), 
		&EsEnumSlave::onEnum, reinterpret_cast<void*>(&enumSlave), brk.getCommChannelBreakPtr() );
		
	return enumSlave.m_slaves;
}

EsVariant EsRpcMaster::getImplementedRPCs(EsBaseIntfPtr breaker)
{
	EsVariant result(EsVariant::VAR_VARIANT_COLLECTION);
	EsBinBuffer ba = BYTEARRAY_Call( RPID_STD_CAPS_GET, breaker );
	wxASSERT( 0 == (ba.size() % 2) );
	UINT16* id = reinterpret_cast<UINT16*>(&ba[0]);
	UINT16* end = reinterpret_cast<UINT16*>(&ba[0] + ba.size());
	while( id < end )
	{
		result.addToVariantCollection( *id );
		++id;
	}

	return result; 
}

ApplianceFirmwareID EsRpcMaster::getFwID(EsIoBreakIntf::Ptr breaker)
{
	ApplianceFirmwareID result;
	EsBinBuffer ba = BYTEARRAY_Call( RPID_STD_FWID_GET, breaker );
	if( ApplianceFirmwareID_SZE == ba.size() )
		memcpy(&result, &ba[0], ApplianceFirmwareID_SZE);
	else
	{
		if( HubFirmwareID_SZE == ba.size() )
			EsException::Throw(0, EsException::Generic, _("Connected device is hub, not appliance."));
		else
			EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2, 
				ApplianceFirmwareID_SZE, ba.size());
	}

	return result;
}

HubFirmwareID EsRpcMaster::getHubFwID(EsIoBreakIntf::Ptr breaker)
{
	HubFirmwareID result;

	EsBinBuffer ba = BYTEARRAY_Call( RPID_STD_FWID_GET, breaker );
	if( HubFirmwareID_SZE == ba.size() )
		memcpy(&result, &ba[0], HubFirmwareID_SZE);
	else
	{
		if( ApplianceFirmwareID_SZE == ba.size() )
			EsException::Throw(0, EsException::Generic, _("Connected device is appliance, not hub."));
		else
			EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2, 
				HubFirmwareID_SZE, ba.size());
	}
	
	return result;
}

EsVariant EsRpcMaster::getHwUID(EsBaseIntfPtr breaker)
{
	UID uid;
	EsBinBuffer ba = BYTEARRAY_Call( RPID_STD_HW_UID_GET, breaker );
	if( UID_SZE == ba.size() )
		memcpy(uid, &ba[0], UID_SZE);
	else
		EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2,
			UID_SZE, ba.size());
	
	return EsUID(uid);
}

ulong EsRpcMaster::getHealthInfo(EsBaseIntfPtr breaker)
{
	return UINT32_Call( RPID_STD_HEALTH_INFO_GET, breaker );
}

void EsRpcMaster::queueHealthCheck(EsBaseIntfPtr breaker)
{
	VOID_Call( RPID_STD_HEALTH_EXEC, breaker );
}

EsVariant EsRpcMaster::getDateTime(EsBaseIntfPtr breaker)
{
	return EsDateTime( DATETIME_Call( RPID_STD_DATETIME_GET, breaker ) );
}

bool EsRpcMaster::setDateTime(EsBaseIntfPtr breaker, cr_EsVariant dt)
{
	return BOOL_Call_DATETIME( RPID_STD_DATETIME_SET, breaker, EsDateTime(dt) ) == TRUE;
}

void EsRpcMaster::factoryReset(EsBaseIntfPtr breaker)
{
	VOID_Call( RPID_STD_FACTORY_RESET, breaker );
}

void EsRpcMaster::shutdown(EsBaseIntfPtr breaker)
{
	VOID_Call( RPID_STD_SHUTDOWN, breaker );
}

void EsRpcMaster::queueBootloader(EsBaseIntfPtr breaker, ulong burnTmo)
{
	VOID_Call_UINT32( RPID_STD_ENTER_BOOT, breaker, burnTmo );
}

EsVariant EsRpcMaster::getPowerStatus(EsBaseIntfPtr breaker)
{
	PowerStatus result;
	EsBinBuffer ba = BYTEARRAY_Call( RPID_STD_POWER_STATUS_GET, breaker );
	if( ba.size() == sizeof(PowerStatus) )
		memcpy(&result, &ba[0], PowerStatus_SZE);
	else
		EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2, 
			PowerStatus_SZE, ba.size());
			
	return EsPowerStatus(result);
}

FsResult EsRpcMaster::dirListStart(EsIoBreakIntf::Ptr breaker, const EsString& dir)
{
	// send zero-terminated buffer, including terminating 0
	wxString _dir(dir.c_str());
	wxCharBuffer ascii = _dir.ToAscii();
	EsBinBuffer buff( ascii.data(), ascii.data()+dir.size() );
	buff.push_back( 0 );
	return UINT16_Call_BYTEARRAY(RPID_STD_DIR_LIST_START, breaker, buff);
}

FsResult EsRpcMaster::dirListGetNextItem(EsIoBreakIntf::Ptr breaker, FsItem& item)
{
	FsItemResult result;
	memset(&result, 0, FsItemResult_SZE);
	EsBinBuffer ba = BYTEARRAY_Call(RPID_STD_DIR_LIST_NEXT_GET, breaker);
	if( ba.size() == FsItemResult_SZE || ba.size() == FsResult_SZE )
	{
		memcpy(&result, &ba[0], ba.size());
		item = result.item;
	}
	else
		EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2, 
			FsItemResult_SZE, ba.size());		
	 
	return result.result;
}

FsResult EsRpcMaster::dirDelete(EsIoBreakIntf::Ptr breaker, const EsString& dir, bool force/* = false*/)
{
	// send zero-terminated buffer, including terminating 0
	wxString _dir(dir.c_str());
	wxCharBuffer ascii = _dir.ToAscii();
	EsBinBuffer buff( ascii.data(), ascii.data()+dir.size() );
	buff.push_back( 0 );
	// delete specified directory
	return UINT16_Call_BYTEARRAY_BOOL(RPID_STD_DIR_DELETE, breaker, buff, force == true);
}

FsResult EsRpcMaster::fileDelete(EsIoBreakIntf::Ptr breaker, const EsString& file)
{
	// send zero-terminated buffer, including terminating 0
	wxString _file(file.c_str());
	wxCharBuffer ascii = _file.ToAscii();
	EsBinBuffer buff( ascii.data(), ascii.data()+file.size() );
	buff.push_back( 0 );
	// delete specified file
	return UINT16_Call_BYTEARRAY( RPID_STD_FILE_DELETE, breaker, buff);
}

FsResult EsRpcMaster::fileSysFreeSpaceInfoGet(EsIoBreakIntf::Ptr breaker, EsMemSpace& total, EsMemSpace& free)
{
	FsSpaceInfo result;
	EsBinBuffer ba = BYTEARRAY_Call(RPID_STD_FREESPACE_GET, breaker);
	if( FsSpaceInfo_SZE == ba.size() )
	{
		memcpy(&result, &ba[0], FsSpaceInfo_SZE);
		total = result.total;
		free = result.free;
	}
	else
		EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2, 
			FsSpaceInfo_SZE, ba.size());
		
	return result.result;
}

FsResult EsRpcMaster::fileReadStart(EsIoBreakIntf::Ptr breaker, const EsString& file, DWORD& chunks)
{
	FsFileReadRequest result;
	// send zero-terminated buffer, including terminating 0
	wxString _file(file.c_str());
	wxCharBuffer ascii = _file.ToAscii();
	EsBinBuffer buff( ascii.data(), ascii.data()+file.size() );
	buff.push_back( 0 );
	EsBinBuffer ba = BYTEARRAY_Call_BYTEARRAY( RPID_STD_FILE_READ_START, breaker, buff );
	if( FsFileReadRequest_SZE == ba.size() )
	{
		memcpy(&result, &ba[0], FsFileReadRequest_SZE);
		chunks = result.chunks;
	}
	else
		EsException::Throw(0, EsException::Generic, WRONG_DATA_SIZE_EXPECTED_D1_RECEIVED_D2, 
			FsFileReadRequest_SZE, ba.size());
	
	return result.result;
}

EsBinBuffer EsRpcMaster::fileReadNext(EsBaseIntfPtr breaker)
{
	return BYTEARRAY_Call( RPID_STD_FILE_READ_NEXT, breaker );	
}

// advanced communication services
//
// ping with optional channel settings tune-up (advanced ping)
// optional EsThread::Worker pointer may be used in threaded execution to check for 
// thread termination request
//
bool EsRpcMaster::pingEx(EsBaseIntfPtr breaker)
{
	// save retry count, set minimum retries
	ulong retries = get_packetRetries();
	set_packetRetries(1);
	// first - try to ping device with current channel settings
	bool result = ping(breaker);
	
	if( !result && 
			isActive() )
	{
		EsRateCtlIntf::Ptr rateCtl = m_io.request<EsRateCtlIntf>();
		if( rateCtl )
		{
			// preserve current rate (if all ping tries failed - restore current rate)
			long savedRate = rateCtl->getRate();
			bool proceed = true;
			// prepare rate sweep
			EsVariant rates = rateCtl->getSupportedRates();
			long rate = rateCtl->getDefaultRate();
			// find index of the default rate item in rates collection
			int defIdx = 0;
			EsIoBreakIntf::Ptr brk = breaker;
			for(int idx = 0; idx < rates.getCount(); ++idx )
				if( rates.getItem(idx).asLong() == rate )
				{
					defIdx = idx;
					break;
				}
				// ping from default and up
				for( int idx = defIdx; idx < rates.getCount() && !result;	++idx )
				{
					if(	brk && 
							brk->isBreaking())
					{
						proceed = false;
						break;
					}
					rate = rates.getItem(idx).asLong();
					deactivate();
					rateCtl->setRate(rate);
					activate();

					result = ping(brk);
				}

				if( proceed )
				{
					// ping from pre-default and down
					for( int idx = defIdx-1; idx >= 0 && !result;	--idx )
					{
						if(	brk && 
								brk->isBreaking())
							break;

						rate = rates.getItem(idx).asLong();
						deactivate();
						rateCtl->setRate(rate);
						activate();

						result = ping(brk);
					}
				}

				// if result is still false - restore saved rate
				if( !result )
				{
					deactivate();
					rateCtl->setRate(savedRate);
				}
		}
		
		// finally activate services again
		activate();	
	}
	// restore packet retries
	set_packetRetries(retries);			
	
	return result;
}

// universal device identification. returned is reflected device hardware id 
EsVariant EsRpcMaster::getDeviceID(EsBaseIntfPtr breaker)
{
	EsVariant result;
	
	EsBinBuffer ba = BYTEARRAY_Call( RPID_STD_FWID_GET, breaker );
	if( ApplianceFirmwareID_SZE == ba.size() )
	{
		ApplianceFirmwareID* id = reinterpret_cast<ApplianceFirmwareID*>(&ba[0]);
		result = EsShortHwId::create( *id );
	}
	else if( HubFirmwareID_SZE == ba.size() )
	{
		HubFirmwareID* id = reinterpret_cast<HubFirmwareID*>(&ba[0]);
		result = EsLongHwId::create( *id );	
	}
	else
		EsException::Throw(0, EsException::Generic, _("Wrong data size. Expected %d or %d bytes, got %d bytes"), 
			ApplianceFirmwareID_SZE, HubFirmwareID_SZE, ba.size());

	return result;
}

// auto-generate C++ wrappers around reflected C RPC
//
#define RPC_REFLECTION_BEGIN
#define	RPC_DEF_HANDLER0(ReturnType) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call(UINT16 rpcId, EsBaseIntfPtr breaker) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc(CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret ); \
	checkThrowRpcStatus(ReturnType ## _RpcSig, rpcId, stat); return ret; }
#define RPC_DEF_HANDLER1(ReturnType, Param0Type) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call_ ## Param0Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret, p0 ); \
	checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type, rpcId, stat); return ret; }
#define RPC_DEF_HANDLER2(ReturnType, Param0Type, Param1Type) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret, p0, p1); \
	checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type, rpcId, stat); return ret; }
#define RPC_DEF_HANDLER3(ReturnType, Param0Type, Param1Type, Param2Type) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret, p0, p1, p2); \
	checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type, rpcId, stat); return ret; }	
#define RPC_DEF_HANDLER4(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret, p0, p1, p2, p3); \
	checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type, rpcId, stat); return ret; }	
#define RPC_DEF_HANDLER5(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret, p0, p1, p2, p3, p4); \
	checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type, rpcId, stat); return ret; }	
#define RPC_DEF_HANDLER6(ReturnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
	To_TS_TYPE(ReturnType) EsRpcMaster:: ## ReturnType ## _Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5) { ReturnType ret; checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = ReturnType ## _Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), &ret, p0, p1, p2, p3, p4, p5); \
	checkThrowRpcStatus(ReturnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type, rpcId, stat); return ret; }	
#define	RPC_DEF_HANDLER0_NORETURN() \
	void EsRpcMaster::VOID_Call(UINT16 rpcId, EsBaseIntfPtr breaker) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc(CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr()); \
	checkThrowRpcStatus(VOID_RpcSig, rpcId, stat); }
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
	void EsRpcMaster::VOID_Call_ ## Param0Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc_ ## Param0Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), p0); \
	checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type, rpcId, stat); }
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
	void EsRpcMaster::VOID_Call_ ## Param0Type ## _ ## Param1Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc_ ## Param0Type ## _ ## Param1Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), p0, p1); \
	checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type, rpcId, stat); }
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
	void EsRpcMaster::VOID_Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type (UINT16 rpcId, Param0Type p0, Param1Type p1, Param2Type p2) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), p0, p1, p2); \
	checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type, rpcId, stat); }
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
	void EsRpcMaster::VOID_Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), p0, p1, p2, p3); \
	checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type, rpcId, stat); }
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
	void EsRpcMaster::VOID_Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), p0, p1, p2, p3, p4); \
	checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type, rpcId, stat); }
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
	void EsRpcMaster::VOID_Call_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type (UINT16 rpcId, EsBaseIntfPtr breaker, Param0Type p0, Param1Type p1, Param2Type p2, Param3Type p3, Param4Type p4, Param5Type p5) { checkChannel(); checkChannelActive(); wxCriticalSectionLocker csLock(m_cs); \
	EsCommChannelBreakImpl brk(breaker); \
	RpcStatus stat = VOID_Rpc_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type ## (CAST_IO_STATE, rpcId, brk.getCommChannelBreakPtr(), p0, p1, p2, p3, p4, p5); \
	checkThrowRpcStatus(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type, rpcId, stat); }
#define RPC_REFLECTION_END

#include <common/rpcMap/rpcReflection.cc>

// return string representation for rpc signature
EsString EsRpcMaster::getRpcSignatureString(RpcReflectionSignature sig)
{
#define RPC_REFLECTION_BEGIN \
	EsString msg; \
	switch(sig) { 
#define	RPC_DEF_HANDLER0(RetutnType) \
	case RetutnType ## _RpcSig: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig); \
		break;
#define RPC_DEF_HANDLER1(RetutnType, Param0Type) \
	case RetutnType ## _RpcSig_ ## Param0Type: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig_ ## Param0Type); \
		break;
#define RPC_DEF_HANDLER2(RetutnType, Param0Type, Param1Type) \
	case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type); \
		break;
#define RPC_DEF_HANDLER3(RetutnType, Param0Type, Param1Type, Param2Type) \
	case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type); \
		break;
#define RPC_DEF_HANDLER4(RetutnType, Param0Type, Param1Type, Param2Type, Param3Type) \
	case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type); \
		break;
#define RPC_DEF_HANDLER5(RetutnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
	case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type); \
		break;
#define RPC_DEF_HANDLER6(RetutnType, Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
	case RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type: \
		msg = wxSTRINGIZE_T(RetutnType ## _RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type); \
		break;
#define	RPC_DEF_HANDLER0_NORETURN() \
	case VOID_RpcSig: \
		msg = wxT("VOID_RpcSig"); \
		break;
#define RPC_DEF_HANDLER1_NORETURN(Param0Type) \
	case VOID_RpcSig_ ## Param0Type: \
		msg = wxSTRINGIZE_T(VOID_RpcSig_ ## Param0Type); \
		break;
#define RPC_DEF_HANDLER2_NORETURN(Param0Type, Param1Type) \
	case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type: \
		msg = wxSTRINGIZE_T(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type); \
		break;
#define RPC_DEF_HANDLER3_NORETURN(Param0Type, Param1Type, Param2Type) \
	case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type: \
		msg = wxSTRINGIZE_T(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type); \
		break;
#define RPC_DEF_HANDLER4_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type) \
	case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type: \
		msg = wxSTRINGIZE_T(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type); \
		break;
#define RPC_DEF_HANDLER5_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type) \
	case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type: \
		msg = wxSTRINGIZE_T(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type); \
		break;
#define RPC_DEF_HANDLER6_NORETURN(Param0Type, Param1Type, Param2Type, Param3Type, Param4Type, Param5Type) \
	case VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type: \
		msg = wxSTRINGIZE_T(VOID_RpcSig_ ## Param0Type ## _ ## Param1Type ## _ ## Param2Type ## _ ## Param3Type ## _ ## Param4Type ## _ ## Param5Type); \
		break;
#define RPC_REFLECTION_END \
	default: \
		msg = _("Unknown signature"); \
		break; \
	} return msg;

#include <common/rpcMap/rpcReflection.cc>
}

EsString EsRpcMaster::getRpcStatusString(RpcStatus stat)
{
	EsString msg;
	switch( stat )
	{
	case RpcNotImplemented:								// rpc with this id is not implemented
		msg = _("RPC with this id is not implemented in client");
		break;
	case RpcStackCorrupt:									// rpc stack is corrupt and unusable
		msg = _("RPC stack is corrupt and unusable");
		break;
	case RpcTimedOut:											// RPC execution was prematurely cancelled due to timeout
		msg = _("RPC execution was prematurely cancelled due to timeout");
		break;
	case RpcStackOverflow:								// too many parameters, rpc stack overflown
		msg = _("RPC has to many parameters, breaking call to prevent stack overflow");
		break;
	case RpcUnknownSignature:							// RPC signature is unknown
		msg = _("RPC signature is unknown");
		break;
	case RpcSignatureMismatch:						// RPC signature of caller and callee do not match each other
		msg = _("RPC signatures of caller and callee do not match");
		break;
	case RpcResponseIdMismatch:						// RPC procedure id of response does not match one of request 
		msg = _("RPC id of response does not match one of request");
		break;
	case RpcCommunicationError:						// RPC failed due to communication layer
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
EsString EsRpcMaster::getCommunicationErrorString(int err)
{
	EsString msg;
	switch( err )
	{
	case EKOSF_ERR_DATA_SIZE:
		msg = _("Data size in header does not equal to real data size; data packet may be incomplete.");
		break;
	case EKOSF_ERR_DATA_MISALIGN:
		msg = _("Wrong data size alignment (not multiple of 2).");
		break;
	case EKOSF_ERR_DATA_CRC:
		msg = _("CRC data error.");
		break;
	case EKOSF_ERR_CMD_UNKNOWN:
		msg = _("Unknown command.");
		break;
	case EKOSF_ERR_DATA_TOO_LONG:
		msg = _("Pending data packet is too long.");
		break;
	case EKOSF_ERR_DATA_SIZE_UNEXPECTED:
		msg = _("Actual data size is unexpected for this command.");
		break;
	case EKOSF_ERR_WRONG_RATE:
		msg = _("Requested rate value is not supported.");
		break;
	case EKOSF_ERR_WRONG_ADDR:
		msg = _("Requested device address is out of range.");
		break;
	case EKOSF_ERR_NOTREADY:
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
	EsRpcException::Throw(sig, rpcId, stat, CAST_IO_STATE->err); 
}