#include "stdafx.h"
#pragma hdrstop

#include <wx/spinctrl.h>
#include "UartIoChannelConfigPane.h"
#include "res/rescan.xpm"

const wxChar* c_sysPortPrefix = wxT("\\\\.\\");

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(UartIoChannelConfigPaneWnd::UartIoChannelConfigPane, UartIoChannelConfigPane, _i("UART channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(UartIoChannelConfigPaneWnd::UartIoChannelConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("UartIoChannelConfigPane constructor."))
ES_DECL_CLASS_INFO_END

UartIoChannelConfigPaneWnd::UartIoChannelConfigPane::~UartIoChannelConfigPane() {}

ES_IMPL_INTF_METHOD(void, UartIoChannelConfigPaneWnd::UartIoChannelConfigPane::setChannelConfigString)(const EsString& config) 
{ 
	EsDictionary cfg(UartIoChannel::getClassNameStatic(), config);
	if( 0 != cfg.getName().compare( UartIoChannel::getClassNameStatic() ) )
		EsException::Throw(0, EsException::Generic, _("Wrong class name in configuration string. Expected %s, got %s."),
		UartIoChannel::getClassNameStatic().c_str(), cfg.getName().c_str() );
	m_this.updateControls( cfg );
}

ES_IMPL_INTF_METHOD(EsString, UartIoChannelConfigPaneWnd::UartIoChannelConfigPane::getChannelConfigString)() const
{
	EsDictionary cfg(UartIoChannel::getClassNameStatic());
	m_this.updateConfig(cfg);

	return cfg.getAsString();
}

// configuration pane itself
//
UartIoChannelConfigPaneWnd::UartIoChannelConfigPaneWnd(wxWindow* parent) :
wxPanel(parent),
m_intf(*this)
{
	wxBoxSizer* contents;
	contents = new wxBoxSizer( wxVERTICAL );
	
	m_btnResetToDefaults = new wxButton( this, wxID_ANY, _("Reset to defaults"), wxDefaultPosition, wxDefaultSize, 0 );
	contents->Add( m_btnResetToDefaults, 0, wxALL, 5 );
	
	m_settings = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	m_pnlStd = new wxPanel( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxFlexGridSizer* controlsStd;
	controlsStd = new wxFlexGridSizer( 0, 2, 0, 0 );
	controlsStd->AddGrowableCol( 1 );
	controlsStd->SetFlexibleDirection( wxHORIZONTAL );
	controlsStd->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lblPort = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPort->Wrap( -1 );
	controlsStd->Add( m_lblPort, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	m_edPortName = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	box->Add(m_edPortName, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL);
	box->AddSpacer(5);
	m_btnRescan = new wxBitmapButton( m_pnlStd, wxID_ANY, wxBitmap(rescan_xpm, wxBITMAP_TYPE_XPM));
	m_btnRescan->SetToolTip( _("Rescan serial ports") );
	box->Add(m_btnRescan, 0, wxALIGN_CENTER_VERTICAL);
	controlsStd->Add( box, 1, wxALL|wxEXPAND, 5 );
	
	m_lblBaud = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblBaud->Wrap( -1 );
	controlsStd->Add( m_lblBaud, 0, wxALL|wxEXPAND, 5 );
	
	m_edBaud = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edBaud, 1, wxALL|wxEXPAND, 5 );
	
	m_lblByteSize = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblByteSize->Wrap( -1 );
	controlsStd->Add( m_lblByteSize, 0, wxALL|wxEXPAND, 5 );
	
	m_edByteSize = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edByteSize, 1, wxALL|wxEXPAND, 5 );
	
	m_lblParity = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblParity->Wrap( -1 );
	controlsStd->Add( m_lblParity, 0, wxALL|wxEXPAND, 5 );
	
	m_edParity = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edParity, 1, wxALL|wxEXPAND, 5 );
	
	m_lblStopBits = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblStopBits->Wrap( -1 );
	controlsStd->Add( m_lblStopBits, 0, wxALL|wxEXPAND, 5 );
	
	m_edStopBits = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	controlsStd->Add( m_edStopBits, 1, wxALL|wxEXPAND, 5 );
	
	m_pnlStd->SetSizer( controlsStd );
	m_pnlStd->Layout();
	controlsStd->Fit( m_pnlStd );
	m_settings->AddPage( m_pnlStd, _("Generic"), true );
	m_pnlAdvanced = new wxPanel( m_settings, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* controlsAdvanced;
	controlsAdvanced = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* ctlsGrid;
	ctlsGrid = new wxFlexGridSizer( 0, 2, 0, 0 );
	ctlsGrid->AddGrowableCol( 1 );
	ctlsGrid->SetFlexibleDirection( wxHORIZONTAL );
	ctlsGrid->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_lblRxBuffer = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblRxBuffer->Wrap( -1 );
	ctlsGrid->Add( m_lblRxBuffer, 0, wxALL|wxEXPAND, 5 );
	
	m_edRxBuff = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRxBuff, 1, wxALL|wxEXPAND, 5 );
	
	m_lblTxBuffer = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTxBuffer->Wrap( -1 );
	ctlsGrid->Add( m_lblTxBuffer, 0, wxALL|wxEXPAND, 5 );
	
	m_edTxBuff = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edTxBuff, 1, wxALL|wxEXPAND, 5 );
	
	controlsAdvanced->Add( ctlsGrid, 0, wxEXPAND, 5 );
	
	m_chkResetOnRxTmo = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	controlsAdvanced->Add( m_chkResetOnRxTmo, 0, wxALL|wxEXPAND, 5 );
	
	m_pnlAdvanced->SetSizer( controlsAdvanced );
	m_pnlAdvanced->Layout();
	controlsAdvanced->Fit( m_pnlAdvanced );
	m_settings->AddPage( m_pnlAdvanced, _("Advanced"), false );
	
	contents->Add( m_settings, 1, wxEXPAND | wxALL, 5 );
	SetSizer( contents );
	Layout();	
	
	// set editor labels
	m_lblPort->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, port)->getDescr() + wxT(":") );
	m_lblBaud->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, baud)->getDescr() + wxT(":") );
	m_lblByteSize->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, bits)->getDescr() + wxT(":") );
	m_lblParity->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, parity)->getDescr() + wxT(":") );
	m_lblStopBits->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, stopBits)->getDescr() + wxT(":") );
	m_lblRxBuffer->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, rxBuffLen)->getDescr() + wxT(":") );
	m_lblTxBuffer->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, txBuffLen)->getDescr() + wxT(":") );
	m_chkResetOnRxTmo->SetLabel( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, resetOnRxTmo)->getDescr() );

	// set buffer controls restrictions && defaults
	const PropertyInfo::Restriction& rxRange = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, rxBuffLen)->getRestriction();
	m_edRxBuff->SetRange(rxRange.getVal().getItem(0).asLong(), rxRange.getVal().getItem(1).asLong());
	m_edRxBuff->SetValue(ES_REFLECTED_PROP_INFO_GET(UartIoChannel, rxBuffLen)->getDefault().asLong());
	const PropertyInfo::Restriction& txRange = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, txBuffLen)->getRestriction();
	m_edTxBuff->SetRange(txRange.getVal().getItem(0).asLong(), txRange.getVal().getItem(1).asLong());
	m_edTxBuff->SetValue(ES_REFLECTED_PROP_INFO_GET(UartIoChannel, txBuffLen)->getDefault().asLong());
	m_chkResetOnRxTmo->SetValue(ES_REFLECTED_PROP_INFO_GET(UartIoChannel, resetOnRxTmo)->getDefault().asBool());
	// populate selectors
	//
	// ports
	populatePorts();
	// bauds
	populateBauds();
	// bits
	populateBits();
	// parity
	populateParity();
	// stop bits
	populateStopBits();
	// adjust width to fit longest string inside port names list
	wxSize sze = m_edPortName->GetClientSize();
	sze.x -= wxSystemSettings::GetMetric( wxSYS_HTHUMB_X ) + 2 * wxSystemSettings::GetMetric( wxSYS_EDGE_X );
	EsString::Array strs = EsUtilities::fromArrayString(m_edPortName->GetStrings());
	int delta = wxMax( EsUtilities::getLongestStringWidth(strs, *m_edPortName, GetFont()), sze.x) - sze.x;
	if( 0 < delta )
	{
		sze = GetClientSize();
		sze.x += delta;
		SetClientSize(sze);
		Layout();
	}
	
	// Connect Events
	m_btnResetToDefaults->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &UartIoChannelConfigPaneWnd::onResetToDefaults, this );
	m_btnRescan->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &UartIoChannelConfigPaneWnd::onRescan, this );
}

// populate port selector
void UartIoChannelConfigPaneWnd::populatePorts()
{
	m_edPortName->Clear();
	// enumerate communication ports on this machine, ignoring busy ones
	m_ports.enumerate(true);
	// fill-in port selector with user-friendly port names, ignoring modems
	for(size_t idx = 0; idx < m_ports.size(); ++idx)
	{
		if( !m_ports[idx].m_bModem )
			m_edPortName->AppendString( m_ports[idx].m_strFriendlyName.c_str() );
	}

	// set default port
	if( m_ports.size() )
	{
		wxString simpleName;
		wxString portName = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, port)->getDefault().asString().c_str();
		if( !portName.StartsWith(c_sysPortPrefix, &simpleName) )
			simpleName = portName;
		const EsCommPortInfo& info = m_ports.findPortInfoByPortName(simpleName);
		if( info.isOk() )
			m_edPortName->SetValue(info.m_strFriendlyName.c_str());
		else
			m_edPortName->SetValue(wxEmptyString);
	}
}

// populate baud rate selector
void UartIoChannelConfigPaneWnd::populateBauds()
{
	m_edBaud->Clear();
	EsString::Array bauds = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, baud)->getAllLabels();
	wxArrayString _bauds = EsUtilities::toArrayString(bauds);
	m_edBaud->Append( _bauds );
	m_edBaud->SetValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, baud)->getValueLabel(
		ES_REFLECTED_PROP_INFO_GET(UartIoChannel, baud)->getDefault()).c_str() );
}

// populate bit size selector
void UartIoChannelConfigPaneWnd::populateBits()
{
	m_edByteSize->Clear();	
	EsString::Array bits = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, bits)->getAllLabels();
	wxArrayString _bits = EsUtilities::toArrayString(bits);
	m_edByteSize->Append( _bits );
	m_edByteSize->SetValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, bits)->getValueLabel(
		ES_REFLECTED_PROP_INFO_GET(UartIoChannel, bits)->getDefault()).c_str() );
}

// populate parity selector
void UartIoChannelConfigPaneWnd::populateParity()
{
	m_edParity->Clear();
	EsString::Array parity = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, parity)->getAllLabels();
	wxArrayString _parity = EsUtilities::toArrayString(parity);
	m_edParity->Append( _parity );
	m_edParity->SetValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, parity)->getValueLabel(
		ES_REFLECTED_PROP_INFO_GET(UartIoChannel, parity)->getDefault()).c_str() );
}

// populate stop bits
void UartIoChannelConfigPaneWnd::populateStopBits()
{
	m_edStopBits->Clear();
	EsString::Array stopBits = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, stopBits)->getAllLabels();
	wxArrayString _stopBits = EsUtilities::toArrayString(stopBits);
	m_edStopBits->Append( _stopBits );
	m_edStopBits->SetValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, stopBits)->getValueLabel(
		ES_REFLECTED_PROP_INFO_GET(UartIoChannel, stopBits)->getDefault()).c_str() );
}

// update dialog controls in accordance with cfg
void UartIoChannelConfigPaneWnd::updateControls( const EsDictionary& cfg )
{
	const PropertyInfo* propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, port);
	// port
	wxString simpleName;
	wxString portName = cfg.getValueDef( propInfo->getName(), propInfo->getDefault() ).asString().c_str();
	if( !portName.StartsWith(c_sysPortPrefix, &simpleName) )
		simpleName = portName;
	const EsCommPortInfo& info = m_ports.findPortInfoByPortName(simpleName);
	if( info.isOk() )
		m_edPortName->SetValue(info.m_strFriendlyName.c_str());
	else
		m_edPortName->SetValue(wxEmptyString);
	// baud
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, baud);
	m_edBaud->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// bits
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, bits);
	m_edByteSize->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// parity
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, parity);
	m_edParity->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// stop bits
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, stopBits);
	m_edStopBits->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// set advanced controls
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, rxBuffLen);
	m_edRxBuff->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, txBuffLen);
	m_edTxBuff->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(UartIoChannel, resetOnRxTmo);
	m_chkResetOnRxTmo->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asBool() );
}

// update config from controls
void UartIoChannelConfigPaneWnd::updateConfig(EsDictionary& cfg) const
{
	// get all controls data into config structure
	//
	// port 
	EsCommPortInfo info = m_ports.findPortInfoByFriendlyName( m_edPortName->GetValue() );
	if( info.isOk() ) 
		cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, port)->getName(), EsString(c_sysPortPrefix) + info.m_strPortName, true );
	else
		cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, port)->getName(), EsString::s_null, true );
	// baud
	long tmp;
	m_edBaud->GetValue().ToLong(&tmp);
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, baud)->getName(), tmp, true );
	// bits
	m_edByteSize->GetValue().ToLong(&tmp);
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, bits)->getName(), tmp, true );
	// parity
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, parity)->getName(), m_edParity->GetSelection(), true );
	// stop bits
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, stopBits)->getName(), m_edStopBits->GetSelection(), true );
	// advanced values
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, rxBuffLen)->getName(), m_edRxBuff->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, txBuffLen)->getName(), m_edTxBuff->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(UartIoChannel, resetOnRxTmo)->getName(), m_chkResetOnRxTmo->GetValue(), true );
}

// reset button click handler
void UartIoChannelConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_intf.setChannelConfigString(EsString::s_null);	
}

void UartIoChannelConfigPaneWnd::onRescan(wxCommandEvent& WXUNUSED(evt))
{
	populatePorts();
}
