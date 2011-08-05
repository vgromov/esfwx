#ifndef _customize_shell_dlg_h_
#define _customize_shell_dlg_h_

#include <wx/fontpicker.h>

// shell options customization dialog
class CustomizeShellDlg : public wxDialog
{
public:
	enum {
		IDC_CTLS_FONT_PICKER = wxID_HIGHEST+1,
		IDC_MENU_FONT_PICKER,
	};
	
public:
	CustomizeShellDlg(wxWindow* parent);
	virtual ~CustomizeShellDlg();

	// GUI font configuration access
	wxFont getMenuFont() const { return m_pickMenuFont->GetSelectedFont(); }
	void setMenuFont(const wxFont& fnt) { m_pickMenuFont->SetSelectedFont(fnt); }
	wxFont getCtlsFont() const { return m_pickCtlsFont->GetSelectedFont(); }
	void setCtlsFont(const wxFont& fnt) { m_pickCtlsFont->SetSelectedFont(fnt); }
	
protected:
	//// event handlers
	//void onFontPickerChanged(wxFontPickerEvent& evt);

protected:
	// data members
	wxFontPickerCtrl* m_pickMenuFont;
	wxFontPickerCtrl* m_pickCtlsFont;
};

#endif // _customize_shell_dlg_h_
