#include "stdafx.h"
#pragma hdrstop

#include "EsSpecDataEvent.h"

// data event
//
EsEvtSpecData::EsEvtSpecData( wxEventType eventType, EsBinBuffer::const_pointer data, size_t len,int id /*= wxID_ANY*/) :
EsEvent(eventType, id),
m_data(data, data+len)
{
}

EsEvtSpecData::EsEvtSpecData(wxEventType eventType, const EsBinBuffer& data, int id /*= wxID_ANY*/) :
EsEvent(eventType, id),
m_data(data)
{
}

EsEvtSpecData::EsEvtSpecData(const EsEvtSpecData& src) :
EsEvent(src),
m_data(src.m_data)
{
}

wxEvent* EsEvtSpecData::Clone() const
{
	return new EsEvtSpecData(*this);
}

const EsBinBuffer& EsEvtSpecData::getData() const
{
	return m_data;
}
