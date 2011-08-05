#include "stdafx.h"
#pragma hdrstop

#include "SocketServerIoChannelConfigPane.h"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPane, SocketServerIoChannelConfigPane, _i("Client socket channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("SocketServerIoChannelConfigPane constructor."))
ES_DECL_CLASS_INFO_END

SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPane::~SocketServerIoChannelConfigPane() {}

ES_IMPL_INTF_METHOD(void, SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPane::setChannelConfigString)(const EsString& config) 
{ 
	EsDictionary cfg(EsString::s_null, config);
	if( cfg.getName() != SocketServerIoChannel::getClassNameStatic() )
		EsException::Throw(0, EsException::Generic, _("Wrong class name in configuration string. Expected %s, got %s."),
		SocketServerIoChannel::getClassNameStatic().c_str(), cfg.getName().c_str() );
	m_this.updateControls( cfg );
}

ES_IMPL_INTF_METHOD(EsString, SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPane::getChannelConfigString)() const
{
	EsDictionary cfg(SocketServerIoChannel::getClassNameStatic());
	m_this.updateConfig(cfg);

	return cfg.getAsString();
}

// configuration pane itself
//
SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this),
m_reset(new wxButton(this, wxID_ANY, _("Reset to defaults"))),
m_port(new wxSpinCtrl(this, wxID_ANY)),
m_tmo(new wxSpinCtrl(this, wxID_ANY))
{
	wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
	wxASSERT(contents);
	contents->Add(m_reset, 0, wxALL, 5);

	wxFlexGridSizer* ctls = new wxFlexGridSizer(2, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	const PropertyInfo* prop = ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, port);
	wxStaticText* lbl = new wxStaticText(this, wxID_ANY, prop->getDescr() +	wxT(":") );
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	ctls->Add(m_port, 1, wxALL|wxEXPAND, 5);
	EsVariant range = prop->getRestriction().getVal();
	m_port->SetRange( range.getItem(0).asLong(), range.getItem(1).asLong() );
	prop = ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, operationTimeout);
	lbl = new wxStaticText(this, wxID_ANY, prop->getDescr() +	wxT(":") );
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	ctls->Add(m_tmo, 1, wxALL|wxEXPAND, 5);
	range = prop->getRestriction().getVal();
	m_tmo->SetRange( range.getItem(0).asLong(), range.getItem(1).asLong() );
	contents->Add(ctls, 1, wxALL|wxEXPAND, 0);	

	SetSizer( contents );
	Layout();

	m_reset->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SocketServerIoChannelConfigPaneWnd::onResetToDefaults), 0, this);
	setDefaults();
}

SocketServerIoChannelConfigPaneWnd::~SocketServerIoChannelConfigPaneWnd()
{
	m_reset->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SocketServerIoChannelConfigPaneWnd::onResetToDefaults), 0, this);
}

// update dialog controls in accordance with m_cfg
void SocketServerIoChannelConfigPaneWnd::updateControls( const EsDictionary& cfg )
{
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, port);
	m_port->SetValue( cfg.getValueDef(info->getName(), info->getDefault() ).asLong() );
	info = ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, operationTimeout);
	m_tmo->SetValue( cfg.getValueDef(info->getName(),	info->getDefault() ).asLong() );		
}

// update config from controls
void SocketServerIoChannelConfigPaneWnd::updateConfig( EsDictionary& cfg ) const
{
	cfg.setName( SocketServerIoChannel::getClassNameStatic() );
	cfg.clearAll();
	cfg.setValue(ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, port)->getName(),
		m_port->GetValue(), true);
	cfg.setValue(ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, operationTimeout)->getName(),
		m_tmo->GetValue(), true);
}

// reset button click handler
void SocketServerIoChannelConfigPaneWnd::setDefaults()
{
	m_port->SetValue( ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, port)->getDefault().asLong() );
	m_tmo->SetValue( ES_REFLECTED_PROP_INFO_GET(SocketServerIoChannel, operationTimeout)->getDefault().asLong() );
}

void SocketServerIoChannelConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	setDefaults();
}
