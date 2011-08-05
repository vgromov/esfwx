#include "stdafx.h"
#pragma hdrstop

#include "SocketClientIoChannel.h"
#include "SocketClientIoChannelconfigPane.h"

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(SocketClientIoChannel, _i("Client socket (network) connection channel"))
	// reflected property infos declarations
	ES_DECL_PROP_INFO(SocketClientIoChannel, target, EsString, _i("Server address"), wxT("localhost"))
	ES_DECL_PROP_INFO_RANGE(SocketClientIoChannel, targetPort, long, _i("Server port"), 50000, 50000, 65535)
	ES_DECL_PROP_INFO_RANGE(SocketClientIoChannel, operationTimeout, long, _i("Operation timeout (s)"), 5, 1, 15)
	// reflected interface methods
	//
	// EsReflectedClassIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsReflectedClassIntf, setConfigString, void_Call_cr_EsString_bool, _i("Set client socket channel configuration string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsReflectedClassIntf, getConfigString, EsString_CallConst, _i("Get client socket channel configuration string"))
	// EsIoChannelIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, open, bool_Call, _i("Open client socket channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, close, void_Call, _i("Close client socket channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, isOpen, bool_CallConst, _i("Return true if client socket channel is open and connected to server"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, putBytes, ulong_Call_cr_EsBinBuffer_ulong_EsBaseIntfPtr, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, getBytes, EsBinBuffer_Call_ulong_ulong_EsBaseIntfPtr, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, getTxTimeEstimate, ulong_CallConst_ulong, NO_METHOD_DESCR)	
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, reset, void_Call, _i("Reset client socket channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketClientIoChannel, EsIoChannelIntf, getError, long_CallConst, _i("Get client socket channel-specific error code"))
	// reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(SocketClientIoChannel, EsBaseIntfPtr_ClassCall, _i("SocketClientIoChannel constructor"))
ES_DECL_CLASS_INFO_END

// client channel implementation
//
SocketClientIoChannel::SocketClientIoChannel(wxEvtHandler* sink /*= 0*/) :
m_adapter( new wxSocketClient )
{
	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);
	setEvtSink(sink);
	m_adapter.getSocket()->SetTimeout( m_operationTimeout );
}

SocketClientIoChannel::~SocketClientIoChannel()
{
	close();
	setEvtSink(0);
}

// socket channel configuraiton properties
//
EsString SocketClientIoChannel::get_target() const
{
	wxMutexLocker lock(m_mx);
	return m_target;
}

void SocketClientIoChannel::set_target(const EsString &val)
{
	wxMutexLocker lock(m_mx);
	if( m_adapter.isConnected() )
		EsException::Throw(0, EsException::Generic, _("Could not set target address while socket is connected"));
	
	m_target = val;
}

long SocketClientIoChannel::get_targetPort() const
{
	wxMutexLocker lock(m_mx);
	return m_targetPort;
}

void SocketClientIoChannel::set_targetPort(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_adapter.isConnected() )
		EsException::Throw(0, EsException::Generic, _("Could not set target port while socket is connected"));

	if( m_targetPort != val && ES_THIS_REFLECTED_PROP_INFO_GET(targetPort)->validate(val, true) )
		m_targetPort = val;
}

long SocketClientIoChannel::get_operationTimeout() const
{
	wxMutexLocker lock(m_mx);
	return m_operationTimeout;
}

void SocketClientIoChannel::set_operationTimeout(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_adapter.isConnected() )
		EsException::Throw(0, EsException::Generic, _("Could not set operation timeout while socket is connected"));
	
	if( m_operationTimeout != val && ES_THIS_REFLECTED_PROP_INFO_GET(operationTimeout)->validate(val, true) )
	{
		m_operationTimeout = val;
		m_adapter.getSocket()->SetTimeout( m_operationTimeout );
	}
}

// EsIoChannelIntf interface
//

// state manipulation
ES_IMPL_INTF_METHOD(bool, SocketClientIoChannel::open)()
{
	wxMutexLocker lock(m_mx);
	return m_adapter.connect( m_target, m_targetPort );
}

ES_IMPL_INTF_METHOD(void, SocketClientIoChannel::close)()
{
	wxMutexLocker lock(m_mx);
	m_adapter.close();
}

ES_IMPL_INTF_METHOD(bool, SocketClientIoChannel::isOpen)() const
{
	wxMutexLocker lock(m_mx);
	return m_adapter.isConnected();
}

// byte io
ES_IMPL_INTF_METHOD(DWORD, SocketClientIoChannel::putBytes)(const BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr WXUNUSED(breaker))
{
	wxMutexLocker lock(m_mx);
	return m_adapter.putBytes(data, len);
}

ES_IMPL_INTF_METHOD(DWORD, SocketClientIoChannel::getBytes)(BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr WXUNUSED(breaker))
{
	wxMutexLocker lock(m_mx);
	return m_adapter.getBytes(data, len);
}

ES_IMPL_INTF_METHOD(ulong, SocketClientIoChannel::putBytes)(const EsBinBuffer& data, ulong tmo, EsBaseIntfPtr breaker)
{
	if( !data.empty() )
		return putBytes(&data[0], data.size(), tmo, breaker);
	
	return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, SocketClientIoChannel::getBytes)(ulong len, ulong tmo, EsBaseIntfPtr breaker)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = getBytes(&result[0], len, tmo, breaker);
		result.erase(result.begin()+len, result.end());
	}
	
	return result;
}

ES_IMPL_INTF_METHOD(ulong, SocketClientIoChannel::getTxTimeEstimate)(ulong len) const
{
	return 100;	
}

// port reset & error info access
ES_IMPL_INTF_METHOD(void, SocketClientIoChannel::reset)()
{
	wxMutexLocker lock(m_mx);
	m_adapter.reset();
}

ES_IMPL_INTF_METHOD(long, SocketClientIoChannel::getError)() const
{
	wxMutexLocker lock(m_mx);
	return m_adapter.getError();
}

// event sink access
ES_IMPL_INTF_METHOD(void, SocketClientIoChannel::setEvtSink)(wxEvtHandler* sink)
{
	wxMutexLocker lock(m_mx);
	m_evtSink = sink;
	m_adapter.setEvtSink(sink);
}

// the rest of services (non-interfaced)
//

void SocketClientIoChannel::save(wxConfigBase& storage) const
{
	storage.Write( getClassInfo().getName(), static_cast<wxString>(getConfigString()) );
}

void SocketClientIoChannel::load(wxConfigBase& storage)
{
	EsString config = storage.Read( getClassInfo().getName() );
	setConfigString( config, true );
}

// wrappers for c channel interfacing
BOOL SocketClientIoChannel::cLock(busHANDLE h, DWORD tmo)
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
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

void SocketClientIoChannel::cUnlock(busHANDLE h)
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.Unlock();
		wxASSERT(wxMUTEX_NO_ERROR == err);
	}
	catch(...)
	{}
}

BOOL SocketClientIoChannel::cIsConnected(busHANDLE h)
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
	try
	{
		return chnl->m_adapter.isConnected();
	}
	catch(...)
	{}
	
	return FALSE;
}

BOOL SocketClientIoChannel::cConnect(busHANDLE h)
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
	try
	{
		return chnl->m_adapter.connect( chnl->m_target, chnl->m_targetPort );
	}
	catch(...)
	{}
	
	return FALSE;	
}

void SocketClientIoChannel::cDisconnect(busHANDLE h)
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
	try
	{
		chnl->m_adapter.close();
	}
	catch(...)
	{}
}

DWORD SocketClientIoChannel::cPutBytes(busHANDLE h, const BYTE* data, DWORD count, CommChannelBreak* WXUNUSED(brk))
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;	
	try
	{
		return chnl->m_adapter.putBytes(data, count);
	}
	catch(...)
	{}
	
	return 0;
}

DWORD SocketClientIoChannel::cGetBytes(busHANDLE h, BYTE* data, DWORD count, 
	DWORD WXUNUSED(timeout), CommChannelBreak* WXUNUSED(brk))
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;	
	try
	{
		return chnl->m_adapter.getBytes(data, count);
	}
	catch(...)
	{}
	
	return 0;
}

void SocketClientIoChannel::cResetIo(busHANDLE h, DWORD WXUNUSED(timeout))
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
	try
	{
		chnl->m_adapter.reset();
	}
	catch(...)
	{}	
}

int SocketClientIoChannel::cGetError(busHANDLE h)
{
	SocketClientIoChannel* chnl = (SocketClientIoChannel*)h;
	try
	{
		return static_cast<int>(chnl->m_adapter.getError());
	}
	catch(...)
	{}	
	
	return 0;
}

// initialize C CommChannel from existing EsIoChannelIntf
ES_IMPL_INTF_METHOD(void, SocketClientIoChannel::initCommChannel)(CommChannel* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_UART;
	chnl->lock = cLock;
	chnl->unlock = cUnlock;
	chnl->isConnected = cIsConnected;
	chnl->connect = cConnect;
	chnl->disconnect = cDisconnect;	
	// data io
	chnl->putBytes = cPutBytes;
	chnl->getBytes = cGetBytes;
	chnl->resetIo = cResetIo;
	// misc
	chnl->getError = cGetError;
}
