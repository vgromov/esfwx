#include "stdafx.h"
#pragma hdrstop

#include "SocketServerIoChannel.h"
#include "SocketServerIoChannelConfigPane.h"

// -------------------------------------------------------------------------------------------------------
// reflection class info declaration
ES_DECL_BASE_CLASS_INFO_BEGIN(SocketServerIoChannel, _i("Server socket (network) connection channel"))
	// reflected property infos declarations
	ES_DECL_PROP_INFO_RANGE(SocketServerIoChannel, port, long, _i("Server port"), 50000, 50000, 65535)
	ES_DECL_PROP_INFO_RANGE(SocketServerIoChannel, operationTimeout, long, _i("Operation timeout (s)"), 5, 1, 15)	
	// reflected interface methods
	//
	// EsReflectedClassIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsReflectedClassIntf, setConfigString, void_Call_cr_EsString_bool, _i("Set server socket channel configuration string"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsReflectedClassIntf, getConfigString, EsString_CallConst, _i("Get server socket channel configuration string"))
	// EsIoChannelIntf
	//
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, open, bool_Call, _i("Open server socket channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, close, void_Call, _i("Close server socket channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, isOpen, bool_CallConst, _i("Return true, if server socket channel is listening"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, putBytes, ulong_Call_cr_EsBinBuffer_ulong_EsBaseIntfPtr, _i("Send bytes over a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, getBytes, EsBinBuffer_Call_ulong_ulong_EsBaseIntfPtr, _i("Receive bytes from a channel"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, getTxTimeEstimate, ulong_CallConst_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, reset, void_Call, _i("Reset server socket channel IO buffers"))
	ES_DECL_REFLECTED_INTF_METHOD_INFO(SocketServerIoChannel, EsIoChannelIntf, getError, long_CallConst, _i("Get server socket channel-specific error code"))
	// reflected class methods
	//
	ES_DECL_REFLECTED_CTOR_INFO(SocketServerIoChannel, EsBaseIntfPtr_ClassCall, _i("SocketServerIoChannel constructor"))
ES_DECL_CLASS_INFO_END

// server listener thread implementation
SocketServerIoChannel::Listener::Listener(SocketServerIoChannel& chnl) :
m_owner(chnl),
m_srv(0),
m_adapter(0)
{}

void SocketServerIoChannel::Listener::onEnterWorker()
{
	wxMutexLocker lock(m_mxSocket);
	wxIPV4address addr;
	addr.Service(m_owner.m_port);
	m_srv = new wxSocketServer(addr, wxSOCKET_BLOCK|wxSOCKET_WAITALL);
	if( !m_srv->IsOk() )
		EsException::Throw(0, EsException::Generic, _("Server socket cannot listen on port %d"),
		m_owner.m_port);
	// set operations timeout
	m_srv->SetTimeout( SocketIoAdapter::TMO_GRANULARITY );			
	if( m_owner.m_sink )
		wxQueueEvent( m_owner.m_sink,
			new IoChannelEvent(IoChannelEvent::TypeOpen, 
				EsString::format(_("Listening for incoming client connections on port %d"), m_owner.m_port) )
		);
	wxSocketBase* sock = new wxSocketBase;
	wxASSERT(sock);
	m_adapter.setSocket(sock);
	m_adapter.getSocket()->SetTimeout( m_owner.get_operationTimeout() );	
}

void SocketServerIoChannel::Listener::onExitWorker()
{
	wxMutexLocker lock(m_mxSocket);
	// close spawned connection, if any
	m_adapter.setSocket(0);
	// gracefully destroy server object
	if( m_srv )
		m_srv->Destroy();

	if( m_owner.m_sink )
		wxQueueEvent( m_owner.m_sink,
			new IoChannelEvent(IoChannelEvent::TypeClose, 
				EsString::format(_("Stop listening for incoming client connections on port %d"), m_owner.m_port) )
		);
}

// internal socket state checkers
void SocketServerIoChannel::Listener::internalCheckAcceptClient()
{
	wxMutexLocker lock(m_mxSocket);
	if( !m_adapter.isConnected() ) // if no client accepted already
	{
		if( m_srv->AcceptWith(*m_adapter.getSocket()) )
		{
			if( m_owner.m_sink )
			{
				wxIPV4address addr;
				m_adapter.getSocket()->GetPeer(addr);
				wxQueueEvent( m_owner.m_sink, new IoChannelEvent(IoChannelEvent::TypeInfo, 
					EsString::format(_("Client connection accepted on %d from '%s'"), 
						m_owner.m_port,
						addr.IPAddress().c_str() ) ) 
				);
			}
		}
	}
}

void SocketServerIoChannel::Listener::internalCheckLostClient()
{
	wxMutexLocker lock(m_mxSocket);
	m_adapter.checkConnectionIsLost();		
}

long SocketServerIoChannel::Listener::worker(EsThread::Worker& thread)
{
	// local socket states
	while( !thread.checkForStop(250) )
	{
		// check for accepted client connection
		internalCheckAcceptClient();
		// check for lost client connection
		internalCheckLostClient();
	}

	return 0;
}

// server channel implementation
SocketServerIoChannel::SocketServerIoChannel(wxEvtHandler *sink /*= 0*/) :
m_sink(0),
m_listener(*this)
{
	// initialize properties to their default values
	getClassInfoStatic().resetAllProperties(this);
	setEvtSink(sink);
}

SocketServerIoChannel::~SocketServerIoChannel()
{
	// stop server listener first
	m_listener.stopAndWait();
}

// socket channel configuraiton properties
//
long SocketServerIoChannel::get_port() const
{
	wxMutexLocker lock(m_mx);
	return m_port;
}

void SocketServerIoChannel::set_port(const long &val)
{
	wxMutexLocker lock(m_mx);
	if( m_listener.exists() )
		EsException::Throw(0, EsException::Generic, _("Could not set server socket port while server is active"));
	
	if( ES_THIS_REFLECTED_PROP_INFO_GET(port)->validate(val, true) )
		m_port = val;
}

long SocketServerIoChannel::get_operationTimeout() const
{
	wxMutexLocker lock(m_mx);
	return m_operationTimeout;
}

void SocketServerIoChannel::set_operationTimeout(const long &val)
{
	wxMutexLocker lock(m_listener.m_mxSocket);
	if( m_listener.m_adapter.isConnected() )
		EsException::Throw(0, EsException::Generic, _("Could not set operation timeout while socket is connected"));
	
	if( m_operationTimeout != val && ES_THIS_REFLECTED_PROP_INFO_GET(operationTimeout)->validate(val, true) )
		m_operationTimeout = val;
}

// EsIoChannelIntf interface
//

// state manipulation
ES_IMPL_INTF_METHOD(bool, SocketServerIoChannel::open)()
{
	if( !m_listener.exists() )
		m_listener.start();
 
	return m_listener.exists();
}

ES_IMPL_INTF_METHOD(void, SocketServerIoChannel::close)()
{
	// stop incoming connection listener
	m_listener.stopAndWait();
}

ES_IMPL_INTF_METHOD(bool, SocketServerIoChannel::isOpen)() const
{
	return m_listener.exists();
}

// byte io
ES_IMPL_INTF_METHOD(DWORD, SocketServerIoChannel::putBytes)(const BYTE* data, DWORD len, DWORD WXUNUSED(tmo), EsIoBreakIntf::Ptr WXUNUSED(breaker))
{
	wxMutexLocker lock(m_listener.m_mxSocket);
	return m_listener.m_adapter.putBytes(data, len);
}

ES_IMPL_INTF_METHOD(DWORD, SocketServerIoChannel::getBytes)(BYTE* data, DWORD len, DWORD WXUNUSED(tmo), EsIoBreakIntf::Ptr WXUNUSED(breaker))
{
	wxMutexLocker lock(m_listener.m_mxSocket);
	return m_listener.m_adapter.getBytes(data, len);
}

ES_IMPL_INTF_METHOD(ulong, SocketServerIoChannel::putBytes)(const EsBinBuffer& data, ulong tmo, EsBaseIntfPtr breaker)
{
	if( !data.empty() )
		return putBytes(&data[0], data.size(), tmo, breaker);
	
	return 0;
}

ES_IMPL_INTF_METHOD(EsBinBuffer, SocketServerIoChannel::getBytes)(ulong len, ulong tmo, EsBaseIntfPtr breaker)
{
	EsBinBuffer result(len);
	if( len )
	{
		len = getBytes(&result[0], len, tmo, breaker);
		result.erase(result.begin()+len, result.end());
	}
	
	return result;
}

ES_IMPL_INTF_METHOD(ulong, SocketServerIoChannel::getTxTimeEstimate)(ulong len) const
{
	return 100;	
}

// port reset & error info access
ES_IMPL_INTF_METHOD(void, SocketServerIoChannel::reset)()
{
	wxMutexLocker lock(m_listener.m_mxSocket);
	m_listener.m_adapter.reset();
}

ES_IMPL_INTF_METHOD(long, SocketServerIoChannel::getError)() const
{
	wxMutexLocker lock(m_listener.m_mxSocket);
	return m_listener.m_adapter.getError();
}

// event sink access
ES_IMPL_INTF_METHOD(void, SocketServerIoChannel::setEvtSink)(wxEvtHandler* sink)
{
	wxMutexLocker lock(m_mx);
	m_sink = sink;
	wxMutexLocker lockSock(m_listener.m_mxSocket);
	m_listener.m_adapter.setEvtSink(sink);
}

// the rest of services (non-interfaced)
//
void SocketServerIoChannel::save(wxConfigBase& storage) const
{
	storage.Write( getClassName(), static_cast<wxString>(getConfigString()) );
}

void SocketServerIoChannel::load(wxConfigBase& storage)
{
	setConfigString( storage.Read( getClassName() ), true );
}

// wrappers for c channel interfacing
BOOL SocketServerIoChannel::cLock(busHANDLE h, DWORD tmo)
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
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

void SocketServerIoChannel::cUnlock(busHANDLE h)
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
	try
	{
		wxMutexError err = chnl->m_mx.Unlock();
		wxASSERT(wxMUTEX_NO_ERROR == err);
	}
	catch(...)
	{}
}

BOOL SocketServerIoChannel::cIsConnected(busHANDLE h)
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
	try
	{
		return chnl->m_listener.m_adapter.isConnected();
	}
	catch(...)
	{}
	
	return FALSE;
}

BOOL SocketServerIoChannel::cConnect(busHANDLE h)
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
	try
	{
		return chnl->open();
	}
	catch(...)
	{}
	
	return FALSE;
}

void SocketServerIoChannel::cDisconnect(busHANDLE h)
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
	try
	{
		chnl->close();
	}
	catch(...)
	{}
}	

DWORD SocketServerIoChannel::cPutBytes(busHANDLE h, const BYTE* data, DWORD count, 
	CommChannelBreak* WXUNUSED(brk))
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;	
	try
	{
		return chnl->m_listener.m_adapter.putBytes(data, count);
	}
	catch(...)
	{}
	
	return 0;
}

DWORD SocketServerIoChannel::cGetBytes(busHANDLE h, BYTE* data, DWORD count, 
	DWORD WXUNUSED(timeout), CommChannelBreak* WXUNUSED(brk))
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;	
	try
	{
		return chnl->m_listener.m_adapter.getBytes(data, count);
	}
	catch(...)
	{}
	
	return 0;
}

void SocketServerIoChannel::cResetIo(busHANDLE h, DWORD WXUNUSED(timeout))
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
	try
	{
		chnl->m_listener.m_adapter.reset();
	}
	catch(...)
	{}
}

int SocketServerIoChannel::cGetError(busHANDLE h)
{
	SocketServerIoChannel* chnl = (SocketServerIoChannel*)h;
	try
	{
		return static_cast<int>(chnl->m_listener.m_adapter.getError());
	}
	catch(...)
	{}
	
	return 0;
}

// initialize C CommChannel from existing EsIoChannelIntf
ES_IMPL_INTF_METHOD(void, SocketServerIoChannel::initCommChannel)(CommChannel* chnl)
{
	chnlInit(chnl, this);
	chnl->m_type = CHNL_UART;
	chnl->lock = SocketServerIoChannel::cLock;
	chnl->unlock = SocketServerIoChannel::cUnlock;
	chnl->isConnected = SocketServerIoChannel::cIsConnected;
	chnl->connect = SocketServerIoChannel::cConnect;
	chnl->disconnect = SocketServerIoChannel::cDisconnect;
	// data io
	chnl->putBytes = SocketServerIoChannel::cPutBytes;
	chnl->getBytes = SocketServerIoChannel::cGetBytes;
	chnl->resetIo = SocketServerIoChannel::cResetIo;
	// misc
	chnl->getError = SocketServerIoChannel::cGetError;
}
