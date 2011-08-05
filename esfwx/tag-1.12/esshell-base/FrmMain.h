#ifndef _Frm_Main_h_
#define _Frm_Main_h_

// fwd decl
//

// plugin module class
//
class EKOSF_SHELL_BASE_CLASS PluginModule :	public wxDynamicLibrary
{
public:
	PluginModule(const EsString& modulePathName);
	PluginModuleIntf* getIntf() const;

protected:
	PluginModuleIntfGetter m_pfn;
};

// main application form class
//
class EKOSF_SHELL_BASE_CLASS FrmMain :	public wxFrame
{
protected:
	struct MainWindowIntfImpl : public MainWindowIntf
	{
		MainWindowIntfImpl(FrmMain& This) : m_this(This) {}

		// supported interfaces map
		ES_NON_RC_INTF_MAP_BEGIN(MainWindowIntfImpl)
			ES_INTF_SUPPORTS(MainWindowIntfImpl, MainWindowIntf)
		ES_NON_RC_INTF_MAP_END
		{ /* destroy does nothing */ }
		
		// EsBaseIntf override
		//
		ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
		{ 
			return getClassName(); 
		}
		// MainWindowIntf implementation
		//
		// application interface access
		//
		ES_DECL_INTF_METHOD(EsAppIntf*, getApp)() const { return dynamic_cast<ShellApp*>(wxTheApp)->getIntf(); }
		// access main window object
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() const { return &m_this; }

		// provide menu entry points for plugin installation
		//
		ES_DECL_INTF_METHOD(wxMenuBar&, getMenuBar)() const;
		ES_DECL_INTF_METHOD(wxMenu&, getFileMenu)() const;
		ES_DECL_INTF_METHOD(wxMenu&, getActionsMenu)() const;
		ES_DECL_INTF_METHOD(wxMenu&, getOptionsMenu)() const;
		ES_DECL_INTF_METHOD(wxMenu&, getViewMenu)() const;
		ES_DECL_INTF_METHOD(wxMenu&, getHelpMenu)() const;
		// allow creation of toolbars
		ES_DECL_INTF_METHOD(wxAuiToolBar*, createToolbar)(int id, bool shown = true);
		ES_DECL_INTF_METHOD(void, finalizeToolbar)(wxAuiToolBar* toolbar);
		ES_DECL_INTF_METHOD(void, deleteToolbar)(wxAuiToolBar* toolbar);
		ES_DECL_INTF_METHOD(void, showToolbar)(wxAuiToolBar* toolbar, bool show);
		ES_DECL_INTF_METHOD(bool, toolbarIsShown)(wxAuiToolBar* toolbar) const;
		// return plugin view container
		ES_DECL_INTF_METHOD(wxScrolledWindow&, getViewContainer)() const;
		// return (communication)logger intf
		ES_DECL_INTF_METHOD(EsLogIntf*, getLogger)() const;
		// return notification interface
		ES_DECL_INTF_METHOD(EsNotifierIntf*, getNotifier)() const;
		// printing framework access
		ES_DECL_INTF_METHOD(void, pageSetup)() const;
		ES_DECL_INTF_METHOD(bool, previewHtml)(const wxString& html) const;
		ES_DECL_INTF_METHOD(bool, printHtml)(const wxString& html) const;
				
		FrmMain& m_this;
	};
	friend struct MainWindowIntfImpl;

public:
	enum {
		// count of fields in the status bar
		STATUSBAR_FIELDS_COUNT = 2,
		// GUI element ids
		ID_MAIN_FRAME = escoreID_HIGHEST, // this frame id
		ID_MENUBAR,
		ID_TOOLBAR,
		ID_STATUSBAR,
		ID_PLGLIST,
		ID_PLGVIEW,
		// menu command IDs
		IDM_FILE_LOGOFF,
		IDM_ACTIONS_CLEAR_LOG,
		IDM_ACTIONS_LOG_TO_FILE,
		IDM_OPTIONS_SHELL,
		IDM_OPTIONS_LANGUAGE_START, // wxLANGUAGE_DEFAULT,
		IDM_OPTIONS_LANGUAGE_END = IDM_OPTIONS_LANGUAGE_START + wxLANGUAGE_USER_DEFINED-1,
		IDM_VIEW_TOOLBAR,
		IDM_VIEW_PLGLIST,
		IDM_VIEW_FULL_SCREEN,
		IDM_VIEW_LOG,
		IDM_HELP_UPDATE,

		// this one MUST go last
		ID_CORE_HIGHEST
	};

protected:
	typedef std::list<PluginModule*> PluginModuleList;
	typedef std::list<PluginIntf*> PluginList;

public:
	FrmMain();
	virtual ~FrmMain();
	// post-construction initialization
	void init();
	void uninit();
	// wrapper around plugin->activate with proper additional logic
	bool setActivePlugin( PluginIntf* plugin );	
	// return implemented interface instance
	MainWindowIntf* getIntf() const { return &const_cast<MainWindowIntfImpl&>(m_intf); }
	// update frame title in accordance with currently active plugin and user account
	void updateTitle();
	
protected:
	// base class overrides
	//
	virtual bool TryBefore(wxEvent& evt);

	// helper methods
	void initAuiMgr();
	void uninitAuiMgr();
	void resetPlugSelImgList();
	void initMenuAndToolBar();
	void uninitMenuAndToolBar();
	void initStatusBar();
	void uninitStatusBar();
	void initPluginsView();
	void uninitPluginsView();
	void initEventHandlers();
	void uninitEventHandlers();

	// plugin manager-related stuff
	//
	void initPlugins();
	void uninitPlugins();
	// rebuild plugins view
	void reBuildPluginsView();
	// find category by name
	wxTreeItemId findCategoryBranch(const EsString& category) const;
	// find plugin tree leaf
	wxTreeItemId findPluginLeaf(PluginIntf* plugin) const;
	// append plugin entry to the plugin selector
	void addPluginEntry(wxTreeItemId category, PluginIntf* plugin);
	// return currently active plugin entry
	PluginIntf* getActivePlugin() const;
	// return all loaded plugin categories
	EsString::Array getPluginCategories() const;
	// return plugins per category
	PluginList getPluginsByCategory(const EsString& category) const;
	// return plugin by its id string
	PluginIntf* findPluginById(const EsString& id) const;
	// return total plugins count
	int getPluginsCount() const;
	// do plugin view re-layout
	void adjustPluginViewSize(PluginIntf* plugin);
	// show in full screen mode
	void showFullScreen();
	
	// settings serialization
	//
	void loadSettings();
	void saveSettings();

protected:
	// event handlers
	//
	// application exit
	void onFileExit(wxCommandEvent& evt);
	void onClose(wxCloseEvent& evt);
	// printer setup
	void onPageSetup(wxCommandEvent& evt);
	// plugin view size handler
	void onPluginViewSize(wxSizeEvent& evt);
	// actions mnu handlers
	void onActClearLog(wxCommandEvent& evt);
	void onActLogToFileToggle(wxCommandEvent& evt);
	// options mnu handlers
	void onOptShell(wxCommandEvent& evt);
	void onOptLang(wxCommandEvent& evt);
	// view mnu handlers
	void onViewPluginSelector(wxCommandEvent& evt);
	void onViewToolbar(wxCommandEvent& evt);
	void onViewLog(wxCommandEvent& evt);
	void onViewFullscreen(wxCommandEvent& evt);
	// handle help commands
	void onHelp(wxCommandEvent& evt);
	void onHelpAbout(wxCommandEvent& evt);
	// activate plugin leaf in the tree
	void onPluginActivate(wxTreeEvent& evt);
	// respond to tree branch collapsing
	void onPluginCategoryCollapsing(wxTreeEvent& evt);
	// status bar sizing handler
	void onStatusBarSize(wxSizeEvent& evt);
	// view mnu update handling
	void onUpdateUi(wxUpdateUIEvent& evt);

protected:	
	// data members
	// 
	// initialization flag
	bool m_initialized;
	// fullscreen mode
	bool m_fullScreen;
	// AUI manager
	wxAuiManager m_mgr;
	wxString m_mgrCfg;
	// menu|tool bars
	wxMenuBar* m_mnuBar;
	//main toolbar 
	wxAuiToolBar* m_toolBar;
	// status bar
	wxStatusBar* m_statBar;
	// plugin list view
	wxTreeCtrl* m_plgSelector;
	wxScrolledWindow* m_plgView;
	// menu entries
	//
	// common
	wxMenu* m_mnuFile;
	wxMenu* m_mnuActions;
	wxMenu* m_mnuOptions;
	wxMenu* m_mnuView;
	wxMenu* m_mnuHelp;
	// plugin manager-related stuff
#pragma warning(push)
#pragma warning(disable: 4251)
	PluginModuleList m_modules;
#pragma warning(pop)
	EsString m_recentActivePluginId;
	// images for plugin selector
	wxImageList m_lstPlgSelector;
#pragma warning(push)
#pragma warning(disable: 4251)
	// log frame
	FrmLog* m_log;
	// notifier control
	NotifierCtl* m_notifier;
	// interface implementor
	MainWindowIntfImpl m_intf;
#pragma warning(pop)
	// printing support
	wxHtmlEasyPrinting* m_printing;

	friend class ShellApp;
};

#endif // _Frm_Main_h_