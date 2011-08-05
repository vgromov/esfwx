#include "stdafx.h"
#pragma hdrstop

#include "SocketIoAdapter.h"

SocketIoAdapter::SocketIoAdapter(wxSocketBase* sock) :
m_sock(0)
{
	setSocket(sock);
}

SocketIoAdapter::~SocketIoAdapter()
{
	if( m_sock )
		m_sock->Destroy();
}

// socket pointer setter. destroys previously owned socket
void SocketIoAdapter::setSocket(wxSocketBase* sock)
{
	if( m_sock )
		m_sock->Destroy();

	m_sock = sock;
	if( m_sock )
		m_sock->SetFlags(wxSOCKET_BLOCK|wxSOCKET_WAITALL);
}

// byte IO
DWORD SocketIoAdapter::putBytes(const BYTE* data, DWORD len)
{
	DWORD written = 0;
	if( m_sock && m_sock->IsConnected() )
	{
		written = m_sock->Write(data, len).LastCount();
		if( written )
		{
			// send to sink
			if( m_sink )
				wxQueueEvent( m_sink,
					new IoChannelEvent(IoChannelEvent::TypeTX, 
						EsString::format(_("%d bytes sent."), written), 
						data, data+written)
				);
		}

		if( m_sock->Error() )
			EsException::Throw(0, EsException::Generic, _("Error occured while writing to socket:\n'%s'"),
			SocketIoAdapter::getErrorString(m_sock->LastError()).c_str() ); 
	}

	return written;
}

DWORD SocketIoAdapter::getBytes(BYTE* data, DWORD len)
{
	DWORD read = 0;
	if( m_sock && m_sock->IsConnected() )
	{
		read = m_sock->Read(data, len).LastCount();
		if( read )
		{
			// send to sink
			if( m_sink )
				wxQueueEvent( m_sink,
					new IoChannelEvent(IoChannelEvent::TypeRX, 
						EsString::format(_("%d bytes received."), read), 
						data, data+read)
				);
		}

		if( m_sock->Error() )
			EsException::Throw(0, EsException::Generic, _("Error occured while reading from socket:\n'%s'"),
			SocketIoAdapter::getErrorString(m_sock->LastError()).c_str() ); 
	}

	return read;
}

bool SocketIoAdapter::connect(const EsString& target, long targetPort)
{
	if( !m_sock )
		return false;

	if( m_sock->IsConnected() )
		return true;
		
	wxSocketClient* client = wxDynamicCast(m_sock, wxSocketClient);
	if( client )
	{
		wxIPV4address addr;
		if( !addr.Hostname( target ) )
			EsException::Throw(0, EsException::Generic, _("Could not use '%s' as target address"),
			target.c_str());
		if( !addr.Service( targetPort ) )
			EsException::Throw(0, EsException::Generic, _("Could not use '%d' as target port"),
			targetPort);

		bool ok = client->Connect(addr);
		if( ok && !m_sock->IsConnected() )
			EsException::Throw(0, EsException::Generic, _("Connection to '%s:%d' is refused by peer"),
			target.c_str(), targetPort );

		return ok;
	}

	return false;
}

// peer connection lost handler. underlying socket may be destroyed if
// peer connection is lost
void SocketIoAdapter::checkConnectionIsLost()
{
	if( m_sock && m_sock->IsConnected() && m_sock->WaitForLost(0, TMO_GRANULARITY) )
	{
		if( m_sink )
			wxQueueEvent( m_sink, 
				new	IoChannelEvent(IoChannelEvent::TypeInfo, 
					_("Client socket connection is lost") )
			);
		m_sock->Close();	
	}
}

// error access
long SocketIoAdapter::getError() const 
{ 
	if( m_sock && m_sock->IsOk() ) 
		return static_cast<long>(m_sock->LastError()); 
	else
		return 0;
}

// reset IO
void SocketIoAdapter::reset()
{
	if( m_sock && m_sock->IsConnected() )
		m_sock->Discard();
}

// static services
//
// retrieve socket error string
EsString SocketIoAdapter::getErrorString( wxSocketError err )
{
	switch( err )
	{
	case wxSOCKET_NOERROR:
		return _("No socket error");
	case wxSOCKET_INVOP:
		return _("Invalid socket operation");
	case wxSOCKET_IOERR:
		return _("Input/Output socket error");  
	case wxSOCKET_INVADDR:
		return _("Invalid socket address");  
	case wxSOCKET_INVSOCK:
		return _("Invalid (uninitialized) socket");
	case wxSOCKET_NOHOST:
		return _("Host is not found");  
	case wxSOCKET_INVPORT:
		return _("Invalid socket port");  
	case wxSOCKET_WOULDBLOCK:
		return _("The socket is non-blocking and the operation would block");  
	case wxSOCKET_TIMEDOUT:
		return _("The timeout for socket operation expired");  
	case wxSOCKET_MEMERR:
		return _("Socket memory exhausted"); 
	}

	return _("Unknown socket error");
}
