#ifndef _io_channel_event_h_
#define _io_channel_event_h_

// special event class for io channel communications
// IoChannelEvent notification event type
//
class EKOSF_COMM_CLASS IoChannelEvent : public wxEvent
{
public:
	enum Type {
		TypeNone,
		TypeOpen,
		TypeClose,
		TypeInfo,
		TypeRX,
		TypeTX,
		TypeError
	};

public:
	IoChannelEvent( Type t = TypeNone, const EsString& msg = EsString::s_null, const BYTE* beg = 0, const BYTE* end = 0 );
	IoChannelEvent( const IoChannelEvent& src );
	virtual wxEvent* Clone() const { return new IoChannelEvent( *this ); }

	const IoChannelEvent::Type getType() const { return static_cast<IoChannelEvent::Type>( GetId() ); }
	EsString getMsg() const { return m_msg; }
	const EsBinBuffer& getData() const { return m_data; }

protected:
#pragma warning(push)
#pragma warning(disable: 4251)
	EsBinBuffer m_data;
#pragma warning(pop)	
	EsString m_msg;
};
typedef void (wxEvtHandler::*IoChannelEventFunction)(IoChannelEvent&);
#define IoChannelEventHandler( f )	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(IoChannelEventFunction, &f)
wxDECLARE_EXPORTED_EVENT(EKOSF_COMM_DATA, evtIO_CHANNEL, IoChannelEvent);

#endif // _io_channel_event_h_