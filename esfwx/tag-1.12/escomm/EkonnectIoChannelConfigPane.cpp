#include <stdafx.h>
#pragma hdrstop

#include <wx/spinctrl.h>
#include "EkonnectIoChannelConfigPane.h"
#include "res/rescan.xpm"

// configuration pane interface implementor proxy
//
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPane, EkonnectIoChannelConfigPane, _i("UART channel configuration GUI."))
	// class services
	ES_DECL_REFLECTED_CTOR_INFO(EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPane, EsBaseIntfPtr_ClassCall_p_wxObject, _i("EkonnectIoChannelConfigPane constructor."))
ES_DECL_CLASS_INFO_END

EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPane::~EkonnectIoChannelConfigPane() {}

ES_IMPL_INTF_METHOD(void, EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPane::setChannelConfigString)(const EsString& config) 
{ 
	EsDictionary cfg(EkonnectIoChannel::getClassNameStatic(), config);
	if( 0 != cfg.getName().compare( EkonnectIoChannel::getClassNameStatic() ) )
		EsException::Throw(0, EsException::Generic, _("Wrong class name in configuration string. Expected %s, got %s."),
		EkonnectIoChannel::getClassNameStatic().c_str(), cfg.getName().c_str() );
	m_this.updateControls( cfg );
}

ES_IMPL_INTF_METHOD(EsString, EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPane::getChannelConfigString)() const
{
	EsDictionary cfg(EkonnectIoChannel::getClassNameStatic());
	m_this.updateConfig(cfg);

	return cfg.getAsString();
}

// configuration pane itself
//
EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPaneWnd(wxWindow* parent) :
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
	
	m_lblDevice = new wxStaticText( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblDevice->Wrap( -1 );
	controlsStd->Add( m_lblDevice, 0, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	m_edDeviceName = new wxComboBox( m_pnlStd, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	box->Add(m_edDeviceName, 1, wxEXPAND|wxALIGN_CENTER_VERTICAL);
	box->AddSpacer(5);
	m_btnRescan = new wxBitmapButton( m_pnlStd, wxID_ANY, wxBitmap(rescan_xpm, wxBITMAP_TYPE_XPM));
	m_btnRescan->SetToolTip( _("Rescan EKONNECT devices") );
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
	
	m_lblRxTmo = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblRxTmo->Wrap( -1 );
	ctlsGrid->Add( m_lblRxTmo, 0, wxALL|wxEXPAND, 5 );
	
	m_edRxTmo = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRxTmo, 1, wxALL|wxEXPAND, 5 );
	
	m_lblTxTmo = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblTxTmo->Wrap( -1 );
	ctlsGrid->Add( m_lblTxTmo, 0, wxALL|wxEXPAND, 5 );
	
	m_edTxTmo = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edTxTmo, 1, wxALL|wxEXPAND, 5 );
	
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
	
	m_lblRebootDelay = new wxStaticText( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_lblRebootDelay->Wrap( -1 );
	ctlsGrid->Add( m_lblRebootDelay, 0, wxALL|wxEXPAND, 5 );
	
	m_edRebootDelay = new wxSpinCtrl( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 10, 0 );
	ctlsGrid->Add( m_edRebootDelay, 1, wxALL|wxEXPAND, 5 );
	
	controlsAdvanced->Add( ctlsGrid, 0, wxEXPAND, 5 );
	
	m_chkResetOnRxTmo = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	controlsAdvanced->Add( m_chkResetOnRxTmo, 0, wxALL|wxEXPAND, 5 );
	
	m_chkUseStandardSocket = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	controlsAdvanced->Add( m_chkUseStandardSocket, 0, wxALL|wxEXPAND, 5 );
	
	m_chkSupplyAuxPower = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	controlsAdvanced->Add( m_chkSupplyAuxPower, 0, wxALL|wxEXPAND, 5 );
	
	m_chkProgrammingMode = new wxCheckBox( m_pnlAdvanced, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	
	controlsAdvanced->Add( m_chkProgrammingMode, 0, wxALL|wxEXPAND, 5 );
	
	m_pnlAdvanced->SetSizer( controlsAdvanced );
	m_pnlAdvanced->Layout();
	controlsAdvanced->Fit( m_pnlAdvanced );
	m_settings->AddPage( m_pnlAdvanced, _("Advanced"), false );
	
	contents->Add( m_settings, 1, wxEXPAND | wxALL, 5 );
	
	SetSizer( contents );
	Layout();
	
	// set editor labels
	m_lblDevice->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, device)->getDescr() + wxT(":") );
	m_lblBaud->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, baud)->getDescr() + wxT(":") );
	m_lblByteSize->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, bits)->getDescr() + wxT(":") );
	m_lblParity->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, parity)->getDescr() + wxT(":") );
	m_lblStopBits->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, stopBits)->getDescr() + wxT(":") );
	m_lblRxTmo->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxTmo)->getDescr() + wxT(":") );
	m_lblTxTmo->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txTmo)->getDescr() + wxT(":") );
	m_lblRxBuffer->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxBuffLen)->getDescr() + wxT(":") );
	m_lblTxBuffer->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txBuffLen)->getDescr() + wxT(":") );
	m_lblRebootDelay->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rebootDelay)->getDescr() + wxT(":") );
	m_chkResetOnRxTmo->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, resetOnRxTmo)->getDescr() );
	m_chkUseStandardSocket->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, useStandardSocket)->getDescr() );
	m_chkSupplyAuxPower->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, supplyAuxPower)->getDescr() );
	m_chkProgrammingMode->SetLabel( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, programmingMode)->getDescr() );

	// set buffer controls restrictions && defaults
	const PropertyInfo::Restriction& rxTmoRange = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxTmo)->getRestriction();
	m_edRxTmo->SetRange(rxTmoRange.getVal().getItem(0).asLong(), rxTmoRange.getVal().getItem(1).asLong());
	m_edRxTmo->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxTmo)->getDefault().asLong());
	const PropertyInfo::Restriction& txTmoRange = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txTmo)->getRestriction();
	m_edTxTmo->SetRange(txTmoRange.getVal().getItem(0).asLong(), txTmoRange.getVal().getItem(1).asLong());
	m_edTxTmo->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txTmo)->getDefault().asLong());
	const PropertyInfo::Restriction& rxRange = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxBuffLen)->getRestriction();
	m_edRxBuff->SetRange(rxRange.getVal().getItem(0).asLong(), rxRange.getVal().getItem(1).asLong());
	m_edRxBuff->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxBuffLen)->getDefault().asLong());
	const PropertyInfo::Restriction& txRange = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txBuffLen)->getRestriction();
	m_edTxBuff->SetRange(txRange.getVal().getItem(0).asLong(), txRange.getVal().getItem(1).asLong());
	m_edTxBuff->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txBuffLen)->getDefault().asLong());
	const PropertyInfo::Restriction& rebootDlyRange = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rebootDelay)->getRestriction();
	m_edRebootDelay->SetRange(rebootDlyRange.getVal().getItem(0).asLong(), rebootDlyRange.getVal().getItem(1).asLong());
	m_edRebootDelay->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rebootDelay)->getDefault().asLong());
	m_chkResetOnRxTmo->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, resetOnRxTmo)->getDefault().asBool());
	m_chkUseStandardSocket->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, useStandardSocket)->getDefault().asBool());
	m_chkSupplyAuxPower->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, supplyAuxPower)->getDefault().asBool());
	m_chkProgrammingMode->SetValue(ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, programmingMode)->getDefault().asBool());

	// populate selectors
	//
	// ports
	populateDevices();
	// bauds
	populateBauds();
	// bits
	populateBits();
	// parity
	populateParity();
	// stop bits
	populateStopBits();
	// adjust width to fit longest string inside porn names list
	wxSize sze = m_edDeviceName->GetClientSize();
	sze.x -= wxSystemSettings::GetMetric( wxSYS_HTHUMB_X ) + 2 * wxSystemSettings::GetMetric( wxSYS_EDGE_X );
	EsString::Array strs = EsUtilities::fromArrayString(m_edDeviceName->GetStrings());
	int delta = wxMax( EsUtilities::getLongestStringWidth(strs, *m_edDeviceName, GetFont()), sze.x) - sze.x;
	if( 0 < delta )
	{
		sze = GetClientSize();
		sze.x += delta;
		SetClientSize(sze);
		Layout();
	}
	
	// Connect Events
	m_btnResetToDefaults->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EkonnectIoChannelConfigPaneWnd::onResetToDefaults, this );
	m_btnRescan->Bind( wxEVT_COMMAND_BUTTON_CLICKED, &EkonnectIoChannelConfigPaneWnd::onRescan, this );
}

// populate port selector
void EkonnectIoChannelConfigPaneWnd::populateDevices()
{
	m_edDeviceName->Clear();
	// enumerate Ekonnect communication devices on this machine
	FtdiDriver::DeviceList devices = EkonnectIoChannel::getDeviceList();
	// fill-in device selector with user-friendly device names
	for(size_t idx = 0; idx < devices.size(); ++idx)
	{
		m_edDeviceName->AppendString( devices[idx].getSerialNumberStr() );
	}

	// set default port
	if( !devices.empty() )
	{
		m_edDeviceName->SetValue( devices[0].getSerialNumberStr() );
	}
}

// populate baud rate selector
void EkonnectIoChannelConfigPaneWnd::populateBauds()
{
	m_edBaud->Clear();
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, baud);
	EsString::Array bauds = info->getAllLabels();
	wxArrayString _bauds = EsUtilities::toArrayString(bauds);
	m_edBaud->Append( _bauds );
	m_edBaud->SetValue( info->getValueLabel(info->getDefault()).c_str());
}

// populate bit size selector
void EkonnectIoChannelConfigPaneWnd::populateBits()
{
	m_edByteSize->Clear();	
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, bits);
	EsString::Array bits = info->getAllLabels();
	wxArrayString _bits = EsUtilities::toArrayString(bits);
	m_edByteSize->Append( _bits );
	m_edByteSize->SetValue( info->getValueLabel(info->getDefault()).c_str() );
}

// populate parity selector
void EkonnectIoChannelConfigPaneWnd::populateParity()
{
	m_edParity->Clear();
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, parity);
	EsString::Array parity = info->getAllLabels();
	wxArrayString _parity = EsUtilities::toArrayString(parity);
	m_edParity->Append( _parity );
	m_edParity->SetValue( info->getValueLabel( info->getDefault()).c_str() );
}

// populate stop bits
void EkonnectIoChannelConfigPaneWnd::populateStopBits()
{
	m_edStopBits->Clear();
	const PropertyInfo* info = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, stopBits);
	EsString::Array stopBits = info->getAllLabels();
	wxArrayString _stopBits = EsUtilities::toArrayString(stopBits);
	m_edStopBits->Append( _stopBits );
	m_edStopBits->SetValue( info->getValueLabel( info->getDefault()).c_str() );
}

// update dialog controls in accordance with cfg
void EkonnectIoChannelConfigPaneWnd::updateControls( const EsDictionary& cfg )
{
	const PropertyInfo* propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, device);
	// device
	m_edDeviceName->SetValue( cfg.getValueDef( propInfo->getName(), propInfo->getDefault() ).asString().c_str() );
	// baud
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, baud);
	m_edBaud->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// bits
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, bits);
	m_edByteSize->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// parity
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, parity);
	m_edParity->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// stop bits
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, stopBits);
	m_edStopBits->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asString().c_str() );
	// set advanced controls
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxTmo);
	m_edRxTmo->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txTmo);
	m_edTxTmo->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxBuffLen);
	m_edRxBuff->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txBuffLen);
	m_edTxBuff->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rebootDelay);
	m_edRebootDelay->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asLong() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, resetOnRxTmo);
	m_chkResetOnRxTmo->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asBool() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, useStandardSocket);
	m_chkUseStandardSocket->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asBool() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, supplyAuxPower);
	m_chkSupplyAuxPower->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asBool() );
	propInfo = ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, programmingMode);
	m_chkProgrammingMode->SetValue( cfg.getValueDef(propInfo->getName(), propInfo->getDefault()).asBool() );	
}

// update config from controls
void EkonnectIoChannelConfigPaneWnd::updateConfig(EsDictionary& cfg) const
{
	// get all controls data into config structure
	//
	// device 
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, device)->getName(), m_edDeviceName->GetValue().wc_str(), true );			
	// baud
	long tmp;
	m_edBaud->GetValue().ToLong(&tmp);
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, baud)->getName(), tmp, true );
	// bits
	m_edByteSize->GetValue().ToLong(&tmp);
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, bits)->getName(), tmp, true );
	// parity
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, parity)->getName(), m_edParity->GetSelection(), true );
	// stop bits
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, stopBits)->getName(), m_edStopBits->GetSelection(), true );
	// advanced values
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxTmo)->getName(), m_edRxTmo->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txTmo)->getName(), m_edTxTmo->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rxBuffLen)->getName(), m_edRxBuff->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, txBuffLen)->getName(), m_edTxBuff->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, rebootDelay)->getName(), m_edRebootDelay->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, resetOnRxTmo)->getName(), m_chkResetOnRxTmo->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, useStandardSocket)->getName(), m_chkUseStandardSocket->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, supplyAuxPower)->getName(), m_chkSupplyAuxPower->GetValue(), true );
	cfg.setValue( ES_REFLECTED_PROP_INFO_GET(EkonnectIoChannel, programmingMode)->getName(), m_chkProgrammingMode->GetValue(), true );
}

// reset button click handler
void EkonnectIoChannelConfigPaneWnd::onResetToDefaults(wxCommandEvent& WXUNUSED(evt))
{
	m_intf.setChannelConfigString(EsString::s_null);
}

// rescan button click
void EkonnectIoChannelConfigPaneWnd::onRescan(wxCommandEvent& WXUNUSED(evt))
{
	populateDevices();
}
