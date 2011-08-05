#include "StdAfx.h"
#pragma hdrstop

#include "CommConfigDlg.h"

CommConfigDlg::CommConfigDlg(wxWindow* parent, bool showProtocolPane /*= false*/,
	wxConfigBase* cfg /*= 0*/, const EsString& path /*= EsString::s_null*/) :
wxDialog(parent, wxID_ANY, _("Communication channel configuration"), wxDefaultPosition),
m_btnOK( new wxButton( this, wxID_OK ) ),
m_btnCancel( new wxButton( this, wxID_CANCEL ) ),
m_cfgPane(0),
m_protoCfgPane(0),
m_showProtocolPane(showProtocolPane)
{
	SetClientSize( wxSize(380, 300) );
	wxBoxSizer* contents = new wxBoxSizer( wxVERTICAL );
	wxStdDialogButtonSizer* buttons = new wxStdDialogButtonSizer();
	
	buttons->AddButton( m_btnOK );
	buttons->AddButton( m_btnCancel );
	buttons->Realize();
	
	if( m_showProtocolPane )
	{
		wxNotebook* nb = new wxNotebook(this, wxID_ANY);
		m_cfgPane = new CommConfigPane(nb, cfg, path);
		wxASSERT(m_cfgPane);
		nb->AddPage(m_cfgPane, _("Channel"), true);
		m_protoCfgPane = new EsProtocolConfigPane(*nb);
		wxASSERT(m_protoCfgPane);
		nb->AddPage(m_protoCfgPane, _("Protocol"));
		contents->Add(nb, wxSizerFlags().Expand());
	}
	else
	{
		m_cfgPane = new CommConfigPane(this, cfg, path);
		wxASSERT(m_cfgPane);
		contents->Add(m_cfgPane, wxSizerFlags().Expand());		
	}	

	contents->Add( buttons, wxSizerFlags().Expand().Border() );
			
	SetSizer( contents );
	if( parent )
		EsUtilities::applyCtlsFont(this, parent->GetFont());
	Layout();
	Fit();

	CenterOnScreen();
}
