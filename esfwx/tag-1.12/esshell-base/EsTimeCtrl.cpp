#include "stdafx.h"
#pragma hdrstop

#include "EsTimeCtrl.h"

EsTimeCtrl::EsTimeCtrl(wxWindow* parent, wxWindowID id, const wxPoint &pos, const wxSize &size) :
wxPanel(parent, id, pos, size),
m_box(0),
m_lblH(0),
m_h(0),
m_lblM(0),
m_m(0),
m_lblS(0),
m_s(0),
m_min(0, 0, 0, 0),
m_max(23, 59, 59, 0)	
{
	m_box = new wxBoxSizer(wxHORIZONTAL);
	wxASSERT(m_box);
	SetSizer(m_box);
	
	m_lblH = new wxStaticText(this, wxID_ANY, wxEmptyString);
	wxASSERT(m_lblH);
	m_box->Add(m_lblH, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxRIGHT));
	m_box->Show(m_lblH, false);
	m_h = new wxSpinCtrl(this);
	wxASSERT(m_h);
	m_h->SetSizeHints(30, -1);
	m_h->SetValue(0);
	m_h->SetRange(0, 23);
	m_box->Add(m_h, wxSizerFlags().Proportion(1));
	m_box->AddSpacer(2);

	m_lblM = new wxStaticText(this, wxID_ANY, wxT(':'));
	wxASSERT(m_lblM);
	m_box->Add(m_lblM, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxRIGHT));
	m_m = new wxSpinCtrl(this);
	wxASSERT(m_m);
	m_m->SetSizeHints(30, -1);
	m_m->SetValue(0);
	m_m->SetRange(0, 59);
	m_box->Add(m_m, wxSizerFlags().Proportion(1));
	m_box->AddSpacer(2);

	m_lblS = new wxStaticText(this, wxID_ANY, wxT(':'));
	wxASSERT(m_lblS);
	m_box->Add(m_lblS, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border(wxLEFT|wxRIGHT));
	m_s = new wxSpinCtrl(this);
	wxASSERT(m_s);
	m_s->SetSizeHints(30, -1);
	m_s->SetValue(0);
	m_s->SetRange(0, 59);
	m_box->Add(m_s, wxSizerFlags().Proportion(1));
	
	m_h->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &EsTimeCtrl::onUpdateTimeEditor, this);
	m_m->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &EsTimeCtrl::onUpdateTimeEditor, this);
	m_s->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &EsTimeCtrl::onUpdateTimeEditor, this);		
}

EsTimeCtrl::~EsTimeCtrl()
{
	m_h->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &EsTimeCtrl::onUpdateTimeEditor, this);
	m_m->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &EsTimeCtrl::onUpdateTimeEditor, this);
	m_s->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &EsTimeCtrl::onUpdateTimeEditor, this);		
}
	
// control labels text. if not set, default text is used (nothing for H, : for M and S)
void EsTimeCtrl::setLabels(const wxString& lblH, const wxString& lblM, const wxString& lblS)
{
	m_lblH->SetLabel(lblH);
	m_box->Show(m_lblH, !lblH.empty());
	
	if( lblM.empty() )
		m_lblM->SetLabel(wxT(':'));
	else
		m_lblM->SetLabel(lblM);

	if( lblS.empty() )
		m_lblS->SetLabel(wxT(':'));
	else
		m_lblS->SetLabel(lblS);

	Layout();
}

// access applicable time range. if not set, default 0-23:59:59 is used
void EsTimeCtrl::setRange(const wxDateTime& min, const wxDateTime& max)
{
	if( !min.IsValid() )
		EsException::Throw( 0, EsException::Generic, _("Minimum value of time range is not valid") );

	if( !max.IsValid() )
		EsException::Throw( 0, EsException::Generic, _("Maximum value of time range is not valid") );
		
	if( max.IsEarlierThan(min) )
		EsException::Throw( 0, EsException::Generic, _("Miminum time value may not be greater than maximum") );
	
	wxDateTime::Tm dtm = min.GetTm();
	m_min.Set( dtm.hour, dtm.min, dtm.sec, 0 );
	dtm = max.GetTm();
	m_max.Set( dtm.hour, dtm.min, dtm.sec, 0 );	
}

const wxDateTime& EsTimeCtrl::getMin() const
{
	return m_min;
}

const wxDateTime& EsTimeCtrl::getMax() const
{
	return m_max;
}

// time access
void EsTimeCtrl::setTime(const wxDateTime& dt)
{
	// extract time part of dt
	wxDateTime::Tm dtm = dt.GetTm();	
	wxDateTime dest(dtm.hour, dtm.min, dtm.sec, 0);
	if( dest.IsBetween(m_min, m_max) )
	{
		m_h->SetValue(dtm.hour);
		m_m->SetValue(dtm.min);		
		m_s->SetValue(dtm.sec);		
	}
	else
		EsException::Throw(0, EsException::Generic, _("Time value is out of range"));
}

wxDateTime EsTimeCtrl::getTime() const
{
	wxDateTime dt(m_h->GetValue(), m_m->GetValue(), m_s->GetValue(), 0);
	return dt; 
}

// base class overrides
bool EsTimeCtrl::Enable(bool enable)
{
	bool result = wxPanel::Enable(enable);
	if( result )
	{
		m_h->Enable(enable);
		m_m->Enable(enable);		
		m_s->Enable(enable);		
	}
	
	return result;
}

// event handling
void EsTimeCtrl::onUpdateTimeEditor(wxSpinEvent& evt)
{
	int h, m, s;
	if( evt.GetEventObject() == m_h )
	{
		h = evt.GetPosition();
		m = m_m->GetValue();
		s = m_s->GetValue();
	}
	else if( evt.GetEventObject() == m_m )
	{
		h = m_h->GetValue();
		m = evt.GetPosition();
		s = m_s->GetValue();
	}
	else
	{
		h = m_h->GetValue();
		m = m_m->GetValue();
		s = evt.GetPosition();
	}
	wxDateTime newTm(h, m, s, 0);
	if( newTm < m_min || newTm > m_max )
		evt.Veto();
}
