#include "stdafx.h"
#pragma hdrstop

#include <locale>
#include <algorithm>
#include <wx/tooltip.h> 
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>
#include "ShellApp.h"
#include "CustomizeShellDlg.h"

// {97AE12B9-3485-4845-A61B-0CE64AE9E3EF}
static const GUID c_key = {0x97ae12b9, 0x3485, 0x4845, 0xa6, 0x1b, 0xc, 0xe6, 0x4a, 0xe9, 0xe3, 0xef};

// internal logon dialog class
class LogOnDialog : public wxDialog
{
public:
	LogOnDialog() :
			wxDialog(0, wxID_ANY, EsString::format(_("Log On - '%s'"), ShellApp::getInstance()->getLocalAppName().c_str()),
				wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP ),
				m_user(new wxTextCtrl(this, wxID_ANY)),
				m_pwd(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD)),
				m_remember(new wxCheckBox(this, wxID_ANY, _("Remember password"))),
				m_role(new wxStaticText(this, wxID_ANY, wxEmptyString)),
				m_ok(new wxButton(this, wxID_OK))
			{
				SetIcons( ShellApp::getInstance()->getIcons() );

				wxASSERT(m_user);
				wxASSERT(m_pwd);
				wxASSERT(m_role);
				wxASSERT(m_ok);
				m_ok->SetDefault();

				wxBoxSizer* contents = new wxBoxSizer(wxVERTICAL);
				wxASSERT(contents);

				// add logon controls
				wxFlexGridSizer* ctls = new wxFlexGridSizer(4, 2, 0, 0);
				wxASSERT(ctls);
				ctls->AddGrowableCol(1,1);

				wxStaticText* lbl = new wxStaticText(this, wxID_ANY, _("User:"));
				wxASSERT(lbl);
				ctls->Add(lbl, 0, wxEXPAND|wxALL, 5);
				ctls->Add(m_user, 1, wxEXPAND|wxALL, 5);

				lbl = new wxStaticText(this, wxID_ANY, _("Password:"));
				wxASSERT(lbl);
				ctls->Add(lbl, 0, wxEXPAND|wxALL, 5);
				ctls->Add(m_pwd, 1, wxEXPAND|wxALL, 5);

				ctls->AddSpacer(5);
				wxASSERT(m_remember);
				ctls->Add(m_remember, 0, wxEXPAND|wxALL, 5);

				lbl = new wxStaticText(this, wxID_ANY, _("Role:"));
				wxASSERT(lbl);
				ctls->Add(lbl, 0, wxEXPAND|wxALL, 5);
				ctls->Add(m_role, 1, wxEXPAND|wxALL, 5);

				// add dialog buttons
				wxStdDialogButtonSizer* btns = new wxStdDialogButtonSizer;
				wxASSERT(btns);

				btns->AddButton(m_ok);

				wxButton* btn = new wxButton(this, wxID_CANCEL);
				wxASSERT(btn);
				btns->AddButton(btn);
				btns->Realize();

				contents->Add(ctls, 1, wxALL|wxEXPAND, 5);
				contents->Add(btns, 0, wxALL|wxEXPAND, 5);

				SetSizer(contents);

				EsUtilities::applyCtlsFont(this, ShellApp::getInstance()->getCtlsFont());
				Layout();

				onUserUpdated();

				m_user->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LogOnDialog::onUserTextChanged), 0, this);
				SetMinClientSize( wxSize(320,200) );
				SetClientSize( wxSize(320,200) );

				CenterOnScreen();
			}

			virtual ~LogOnDialog()
			{
				m_user->Disconnect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(LogOnDialog::onUserTextChanged), 0, this);	
			}

			void onUserTextChanged(wxCommandEvent& WXUNUSED(evt))
			{
				onUserUpdated();
			}

			void onUserUpdated()
			{
				EsString user = m_user->GetValue();
				EsVariant rec = ShellApp::getInstance()->findAccount(user);

				if( rec.isEmpty() )
				{
					m_remember->Enable(false);
					m_ok->Enable(false);
					m_role->SetLabel(_("N/A"));
				}
				else
				{
					m_remember->Enable();
					m_ok->Enable();
					m_role->SetLabel( EsUtilities::getAslString(rec.getItem(2).asInt()).c_str() );			
				}
			}

			void setUser(const EsString& user)
			{
				m_user->SetValue(user.c_str());
			}

			EsString getUser() const
			{
				return m_user->GetValue();
			}

			void setRememberUser(bool remember)
			{
				if( m_remember->IsEnabled() )
					m_remember->SetValue(remember);
			}

			bool getRememberUser() const
			{
				return m_remember->GetValue();
			}

			EsString getPassword() const
			{
				return m_pwd->GetValue();
			}

			void setPassword(const EsString& pwd)
			{
				return m_pwd->SetValue(pwd);
			}

protected:
	wxTextCtrl* m_user;
	wxTextCtrl* m_pwd;
	wxCheckBox* m_remember;
	wxStaticText* m_role;
	wxButton* m_ok;
};

// internal security data services
//
// password encoder key
// {2E021FF8-5B5A-4bf6-95C4-505DE0A77AAD}
//
static const GUID c_pwdKey = {0x2e021ff8, 0x5b5a, 0x4bf6, {0x95, 0xc4, 0x50, 0x5d, 0xe0, 0xa7, 0x7a, 0xad}};
static EsString encodePwd(const EsString& pwd)
{
	return EsUtilities::sToE(pwd, EsUtilities::GUIDtoStr(c_pwdKey));
}

static EsString decodePwd(const EsString& pwd)
{
	return EsUtilities::eToS(pwd, EsUtilities::GUIDtoStr(c_pwdKey));
}

// each security record looks like {name, pwd, asl, locked, system}
//
// internal srvice for creating default security data at the first program start
static void initSecurityData(EsDictionary& security)
{
	if( security.isEmpty() )
	{
		security.setName( wxT("security") );
		// add system administration && guest records
		EsVariant rec(EsVariant::VAR_VARIANT_COLLECTION);
		EsVariant accounts(EsVariant::VAR_VARIANT_COLLECTION);
		// encoded administrative pwd belTS37cirES
		rec.addToVariantCollection(wxT("Admin")).
			addToVariantCollection(decodePwd( wxT("E923F4B1BEB5913DCC8249FE0EC178137C9C457D5FC9F0E9") )).
			addToVariantCollection(AslAdministrator).
			addToVariantCollection(false).
			addToVariantCollection(true);
		accounts.addToVariantCollection(rec);

		rec.setToNull(EsVariant::VAR_VARIANT_COLLECTION);
		rec.addToVariantCollection(wxT("Superuser")).
			addToVariantCollection(wxT("suser")).
			addToVariantCollection(AslSuperuser).
			addToVariantCollection(false).
			addToVariantCollection(true);
		accounts.addToVariantCollection(rec);

		rec.setToNull(EsVariant::VAR_VARIANT_COLLECTION);
		rec.addToVariantCollection(wxT("Guest")).
			addToVariantCollection(wxT("guest")).
			addToVariantCollection(AslGuest).
			addToVariantCollection(true).
			addToVariantCollection(true);
		accounts.addToVariantCollection(rec);

		security.setValue( wxT("accounts"), accounts, true);
	}
}

// update | rewrite security file
static void updateSecurityDataFile(const EsDictionary& security)
{
	EsString key;
	EsString::binToHex(reinterpret_cast<const BYTE*>(&c_key), sizeof(c_key), key);
	wxFileName file(wxStandardPaths::Get().GetUserDataDir(), wxT("accounts.dat"));
	wxASSERT( wxFileName::DirExists( file.GetPath() ) );
	wxFFileOutputStream out( file.GetFullPath() );
	wxStringInputStream in( security.getAsString().c_str() );	
	EsUtilities::makeFwe( in, out, key);
}

// access accounts data. copy collection is returned
static EsVariant getAccounts(const EsDictionary& security)
{
	EsVariant accounts = security.getValue(wxT("accounts"));
	wxASSERT(EsVariant::VAR_VARIANT_COLLECTION == accounts.getType());

	return accounts;	
}

// set accounts data. existing accounts get overwritten
static void setAccounts(EsDictionary& security, const EsVariant& accounts)
{
	wxASSERT(EsVariant::VAR_VARIANT_COLLECTION == accounts.getType());
	security.setValue(wxT("accounts"), accounts);
}

// open && load|create security file
static void openSecurityDataFile(EsDictionary& security)
{
	EsString key;
	EsString::binToHex(reinterpret_cast<const BYTE*>(&c_key), sizeof(c_key), key);
	wxFileName file(wxStandardPaths::Get().GetUserDataDir(), wxT("accounts.dat"));
	wxASSERT( wxFileName::DirExists( file.GetPath() ) );
	wxString secFile = file.GetFullPath();
	initSecurityData(security);
	if( wxFileName::FileExists(secFile) )
	{
		wxFFileInputStream in( secFile );
		wxStringOutputStream out;
		EsUtilities::makeFw( in, out, key);
		security.setAsString(out.GetString());
	}
	else
		updateSecurityDataFile(security);
}

// ShellApp inmplementation
//
DEF_CFG_SECTION(GLOBAL);
DEF_CFG_KEY(LANGUAGE);
DEF_CFG_KEY(USER);
DEF_CFG_KEY(PWD);
DEF_CFG_SECTION(SHELL_GUI);
DEF_CFG_KEY(MENU_FONT);
DEF_CFG_KEY(CTLS_FONT);	

ShellApp::AppIntfImpl::~AppIntfImpl()
{
}

ES_IMPL_INTF_METHOD(MainWindowIntf*, ShellApp::AppIntfImpl::getMainWindow)() const 
{ 
	if( m_this.m_frm )
		return m_this.m_frm->getIntf(); 

	return 0;
}

// get application default logger
ES_IMPL_INTF_METHOD(EsLogIntf*, ShellApp::AppIntfImpl::getLogger)() const
{
	if( m_this.m_frm )
		return m_this.m_frm->getIntf()->getLogger(); 

	return 0;
}

ES_IMPL_INTF_METHOD(void, ShellApp::AppIntfImpl::setLanguageId)(int langId) 
{ 
	m_this.setLanguageId(langId);
}

ShellApp::ShellApp() :
wxApp(),
m_instCheck( wxT("107A715E-D01D-4b19-80FD-28FE47FC41DF") ),
m_locale(),
#ifdef _DEBUG
m_asl( AslDeveloper ),
#else
m_asl( AslGuest ),
#endif
m_frm(0),
m_intf(*this),
m_curLangId(0),
m_pendingLogOff(false),
m_sockOk(false)
{
#ifdef USE_MEMLEAK_DETECTION 
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );
#endif
	// handle fatal exceptions & SEH support
	wxHandleFatalExceptions();
	// use both vendor and app info in standard paths
	wxStandardPaths::Get().UseAppInfo(wxStandardPaths::AppInfo_AppName|wxStandardPaths::AppInfo_VendorName);
	// init vendor name
	SetVendorName( VENDOR );
	// set application title	
	SetAppName( PRODUCT );	

	// activate support for all standard image types
	wxInitAllImageHandlers();

#ifdef __WINDOWS__
	// in addition, include support for MSW image handlers, MSW resource support need this
	wxGDIImage::InitStandardHandlers();
#endif

	// assign defaul values to gui settings
	m_menuFont = m_ctlsFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

	Connect(FrmMain::IDM_FILE_LOGOFF, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ShellApp::onLogOff));
	Connect(wxEVT_IDLE, wxIdleEventHandler(ShellApp::onIdle));

	// init icons bundle
	//
	// small
	m_icons.AddIcon( wxIcon( wxT("wxicon"), wxBITMAP_TYPE_ICO_RESOURCE, 16, 16) );
	// medium
	m_icons.AddIcon( wxIcon( wxT("wxicon"), wxBITMAP_TYPE_ICO_RESOURCE, 32, 32) );
	// large
	m_icons.AddIcon( wxIcon( wxT("wxicon"), wxBITMAP_TYPE_ICO_RESOURCE, 48, 48) );

	// exit application when top window's closed
	SetExitOnFrameDelete(true);
	// globally enable tooltips for applicatoin
	wxToolTip::Enable(true);

	// initialize socket framework
	m_sockOk = wxSocketBase::Initialize();
}

// application instance
ShellApp* ShellApp::getInstance()
{
	ShellApp* app = wxDynamicCast( wxTheApp, ShellApp );
	return app;
}

// application paths
//
EsString ShellApp::getAppRootStatic()
{
	wxFileName tmp(wxStandardPaths::Get().GetExecutablePath());
	return tmp.GetPathWithSep();
}

EsString ShellApp::getPluginsRootStatic()
{
	wxFileName tmp = wxFileName::DirName(getAppRootStatic().c_str());
	tmp.AppendDir( wxT("plugins") );
	return tmp.GetPathWithSep();
}

ShellApp::~ShellApp()
{	
	Disconnect(FrmMain::IDM_FILE_LOGOFF, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ShellApp::onLogOff));
	Disconnect(wxEVT_IDLE, wxIdleEventHandler(ShellApp::onIdle));
	// cleanup global config, just in case. when terminating application abnormally,
	// the OnExit call, where automatic config pointer release is handled, may be missed
	//
	wxConfigBase* cfg = wxConfigBase::Get();
	if( cfg )
	{
		cfg->Flush();
		delete cfg;
	}

	// uninitialize socket ramework
	if( m_sockOk )
	{
		wxSocketBase::Shutdown();
		m_sockOk = false;
	}
	// cleanup application-wide locale pointer
	EsUtilities::setAppLocale(0);
}

void ShellApp::setCtlsFont(const wxFont& fnt, bool force)
{
	if( fnt.IsOk() && (m_ctlsFont != fnt || force))
	{
		m_ctlsFont = fnt;
		if( m_frm )
		{
			EsUtilities::applyCtlsFont(m_frm, m_ctlsFont);
			//// apply font to the current aui art provider(s) as well
			//m_frm->getCurrentAuiArtProvider()->SetFont(wxAUI_DOCKART_CAPTION_FONT, m_ctlsFont);
			// kick plugin view to adjust its controls
			m_frm->adjustPluginViewSize( m_frm->getActivePlugin() );
		}
	}
}

void ShellApp::setMenuFont(const wxFont& fnt, bool force)
{
	if( fnt.IsOk() && (m_menuFont != fnt || force) )
	{
		m_menuFont = fnt;

		if( m_frm )
			EsUtilities::applyMenuFont(m_frm->GetMenuBar(), m_menuFont);
	}
}

// apply existing menu font
void ShellApp::setMenuFont()
{
	ShellApp* app = getInstance();
	wxASSERT(app);
	app->setMenuFont(app->m_menuFont, true);
}

void ShellApp::setLanguageId(int langId)
{
	std::auto_ptr<wxLocale> loc;
	if( m_curLangId != langId )
	{
		loc.reset( EsUtilities::initLocale(langId) );
		if( loc.get() )
		{
			m_locale = loc;
			m_locale->AddCatalog( wxT("wxstd") );
			m_locale->AddCatalog( wxT("esshell") );
			// add search path for plugin localization catalogues
			wxFileName pluginI18nPath(wxStandardPaths::Get().GetResourcesDir(), EsString::s_null);
			pluginI18nPath.AppendDir( wxT("plugins") );
			m_locale->AddCatalogLookupPathPrefix( pluginI18nPath.GetPath() );			
			m_curLangId = langId;
			// assign application-wide locale pointer
			EsUtilities::setAppLocale(m_locale.get());
		}
	}
}

bool ShellApp::OnInit()
{
	// get list of installed languages
	m_langs = EsUtilities::getInstalledLanguages(wxStandardPaths::Get().GetResourcesDir(), wxT("esshell"));

	// create standard path, if not there
	if( !wxDir::Make( wxStandardPaths::Get().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL ) )
	{
		EsUtilities::showErrorFmt( _("Error"), _("Cannot create standard directory structure\n'%s'").t_str(), 
			wxStandardPaths::Get().GetUserDataDir().t_str() );
		return false;	
	}

	// init application settings storage
	wxFileName cfg(wxStandardPaths::Get().GetUserDataDir(), wxT("config.ini"));
	wxASSERT( wxFileName::DirExists( cfg.GetPath() ) );
	// use our specific instance as global settings storage
	wxFileConfig* fileCfg = new wxFileConfig(EsString::s_null, EsString::s_null, 
		cfg.GetFullPath(), EsString::s_null, wxCONFIG_USE_LOCAL_FILE);
	wxASSERT(fileCfg);
	wxConfigBase::Set( fileCfg );
	wxConfigBase::DontCreateOnDemand();	

	// load user and language setting from the ini file
	const wxLanguageInfo* info = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
	wxASSERT(info);
	fileCfg->SetPath(CFG_GLOBAL_GROUP);
	EsString user =	fileCfg->Read(CFG_USER_KEY);
	EsString pwd = fileCfg->Read(CFG_PWD_KEY);

	wxString locName = fileCfg->Read(CFG_LANGUAGE_KEY, info->Description);
	info = wxLocale::FindLanguageInfo(locName);

	if( info )
		setLanguageId( info->Language );

	if( !m_locale.get() )
		return false;

	if( m_instCheck.IsAnotherRunning() )
	{
		EsUtilities::showErrorFmt( _("Error"), _("Another instance of '%s' is already running, aborting."), getLocalAppName().c_str() );
		return false;
	}
	
	// initialize help framework
  wxHelpControllerHelpProvider* provider = new wxHelpControllerHelpProvider;
	wxHelpProvider::Set(provider);
	provider->SetHelpController(&m_hc);

	// init security data
	openSecurityDataFile(m_security);

	// load GUI settings
	fileCfg->SetPath( CFG_SHELL_GUI_GROUP );
	wxString str;
	if( fileCfg->Read(CFG_CTLS_FONT_KEY, &str) )
	{
		m_ctlsFont.SetNativeFontInfo(str);
		setCtlsFont(m_ctlsFont, true);
	}
	if( fileCfg->Read(CFG_MENU_FONT_KEY, &str) )
		m_menuFont.SetNativeFontInfo(str);	

	// load main frame, prompt for logon
	m_frm = new FrmMain;
	SetTopWindow(m_frm);

	if( !pwd.empty() )
		pwd = decodePwd(pwd);
	return logOnPrompt(user, pwd);
}

int ShellApp::OnExit()
{
	wxConfigBase* cfg = wxConfigBase::Get();
	wxASSERT(cfg);
	// save currently used language
	cfg->SetPath(CFG_GLOBAL_GROUP);
	cfg->Write(CFG_LANGUAGE_KEY, wxLocale::GetLanguageName( m_curLangId ) );
	cfg->Write(CFG_USER_KEY, m_user.c_str());	

	// save GUI settings
	cfg->SetPath( CFG_SHELL_GUI_GROUP );
	cfg->Write(CFG_CTLS_FONT_KEY, m_ctlsFont.GetNativeFontInfoDesc());
	cfg->Write(CFG_MENU_FONT_KEY, m_menuFont.GetNativeFontInfoDesc());	
	
	// uninitialize help framework
	delete wxHelpProvider::Set(0);	
	
	return wxApp::OnExit();
}

// get localized application name
EsString ShellApp::getLocalAppName() const
{
	return _("Ekosfera Shell");
}

// logoff command handler
void ShellApp::onLogOff(wxCommandEvent& WXUNUSED(evt))
{
	if( EsUtilities::showMessage(_("Are you sure you want to log-off current user session?"), 
		_("Confirm log-off"),
		wxYES_NO|wxICON_QUESTION) == wxID_YES )
		// set pending logoff flag. actual processing will be done in idle loop
		m_pendingLogOff = true;
}

void ShellApp::onIdle(wxIdleEvent& evt)
{
	if( m_pendingLogOff )
	{
		m_pendingLogOff = false;	
		// remember recent user
		EsString user = getCurrentUser();
		if( logOff() )
		{
			wxConfigBase* cfg = wxConfigBase::Get();
			wxASSERT(cfg);
			cfg->SetPath(CFG_GLOBAL_GROUP);
			EsString pwd = cfg->Read(CFG_PWD_KEY);
			if( !pwd.empty() )
				pwd = decodePwd(pwd);
			if( !logOnPrompt(user, pwd) )
				m_frm->Close(true);
		}
	}

	evt.Skip();
}

bool ShellApp::OnExceptionInMainLoop()
{
	try
	{
		throw;
	}
	catch( EsException& ex )
	{
		if( ex.getType() == EsException::Warning )
			EsUtilities::showMessage( ex.getMsg(), _("Warning"), wxOK|wxICON_EXCLAMATION );
		else	
			EsUtilities::showError( ex.getMsg() );
	}

	return true;
}

void ShellApp::OnUnhandledException()
{
	// this shows how we may let some exception propagate uncaught
	try
	{
		throw;
	}
	catch( const EsException& ex )
	{
		if( ex.getType() == EsException::Warning )
			EsUtilities::showMessage( ex.getMsg(), _("Warning"), wxOK|wxICON_EXCLAMATION );
		else	
			EsUtilities::showError( ex.getMsg() );
	}
	catch( ... )
	{
		wxLogError( _("Unhandled exception caught, program will terminate.") );
	}
}

void ShellApp::OnFatalException()
{
	// report the state during the last (fatal) exception
	generateDebugReport(wxDebugReport::Context_Exception, _("Fatal exception occured"));
}

// debug report mailer
//
class EsDebugReport : public wxDebugReportCompress
{
public:
	virtual bool DoProcess()
	{
		//if( wxDebugReportCompress::DoProcess() )
		//{
		//	wxEmailMessage msg(GetReportName() + wxT(" crash report"),
		//		wxT("vsevolod.gromov@mailfrom.ru"),
		//		wxEmptyString, // mail body
		//		wxEmptyString, // from address
		//		GetCompressedFileName(),
		//		wxT("esshell_crash.zip"));

		//	return wxEmail::Send(msg);
		//}
		
    //return false;
    return wxDebugReportCompress::DoProcess();
	}
};

void ShellApp::generateDebugReport(wxDebugReport::Context ctx, wxString msg)
{
	EsDebugReport report;

	// add all standard files
	report.AddAll(ctx);

	// you can call report->AddFile(...) with your own log files, files created
	// using wxRegKey::Export() and so on.
	//wxDateTime dt = wxDateTime::Now();
	//report.AddText(_T("extra.dat"),
	//	dt.FormatISODate() + _T(' ') + dt.FormatISOTime() + _T('\n') +
	//	_T('\n') + msg + _T('\n'), _T("Extra data about this report"));

	// calling Show() is not mandatory, but is more polite
	if( wxDebugReportPreviewStd().Show(report) )
	{
		if( report.Process() )
		{
			EsUtilities::getAppLogger()->append( EsLogIntf::LevelInfo,
				EsString::format(_("Crash report generated in '%s'"), 
					report.GetCompressedFileName().c_str()) );
			report.Reset();
		}
	}
	//else: user canceled the report
}

void ShellApp::customizeShell()
{
	CustomizeShellDlg dlg( GetTopWindow() );
	dlg.setCtlsFont( getCtlsFont() );
	dlg.setMenuFont( getMenuFont() );
	if( dlg.ShowModal() == wxID_OK )
	{
		setCtlsFont( dlg.getCtlsFont() );
		setMenuFont( dlg.getMenuFont() );
	}
}

// security management
//

// log on|log off services
bool ShellApp::logOnPrompt(const EsString& user, const EsString& pwd /*= EsString::s_null*/ )
{
	LogOnDialog dlg;
	EsVariant rec = findAccount(user);
	// use supplied user name in dialog if user account exists and not locked
	if( !rec.isEmpty() && !rec.getItem(3).asBool() )
		dlg.setUser(user);

	if( !pwd.empty() )
	{
		dlg.setRememberUser(true);
		dlg.setPassword(pwd);
	}

	bool result = false;	
	while( !result && dlg.ShowModal() == wxID_OK )
	{
		try
		{
			EsString pwd = dlg.getPassword();
			logOn( dlg.getUser(), pwd );
			// optionally, save remember use settings, if any
			wxConfigBase* cfg = wxConfigBase::Get();
			wxASSERT(cfg);			
			cfg->SetPath(CFG_GLOBAL_GROUP);
			if( dlg.getRememberUser() )
				cfg->Write( CFG_PWD_KEY, static_cast<wxString>( encodePwd( dlg.getPassword() ) ) );
			else
				cfg->DeleteEntry( CFG_PWD_KEY );			

			result = true;
		}
		catch(const EsException& ex)
		{
			EsUtilities::showError(ex.getMsg());
			result = false;
		}
	}

	return result;
}

void ShellApp::logOn(const EsString& user, const EsString& pwd)
{
	checkSecurityData(m_security);
	if( m_user != user )
	{
		EsVariant rec = findAccount(user);
		if( rec.isEmpty() )
			EsException::ThrowUserDoesNotExist(user);
		if( rec.getItem(1).asString() != pwd )
			EsException::ThrowPasswordNotValid();
		if( rec.getItem(3).asBool() )
			EsException::Throw(0, EsException::Generic, _("Account '%s' is locked"), user.c_str());

		if( logOff() )
		{
			m_user = user;
			int asl = rec.getItem(2).asInt();
			wxASSERT(asl > AslNone && asl <= AslDeveloper);
			m_asl = static_cast<AppSecurityLevel>(asl);

			// init frame
			wxASSERT(m_frm);
			m_frm->init();
		}
	}	
}

bool ShellApp::logOff()
{
	bool result = true;
	if( !m_user.empty() )
	{
		wxASSERT(m_frm);
		if( m_frm->setActivePlugin(0) )
		{
			// request plugins unload && main frame uninit
			m_frm->uninit();
			// cleanup current logon info
			m_user.clear();
			m_asl = AslNone;
		}
		else
			result = false;			
	}

	return result;
}

// user management services
//
// return all user names
EsString::Array ShellApp::getAllUsers() const
{
	EsString::Array result;
	checkSecurityData(m_security);	
	EsVariant accounts = getAccounts(m_security);
	result.reserve( accounts.getCount() );
	for( long idx = 0; idx < accounts.getCount(); ++idx )
	{
		EsVariant rec = accounts.getItem(idx);
		result.push_back( rec.getItem(0).asString() );
	}

	return result;
}

// add new user. if user with specified name already exist, throw exception
void ShellApp::addUser(const EsString& user, const EsString& pwd, AppSecurityLevel asl)
{
	checkSecurityData(m_security);
	checkAddRights(getCurrentAsl(), asl);
	checkUserNamePolicy(user);
	checkPasswordPolicy(pwd);

	if( isExistingUser(user) )
		EsException::ThrowUserAlreadyExists(user);
	// do not allow creation of Guest accounts (there is no sense in it)	
	if( AslGuest == asl )	
		EsException::ThrowAddingUsersWithAslIsNotAllowed(asl);

	EsVariant accounts = getAccounts(m_security);
	EsVariant rec(EsVariant::VAR_VARIANT_COLLECTION);
	rec.addToVariantCollection(user).
		addToVariantCollection(pwd).
		addToVariantCollection(asl).
		addToVariantCollection(false).
		addToVariantCollection(false);
	accounts.addToVariantCollection(rec);
	setAccounts(m_security, accounts);
	// update security file
	updateSecurityDataFile(m_security);
}

// get asl of given user
AppSecurityLevel ShellApp::getUserAsl(const EsString& user) const
{
	EsVariant rec = findAccount(user);
	if( !rec.isEmpty() )
		return static_cast<AppSecurityLevel>(rec.getItem(SEC_FIELD_ASL).asLong());

	return AslNone;
}

// internal security account record getter. if pidx is not 0, then optionally return index of found record
EsVariant ShellApp::findAccount(const EsString& user, int* pidx/* = 0*/) const
{
	EsVariant accounts = getAccounts(m_security);
	for( int idx = 0; idx < accounts.getCount(); ++idx )
	{
		EsVariant rec = accounts.getItem(idx);
		wxASSERT( EsVariant::VAR_VARIANT_COLLECTION == rec.getType() && 5 == rec.getCount() );

		if( rec.getItem(0).asString() == user )
		{
			if( 0 != pidx )
				*pidx = idx;
			return rec;
		}
	}

	if( 0 != pidx )
		*pidx = -1;
	return EsVariant::s_null;	
}

// delete user, if current asl allows that
void ShellApp::deleteUser(const EsString& user)
{	
	checkSecurityData(m_security);
	// active user cannot delete itself
	if( getCurrentUser() == user )
		EsException::Throw(0, EsException::Generic, _("User may not delete itself"));
	// check if user exists
	int pos = -1;
	EsVariant rec = findAccount(user, &pos);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(user);
	// retrieve target user asl && check rights
	checkDeleteRights(getCurrentAsl(), rec.getItem(2).asInt());
	// check if it's system record
	if( rec.getItem(4).asBool() )
		EsException::Throw(0, EsException::Generic, _("User '%s' is system user and may not be deleted"), rec.getItem(0).asString().c_str());
	// finally, delete user record
	EsVariant accounts = getAccounts(m_security);
	EsVariant newAccounts(EsVariant::VAR_VARIANT_COLLECTION);
	for(int idx = 0; idx < accounts.getCount(); ++idx)
	{
		if( idx != pos )
			newAccounts.addToVariantCollection(accounts.getItem(idx));
	}
	setAccounts(m_security, newAccounts);
	// update security file
	updateSecurityDataFile(m_security);
}

// change user name if current asl allows that
void ShellApp::changeUserName(const EsString& oldName, const EsString& newName)
{
	checkSecurityData(m_security);
	checkUserNamePolicy(newName);
	// check whether newName is the same as the old one
	if( newName == oldName )
		EsException::Throw(0, EsException::Generic, _("New user name may not be the same as the old one"));
	// check for existing user names
	EsVariant rec = findAccount(newName);
	if( !rec.isEmpty() )
		EsException::ThrowUserAlreadyExists(newName); 
	int pos = -1;
	rec = findAccount(oldName, &pos);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(oldName);
	checkChangeRights(getCurrentAsl(), rec.getItem(2).asInt());

	// finally, change user name
	rec.setItem(0, newName);
	EsVariant accounts = getAccounts(m_security);
	accounts.setItem(pos, rec);
	setAccounts(m_security, accounts);
	// update security data file
	updateSecurityDataFile(m_security);
	// update current user name if it was changed
	if( m_user == oldName )
	{
		m_user = newName;
		// publish name update
		if( m_frm	)
			m_frm->updateTitle();
	}	
}

// change user asl if current asl allows that
void ShellApp::changeUserAsl(const EsString& user, AppSecurityLevel asl)
{
	checkSecurityData(m_security);

	int pos = -1;
	EsVariant rec = findAccount(user, &pos);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(user);

	checkChangeRights(getCurrentAsl(), rec.getItem(2).asInt());
	// we may not change asls of the system accounts
	if( rec.getItem(4).asBool() )
		EsException::Throw(0, EsException::Generic, _("Could not change security role of the system account '%s'"), user.c_str());
	// we may not upgrade asls to values higher than ouw own, unless we're administrator
	if( AslAdministrator > getCurrentAsl() && getCurrentAsl() < asl )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' may not change account role to '%s'"), 
		EsUtilities::getAslString(getCurrentAsl()).c_str(), EsUtilities::getAslString(asl).c_str());
	// prohibit setting asl to guest (which would effectively create new guest account)
	if( AslGuest == asl )
		EsException::ThrowAddingUsersWithAslIsNotAllowed(asl);

	// respond if current user's asl is changed
	if( m_user == user )
	{
		if( requestLogOff() )
			m_pendingLogOff = true;
		else	
			return;
	}

	// finally, change asl of the account
	rec.setItem(2, asl);
	EsVariant accounts = getAccounts(m_security);
	accounts.setItem(pos, rec);
	setAccounts(m_security, accounts);	
	// update security data file
	updateSecurityDataFile(m_security);	
}

// change user password if current asl allows that
void ShellApp::changeUserPassword(const EsString& user, const EsString& oldPwd, const EsString& newPwd)
{
	checkSecurityData(m_security);
	checkPasswordPolicy(newPwd);

	int pos = -1;
	EsVariant rec = findAccount(user, &pos);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(user);

	checkChangeRights(getCurrentAsl(), rec.getItem(2).asInt());	
	// for user asl additionally check if we're changing ourselves
	if( AslUser == getCurrentAsl() && user != getCurrentUser() )
		EsException::ThrowUsersMayOnlyChangeThemselves();
	// check existing password
	if( rec.getItem(1).asString() != oldPwd )
		EsException::ThrowPasswordNotValid();
	// finally, change existing password
	rec.setItem(1, newPwd);
	EsVariant accounts = getAccounts(m_security);
	accounts.setItem(pos, rec);
	setAccounts(m_security, accounts);	
	// update security data file
	updateSecurityDataFile(m_security);		
}

// request mandatory logoff. return true if logoff request was successful
bool ShellApp::requestLogOff()
{
	if( EsUtilities::showMessage(
		_("Log-off is required to make requested changes.\nClick 'Yes' if want to log-off, 'No' to cancel changes."), 
		_("Log-off required"), wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION) == wxID_YES )
		return true;
	else
		EsUtilities::showMessage(_("Log-off cancelled. Operation aborted."), 
		_("Log-off cancelled"), wxOK);

	return false;
}

// lock user account if current asl allows that
void ShellApp::lockAccount(const EsString& user, bool lock/* = true*/)
{
	checkSecurityData(m_security);

	int pos = -1;
	EsVariant rec = findAccount(user, &pos);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(user);	

	checkChangeRights(getCurrentAsl(), rec.getItem(SEC_FIELD_ASL).asLong());
	// we may not lock system administrator record
	if( rec.getItem(SEC_FIELD_ASL).asLong() == AslAdministrator && rec.getItem(SEC_FIELD_SYSTEM).asBool() )
		EsException::Throw(0, EsException::Generic, _("System administrator account may not be locked"));

	// if we're locking ourselves, request logoff first
	if( m_user == user && lock )
	{
		if( requestLogOff() )
			m_pendingLogOff = true;
		else
			return;
	}

	rec.setItem(SEC_FIELD_LOCKED, lock);
	EsVariant accounts = getAccounts(m_security);
	accounts.setItem(pos, rec);
	setAccounts(m_security, accounts);	
	// update security data file
	updateSecurityDataFile(m_security);
}

// lock state getter
bool ShellApp::isAccountLocked(const EsString& user) const
{
	checkSecurityData(m_security);
	EsVariant rec = findAccount(user);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(user);

	return rec.getItem(3).asBool();	
}

// system account check
bool ShellApp::isAccountSystem(const EsString& user) const
{
	checkSecurityData(m_security);
	EsVariant rec = findAccount(user);
	if( rec.isEmpty() )
		EsException::ThrowUserDoesNotExist(user);

	return rec.getItem(4).asBool();	
}

// static services
//

// check if security data was properly initialized
void ShellApp::checkSecurityData(const EsDictionary& security)
{
	if( security.isEmpty() )
		EsException::Throw(0, EsException::Generic, _("Security information was not initialized properly"));
}

// check password policy, longer than or equal to 6 symbols, non-blank. throws if does not comply with policy 
void ShellApp::checkPasswordPolicy(const EsString& pwd)
{
	// check length
	if( pwd.size() < minPwdLen )
		EsException::Throw(0, EsException::Generic, _("Password length must not be shorter than %d symbols"), minPwdLen);
}

// check user name policy. longer than, or equal to 4 symbols, non-blank. throws if does not comply with policy
void ShellApp::checkUserNamePolicy(const EsString& user)
{
	// check length
	if( user.size() < minUserNameLen )
		EsException::Throw(0, EsException::Generic, _("User name length must not be shorter than %d symbols"), minUserNameLen);

	// check leading|trailing non-alphanumerics
	std::locale loc;
	if( !std::isalnum(user[0], loc) )
		EsException::Throw(0, EsException::Generic, _("User name must begin with alphanumeric symbol"));

	if( !std::isalnum(user[ user.size()-1 ], loc) )
		EsException::Throw(0, EsException::Generic, _("User name must end with alphanumeric symbol"));
}

// check security operations rights. if inappropriate rights detected, exception is thrown
void ShellApp::checkDeleteRights(AppSecurityLevel currentAsl, int targetAsl)
{
	// asls less than superuser may not delete users at all
	if( currentAsl < AslSuperuser )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' do not have rights to delete security accounts"), EsUtilities::getAslString(currentAsl).c_str());

	// superusers may remove only lesser asls
	if( AslSuperuser == currentAsl && targetAsl > currentAsl )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' may only delete less secure accounts"), EsUtilities::getAslString(currentAsl).c_str());

	// administrators and developers may remove all accounts
}

void ShellApp::checkChangeRights(AppSecurityLevel currentAsl, int targetAsl)
{
	// asls less than users may not change accounts at all
	if( currentAsl < AslUser )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' do not have rights to change security accounts"), EsUtilities::getAslString(currentAsl).c_str());

	// users may change only themselves
	if( currentAsl == AslUser && targetAsl != AslUser )
		EsException::ThrowUsersMayOnlyChangeThemselves();

	// superusers may change only lesser asls
	if( AslSuperuser == currentAsl && targetAsl > currentAsl )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' may only change less secure accounts"), EsUtilities::getAslString(currentAsl).c_str());

	// administrators and developers may change all accounts
}

void ShellApp::checkAddRights(AppSecurityLevel currentAsl, int targetAsl)
{
	// asls less than superuser may not add accounts at all
	if( currentAsl < AslSuperuser )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' do not have rights to add security accounts"), EsUtilities::getAslString(currentAsl).c_str());

	// superusers may only add less secure accounts
	if( AslSuperuser == currentAsl && targetAsl > currentAsl )
		EsException::Throw(0, EsException::Generic, _("Users with role '%s' may only add less secure accounts"), EsUtilities::getAslString(currentAsl).c_str());

	// administrators and developers may add everything	
}
