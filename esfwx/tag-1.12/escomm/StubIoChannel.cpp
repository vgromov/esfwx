#include "stdafx.h"
#pragma hdrstop

#include "StubIoChannel.h"
#include "StubIoChannelConfigPane.h"

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(StubIoChannel, _i("Stub channel"))
	// reflected property infos declarations
	//
	// reflected interface methods
	//
	// EsReflectedClassIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsReflectedClassIntf, setConfigString, void_Call_cr_EsString_bool, _i("Set stub channel configuration string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsReflectedClassIntf, getConfigString, EsString_CallConst, _i("Get stub channel configuration string"))
	// EsIoChannelIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, open, bool_Call, _i("Open stub channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, close, void_Call, _i("Close stub channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, isOpen, bool_CallConst, _i("Return true if stub channel is open"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, putBytes, ulong_Call_cr_EsBinBuffer_ulong_EsBaseIntfPtr, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, getBytes, EsBinBuffer_Call_ulong_ulong_EsBaseIntfPtr, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, getTxTimeEstimate, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, reset, void_Call, _i("Reset channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(StubIoChannel, EsIoChannelIntf, getError, long_CallConst, _i("Get channel-specific error code"))
	// reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(StubIoChannel, EsBaseIntfPtr_ClassCall, _i("StubIoChannel constructor"))
ES_DECL_CLASS_INFO_END

// -------------------------------------------------------------------------------------------------------
// communication class
//
StubIoChannel::StubIoChannel(wxEvtHandler* sink) :
m_evtSink(0),
m_open(false)
{
	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);

	setEvtSink(sink);
}

StubIoChannel::~StubIoChannel()
{
	close();
	setEvtSink(0);
}

// internal services
//
bool StubIoChannel::internalIsOpen() const
{
	return m_open;
}

bool StubIoChannel::internalOpen()
{
	if( !internalIsOpen() )
	{
		if( m_evtSink )
			wxQueueEvent( m_evtSink,
				new IoChannelEvent(IoChannelEvent::TypeOpen, _("Stub io channel is open") )
			);		
		m_open = true;
	}
	
	return m_open;
}

void StubIoChannel::internalClose()
{
	if( internalIsOpen() )
	{
		if( m_evtSink )
			wxQueueEvent( m_evtSink,
				new IoChannelEvent(IoChannelEvent::TypeOpen, _("Stub io channel is closed") )
			);		
		m_open = true;
	}
}

DWORD StubIoChannel::internalPutBytes(const BYTE* data, DWORD len, DWORD WXUNUSED(tmo), CommChannelBreak* WXUNUSED(brk))
{
	if( internalIsOpen() )
	{
		if( m_evtSink && len )
			wxQueueEvent( m_evtSink, 
				new IoChannelEvent(IoChannelEvent::TypeTX, 
					EsString::format(_("%d bytes sent."), len), 
					data, data+len)
			);	
			
		return len;
	}
	
	return 0;
}

DWORD StubIoChannel::internalGetBytes(BYTE* WXUNUSED(data), DWORD WXUNUSED(len), DWORD WXUNUSED(tmo), CommChannelBreak* WXUNUSED(brk))
{
	wxThread::Sleep(1);
	return 0;
}

void StubIoChannel::internalReset()
{
}

long StubIoChannel::internalGetError() const
{
	return 0;
}

// EsIoChannelIntf interface
//
// state manipulation
ES_IMPL_INTF_METHOD(bool, StubIoChannel::open)()
{
	wxMutexLocker lock(m_mx);
	return internalOpen();
}

ES_IMPL_INTF_METHOD(void, StubIoChannel::close)()
{
	wxMutexLocker lock(m_mx);
	internalClose();
}

ES_IMPL_INTF_METHOD(bool, StubIoChannel::isOpen)() const
{
	wxMutexLocker lock(m_mx);
	return internalIsOpen();
}

// byte io
ES_IMPL_INTF_METHOD(DWORD, StubIoChannel::putBytes)(const BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr WXUNUSED(breaker))
{
	wxMutexLocker lock(m_mx);
	return internalPutBytes(data, len, tmo, 0);
}

ES_IMPL_INTF_METHOD(DWORD, StubIoChannel::getBytes)(BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr WXUNUSED(breaker))
{
	wxMutexLocker lock(m_mx);
	return internalGetBytes(data, len, tmo, 0);
}

ES_IMPL_INTF_METHOD(ulong, StubIoChannel::putBytes)(cr_EsBinBuffer data, ulong tmo, EsBaseIntfPtr breaker)
{
	if( !data.empty() )
		return putBytes(&data[0], data.size(), tmo, breaker);
	
	return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, StubIoChannel::getBytes)(ulong len, ulong tmo, EsBaseIntfPtr breaker)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = getBytes(&result[0], len, tmo, breaker);
		result.erase(result.begin()+len, result.end());
	}
	
	return result;
}

ES_IMPL_INTF_METHOD(ulong, StubIoChannel::getTxTimeEstimate)(ulong len) const
{
	return 1;	
}

// port reset & error info access
ES_IMPL_INTF_METHOD(void, StubIoChannel::reset)()
{
	wxMutexLocker lock(m_mx);
	internalReset();
}

ES_IMPL_INTF_METHOD(long, StubIoChannel::getError)() const
{
	wxMutexLocker lock(m_mx);
	return internalGetError();
}

// event sink access
ES_IMPL_INTF_METHOD(void, StubIoChannel::setEvtSink)(wxEvtHandler* sink)
{
	wxMutexLocker lock(m_mx);	
	m_evtSink = sink;
}

// wrappers for c channel interface
BOOL StubIoChannel::cLock(busHANDLE h, DWORD tmo)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.LockTimeout(tmo);
		wxASSERT( wxMUTEX_DEAD_LOCK != err );
		return wxMUTEX_NO_ERROR == err;
	}
	catch(...)
	{}
	
	return FALSE;
}

void StubIoChannel::cUnlock(busHANDLE h)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.Unlock();
		wxASSERT(wxMUTEX_NO_ERROR == err);
	}
	catch(...)
	{}
}

BOOL StubIoChannel::cIsConnected(busHANDLE h)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	return chnl->internalIsOpen();
}

BOOL StubIoChannel::cConnect(busHANDLE h)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	return true == chnl->internalOpen();
}

void StubIoChannel::cDisconnect(busHANDLE h)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	chnl->internalClose();
}

DWORD StubIoChannel::cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* brk)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	return chnl->internalPutBytes(data, count, 0, brk);
}

DWORD StubIoChannel::cGetBytes(busHANDLE h, BYTE* data, DWORD count, DWORD timeout, CommChannelBreak* brk)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	return chnl->internalGetBytes(data, count, timeout, brk);
}

void StubIoChannel::cResetIo(busHANDLE h, DWORD WXUNUSED(timeout))
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	chnl->internalReset();
}

BOOL StubIoChannel::cWaitTxEmpty(busHANDLE WXUNUSED(h), CommChannelBreak* WXUNUSED(brk))
{
	return TRUE;
}

int StubIoChannel::cGetError(busHANDLE h)
{
	StubIoChannel* chnl = (StubIoChannel*)h;
	return static_cast<long>(chnl->internalGetError());
}

// initialize C CommChannel from existing EsIoChannelIntf
ES_IMPL_INTF_METHOD(void, StubIoChannel::initCommChannel)(CommChannel* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_MEM;
	chnl->lock = StubIoChannel::cLock;
	chnl->unlock = StubIoChannel::cUnlock;
	chnl->isConnected = StubIoChannel::cIsConnected;
	chnl->connect = StubIoChannel::cConnect;
	chnl->disconnect = StubIoChannel::cDisconnect;
	// data io
	chnl->putBytes = StubIoChannel::cPutBytes;
	chnl->getBytes = StubIoChannel::cGetBytes;
	chnl->resetIo = StubIoChannel::cResetIo;
	chnl->waitTxEmpty = StubIoChannel::cWaitTxEmpty;
	// misc
	chnl->getError = StubIoChannel::cGetError;
}
