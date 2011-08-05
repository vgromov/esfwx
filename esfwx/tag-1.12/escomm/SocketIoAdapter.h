#ifndef _socket_io_adapter_h_
#define _socket_io_adapter_h_

class EKOSF_COMM_CLASS SocketIoAdapter
{
public:
	enum {
		// generic socket operation timeout, in seconds
		TMO_GRANULARITY = 1,
	};

public:
	SocketIoAdapter(wxSocketBase* sock);
	~SocketIoAdapter();

	// event sink setter 
	void setEvtSink(wxEvtHandler* sink) { m_sink = sink; }
	// socket pointer setter. destroys previously owned socket
	void setSocket(wxSocketBase* sock);
	wxSocketBase* getSocket() { return m_sock; }
	// check if socket adapter is empty (no underlying socket instance exist)
	bool empty() const { return 0 == m_sock; }

	// contained socket access
	bool isConnected() const { return m_sock && m_sock->IsConnected(); }
	bool connect( const EsString& target, long targetPort );
	void close() { if( m_sock ) m_sock->Close(); }

	// peer connection lost handler
	void checkConnectionIsLost();

	// byte IO
	DWORD putBytes(const BYTE* data, DWORD len);
	DWORD getBytes(BYTE* data, DWORD len);

	// reset IO
	void reset();

	// error access
	long getError() const;

	// static services
	//
	// retrieve socket error string
	static EsString getErrorString( wxSocketError err );

protected:
	// data members
	wxSocketBase* m_sock;
	// IO event sink
	wxEvtHandler* m_sink;
};

#endif // _socket_io_adapter_h_
