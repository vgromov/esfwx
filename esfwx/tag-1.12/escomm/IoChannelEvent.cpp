#include "stdafx.h"
#pragma hdrstop

#include "IoChannelEvent.h"

// -------------------------------------------------------------------------------------------------------
// io channel event class
wxDEFINE_EVENT(evtIO_CHANNEL, IoChannelEvent);

IoChannelEvent::IoChannelEvent( Type t, const EsString& msg, const BYTE* beg, const BYTE* end ) :
wxEvent(t, evtIO_CHANNEL),
m_msg(msg)
{
	if(beg && end && end > beg )
		m_data.assign(beg, end);
}

IoChannelEvent::IoChannelEvent( const IoChannelEvent& src ) :
wxEvent(src),
m_msg(src.m_msg),
m_data(src.m_data)
{
}