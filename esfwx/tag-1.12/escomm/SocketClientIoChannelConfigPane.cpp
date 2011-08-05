#include "stdafx.h"
#pragma hdrstop

#include "SocketClientIoChannelConfigPane.h"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPane, SocketClientIoChannelConfigPane, _i("Client socket channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("SocketClientIoChannelConfigPane constructor."))
ES_DECL_CLASS_INFO_END

SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPane::~SocketClientIoChannelConfigPane() {}

ES_IMPL_INTF_METHOD(void, SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPane::setChannelConfigString)(const EsString& config) 
{ 
	EsDictionary cfg(EsString::s_null, config);
	if( cfg.getName() != SocketClientIoChannel::getClassNameStatic() )
		EsException::Throw(0, EsException::Generic, _("Wrong class name in configuration string. Expected %s, got %s."),
		SocketClientIoChannel::getClassNameStatic().c_str(), cfg.getName().c_str() );
	m_this.updateControls( cfg );
}

ES_IMPL_INTF_METHOD(EsString, SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPane::getChannelConfigString)() const
{
	EsDictionary cfg(SocketClientIoChannel::getClassNameStatic());
	m_this.updateConfig(cfg);

	return cfg.getAsString();
}

// configuration pane itself
//
SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this),
m_reset(new wxButton(this, wxID_ANY, _("Reset to defaults"))),
m_addr(new wxTextCtrl(this, wxID_ANY)),
m_port(new wxSpinCtrl(this, wxID_ANY)),
m_tmo(new wxSpinCtrl(this, wxID_ANY))
{
	wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
	wxASSERT(contents);
	contents->Add(m_reset, 0, wxALL, 5);

	wxFlexGridSizer* ctls = new wxFlexGridSizer(3, 2, 0, 0);
	wxASSERT(ctls);
	ctls->AddGrowableCol(1, 1);
	const PropertyInfo* prop = ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, target);
	wxStaticText* lbl = new wxStaticText(this, wxID_ANY, prop->getDescr() +	wxT(":") );
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	ctls->Add(m_addr, 1, wxALL|wxEXPAND, 5);
	prop = ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, targetPort);
	lbl = new wxStaticText(this, wxID_ANY, prop->getDescr() +	wxT(":") );
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	ctls->Add(m_port, 1, wxALL|wxEXPAND, 5);
	EsVariant range = prop->getRestriction().getVal();
	m_port->SetRange( range.getItem(0).asLong(), range.getItem(1).asLong() );
	prop = ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, operationTimeout);
	lbl = new wxStaticText(this, wxID_ANY, prop->getDescr() +	wxT(":") );
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	ctls->Add(m_tmo, 1, wxALL|wxEXPAND, 5);
	range = prop->getRestriction().getVal();
	m_tmo->SetRange( range.getItem(0).asLong(), range.getItem(1).asLong() );

	contents->Add(ctls, 1, wxALL|wxEXPAND, 0);

	SetSizer( contents );
	Layout();

	m_reset->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SocketClientIoChannelConfigPaneWnd::onResetToDefaults), 0, this);
	setDefaults();
}

SocketClientIoChannelConfigPaneWnd::~SocketClientIoChannelConfigPaneWnd()
{
	m_reset->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SocketClientIoChannelConfigPaneWnd::onResetToDefaults), 0, this);
}

// update dialog controls in accordance with m_cfg
void SocketClientIoChannelConfigPaneWnd::updateControls( const EsDictionary& cfg )
{
	m_addr->SetValue( 
		cfg.getValue(ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, target)->getName()).asString() );
	m_port->SetValue( 
		cfg.getValue(ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, targetPort)->getName()).asLong() );
	m_tmo->SetValue( 
		cfg.getValue(ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, operationTimeout)->getName()).asLong() );
}

// update config from controls
void SocketClientIoChannelConfigPaneWnd::updateConfig( EsDictionary& cfg ) const
{
	cfg.setName( SocketClientIoChannel::getClassNameStatic() );
	cfg.clearAll();
	cfg.setValue(ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, target)->getName(),
		m_addr->GetValue().t_str(), true);
	cfg.setValue(ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, targetPort)->getName(),
		m_port->GetValue(), true);
	cfg.setValue(ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, operationTimeout)->getName(),
		m_tmo->GetValue(), true);
}

// reset button click handler
void SocketClientIoChannelConfigPaneWnd::setDefaults()
{
	m_addr->SetValue( ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, target)->getDefault().asString() );
	m_port->SetValue( ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, targetPort)->getDefault().asLong() );
	m_tmo->SetValue( ES_REFLECTED_PROP_INFO_GET(SocketClientIoChannel, operationTimeout)->getDefault().asLong() );
}

void SocketClientIoChannelConfigPaneWnd::onResetToDefaults(wxCommandEvent& evt)
{
	setDefaults();
}
