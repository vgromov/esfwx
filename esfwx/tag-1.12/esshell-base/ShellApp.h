#ifndef _shell_h_
#define _shell_h_

#include <wx/snglinst.h>
#include <wx/debugrpt.h>

// shell application
//
class EKOSF_SHELL_BASE_CLASS ShellApp :	public wxApp
{
public:
	enum {
		minPwdLen = 4,
		minUserNameLen = 4,
		// field indexes in security record
		SEC_FIELD_NAME = 0,
		SEC_FIELD_PWD,
		SEC_FIELD_ASL,
		SEC_FIELD_LOCKED,
		SEC_FIELD_SYSTEM
	};
	
protected:
	struct AppIntfImpl : public EsAppIntf
	{
		AppIntfImpl(ShellApp& This) : m_this(This) {}

		// supported interfaces map
		ES_NON_RC_INTF_MAP_BEGIN( AppIntfImpl )
			ES_INTF_SUPPORTS( AppIntfImpl, EsAppIntf )
		ES_NON_RC_INTF_MAP_END
		{ /* destroy does nothing */ }

		// EsBaseIntf override
		//
		ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
		{ 
			return getClassName(); 
		}
		// EsAppIntf implementation
		//
		ES_DECL_INTF_METHOD(wxConfigBase*, getCfg)() const { return wxConfigBase::Get(); }
		// provide system paths
		ES_DECL_INTF_METHOD(EsString, getAppRoot)() const { return ShellApp::getAppRootStatic(); }
		ES_DECL_INTF_METHOD(EsString, getPluginsRoot)() const { return ShellApp::getPluginsRootStatic(); }
		// return supported language names (english)
		ES_DECL_INTF_METHOD(const EsString::Array&, getInstalledLanguages)() const { return m_this.m_langs; }
		// return application locale
		ES_DECL_INTF_METHOD(wxLocale&, getLocale)() const { return *m_this.m_locale.get(); }
		// select language id for the application
		ES_DECL_INTF_METHOD(void, setLanguageId)(int langId);
		ES_DECL_INTF_METHOD(int, getLanguageId)() const { return m_this.m_curLangId; }
		// return main window interface
		ES_DECL_INTF_METHOD(MainWindowIntf*, getMainWindow)() const;
		// get application default logger
		ES_DECL_INTF_METHOD(EsLogIntf*, getLogger)() const;	
		// return current application security level
		ES_DECL_INTF_METHOD(AppSecurityLevel, getAsl)() const { return m_this.getCurrentAsl(); }
		// return application help controller
		ES_DECL_INTF_METHOD(wxHelpControllerBase&, getHelpController)() { return m_this.getHelpController(); }

		ShellApp& m_this;
	};
	friend struct AppIntfImpl;
	
public:
	ShellApp();
	virtual ~ShellApp();

	// access to EsAppIntf implementation
	EsAppIntf* getIntf() const { return &const_cast<AppIntfImpl&>(m_intf); }
	// locale access
	int getLanguageId() const { return m_curLangId; }
	void setLanguageId(int langId);
	const wxLocale& getLocale() const { return *m_locale.get(); }
  // initialization & de-initialization steps
	virtual bool OnInit();
	virtual int OnExit();
	// 2nd-level exception handling: we get all the exceptions occurring in any
	// event handler here
	virtual bool OnExceptionInMainLoop();
	// 3rd, and final, level exception handling: whenever an unhandled
	// exception is caught, this function is called
	virtual void OnUnhandledException();
	// and now for something different: this function is called in case of a
	// crash (e.g. dereferencing null pointer, division by 0, ...)
	virtual void OnFatalException();
	// access application-wide icon bundle
	const wxIconBundle& getIcons() const { return m_icons; }
	// access to GUI settings
	const wxFont& getCtlsFont() const { return m_ctlsFont; }
	void setCtlsFont(const wxFont& fnt, bool force = false);
	const wxFont& getMenuFont() const { return m_menuFont; }
	void setMenuFont(const wxFont& fnt, bool force = false);
	// re|apply existing menu font
	static void setMenuFont();
	// display shell customization options
	void customizeShell();
	// get localized application name
	EsString getLocalAppName() const;
	// return true if socket framework was initialized ok
	bool isSocketsOk() const { return m_sockOk; }
	// security management
	//
	// log on|log off services
	bool logOnPrompt(const EsString& user, const EsString& pwd = EsString::s_null);
	void logOn(const EsString& user, const EsString& pwd);
	bool logOff();
	// get asl of given user
	AppSecurityLevel getUserAsl(const EsString& user) const;
	// currently active user asl access
	AppSecurityLevel getCurrentAsl() const { return m_asl; }
	// return currently active user name
	EsString getCurrentUser() const { return m_user; }
	// user management services
	//
	// return all user names
	EsString::Array getAllUsers() const;
	// check if user already exists
	bool isExistingUser(const EsString& user) const { return !findAccount(user).isEmpty(); }
	// internal security account record getter. if pidx is not 0, then optionally return index of found record
	EsVariant findAccount(const EsString& user, int* pidx = 0) const;
	// add new user. if user with specified name already exist, throw exception
	void addUser(const EsString& user, const EsString& pwd, AppSecurityLevel asl);
	// delete user, if current asl allows that
	void deleteUser(const EsString& user);
	// change user name if current asl allows that
	void changeUserName(const EsString& oldName, const EsString& newName);
	// change user asl if current asl allows that
	void changeUserAsl(const EsString& user, AppSecurityLevel asl);
	// change user password if current asl allows that
	void changeUserPassword(const EsString& user, const EsString& oldPwd, const EsString& newPwd);
	// lock user account if current asl allows that
	void lockAccount(const EsString& user, bool lock = true);
	// lock state getter
	bool isAccountLocked(const EsString& user) const;
	// system account check
	bool isAccountSystem(const EsString& user) const;
	// return main help controller
	wxHelpControllerBase& getHelpController() { return m_hc; }
	
	// static services
	//
	// application instance
	static ShellApp* getInstance();
	// application specific paths
	static EsString getAppRootStatic();
	static EsString getPluginsRootStatic();
	// check if security data was properly initialized
	static void checkSecurityData(const EsDictionary& security);	
	// check password policy, longer than or equal to 6 symbols, non-blank. throws if does not comply with policy 
	static void checkPasswordPolicy(const EsString& pwd);
	// check user name policy. longer than, or equal to 4 symbols, non-blank. throws if does not comply with policy
	static void checkUserNamePolicy(const EsString& user);
	// check security operations rights. if inappropriate rights detected, exception is thrown
	static void checkDeleteRights(AppSecurityLevel currentAsl, int targetAsl);
	static void checkChangeRights(AppSecurityLevel currentAsl, int targetAsl);
	static void checkAddRights(AppSecurityLevel currentAsl, int targetAsl);
		
protected:
	// request mandatory logoff. return true if logoff request was successful
	bool requestLogOff();		
	void onLogOff(wxCommandEvent& evt);
	void onIdle(wxIdleEvent& evt);
	void generateDebugReport(wxDebugReport::Context ctx, wxString msg);
		
protected:
	wxSingleInstanceChecker m_instCheck;
	std::auto_ptr<wxLocale> m_locale;
	// icons bundle
	wxIconBundle m_icons;
	// current security level
	AppSecurityLevel m_asl;
#pragma warning(push)
#pragma warning(disable: 4251)
	// currently active user
	EsString m_user;
	// currently supported languages
	EsString::Array m_langs;
	// interface implementor
	AppIntfImpl m_intf;
#pragma warning(pop)
	// main frame pointer
	FrmMain* m_frm;
	// currently selected language
	int m_curLangId;
	// current security data
	EsDictionary m_security;
	// logoff|logon is pending
	bool m_pendingLogOff;
	// GUI settings
	wxFont m_ctlsFont;
	wxFont m_menuFont;
	// sockets framework initialization
	bool m_sockOk;
	// main help framework support
	wxHelpController m_hc;
};

#endif // _shell_h_
