#ifndef _es_about_dlg_h_
#define _es_about_dlg_h_

#include <wx/aboutdlg.h>

class EsAboutDlg : public wxDialog
{
public:
	EsAboutDlg(wxWindow* parent, const wxAboutDialogInfo& info);
	
};

#endif // _es_about_dlg_h_
