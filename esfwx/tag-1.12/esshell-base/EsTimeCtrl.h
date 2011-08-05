#ifndef _es_time_ctrl_h_
#define _es_time_ctrl_h_

#include <wx/spinctrl.h>

// time editor compound control
//
class EKOSF_SHELL_BASE_CLASS EsTimeCtrl : public wxPanel
{
public:
	EsTimeCtrl(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize);
	virtual ~EsTimeCtrl();
	
	// control labels text. if not set, default text is used (nothing for H, : for M and S)
	void setLabels(const wxString& lblH, const wxString& lblM, const wxString& lblS);
	// access applicable time range. if not set, default 0-23:59:59 is used
	void setRange(const wxDateTime& min, const wxDateTime& max);
	const wxDateTime& getMin() const;
	const wxDateTime& getMax() const;	
	// time access
	void setTime(const wxDateTime& dt);
	wxDateTime getTime() const;
	// base class overrides
	virtual bool Enable(bool enable = true);
	
protected:
	void onUpdateTimeEditor(wxSpinEvent& evt);
	
protected:
	wxBoxSizer* m_box;
	wxStaticText* m_lblH;
	wxSpinCtrl* m_h;
	wxStaticText* m_lblM;	
	wxSpinCtrl* m_m;	
	wxStaticText* m_lblS;	
	wxSpinCtrl* m_s;	
	wxDateTime m_min;
	wxDateTime m_max;
};

#endif // _es_time_ctrl_h_
