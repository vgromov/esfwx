#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoStub.h"

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef ES_COMM_USE_CHANNEL_STUB

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(EsChannelIoStub, _i("Stub channel"))
  // reflected property infos declarations
  //
  ES_DECL_PROP_INFO_WO(EsChannelIoStub, monitor, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  ES_DECL_PROP_INFO_WO(EsChannelIoStub, breaker, EsBaseIntfPtr, NO_PROPERTY_LABEL, NO_PROPERTY_DESCR)
  // reflected interface methods
  //
  // EsChannelIoIntf
  //
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, open, bool_Call, _i("Open stub channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, close, void_Call, _i("Close stub channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, isOpen, bool_CallConst, _i("Return true if stub channel is open"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, idGet, EsString_CallConst, _i("Get channel id string"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, bytesPut, ulong_Call_cr_EsBinBuffer_ulong, _i("Send bytes over a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, bytesGet, EsBinBuffer_Call_ulong_ulong, _i("Receive bytes from a channel"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, txTimeEstimateGet, ulong_CallConst_ulong, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, reset, void_Call, _i("Reset channel IO buffers"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, errorGet, long_CallConst, _i("Get channel-specific error code"))
  ES_DECL_REFLECTED_INTF_METHOD_INFO(EsChannelIoStub, EsChannelIoIntf, errorStringGet, EsString_CallConst, _i("Get channel-specific error string"))  
  // reflected class methods
  //
  ES_DECL_REFLECTED_CTOR_INFO(EsChannelIoStub, EsBaseIntfPtr_ClassCall, _i("EsChannelIoStub constructor"))
ES_DECL_CLASS_INFO_END

// -------------------------------------------------------------------------------------------------------
// communication class
//
EsChannelIoStub::EsChannelIoStub() :
m_open(false)
{
  // initialize properties to their default values
  ES_REFLECTED_PROPERTIES_RESET;
}

EsChannelIoStub::~EsChannelIoStub()
{
  close();
}

EsBaseIntfPtr EsChannelIoStub::NEW()
{
  std::unique_ptr<EsChannelIoStub> chnl = ES_MAKE_UNIQUE( EsChannelIoStub );
  ES_ASSERT(chnl);
  chnl->m_dynamic = true;

  return chnl.release()->asBaseIntfPtr();
}

void EsChannelIoStub::set_monitor(const EsBaseIntfPtr& monitor)
{
  monitorSet( monitor );
}

void EsChannelIoStub::set_breaker(const EsBaseIntfPtr& breaker)
{
  breakerSet( breaker );
}

// io breaker access
ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoStub::breakerGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_breaker;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoStub::breakerSet)(const EsBaseIntf::Ptr& breaker)
{
  EsMutexLocker lock(m_mx);
  m_breaker = breaker;
}

// internal services
//
bool EsChannelIoStub::internalIsBreaking() const
{
  EsVariant result = false;
  ES_DUAL_CALL0(result, m_breaker, EsBreakIntf, isBreaking);
  return result.asBool();
}

bool EsChannelIoStub::internalIsOpen() const
{
  return m_open;
}

EsString EsChannelIoStub::internalIdGet() const
{
  return EsString::format(
    esT("%s-%p"),
    typeNameGet(),
    EsVariant(
      const_cast<EsChannelIoStub*>(this),
      EsVariant::ACCEPT_POINTER
    )
  );
}

bool EsChannelIoStub::internalOpen()
{
  if( !internalIsOpen() )
  {
    ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelOpened, internalIdGet() );    
    m_open = true;
  }
  
  return m_open;
}

void EsChannelIoStub::internalClose()
{
  if( internalIsOpen() )
  {
    ES_DUAL_CALL1_NR(m_monitor, EsChannelIoMonitorIntf, channelClosed, internalIdGet());    
    m_open = true;
  }
}

esU32 EsChannelIoStub::internalPutBytes(const esU8* data, esU32 len, esU32 ES_UNUSED(tmo))
{
  if( internalIsOpen() )
  {
    if( len )
      ES_DUAL_CALL2_NR(m_monitor, EsChannelIoMonitorIntf, channelBytesReceived, internalIdGet(), EsBinBuffer(data, data+len));  
      
    return len;
  }
  
  return 0;
}

esU32 EsChannelIoStub::internalGetBytes(esU8* ES_UNUSED(data), esU32 ES_UNUSED(len), esU32 ES_UNUSED(tmo))
{
  EsThread::sleep(1);
  return 0;
}

void EsChannelIoStub::internalReset()
{
}

long EsChannelIoStub::internalGetError() const
{
  return 0;
}

// EsChannelIoIntf interface
//
ES_IMPL_INTF_METHOD(EsString, EsChannelIoStub::idGet)() const
{
  return internalIdGet();
}

ES_IMPL_INTF_METHOD(EsBaseIntf::Ptr, EsChannelIoStub::monitorGet)() const
{
  EsMutexLocker lock(m_mx);
  return m_monitor;
}

ES_IMPL_INTF_METHOD(void, EsChannelIoStub::monitorSet)(const EsBaseIntf::Ptr& monitor)
{
  EsMutexLocker lock(m_mx);
  m_monitor = monitor;
}

// state manipulation
ES_IMPL_INTF_METHOD(bool, EsChannelIoStub::open)()
{
  EsMutexLocker lock(m_mx);
  return internalOpen();
}

ES_IMPL_INTF_METHOD(void, EsChannelIoStub::close)()
{
  EsMutexLocker lock(m_mx);
  internalClose();
}

ES_IMPL_INTF_METHOD(bool, EsChannelIoStub::isOpen)() const
{
  EsMutexLocker lock(m_mx);
  return internalIsOpen();
}

// byte io
ES_IMPL_INTF_METHOD(esU32, EsChannelIoStub::bytesPut)(const esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  return internalPutBytes(data, len, tmo);
}

ES_IMPL_INTF_METHOD(esU32, EsChannelIoStub::bytesGet)(esU8* data, esU32 len, esU32 tmo)
{
  EsMutexLocker lock(m_mx);
  return internalGetBytes(data, len, tmo);
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoStub::bytesPut)(cr_EsBinBuffer data, ulong tmo)
{
  if( !data.empty() )
    return bytesPut(
      &data[0],
      static_cast<esU32>(data.size()), 
      tmo
    );
  
  return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, EsChannelIoStub::bytesGet)(ulong len, ulong tmo)
{
  EsBinBuffer result(len);
  if( len )
  {
    len = bytesGet(&result[0], len, tmo);
    result.resize(len);
  }
  
  return result;
}

ES_IMPL_INTF_METHOD(ulong, EsChannelIoStub::txTimeEstimateGet)(ulong len) const
{
  return 1;  
}

// port reset & error info access
ES_IMPL_INTF_METHOD(void, EsChannelIoStub::reset)()
{
  EsMutexLocker lock(m_mx);
  internalReset();
}

ES_IMPL_INTF_METHOD(long, EsChannelIoStub::errorGet)() const
{
  EsMutexLocker lock(m_mx);
  return internalGetError();
}

ES_IMPL_INTF_METHOD(EsString, EsChannelIoStub::errorStringGet)() const
{
  return EsString::null();
}

// wrappers for c channel interface
//
esBL EsChannelIoStub::cLock(EseChannelIo* p, esU32 tmo)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.lock(tmo);
    ES_ASSERT( EsMutex::resultDeadlock != result );
    return EsMutex::resultOk == result;
  }
  catch(...)
  {}
  
  return FALSE;
}

void EsChannelIoStub::cUnlock(EseChannelIo* p)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  try
  {
    EsMutex::Result result = chnl->m_mx.unlock();
    ES_ASSERT(EsMutex::resultOk == result);
  }
  catch(...)
  {}
}

esBL EsChannelIoStub::cIsConnected(EseChannelIo* p)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  return chnl->internalIsOpen();
}

esBL EsChannelIoStub::cConnect(EseChannelIo* p)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  return true == chnl->internalOpen();
}

void EsChannelIoStub::cDisconnect(EseChannelIo* p)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  chnl->internalClose();
}

esU32 EsChannelIoStub::cPutBytes(EseChannelIo* p, const esU8* data, esU32 count)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  return chnl->internalPutBytes(data, count, 0);
}

esU32 EsChannelIoStub::cGetBytes(EseChannelIo* p, esU8* data, esU32 count, esU32 timeout)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  return chnl->internalGetBytes(data, count, timeout);
}

void EsChannelIoStub::cResetIo(EseChannelIo* p)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  chnl->internalReset();
}

esBL EsChannelIoStub::cWaitTxEmpty(EseChannelIo* ES_UNUSED(p))
{
  return TRUE;
}

int EsChannelIoStub::cGetError(EseChannelIo* p)
{
  EsChannelIoStub* chnl = (EsChannelIoStub*)p->m_bus;
  return static_cast<long>(chnl->internalGetError());
}

// initialize C EseChannelIo from existing EsChannelIoIntf
ES_IMPL_INTF_METHOD(void, EsChannelIoStub::commChannelInit)(EseChannelIo* chnl)
{
  chnlInit(chnl, this);
  chnl->m_type = CHNL_MEM;
  chnl->lock = EsChannelIoStub::cLock;
  chnl->unlock = EsChannelIoStub::cUnlock;
  chnl->isConnected = EsChannelIoStub::cIsConnected;
  chnl->connect = EsChannelIoStub::cConnect;
  chnl->disconnect = EsChannelIoStub::cDisconnect;
  // data io
  chnl->bytesPut = EsChannelIoStub::cPutBytes;
  chnl->bytesGet = EsChannelIoStub::cGetBytes;
  chnl->resetIo = EsChannelIoStub::cResetIo;
  chnl->waitTxEmpty = EsChannelIoStub::cWaitTxEmpty;
  // misc
  chnl->errorGet = EsChannelIoStub::cGetError;
}

#endif // ES_COMM_USE_CHANNEL_STUB

