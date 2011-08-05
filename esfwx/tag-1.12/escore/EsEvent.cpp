#include "stdafx.h"
#pragma hdrstop
 
#include "EsEvent.h"
 
EsEvent::EsEvent(wxEventType type, wxWindowID id /*= wxID_ANY*/, int val /*= 0*/, const EsString& str /*= EsString::s_null*/) :
wxEvent(id, type),
m_val(val),
m_str(str)
{
}

EsEvent::EsEvent(const EsEvent& src) :
wxEvent(src),
m_str(src.m_str),
m_val(src.m_val)
{
}
	
wxEvent* EsEvent::Clone() const
{
	return new EsEvent(*this);
}

 