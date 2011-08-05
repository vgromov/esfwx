#include "stdafx.h"
#pragma hdrstop

#include <wx/hyperlink.h> 
#include "EsAboutDlg.h"

EsAboutDlg::EsAboutDlg(wxWindow* parent, const wxAboutDialogInfo& info) :
wxDialog(parent, wxID_ANY, EsString::format( _("About '%s'"), ShellApp::getInstance()->GetAppDisplayName().t_str() ) )
{
	SetSize(400, 400);
	SetIcon(ShellApp::getInstance()->getIcons().GetIcon(16));
	wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
	wxASSERT(contents);

	// application info goes here + website address
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	wxASSERT(box);
	if( info.HasIcon() )
	{
		wxStaticBitmap* logo = new wxStaticBitmap(this, wxID_ANY, info.GetIcon());
		wxASSERT(logo);
		box->Add(logo);
		box->AddSpacer(5);
	}
	
	EsString str;
	if( info.HasVersion() )
		str = EsString::format(_("Product version: %s"),
			info.GetVersion().t_str());
	if( info.HasCopyright() )
	{
		if( !str.empty() )
			str += wxT('\n');
		str += EsString::format(_("Copyright: %s"),
			info.GetCopyright().t_str());
	}		
	if( str.empty() )
		str = ShellApp::getInstance()->GetAppDisplayName();
	wxStaticText* lbl = new wxStaticText(this, wxID_ANY, str);
	wxASSERT(lbl);
	box->Add(lbl, 1, wxEXPAND);
	contents->Add(box, 0, wxEXPAND|wxALL, 5);
	
	// information sections notebook
	wxNotebook* nb = new wxNotebook(this, wxID_ANY);
	wxASSERT(nb);
	contents->Add(nb, 1, wxEXPAND|wxALL, 5);
	
	// general information page
	wxScrolledWindow* scroller = new wxScrolledWindow(nb);
	wxASSERT(scroller);
	scroller->SetScrollRate(5, 5);
	nb->AddPage(scroller, _("Generic information"), true);
	box = new wxBoxSizer(wxVERTICAL);
	wxASSERT(box);
	scroller->SetSizer(box);
	wxStaticBoxSizer* sbox = 0;
	// description
	if( info.HasDescription() )
	{
		sbox = new wxStaticBoxSizer(wxVERTICAL, scroller, _("Description:"));
		wxASSERT(sbox);
		box->Add(sbox, 0, wxEXPAND|wxALL, 5);
		lbl = new wxStaticText(scroller, wxID_ANY, info.GetDescription());
		sbox->Add(lbl, 0, wxEXPAND);
	}
	// developers
	if( info.HasDevelopers() )
	{
		sbox = new wxStaticBoxSizer(wxVERTICAL, scroller, _("Developers:"));
		wxASSERT(sbox);
		box->Add(sbox, 0, wxEXPAND|wxALL, 5);
		lbl = new wxStaticText(scroller, wxID_ANY, 
			EsString::fromStringArray( EsUtilities::fromArrayString( info.GetDevelopers() ), wxT("\n") ));
		sbox->Add(lbl, 0, wxEXPAND);	
	}
	// translators
	if( info.HasTranslators() )
	{
		sbox = new wxStaticBoxSizer(wxVERTICAL, scroller, _("Translators:"));
		wxASSERT(sbox);
		box->Add(sbox, 0, wxEXPAND|wxALL, 5);
		lbl = new wxStaticText(scroller, wxID_ANY, 
			EsString::fromStringArray( EsUtilities::fromArrayString( info.GetTranslators() ), wxT("\n") ));
		sbox->Add(lbl, 0, wxEXPAND);	
	}
						
	// loaded plugins information page
	scroller = new wxScrolledWindow(nb);
	wxASSERT(scroller);
	scroller->SetScrollRate(5, 5);
	nb->AddPage(scroller, _("Loaded modules"));
	box = new wxBoxSizer(wxVERTICAL);
	wxASSERT(box);
	scroller->SetSizer(box);
	wxDynamicLibraryDetailsArray libs = wxDynamicLibrary::ListLoaded();
	str.clear();
	for(size_t idx = 0; idx < libs.size(); ++idx)
	{
		const wxDynamicLibraryDetails& libInfo = libs[idx];
		wxString lib = wxFileName::FileName(libInfo.GetName()).GetName() + wxT(" ") + libInfo.GetVersion();
		if( lib.StartsWith(wxT("es")) || lib.Contains(wxT(".plug")) )
		{	
			if( !str.empty() )
				str += wxT("\n");
			str += lib;
		}
	}
	lbl = new wxStaticText(scroller, wxID_ANY, str);
	box->Add(lbl, 0, wxEXPAND|wxALL, 5);
	
	// web site info
	if( info.HasWebSite() )
	{
		wxHyperlinkCtrl* link = new wxHyperlinkCtrl(this, wxID_ANY, 
			info.GetWebSiteDescription(), info.GetWebSiteURL());
		wxASSERT(link);
		contents->Add(link, 0, wxEXPAND|wxALL, 5);
	}
	
	wxStdDialogButtonSizer* btns = CreateStdDialogButtonSizer(wxCLOSE);
	SetEscapeId(wxID_CLOSE);
	SetAffirmativeId(wxID_CLOSE);
	contents->Add(btns, 0, wxALL|wxEXPAND, 5);
	
	SetSizer(contents);
	Layout();
	CenterOnParent();
}