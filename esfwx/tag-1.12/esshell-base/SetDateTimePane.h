#ifndef _set_date_time_pane_h_
#define _set_date_time_pane_h_

#include <wx/datectrl.h>

class EKOSF_SHELL_BASE_CLASS SetDateTimePane : public wxPanel
{
protected:
	enum
	{
		IDC_SYNC_WITH_PC = wxID_HIGHEST+1,
		IDC_TIME,
		IDC_DATE,
		IDT_SYNC,
	};

public:
	SetDateTimePane( wxWindow* parent, wxWindowID id = wxID_ANY );
	virtual ~SetDateTimePane();
	// access sync timer activity
	bool getSyncWithPc() const;
	void setSyncWithPc(bool sync);
	// access date and time value
	wxDateTime getDateTime() const;
	void setDateTime(const wxDateTime& dt);

protected:
	// helper services
	void updateDateTimeCtls();
	void enableSyncTimer(bool enable);
	// event handlers
	void onSyncWithPC( wxCommandEvent& evt );
	void onSyncTimer( wxTimerEvent& evt );
	void onUpdateUI( wxUpdateUIEvent& evt );

protected:
	wxCheckBox* m_chkSyncWithPC;
	EsTimeCtrl* m_edTime;
	wxDatePickerCtrl* m_edDate;
	wxTimer m_timer;
};

#endif // _set_date_time_pane_