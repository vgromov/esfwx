#include <stdafx.h>
#pragma hdrstop

#include "CustomizeShellDlg.h"

CustomizeShellDlg::CustomizeShellDlg(wxWindow* parent) :
wxDialog(parent, wxID_ANY, _("Customize user interface"), wxDefaultPosition),
m_pickMenuFont(0),
m_pickCtlsFont(0)
{
	wxASSERT(parent);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	wxASSERT(sizer);
	// add font selectors
	wxFlexGridSizer* ctls = new wxFlexGridSizer(2);
	wxASSERT(ctls);
	ctls->SetFlexibleDirection(wxHORIZONTAL);
	ctls->AddGrowableCol(0);
	ctls->AddGrowableCol(1,1);
	wxStaticText* lbl = new wxStaticText(this, wxID_ANY, _("Menu font:"));
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	m_pickMenuFont = new wxFontPickerCtrl(this, IDC_MENU_FONT_PICKER);
	wxASSERT(m_pickMenuFont);
	m_pickMenuFont->SetPickerCtrlGrowable();
	ctls->Add(m_pickMenuFont, 1, wxEXPAND|wxALL, 5);
	lbl = new wxStaticText(this, wxID_ANY, _("GUI elements font:"));
	wxASSERT(lbl);
	ctls->Add(lbl, 0, wxALL|wxEXPAND, 5);
	m_pickCtlsFont = new wxFontPickerCtrl(this, IDC_CTLS_FONT_PICKER);
	wxASSERT(m_pickCtlsFont);
	m_pickCtlsFont->SetPickerCtrlGrowable();
	ctls->Add(m_pickCtlsFont, 1, wxEXPAND|wxALL, 5);
	// configure buttons sizer
	wxStdDialogButtonSizer* btns = new wxStdDialogButtonSizer;
	wxASSERT(btns);
	wxButton* btn = new wxButton(this, wxID_CANCEL);
	wxASSERT(btn);
	btns->AddButton(btn);
	btn = new wxButton(this, wxID_OK);
	wxASSERT(btn);
	btn->SetDefault();
	btns->AddButton(btn);
	btns->Realize();
	// add nested sizers && spacers
	sizer->Add(ctls, 0, wxEXPAND|wxALL, 5);
	sizer->AddStretchSpacer();
	sizer->Add(btns, 0, wxEXPAND|wxALL, 5);
	// add dialog sizer
	SetSizer(sizer);
	EsUtilities::applyCtlsFont(this, parent->GetFont());
	Layout();
	Fit();
	Center();
	//// connect event handlers
	//Connect(IDC_CTLS_FONT_PICKER, IDC_MENU_FONT_PICKER, wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler(CustomizeShellDlg::onFontPickerChanged));
}

CustomizeShellDlg::~CustomizeShellDlg()
{
	//// disconnect event handlers
	//Disconnect(IDC_CTLS_FONT_PICKER, IDC_MENU_FONT_PICKER, wxEVT_COMMAND_FONTPICKER_CHANGED, wxFontPickerEventHandler(CustomizeShellDlg::onFontPickerChanged));
}

//// event handlers
//void CustomizeShellDlg::onFontPickerChanged(wxFontPickerEvent& evt)
//{
//}

