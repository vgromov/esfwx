#include "stdafx.h"
#pragma hdrstop

#include "SetDateTimeDlg.h"

SetDateTimeDlg::SetDateTimeDlg(wxWindow* parent) :
wxDialog(parent, wxID_ANY, _("Set Device Date and Time")),
m_pane( new SetDateTimePane(this) )
{
	wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
	wxASSERT(contents);
	contents->Add(m_pane, wxSizerFlags().Expand().Proportion(1));
	wxStdDialogButtonSizer* btns = new wxStdDialogButtonSizer;
	wxASSERT(btns);
	wxButton* btn = new wxButton(this, wxID_OK);
	wxASSERT(btn);
	btn->SetDefault();
	btns->AddButton(btn);
	btn = new wxButton(this, wxID_CANCEL);
	wxASSERT(btn);
	btns->AddButton(btn);
	btns->Realize();
	contents->Add(btns, wxSizerFlags().Expand().Border());
	SetSizer(contents);

	Layout();
	CenterOnScreen();
}
