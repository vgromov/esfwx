#ifndef _set_date_time_dlg_h_
#define _set_date_time_dlg_h_

class EKOSF_SHELL_BASE_CLASS SetDateTimeDlg : public wxDialog
{
public:
	SetDateTimeDlg(wxWindow* parent);

	// pane accessors delegates
	//
	// data access
	bool getSyncWithPc() const { return m_pane->getSyncWithPc(); }
	void setSyncWithPc(bool sync) { m_pane->setSyncWithPc(sync); }
	// get-set time
	wxDateTime getDateTime() const { return m_pane->getDateTime(); }
	void setDateTime(const wxDateTime& dt) { m_pane->setDateTime(dt); }

protected:
	SetDateTimePane* m_pane;
};

#endif // _set_date_time_dlg_h_
