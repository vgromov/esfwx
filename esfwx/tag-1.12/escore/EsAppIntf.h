#ifndef _app_intf_h_
#define _app_intf_h_

// application security levels
//
enum AppSecurityLevel {
	AslNone, // no user is logged on
	AslGuest,
	AslUser,
	AslSuperuser,
	AslAdministrator,
	AslDeveloper,
};

// logger interface
//
ES_DECL_INTF_BEGIN2( 70669FA1, 4A0B43ba, 9C474F2B, A9BC64C6, EsLogIntf, EsBaseIntf )
	enum Level {
		LevelDebug,
		LevelInfo,
		LevelRX,
		LevelTX,
		LevelWarning,
		LevelError
	};
	ES_DECL_INTF_METHOD(void, clear)() = 0;
	ES_DECL_INTF_METHOD(void, append)(EsLogIntf::Level lvl, const EsString& msg) = 0;
	ES_DECL_INTF_METHOD(EsString, getAll)() const = 0;
	ES_DECL_INTF_METHOD(bool, isEmpty)() const = 0;
	ES_DECL_INTF_METHOD(wxEvtHandler*, getAsEventSink)() const = 0;	
	ES_DECL_INTF_METHOD(bool, isFileLoggingEnabled)() const = 0;
	ES_DECL_INTF_METHOD(void, enableFileLogging)(bool enable) = 0;
ES_DECL_INTF_END

// event notifier interface
ES_DECL_INTF_BEGIN2( 4CD157FB, B8884500, 83B43B14, B5A473C4, EsNotifierIntf, EsBaseIntf )
	enum Level {
		LevelDebug,
		LevelInfo,
		LevelRX,
		LevelTX,
		LevelWarning,
		LevelError		
	};
	ES_DECL_INTF_METHOD(void, clear)() = 0;
	ES_DECL_INTF_METHOD(void, notify)(EsNotifierIntf::Level lvl, const EsString& msg, const wxBitmap& icon = wxNullBitmap) = 0;
ES_DECL_INTF_END

class MainWindowIntf;
// application framework interface
//
ES_DECL_INTF_BEGIN2( D1364BC8, 386D42da, 95E3F81C, E2618F93, EsAppIntf, EsBaseIntf )
	// return application-wide configuration storage instance
	ES_DECL_INTF_METHOD(wxConfigBase*, getCfg)() const = 0;
	// provide system paths
	//
	ES_DECL_INTF_METHOD(EsString, getAppRoot)() const = 0;
	ES_DECL_INTF_METHOD(EsString, getPluginsRoot)() const = 0;
	// return supported language names (english)
	ES_DECL_INTF_METHOD(const EsString::Array&, getInstalledLanguages)() const = 0;
	// return application locale
	ES_DECL_INTF_METHOD(wxLocale&, getLocale)() const = 0;
	// set|get language id for the application
	ES_DECL_INTF_METHOD(void, setLanguageId)(int langId) = 0;
	ES_DECL_INTF_METHOD(int, getLanguageId)() const = 0;
	// return main window interface
	ES_DECL_INTF_METHOD(MainWindowIntf*, getMainWindow)() const = 0;
	// get application default logger
	ES_DECL_INTF_METHOD(EsLogIntf*, getLogger)() const = 0;	
	// return current application security level
	ES_DECL_INTF_METHOD(AppSecurityLevel, getAsl)() const = 0;
	// return application help controller
	ES_DECL_INTF_METHOD(wxHelpControllerBase&, getHelpController)() = 0;
ES_DECL_INTF_END

// main window interface
//
ES_DECL_INTF_BEGIN2( 1848F97F, 70744dfb, AAC171F5, 499EEDAE, MainWindowIntf, EsBaseIntf )
	// application access
	//
	ES_DECL_INTF_METHOD(EsAppIntf*, getApp)() const = 0;
	// access main window object
	ES_DECL_INTF_METHOD(wxWindow*, getWindow)() const = 0;
	// provide menu entry points for plugin menus installation
	//
	ES_DECL_INTF_METHOD(wxMenuBar&, getMenuBar)() const = 0;
	ES_DECL_INTF_METHOD(wxMenu&, getFileMenu)() const = 0;
	ES_DECL_INTF_METHOD(wxMenu&, getActionsMenu)() const = 0;
	ES_DECL_INTF_METHOD(wxMenu&, getOptionsMenu)() const = 0;
	ES_DECL_INTF_METHOD(wxMenu&, getViewMenu)() const = 0;
	ES_DECL_INTF_METHOD(wxMenu&, getHelpMenu)() const = 0;
	// allow creation|deletion of toolbars
	ES_DECL_INTF_METHOD(wxAuiToolBar*, createToolbar)(int id, bool shown = true) = 0;
	ES_DECL_INTF_METHOD(void, finalizeToolbar)(wxAuiToolBar* toolbar) = 0;
	ES_DECL_INTF_METHOD(void, deleteToolbar)(wxAuiToolBar* toolbar) = 0;
	ES_DECL_INTF_METHOD(void, showToolbar)(wxAuiToolBar* toolbar, bool show) = 0;	
	ES_DECL_INTF_METHOD(bool, toolbarIsShown)(wxAuiToolBar* toolbar) const = 0;	
	// return plugin view container
	ES_DECL_INTF_METHOD(wxScrolledWindow&, getViewContainer)() const = 0;
	ES_DECL_INTF_METHOD(EsLogIntf*, getLogger)() const = 0;
	ES_DECL_INTF_METHOD(EsNotifierIntf*, getNotifier)() const = 0;
	// printing framework access
	ES_DECL_INTF_METHOD(void, pageSetup)() const = 0;
	ES_DECL_INTF_METHOD(bool, previewHtml)(const wxString& html) const = 0;
	ES_DECL_INTF_METHOD(bool, printHtml)(const wxString& html) const = 0;
ES_DECL_INTF_END

#endif // _app_intf_h_
