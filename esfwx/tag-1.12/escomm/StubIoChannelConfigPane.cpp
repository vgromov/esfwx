#include "stdafx.h"
#pragma hdrstop

#include "StubIoChannel.h"
#include "StubIoChannelConfigPane.h"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(StubIoChannelConfigPaneWnd::StubIoChannelConfigPane, StubIoChannelConfigPane, _i("Stub channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(StubIoChannelConfigPaneWnd::StubIoChannelConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("StubIoChannelConfigPane constructor."))
ES_DECL_CLASS_INFO_END

StubIoChannelConfigPaneWnd::StubIoChannelConfigPane::~StubIoChannelConfigPane() {}

ES_IMPL_INTF_METHOD(void, StubIoChannelConfigPaneWnd::StubIoChannelConfigPane::setChannelConfigString)(const EsString& config) 
{ 
	EsDictionary cfg(StubIoChannel::getClassNameStatic(), config);
	if( 0 != cfg.getName().compare( StubIoChannel::getClassNameStatic() ) )
		EsException::Throw(0, EsException::Generic, _("Wrong class name in configuration string. Expected %s, got %s."),
		StubIoChannel::getClassNameStatic().c_str(), cfg.getName().c_str() );
	m_this.updateControls( cfg );
}

ES_IMPL_INTF_METHOD(EsString, StubIoChannelConfigPaneWnd::StubIoChannelConfigPane::getChannelConfigString)() const
{
	EsDictionary cfg(StubIoChannel::getClassNameStatic());
	m_this.updateConfig(cfg);

	return cfg.getAsString();
}

// configuration pane itself
//
StubIoChannelConfigPaneWnd::StubIoChannelConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this)
{
	wxBoxSizer* contents;
	contents = new wxBoxSizer( wxVERTICAL );
	contents->Add( new wxStaticText(this, wxID_ANY, _("Stub Io channel does not have any adjustable settings")),
		wxSizerFlags(1).Align(wxALIGN_CENTER) );
	
//	m_btnResetToDefaults = new wxButton( this, wxID_ANY, _("Reset to defaults"), wxDefaultPosition, wxDefaultSize, 0 );
//	contents->Add( m_btnResetToDefaults, 0, wxALL, 5 );
	
	SetSizer( contents );
	Layout();	
}

// update dialog controls in accordance with cfg
void StubIoChannelConfigPaneWnd::updateControls( const EsDictionary& cfg )
{
}

// update config from controls
void StubIoChannelConfigPaneWnd::updateConfig(EsDictionary& cfg) const
{
	// get all controls data into config structure
	//
}

// reset button click handler
//void StubIoChannelConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
//{
//	m_intf.setChannelConfigString(EsString::s_null);	
//}