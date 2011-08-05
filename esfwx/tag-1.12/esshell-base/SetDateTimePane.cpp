#include <stdafx.h>
#pragma hdrstop

#include <wx/spinctrl.h>
#include "SetDateTimePane.h"

SetDateTimePane::SetDateTimePane(wxWindow* parent, wxWindowID id /*= wxID_ANY*/) :
wxPanel(parent, id),
m_chkSyncWithPC(0),
m_edTime(0),
m_edDate(0),
m_timer(this, IDT_SYNC)
{
	wxBoxSizer* contents = new wxBoxSizer( wxVERTICAL );
	wxASSERT(contents);
	m_chkSyncWithPC = new wxCheckBox( this, IDC_SYNC_WITH_PC, _("Sync with PC Date and Time") );
	wxASSERT(m_chkSyncWithPC);
	contents->Add(m_chkSyncWithPC, wxSizerFlags().Expand().Border());
	wxFlexGridSizer* grid = new wxFlexGridSizer(2, 0, 0);
	wxASSERT(grid);
	grid->SetFlexibleDirection(wxHORIZONTAL);
	grid->AddGrowableCol(1, 0);
	// time controls label
	wxStaticText* lbl = new wxStaticText( this, wxID_ANY, _("Time, (HH:MM:SS):") );
	wxASSERT(lbl);
	grid->Add(lbl, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border());
	m_edTime = new EsTimeCtrl(this, IDC_TIME);
	wxASSERT(m_edTime);
	// add time controls to the flex grid sizer
	grid->Add(m_edTime, wxSizerFlags().Expand().Border());
	// date control label
	lbl = new wxStaticText(this, wxID_ANY, _("Date:"));
	wxASSERT(lbl);
	grid->Add(lbl, wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL).Border());
	// date control
	m_edDate = new wxDatePickerCtrl( this, IDC_DATE, wxDateTime::Now(), wxDefaultPosition, wxDefaultSize,
		wxDP_DROPDOWN|wxDP_SHOWCENTURY );
	wxASSERT(m_edDate);
	grid->Add(m_edDate, wxSizerFlags().Expand().Border());
	// add grid with controls to the main sizer
	contents->Add(grid, wxSizerFlags().Expand());
	// set pane sizer
	SetSizer( contents );

	// connect events
	Connect( IDC_SYNC_WITH_PC, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SetDateTimePane::onSyncWithPC ) );
	Connect( IDT_SYNC, wxEVT_TIMER, wxTimerEventHandler( SetDateTimePane::onSyncTimer ) );
	Connect( IDC_TIME, IDC_DATE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SetDateTimePane::onUpdateUI ) );
	// initialize default values
	setSyncWithPc(false);
	setDateTime(wxDateTime::Now());
}

SetDateTimePane::~SetDateTimePane()
{
	// disconnect events
	Disconnect( IDC_SYNC_WITH_PC, wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SetDateTimePane::onSyncWithPC ) );
	Disconnect( IDT_SYNC, wxEVT_TIMER, wxTimerEventHandler( SetDateTimePane::onSyncTimer ) );
	Disconnect( IDC_TIME, IDC_DATE, wxEVT_UPDATE_UI, wxUpdateUIEventHandler( SetDateTimePane::onUpdateUI ) );
	// stop timer
	if( m_timer.IsRunning() )
		m_timer.Stop();
}

bool SetDateTimePane::getSyncWithPc() const
{
	return m_chkSyncWithPC->GetValue();
}

void SetDateTimePane::setSyncWithPc(bool sync)
{
	if( m_chkSyncWithPC->GetValue() != sync )
	{
		m_chkSyncWithPC->SetValue(sync);
		enableSyncTimer(sync);
	}
}

// retrieve date and time value
wxDateTime SetDateTimePane::getDateTime() const
{
	wxDateTime result = m_edDate->GetValue();
	wxDateTime time = m_edTime->getTime();
	result.SetHour( time.GetHour() );
	result.SetMinute( time.GetMinute() );
	result.SetSecond( time.GetSecond() );
	
	return result;
}

void SetDateTimePane::setDateTime(const wxDateTime& dt)
{
	m_edTime->setTime( dt );
	m_edDate->SetValue( dt );
}

void SetDateTimePane::updateDateTimeCtls()
{
	// update date-time controls
	setDateTime( wxDateTime::Now() );
}

void SetDateTimePane::enableSyncTimer(bool enable)
{
	if( enable )
	{
		m_timer.Start(1000);
		updateDateTimeCtls();
	}		
	else
		m_timer.Stop();
}

// event handlers
void SetDateTimePane::onSyncWithPC( wxCommandEvent& evt )
{
	enableSyncTimer(evt.IsChecked());
}

void SetDateTimePane::onSyncTimer( wxTimerEvent& evt )
{
	updateDateTimeCtls();
}

void SetDateTimePane::onUpdateUI( wxUpdateUIEvent& evt )
{
	int id = evt.GetId();
	if( IDC_TIME <= id && IDC_DATE >= id )
		evt.Enable( !m_chkSyncWithPC->GetValue() );	
}
