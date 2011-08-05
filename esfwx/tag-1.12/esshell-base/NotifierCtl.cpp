#include "stdafx.h"
#pragma hdrstop

#include "NotifierCtl.h"
// standard icons for different notification levels
#include "res/info.xpm"
#include "res/warning.xpm"
#include "res/stop.xpm"

//---------------------------------------------------------------------------------------------
// EsNotifierIntf implementation
//
NotifierCtl::NotifierIntfImpl::~NotifierIntfImpl()
{
}

ES_IMPL_INTF_METHOD(void, NotifierCtl::NotifierIntfImpl::clear)()
{
	wxCriticalSectionLocker csLock( m_this.m_cs );
	// clear notification
	m_this.m_lvl = EsNotifierIntf::LevelInfo;
	m_this.m_bm = wxNullBitmap;
	m_this.m_lbl = EsString::s_null;
	m_this.m_needUpdate = true;
}

ES_IMPL_INTF_METHOD(void, NotifierCtl::NotifierIntfImpl::notify)(EsNotifierIntf::Level lvl, const EsString& msg, const wxBitmap& icon/* = wxNullBitmap*/)
{
	wxCriticalSectionLocker csLock( m_this.m_cs );	
	// set notification
	m_this.m_lvl = lvl;
	if( icon.IsNull() )
	{
		switch(m_this.m_lvl)
		{
		case LevelInfo:
			m_this.m_bm = wxBitmap(info_xpm);
			break;
		case LevelWarning:
			m_this.m_bm = wxBitmap(warning_xpm);
			break;
		case LevelError:
			m_this.m_bm = wxBitmap(stop_xpm);
			break;
		default:
			m_this.m_bm = icon;
			break;
		}
	}
	else
		m_this.m_bm = icon;

	m_this.m_lbl = msg;
	m_this.m_needUpdate = true;
}

// notifier control itself
//
NotifierCtl::NotifierCtl() :
wxControl(),
m_lvl( EsNotifierIntf::LevelInfo ),
m_bm(wxNullBitmap),
m_needUpdate(false),
m_intf(*this)
{
	SetLabel( m_lbl );
	
	Connect( wxEVT_PAINT, wxPaintEventHandler(NotifierCtl::onPaint) );	
}

NotifierCtl::NotifierCtl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) :
wxControl(parent, id, pos, size, wxBORDER_NONE),
m_lvl( EsNotifierIntf::LevelInfo ),
m_bm(wxNullBitmap),
m_needUpdate(false),
m_intf(*this)
{
	InheritAttributes();
	SetLabel( m_lbl );
	
	Connect( wxEVT_PAINT, wxPaintEventHandler(NotifierCtl::onPaint) );
}

NotifierCtl::~NotifierCtl()
{
	Disconnect( wxEVT_PAINT, wxPaintEventHandler(NotifierCtl::onPaint) );
}

// paint event handler
void NotifierCtl::onPaint(wxPaintEvent& WXUNUSED(evt))
{
	wxPaintDC dc(this);

	wxRect r = GetRect();
	wxColor cl = GetParent()->GetBackgroundColour();
	
	EsNotifierIntf::Level lvl;
	wxBitmap bm;
	EsString txt;
	{
		wxCriticalSectionLocker csLock( m_cs );
		lvl = m_lvl;
		bm = m_bm;
		txt = m_lbl;
	}

	if( EsNotifierIntf::LevelError == lvl )
		cl = wxColour(255, 128, 128);
	else if( EsNotifierIntf::LevelWarning == lvl )
		cl = wxColour(255, 200, 0);

	wxBrush bgnd(cl);
	dc.SetFont(GetFont());
	dc.SetBackground(bgnd);
	dc.SetBrush(bgnd);
	dc.SetPen(*wxTRANSPARENT_PEN);
	dc.DrawRectangle(r);
	
	dc.SetBackgroundMode(wxTRANSPARENT);
	dc.DrawLabel(txt, bm, r, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL);
}

void NotifierCtl::OnInternalIdle()
{
	wxControl::OnInternalIdle();
	wxCriticalSectionLocker lock(m_cs);
	if( m_needUpdate )
	{
		Refresh();
		m_needUpdate = false;
	}
}