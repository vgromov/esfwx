#include "stdafx.h"
#pragma hdrstop

#include <wx/spinctrl.h>
#include "EsProtocolConfigPane.h"

EsProtocolConfigPane::EsProtocolConfigPane(wxWindow& parent) : 
wxPanel(&parent),
m_packetTmo(0),
m_packetRetries(0),
m_slaveAddr(0)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	wxASSERT(sizer);
	
	wxFlexGridSizer* ctls = new wxFlexGridSizer(2);
	wxASSERT(ctls);	
	ctls->AddGrowableCol( 1 );
	ctls->SetFlexibleDirection( wxHORIZONTAL );
	ctls->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );	
	
	wxButton* reset = new wxButton( this, wxID_ANY, _("Reset to defaults") );
	wxASSERT(reset);
	sizer->Add(reset, wxSizerFlags().Border());
	
	// packet tmo
	wxStaticText* lbl = new wxStaticText(this, wxID_ANY, ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetTimeout)->getDescr() + wxT(":"));	
	wxASSERT(lbl);
	ctls->Add(lbl, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT).Border());
	m_packetTmo = new wxSpinCtrl(this, wxID_ANY);
	wxASSERT(m_packetTmo);
	const PropertyInfo::Restriction* range = &ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetTimeout)->getRestriction();
	wxASSERT(PropertyInfo::rxtRange == range->getType());
	m_packetTmo->SetRange(range->getVal().getItem(0).asLong(), range->getVal().getItem(1).asLong());
	m_packetTmo->SetValue(ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetTimeout)->getDefault().asLong());
	ctls->Add(m_packetTmo, wxSizerFlags().Expand().Border());
	// packet retries
	lbl = new wxStaticText(this, wxID_ANY, ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetRetries)->getDescr() + wxT(":"));	
	wxASSERT(lbl);
	ctls->Add(lbl, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT).Border());
	m_packetRetries = new wxSpinCtrl(this, wxID_ANY);
	wxASSERT(m_packetRetries);
	range = &ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetRetries)->getRestriction();
	wxASSERT(PropertyInfo::rxtRange == range->getType());
	m_packetRetries->SetRange(range->getVal().getItem(0).asLong(), range->getVal().getItem(1).asLong());
	m_packetRetries->SetValue(ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetRetries)->getDefault().asLong());
	ctls->Add(m_packetRetries, wxSizerFlags().Expand().Border());
	// slave address
	lbl = new wxStaticText(this, wxID_ANY, ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, clientAddr)->getDescr() + wxT(":"));	
	wxASSERT(lbl);
	ctls->Add(lbl, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT).Border());
	m_slaveAddr = new wxSpinCtrl(this, wxID_ANY);
	wxASSERT(m_slaveAddr);
	range = &ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, clientAddr)->getRestriction();
	wxASSERT(PropertyInfo::rxtRange == range->getType());
	m_slaveAddr->SetRange(range->getVal().getItem(0).asLong(), range->getVal().getItem(1).asLong());
	m_slaveAddr->SetValue(ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, clientAddr)->getDefault().asLong());	
	ctls->Add(m_slaveAddr, wxSizerFlags().Expand().Border());
	// finalize sizer
	sizer->Add(ctls, wxSizerFlags().Expand().Border(wxALL, 0));
	SetSizer(sizer);
	
	// connect events
	reset->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &EsProtocolConfigPane::onResetToDefaults, this);
}

void EsProtocolConfigPane::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	setProtocolConfigString(EsString::s_null);
}

EsString EsProtocolConfigPane::getProtocolConfigString() const
{
	EsDictionary dic( EsRpcMaster::getClassNameStatic() );
	updateConfig(dic);
	
	return dic.getAsString();
}

void EsProtocolConfigPane::setProtocolConfigString(const EsString& cfg)
{
	EsDictionary dic;
	dic.setAsString(cfg);
	updateControls(dic);
}

void EsProtocolConfigPane::updateControls(const EsDictionary& cfg)
{
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetTimeout);
	m_packetTmo->SetValue( cfg.getValueDef(info->getName(), info->getDefault().asLong()).asLong() );
	info = ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetRetries);
	m_packetRetries->SetValue( cfg.getValueDef(info->getName(), info->getDefault().asLong()).asLong() );
	info = ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, clientAddr);
	m_slaveAddr->SetValue( cfg.getValueDef(info->getName(), info->getDefault().asLong()).asLong() );
}

void EsProtocolConfigPane::updateConfig(EsDictionary& cfg) const
{
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetTimeout);
	cfg.setValue( info->getName(), m_packetTmo->GetValue(), true );
	info = ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, packetRetries);
	cfg.setValue( info->getName(), m_packetRetries->GetValue(), true );
	info = ES_REFLECTED_PROP_INFO_GET(EsRpcMaster, clientAddr);
	cfg.setValue( info->getName(), m_slaveAddr->GetValue(), true );
}
