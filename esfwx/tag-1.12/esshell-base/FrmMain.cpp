#include "stdafx.h"
#pragma hdrstop

#include <algorithm>
#include <wx/dir.h>
#include "EsAboutDlg.h"
#include "FrmMain.h"

// bitmap resources
#include "res/file-logoff.xpm"
#include "res/exit.xpm"
#include "res/open.xpm"
#include "res/actions-clear-log.xpm"
#include "res/help.xpm"
#include "res/langs.xpm"
#include "res/menu-options.xpm"

// minimal & default frame size
static const wxSize c_minSize(640, 480);
// status bar field widths
static const int c_StatWidths[FrmMain::STATUSBAR_FIELDS_COUNT] = {-1, -1};

// configuration entries
DEF_CFG_SECTION(MAIN_FORM);
	DEF_CFG_KEY(AUI);
	DEF_CFG_KEY(RECENT_PLUGIN);
	DEF_CFG_KEY(LOG_VISIBLE);	
	DEF_CFG_KEY(FULL_SCREEN);

// string defines
#define STR_EXIT_LABEL				_("E&xit") + wxT("\tCTRL+X")
#define STR_EXIT_HINT					_("Exit application")
#define STR_LOGOFF_LABEL			_("&Log off") + wxT("\tCTRL+L")
#define STR_LOGOFF_HINT				_("Log off from current user session")

// plugin module class
//
PluginModule::PluginModule(const EsString& modulePathName) :
wxDynamicLibrary(modulePathName.c_str(), wxDL_DEFAULT|wxDL_QUIET),
m_pfn(0)
{
	if( IsLoaded() )
	{
		m_pfn = static_cast<PluginModuleIntfGetter>( GetSymbol(PLUGIN_MODULE_INTF_GETTER) );
		if( !m_pfn )
			EsException::Throw(0, EsException::Generic, 
				_("Plugin module entry point could not be found in '%s'"), modulePathName.c_str());
	}
	else
		EsException::Throw(0, EsException::Critical, _("Plugin module '%s' could not be loaded"),
			modulePathName.c_str());
}

PluginModuleIntf* PluginModule::getIntf() const
{
	wxASSERT(m_pfn);
	return m_pfn();
}

// internal tree data class
class PluginSelectorData : public wxTreeItemData
{
public:
	PluginSelectorData(PluginIntf* plugin) :
	wxTreeItemData(),
	m_plugin(plugin) 
	{ wxASSERT(plugin); }

	PluginIntf* getPlugin() const { return m_plugin; }

protected:
	PluginIntf* m_plugin;
};

// main window interface implementation
//
FrmMain::MainWindowIntfImpl::~MainWindowIntfImpl()
{
}

// provide menu entry points for plugin installation
//
ES_IMPL_INTF_METHOD(wxMenuBar&, FrmMain::MainWindowIntfImpl::getMenuBar)() const
{
	wxASSERT(m_this.m_mnuBar);
	return *m_this.m_mnuBar;
}

ES_IMPL_INTF_METHOD(wxMenu&, FrmMain::MainWindowIntfImpl::getFileMenu)() const
{
	wxASSERT(m_this.m_mnuFile);
	return *m_this.m_mnuFile;
}

ES_IMPL_INTF_METHOD(wxMenu&, FrmMain::MainWindowIntfImpl::getActionsMenu)() const
{
	wxASSERT(m_this.m_mnuActions);
	return *m_this.m_mnuActions;
}

ES_IMPL_INTF_METHOD(wxMenu&, FrmMain::MainWindowIntfImpl::getOptionsMenu)() const
{
	wxASSERT(m_this.m_mnuOptions);
	return *m_this.m_mnuOptions;
}

ES_IMPL_INTF_METHOD(wxMenu&, FrmMain::MainWindowIntfImpl::getViewMenu)() const
{
	wxASSERT(m_this.m_mnuView);
	return *m_this.m_mnuView;
}

ES_IMPL_INTF_METHOD(wxMenu&, FrmMain::MainWindowIntfImpl::getHelpMenu)() const
{
	wxASSERT(m_this.m_mnuHelp);
	return *m_this.m_mnuHelp;
}

// allow creation of toolbars
ES_IMPL_INTF_METHOD(wxAuiToolBar*, FrmMain::MainWindowIntfImpl::createToolbar)(int id, bool shown /*= true*/)
{
	wxAuiToolBar* result = new wxAuiToolBar(&m_this, id);
	wxASSERT(result);
	result->SetToolBitmapSize( wxSize(16, 16) );
	result->Show(shown);
	return result;
}

ES_IMPL_INTF_METHOD(void, FrmMain::MainWindowIntfImpl::finalizeToolbar)(wxAuiToolBar* toolbar)
{
	if( toolbar )
	{
		toolbar->Realize();
		m_this.m_mgr.AddPane(toolbar, wxAuiPaneInfo().Name( wxT("tbar") + wxString::Format( wxT("-%d"), toolbar->GetId() ) ).
		 ToolbarPane().Top().Floatable(false).LeftDockable(false).RightDockable(false).
		 Show( m_this.m_mgr.GetPane(m_this.m_toolBar).IsShown() && toolbar->IsShown() ));
		m_this.m_mgr.Update();
	}
}

ES_IMPL_INTF_METHOD(void, FrmMain::MainWindowIntfImpl::deleteToolbar)(wxAuiToolBar* toolbar)
{
	if( toolbar )
	{
		wxAuiPaneInfo& pane = m_this.m_mgr.GetPane(toolbar);
		if( pane.IsOk() )
		{
			m_this.m_mgr.ClosePane( pane );
			m_this.m_mgr.DetachPane(toolbar);
			m_this.m_mgr.Update();
		}
		toolbar->Destroy();
	}
}

ES_IMPL_INTF_METHOD(void, FrmMain::MainWindowIntfImpl::showToolbar)(wxAuiToolBar* toolbar, bool show)
{
	if( toolbar )
	{
		toolbar->Show(show);
		wxAuiPaneInfo& pane = m_this.m_mgr.GetPane(toolbar);
		if(pane.IsOk())
		{
			pane.Show(show);
			m_this.m_mgr.Update();
		}
	}
}

ES_IMPL_INTF_METHOD(bool, FrmMain::MainWindowIntfImpl::toolbarIsShown)(wxAuiToolBar* toolbar) const
{
	if( toolbar )
	{
		wxAuiPaneInfo& pane = m_this.m_mgr.GetPane(toolbar);
		if(pane.IsOk())
			return pane.IsShown() && toolbar->IsShown();
	}

	return false;
}

// return plugin view container
ES_IMPL_INTF_METHOD(wxScrolledWindow&, FrmMain::MainWindowIntfImpl::getViewContainer)() const
{
	wxASSERT(m_this.m_plgView);
	return *m_this.m_plgView;
}

// return (communication)logger intf
ES_IMPL_INTF_METHOD(EsLogIntf*, FrmMain::MainWindowIntfImpl::getLogger)() const
{
	if(m_this.m_log)
		return m_this.m_log->getLogIntf();
	
	return 0;
}

ES_IMPL_INTF_METHOD(EsNotifierIntf*, FrmMain::MainWindowIntfImpl::getNotifier)() const 
{ 
	if(m_this.m_notifier)
		return m_this.m_notifier->getIntf();
		
	return 0;	 
}

// printing framework access
ES_IMPL_INTF_METHOD(void, FrmMain::MainWindowIntfImpl::pageSetup)() const
{
	wxASSERT(m_this.m_printing);
	m_this.m_printing->PageSetup();
}

ES_IMPL_INTF_METHOD(bool, FrmMain::MainWindowIntfImpl::previewHtml)(const wxString& html) const
{
	wxASSERT(m_this.m_printing);
	return m_this.m_printing->PreviewText(html);
}

ES_IMPL_INTF_METHOD(bool, FrmMain::MainWindowIntfImpl::printHtml)(const wxString& html) const
{
	wxASSERT(m_this.m_printing);
	return m_this.m_printing->PrintText(html);
}

// main frame implementation
//
FrmMain::FrmMain() :
wxFrame(0, ID_MAIN_FRAME, wxEmptyString, wxDefaultPosition, c_minSize),
m_initialized(false),
m_fullScreen(false),
m_mnuBar(0),
m_mnuFile(0),
m_mnuActions(0),
m_mnuOptions(0),
m_mnuView(0),
m_mnuHelp(0),
m_toolBar(0),
m_statBar(0),
m_plgSelector(0),
m_plgView(0),
m_lstPlgSelector(24, 24),
m_log(0),
m_notifier(0),
m_intf(*this),
m_printing(0)
{
	// do not show until after the Init is called
	Hide();
	// set frame icon(s)
	SetIcons( ShellApp::getInstance()->getIcons() );
	// manage this window
	m_mgr.SetManagedWindow(this);
	// init plugin selector	
	m_plgSelector = new wxTreeCtrl(this, ID_PLGLIST, wxDefaultPosition, wxDefaultSize,
		wxTR_HIDE_ROOT|wxTR_HAS_BUTTONS|wxTR_TWIST_BUTTONS|wxTR_SINGLE|wxTR_ROW_LINES|wxTR_LINES_AT_ROOT);
	wxASSERT(m_plgSelector);
	m_plgSelector->SetImageList(&m_lstPlgSelector);
	// init plugin view	
	m_plgView = new wxScrolledWindow(this, ID_PLGVIEW);
	wxASSERT(m_plgView);			
	// create logger frame && attach icons to it
	m_log = new FrmLog(this, 1024);
	m_log->SetIcons( ShellApp::getInstance()->getIcons() );
	// attach event handlers
	initEventHandlers();	
}

void FrmMain::updateTitle()
{
	EsString title = EsString::format(_("Ekosfera Shell [%s: %s]"), 
		ShellApp::getInstance()->getCurrentUser().c_str(),
		EsUtilities::getAslString(ShellApp::getInstance()->getCurrentAsl()).c_str());
	
	PluginIntf* plugin = getActivePlugin();
	if( plugin )
		SetTitle(title + wxT(" - ") + plugin->getShortDescription());
	else	
		SetTitle(title);
}

// must be called after the frame construction
void FrmMain::init()
{
	if( !m_initialized )
	{
		// assign shell title
		updateTitle();
		// init printing engine
		m_printing = new wxHtmlEasyPrinting(wxEmptyString, this);
		// restrict minimum frame size
		SetMinSize( c_minSize );
		// init plugin selector image list
		resetPlugSelImgList();
		// init menus and tools
		initMenuAndToolBar();
		// init SB and plugins view
		initStatusBar();
		initPluginsView();
		// load application settings
		loadSettings();
		// init plugins
		initPlugins();
		// show window	
		if( m_fullScreen )
			showFullScreen();
		else
			Show();
		// check if we have registered plugins and none is active
		// in this case we will explicitly activate it 
		PluginIntf* plugin = getActivePlugin();
		if( 0 == plugin && !m_modules.empty() )
		{
			// find first accessible plugin
			for( int idx = 0; idx < m_modules.front()->getIntf()->getPluginCount(); ++idx )
			{
				plugin = m_modules.front()->getIntf()->getPlugin(idx);
				if( plugin )
				{	
					setActivePlugin( plugin );
					break;
				}
			}
		}

		Raise();
		SetFocus();
		
		m_initialized = true;
	}
}

void FrmMain::uninit()
{
	if( m_initialized )
	{
		// unload plugins
		uninitPlugins();
		// save application settings
		saveSettings();
		// uninit menu and toolbar
		uninitMenuAndToolBar();
		// uninit plugin view & status bar
		uninitStatusBar();
		uninitPluginsView();	
		// close logger frame
		m_log->Close();
		// delete html printing framework
		delete m_printing;
		m_printing = 0;
		// reset initialized flag		
		m_initialized = false;
	}
}

FrmMain::~FrmMain()
{
	uninit();
	// uninit aui manager framework
	m_mgr.UnInit();	
	// detouch event handlers
	uninitEventHandlers();	
}

// allow child frames to receive and handle parent command events && channel io events
bool FrmMain::TryBefore(wxEvent& evt)
{
	bool ok = false;
	// do additional processing if needed	
	if( m_initialized )
	{
		int id = evt.GetId();
		if( evt.IsCommandEvent() && 
				(	wxID_PAGE_SETUP == id ||
					wxID_PREVIEW == id || 
					wxID_PRINT == id || 
					id > wxID_HIGHEST ) )
		{
			evt.StopPropagation();
			PluginIntf* plugin = getActivePlugin();
			if( plugin && plugin->getWindow() )
				ok = plugin->getWindow()->HandleWindowEvent(evt);
		}
		else
		{
			wxEventType type = evt.GetEventType();
			if( type == evtIO_CHANNEL || 
					type == evtPROGRESS_ACTION_START || 
					type == evtPROGRESS_ACTION_END ||
					type == evtPROGRESS_PULSE ||
					type == evtPROGRESS_SET_POS )
			{
				evt.StopPropagation();
				if( m_log )
					m_log->HandleWindowEvent(evt);
				PluginIntf* plugin = getActivePlugin();
				if( plugin && plugin->getWindow() )
					ok = plugin->getWindow()->HandleWindowEvent(evt);
			}
		}
	}

	if( !ok )
		// perform standard event processing
		return wxFrame::TryBefore(evt);
	else
		return true;		
}

// init plugin selector image list
void FrmMain::resetPlugSelImgList()
{
	m_lstPlgSelector.RemoveAll();
	m_lstPlgSelector.Add( wxIcon(open_xpm) );
}

// helper methods
void FrmMain::initMenuAndToolBar()
{
	m_mnuBar = new wxMenuBar;
	wxASSERT(m_mnuBar);
	SetMenuBar(m_mnuBar);
	
	// file menu set-up
	m_mnuFile = new wxMenu;
	wxBitmap bmLogoff(file_logoff_xpm, wxBITMAP_TYPE_XPM);
	m_mnuFile->Append( EsUtilities::createNormalMenuItem(m_mnuFile, IDM_FILE_LOGOFF, STR_LOGOFF_LABEL, STR_LOGOFF_HINT, bmLogoff) );
	m_mnuFile->AppendSeparator();
	wxBitmap bmExit = wxBitmap(exit_xpm, wxBITMAP_TYPE_XPM);
	m_mnuFile->Append( EsUtilities::createNormalMenuItem(m_mnuFile, wxID_EXIT, STR_EXIT_LABEL, STR_EXIT_HINT, bmExit) );

	// actions menu set-up
	m_mnuActions = new wxMenu;
	m_mnuActions->Append( EsUtilities::createNormalMenuItem(m_mnuActions, IDM_ACTIONS_CLEAR_LOG, 
		_("Clear log"), _("Clear activities log"), wxBitmap(actions_clear_log_xpm, wxBITMAP_TYPE_XPM)) );
	m_mnuActions->Append( EsUtilities::createCheckMenuItem(m_mnuActions, IDM_ACTIONS_LOG_TO_FILE,
		_("Log to file"), _("Enable logging to file")) );
 
	// options menu set-up
	m_mnuOptions = new wxMenu;
	m_mnuOptions->Append(
		EsUtilities::createNormalMenuItem(m_mnuOptions, IDM_OPTIONS_SHELL, 
		_("Shell Options..."), _("Configure shell options"), wxBitmap(menu_options_xpm, wxBITMAP_TYPE_XPM)) );
	// create language selection menu
	m_mnuOptions->Append( 
		EsUtilities::createNormalMenuItem(m_mnuOptions, wxID_ANY, _("Language"), _("Select user interface language"), 
		wxBitmap(langs_xpm, wxBITMAP_TYPE_XPM), 
			EsUtilities::createLanguageSelectionMenu(
			m_intf.getApp()->getInstalledLanguages(), IDM_OPTIONS_LANGUAGE_START )) );

	// view menu set-up
	m_mnuView = new wxMenu;
	m_mnuView->Append(
		EsUtilities::createCheckMenuItem(m_mnuView, IDM_VIEW_PLGLIST, _("View Plugin List"), _("Show|hide plugin list pane")) );
	m_mnuView->Append(
		EsUtilities::createCheckMenuItem(m_mnuView, IDM_VIEW_TOOLBAR, _("View Toolbar"), _("Show|hide quick access toolbar")) );
	m_mnuView->Append(
		EsUtilities::createCheckMenuItem(m_mnuView, IDM_VIEW_LOG, _("View Log"), _("Show|hide application log")) );
	m_mnuView->AppendSeparator();
	m_mnuView->Append(
		EsUtilities::createCheckMenuItem(m_mnuView, IDM_VIEW_FULL_SCREEN, _("Full screen") + wxT("\tF11"), _("Toggle full screen mode")) );

	// help menu set-up
	m_mnuHelp = new wxMenu;
	m_mnuHelp->Append( EsUtilities::createNormalMenuItem(m_mnuHelp, wxID_HELP, _("Help") + wxT("\tF1"), _("Application help"), 
		wxBitmap(help_xpm, wxBITMAP_TYPE_XPM)) );
//	m_mnuHelp->Append(IDM_HELP_UPDATE, _("Update Shell"));
	m_mnuHelp->AppendSeparator();
	m_mnuHelp->Append( EsUtilities::createNormalMenuItem(m_mnuHelp, wxID_ABOUT, 
		_("About Shell...") + wxT("\tCTRL+F1"), _("Display information about Sell appplication"),
		wxBitmap(help_xpm, wxBITMAP_TYPE_XPM)) );

	m_mnuBar->Append(m_mnuFile, _("File"));
	m_mnuBar->Append(m_mnuActions, _("Actions"));
	m_mnuBar->Append(m_mnuOptions, _("Options"));
	m_mnuBar->Append(m_mnuView, _("View"));
	m_mnuBar->Append(m_mnuHelp, _("Help"));

	// toolbar initialization
	m_toolBar = m_intf.createToolbar(ID_TOOLBAR);
	m_toolBar->AddTool(wxID_EXIT, STR_EXIT_LABEL, bmExit, STR_EXIT_HINT);
	m_toolBar->AddSeparator();	
	m_toolBar->AddTool(IDM_FILE_LOGOFF, STR_LOGOFF_LABEL, bmLogoff, STR_LOGOFF_HINT);
	m_intf.finalizeToolbar(m_toolBar);
	
	// apply menu font settings
	ShellApp::getInstance()->setMenuFont();
}

void FrmMain::uninitMenuAndToolBar()
{
	if( m_mnuBar )
	{
		SetMenuBar(0);
		// remove menu bar && menu branches
		m_mnuBar->Destroy();
		m_mnuBar = 0;
		// all owned menus get deleted by menu bar, so
		// just zero pointers, for convenience
		m_mnuFile = 0;
		m_mnuActions = 0;
		m_mnuOptions = 0;
		m_mnuView = 0;
		m_mnuHelp = 0;
		
		// remove main toolbar
		m_intf.deleteToolbar( m_toolBar );
		m_toolBar = 0;
		
		// reset accelerator table which may left from menu and toolbar
		SetAcceleratorTable( wxAcceleratorTable() );
	}
}

void FrmMain::initStatusBar()
{
	wxASSERT(0 == m_statBar);
	m_statBar = CreateStatusBar(STATUSBAR_FIELDS_COUNT, wxST_SIZEGRIP | wxFULL_REPAINT_ON_RESIZE, ID_STATUSBAR);
	m_statBar->SetStatusWidths(STATUSBAR_FIELDS_COUNT, c_StatWidths);
	SetStatusBarPane(1);

	wxRect r;
	m_statBar->GetFieldRect(0, r);
	m_notifier = new NotifierCtl(m_statBar, wxID_ANY, r.GetPosition(), r.GetSize());
	wxASSERT( m_notifier );
}

void FrmMain::uninitStatusBar()
{
	if(	m_statBar )
	{
		m_statBar->Destroy();
		m_statBar = 0;
		
		// removing status bar automatically removes notifier control which is owned by it
		// so just zero its pointer
		m_notifier = 0;
	}
}

void FrmMain::initPluginsView()
{
	wxASSERT(m_plgSelector);
	m_mgr.AddPane(m_plgSelector, wxAuiPaneInfo().Name(wxT("selector")).
		Left().Floatable(false).TopDockable(false).BottomDockable(false).
		Caption(_("Registered plugins")));
	
	wxASSERT(m_plgView);
	m_mgr.AddPane(m_plgView, wxAuiPaneInfo().Name(wxT("view")).
		Center().Maximize().CaptionVisible(false));
	
	m_mgr.Update();
	m_mgrCfg = m_mgr.SavePerspective();	
}

void FrmMain::uninitPluginsView()
{
	wxAuiPaneInfo pane = m_mgr.GetPane(m_plgSelector);
	if( pane.IsOk() )
	{
		m_mgr.ClosePane(pane);
		m_mgr.DetachPane(m_plgSelector);
	}
	
	pane = m_mgr.GetPane(m_plgView);
	if( pane.IsOk() )
	{
		m_mgr.ClosePane(pane);
		m_mgr.DetachPane(m_plgView);
	}
	
	m_mgr.Update();
}

void FrmMain::initEventHandlers()
{
	// frame events
	//
	Bind(wxEVT_CLOSE_WINDOW, &FrmMain::onClose, this);
	// plugin view events
	//
	m_plgView->Bind(wxEVT_SIZE, &FrmMain::onPluginViewSize, this, ID_PLGVIEW);
	// menu connect events
	//
	// file
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onFileExit, this, wxID_EXIT);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onPageSetup, this, wxID_PAGE_SETUP);
	// view
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewPluginSelector, this, IDM_VIEW_PLGLIST);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewToolbar, this, IDM_VIEW_TOOLBAR);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewLog, this, IDM_VIEW_LOG);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewFullscreen, this, IDM_VIEW_FULL_SCREEN);
	// actions
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onActClearLog, this, IDM_ACTIONS_CLEAR_LOG);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onActLogToFileToggle, this, IDM_ACTIONS_LOG_TO_FILE);
	// options
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onOptShell, this, IDM_OPTIONS_SHELL);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onOptLang, this, IDM_OPTIONS_LANGUAGE_START, IDM_OPTIONS_LANGUAGE_END);
	// help
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onHelp, this, wxID_HELP);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onHelpAbout, this, wxID_ABOUT);
	// control events
	//
	// plugin selector events
	Bind(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &FrmMain::onPluginActivate, this, ID_PLGLIST);
	Bind(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, &FrmMain::onPluginCategoryCollapsing, this, ID_PLGLIST);	
	// update UI events
	Bind(wxEVT_UPDATE_UI, &FrmMain::onUpdateUi, this, IDM_FILE_LOGOFF, ID_CORE_HIGHEST-1);
}

void FrmMain::uninitEventHandlers()
{
	// frame events
	//
	Unbind(wxEVT_CLOSE_WINDOW, &FrmMain::onClose, this);
	// plugin view events
	//
	m_plgView->Unbind(wxEVT_SIZE, &FrmMain::onPluginViewSize, this, ID_PLGVIEW);
	// menu connect events
	//
	// file
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onFileExit, this, wxID_EXIT);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onPageSetup, this, wxID_PAGE_SETUP);
	// view
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewPluginSelector, this, IDM_VIEW_PLGLIST);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewToolbar, this, IDM_VIEW_TOOLBAR);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewLog, this, IDM_VIEW_LOG);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onViewFullscreen, this, IDM_VIEW_FULL_SCREEN);
	// actions
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onActClearLog, this, IDM_ACTIONS_CLEAR_LOG);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onActLogToFileToggle, this, IDM_ACTIONS_LOG_TO_FILE);	
	// options
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onOptShell, this, IDM_OPTIONS_SHELL);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onOptLang, this, IDM_OPTIONS_LANGUAGE_START, IDM_OPTIONS_LANGUAGE_END);
	// help
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onHelp, this, wxID_HELP);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmMain::onHelpAbout, this, wxID_ABOUT);
	// control events
	//
	// plugin selector events
	Unbind(wxEVT_COMMAND_TREE_ITEM_ACTIVATED, &FrmMain::onPluginActivate, this, ID_PLGLIST);
	Unbind(wxEVT_COMMAND_TREE_ITEM_COLLAPSING, &FrmMain::onPluginCategoryCollapsing, this, ID_PLGLIST);	
	// update UI events
	Unbind(wxEVT_UPDATE_UI, &FrmMain::onUpdateUi, this, IDM_FILE_LOGOFF, ID_CORE_HIGHEST-1);
}

void FrmMain::initPlugins()
{
	EsAppIntf* app = m_intf.getApp();

	// enumerate all modules under plugins path,
	// which have proper entry points in them
	//
	EsString plgRoot = app->getPluginsRoot();
	wxDir dir( plgRoot );
	wxString pluginFile;
	bool ok = dir.GetFirst(&pluginFile, wxT("*.plug.dll"), wxDIR_FILES);
	EsString::Array errLog;
	errLog.reserve(32);
	while(ok)
	{
		try
		{
			PluginModule* mod = new PluginModule( plgRoot + pluginFile );
			PluginModuleIntf* intf = mod->getIntf();
			if( intf )
			{
				intf->attach(app);
				m_modules.push_back( mod );
			}
			else
				EsException::Throw(0, EsException::Generic, 
					_("Error accessing plugin interface for '%s'"), pluginFile.t_str());
		}
		catch(const EsException& ex)
		{
			errLog.push_back( ex.getMsg() );
		}
		catch(...)
		{
			errLog.push_back( 
				EsString::format(_("Unidentified error occured while loading '%s' plugin module"), pluginFile.t_str()) );
		}
		ok = dir.GetNext(&pluginFile);
	}

	// build plugins list view
	reBuildPluginsView();
	
	// try to activate recently saved plugin as a last step
	wxConfigBase* cfg = m_intf.getApp()->getCfg();
	wxASSERT(cfg);	
	cfg->SetPath( CFG_MAIN_FORM_GROUP );
	wxString pluginId;	
	if( cfg->Read( CFG_RECENT_PLUGIN_KEY, &pluginId) )
	{
		PluginIntf* plugin = findPluginById( pluginId );
		setActivePlugin(plugin);
	}
	
	// show optional plugin loading error info
	if( !errLog.empty() )
	{
		EsUtilities::showErrorFmt(_("Error(s) occured"), 
			_("The following error(s) occurred while loading plugin modules:\n\t%s"),
			EsString::fromStringArray(errLog, wxT("\n\t")).c_str() );
	}
}

void FrmMain::uninitPlugins()
{
	PluginModuleList::reverse_iterator rit = m_modules.rbegin();
	while( rit != m_modules.rend() )
	{
		PluginModuleIntf* intf = (*rit)->getIntf();
		wxASSERT(intf);
		for( int idx = 0; idx < intf->getPluginCount(); ++idx )
		{
			PluginIntf* plugin = intf->getPlugin(idx);
			if(plugin)
			{
				plugin->deActivate(true);
				wxASSERT(!plugin->isActive());
			}
		}
		
		// unload plugin library here
		delete (*rit);

		++rit;
	}

	// cleanup container
	m_modules.clear();
	
	// update plugins list view
	reBuildPluginsView();
}

// find category by name
wxTreeItemId FrmMain::findCategoryBranch(const EsString& category) const
{
	wxTreeItemIdValue cookie;
	wxTreeItemId root = m_plgSelector->GetRootItem();
	wxASSERT( root.IsOk() );
	wxTreeItemId item = m_plgSelector->GetFirstChild( root, cookie );

	while( item.IsOk() )
	{
		if( 0 == category.compare(m_plgSelector->GetItemText(item).c_str()) )
			return item;

		item = m_plgSelector->GetNextSibling(item);
	}

	// NOK empty item returned
	return wxTreeItemId();
}

// find plugin tree leaf
wxTreeItemId FrmMain::findPluginLeaf(PluginIntf* plugin) const
{
	if( plugin )
	{
		wxTreeItemId category = findCategoryBranch( plugin->getCategory() );
		if( category.IsOk() )
		{
			wxTreeItemIdValue cookie;
			wxTreeItemId item = m_plgSelector->GetFirstChild(category, cookie);
			while( item.IsOk() )
			{
				// find currently active plugin
				PluginSelectorData* data = dynamic_cast<PluginSelectorData*>(m_plgSelector->GetItemData(item));
				if( data && data->getPlugin() == plugin )
					return item;

				item = m_plgSelector->GetNextChild(item, cookie);
			}
		}
	}
	
	// NOK empty item returned
	return wxTreeItemId();
}

void FrmMain::reBuildPluginsView()
{
	wxWindowUpdateLocker noUpdate( m_plgSelector );
	m_plgSelector->DeleteAllItems();
	wxTreeItemId item = m_plgSelector->AddRoot(EsString::s_null);
	wxASSERT( item.IsOk() );
	// reset plugin images
	resetPlugSelImgList();

	PluginModuleList::const_iterator cit = m_modules.begin();
	for(; cit != m_modules.end(); ++cit )
	{
		PluginModuleIntf* intf = (*cit)->getIntf();
		for( int idx = 0; idx < intf->getPluginCount(); ++idx )
		{
			PluginIntf* plugin = intf->getPlugin(idx);
			if( plugin )
			{
				// categories go in root nodes
				wxTreeItemId category = findCategoryBranch( plugin->getCategory() );
				if( !category.IsOk() )
				{
					wxTreeItemId root = m_plgSelector->GetRootItem();
					category = m_plgSelector->AppendItem( root, plugin->getCategory(), 0, 0 );
				}
				// append plugin entry to the plugin seletor
				addPluginEntry(category, plugin);
			}
		}
	}
}

// append plugin entry to the plugin seletor
void FrmMain::addPluginEntry(wxTreeItemId category, PluginIntf* plugin)
{
	wxASSERT( category.IsOk() );
	wxASSERT( plugin );

	int idxImg = m_lstPlgSelector.Add(plugin->getIcon());
	wxTreeItemId item = m_plgSelector->AppendItem(category, 
		plugin->getShortDescription(),
		idxImg, idxImg, new PluginSelectorData(plugin) );
	if( plugin->isActive() )
		m_plgSelector->SetItemBold( item );
}

// return currently active plugin entry
PluginIntf* FrmMain::getActivePlugin() const
{
	PluginModuleList::const_iterator cit = m_modules.begin();
	for(; cit != m_modules.end(); ++cit )
	{
		PluginModuleIntf* intf = (*cit)->getIntf();
		for( int idx = 0; idx < intf->getPluginCount(); ++idx )
		{
			PluginIntf* plugin = intf->getPlugin(idx);
			if( plugin && plugin->isActive() )
				return plugin;
		}
	}

	return 0;
}

// return all loaded plugin categories
EsString::Array FrmMain::getPluginCategories() const
{
	EsString::Array result;

	PluginModuleList::const_iterator cit = m_modules.begin();
	for(; cit != m_modules.end(); ++cit )
	{
		PluginModuleIntf* intf = (*cit)->getIntf();
		for( int idx = 0; idx < intf->getPluginCount(); ++idx )
		{
			PluginIntf* plugin = intf->getPlugin(idx);
			if( plugin )
			{
				EsString category = plugin->getCategory();
				if( std::find(result.begin(), result.end(), category) == result.end() )
					result.push_back(category);
			}
		}
	}

	return result;
}

// return plugins per category
FrmMain::PluginList FrmMain::getPluginsByCategory(const EsString& category) const
{
	PluginList result;

	PluginModuleList::const_iterator cit = m_modules.begin();
	for( ; cit != m_modules.end(); ++cit )
	{
		PluginModuleIntf* intf = (*cit)->getIntf();
		for( int idx = 0; idx < intf->getPluginCount(); ++idx )
		{
			PluginIntf* plugin = intf->getPlugin(idx);
			if( plugin && category == plugin->getCategory() )
				result.push_back(plugin);
		}
	}

	return result;
}

// return plugin by its id string
PluginIntf* FrmMain::findPluginById(const EsString& id) const
{
	PluginModuleList::const_iterator cit = m_modules.begin();
	for( ; cit != m_modules.end(); ++cit )
	{
		PluginModuleIntf* intf = (*cit)->getIntf();
		for( int idx = 0; idx < intf->getPluginCount(); ++idx )
		{
			PluginIntf* plugin = intf->getPlugin(idx);
			if( plugin && id == plugin->getId() )
				return plugin;
		}
	}

	return 0;
}

// return total plugins count
int FrmMain::getPluginsCount() const
{
	int result = 0;

	PluginModuleList::const_iterator cit = m_modules.begin();
	for( ; cit != m_modules.end(); ++cit )
	{
		PluginModuleIntf* intf = (*cit)->getIntf();
		result += intf->getPluginCount();
	}	

	return result;
}

// wrapper around plugin->activate
bool FrmMain::setActivePlugin( PluginIntf* plugin )
{
	bool result = false;
	PluginIntf* oldActive = getActivePlugin();
	if( oldActive != plugin )
	{
		if( !oldActive || (oldActive && oldActive->deActivate()) )
		{
			// un-bold previously active item
			wxTreeItemId item = findPluginLeaf(oldActive);
			if( item.IsOk() )
				m_plgSelector->SetItemBold( item, false );

			if( plugin )
			{
				plugin->activate();
				if( plugin->isActive() )
				{
					item = findPluginLeaf(plugin);
					wxASSERT( item.IsOk() );
					m_plgSelector->SetItemBold( item );
					wxTreeItemId parent = m_plgSelector->GetItemParent(item);
					wxASSERT(parent.IsOk());
					m_plgSelector->Expand(parent);
					result = true;
				}
			}
			else
				result = true;
			
			EsUtilities::applyCtlsFont(m_plgView, ShellApp::getInstance()->getCtlsFont());
			// adjust things
			adjustPluginViewSize(plugin);
		}
	}
	else
		result = true;
	
	// update shell title
	updateTitle();
	
	return result;
}

void FrmMain::loadSettings()
{
	wxConfigBase* cfg = m_intf.getApp()->getCfg();
	wxASSERT(cfg);

	cfg->SetPath( CFG_MAIN_FORM_GROUP );
	EsUtilities::loadWindowPos(cfg, this, c_minSize);
	// AUI manager settings
	m_mgrCfg = cfg->Read( CFG_AUI_KEY ).c_str();
	if( !m_mgrCfg.empty() )
		m_mgr.LoadPerspective(m_mgrCfg.c_str());
	
	// full screen
	m_fullScreen = cfg->ReadBool(CFG_FULL_SCREEN_KEY, false);
		
	bool showLog;
	if( cfg->Read( CFG_LOG_VISIBLE_KEY, &showLog, false ) )
		m_log->Show( showLog );
}

void FrmMain::adjustPluginViewSize(PluginIntf* plugin)
{
	if( m_plgView && plugin )
	{
		wxWindow* frame = plugin->getWindow();
		if(frame)
		{
			wxASSERT(m_plgView);
			wxSize sze = m_plgView->GetClientSize();
			wxSize frameSize = frame->GetMinSize();
			frameSize.x = wxMax( frameSize.x, sze.x );
			frameSize.y = wxMax( frameSize.y, sze.y );
			frame->SetSize( frameSize );
			frame->Layout();
		}	
	}	
}

void FrmMain::showFullScreen()
{
	ShowFullScreen(true, wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION);
}

void FrmMain::saveSettings()
{
	wxConfigBase* cfg = m_intf.getApp()->getCfg();
	wxASSERT(cfg);

	cfg->SetPath( CFG_MAIN_FORM_GROUP );
	EsUtilities::saveWindowPos(cfg, this);
	// active plugin
	if( !m_recentActivePluginId.empty() )
		cfg->Write( CFG_RECENT_PLUGIN_KEY, m_recentActivePluginId.c_str() );
	// AUI manager settings
	cfg->Write( CFG_AUI_KEY, m_mgrCfg );
	// fullscreen
	cfg->Write( CFG_FULL_SCREEN_KEY, m_fullScreen );
	// log visibility
	cfg->Write( CFG_LOG_VISIBLE_KEY, m_log->IsShown() );
}

//---------------------------------------------------------------------------------------------
// event handlers
void FrmMain::onFileExit(wxCommandEvent& WXUNUSED(evt))
{
	Close();
}

void FrmMain::onClose(wxCloseEvent& evt)
{
	m_mgrCfg = m_mgr.SavePerspective();

	// query close active plugin
	PluginIntf* plugin = getActivePlugin();
	bool canClose = true;
	m_recentActivePluginId.clear();
	if( plugin )
	{
		// save last 
		m_recentActivePluginId = plugin->getId();
		canClose = plugin->deActivate( !evt.CanVeto() );
	}
	
	if( !canClose )
		evt.Veto();
	else
		Destroy();
}

void FrmMain::onPageSetup(wxCommandEvent& WXUNUSED(evt))
{
	m_printing->PageSetup();
}

// plugin view size handler
void FrmMain::onPluginViewSize(wxSizeEvent& evt)
{
	adjustPluginViewSize( getActivePlugin() );
	evt.Skip();
}

// actions menu handlers
void FrmMain::onActClearLog(wxCommandEvent& WXUNUSED(evt))
{
	m_intf.getLogger()->clear();
}

void FrmMain::onActLogToFileToggle(wxCommandEvent& evt)
{
	m_intf.getLogger()->enableFileLogging( evt.IsChecked() );
}

// view mnu handlers
void FrmMain::onViewPluginSelector(wxCommandEvent& evt)
{
	m_mgr.GetPane(m_plgSelector).Show( evt.IsChecked() );
	m_mgr.Update();
}

void FrmMain::onViewToolbar(wxCommandEvent& evt)
{
	// hide all toolbar panes
	wxAuiPaneInfoArray& panes = m_mgr.GetAllPanes();
	for(size_t idx = 0; idx < panes.size(); ++idx )
	{
		if( panes[idx].IsToolbar() )
			panes[idx].Show( evt.IsChecked() );
	}
	m_mgr.Update();
}

void FrmMain::onViewLog(wxCommandEvent& evt)
{
	if( evt.IsChecked() )
		m_log->Show();
	else
		m_log->Hide();	
}

void FrmMain::onViewFullscreen(wxCommandEvent& evt)
{
	if( evt.IsChecked() )
	{
		showFullScreen();
		m_fullScreen = true;
	}
	else
	{
		ShowFullScreen(false);
		m_fullScreen = false;
	}
}

// options menu handlers
void FrmMain::onOptShell(wxCommandEvent& evt)
{
	ShellApp::getInstance()->customizeShell();
}

void FrmMain::onOptLang(wxCommandEvent& evt)
{
	m_intf.getApp()->setLanguageId(
		EsUtilities::handleLangSelectionEvent(m_intf.getApp()->getLanguageId(), IDM_OPTIONS_LANGUAGE_START, evt) );
}

// view mnu update handling
void FrmMain::onUpdateUi(wxUpdateUIEvent& evt)
{
	int id = evt.GetId();
	
	if( id >= IDM_OPTIONS_LANGUAGE_START && id <= IDM_OPTIONS_LANGUAGE_END )
	{
		// set radio check on the selected language
		int langId = id-IDM_OPTIONS_LANGUAGE_START;
		evt.Check( langId == m_intf.getApp()->getLanguageId() );
	}
	else
	{
		switch( id )
		{
		case IDM_ACTIONS_CLEAR_LOG:
			evt.Enable( !m_intf.getLogger()->isEmpty() );
			break;
		case IDM_ACTIONS_LOG_TO_FILE:
			evt.Check( m_intf.getLogger()->isFileLoggingEnabled() );
			break;
		case IDM_VIEW_PLGLIST:
			evt.Check( m_mgr.GetPane(m_plgSelector).IsShown() );
			break;
		case IDM_VIEW_TOOLBAR:
			evt.Check( m_mgr.GetPane(m_toolBar).IsShown() );
			break;
		case IDM_VIEW_LOG:
			evt.Check( m_log->IsShown() );
			break;
		case IDM_VIEW_FULL_SCREEN:
			evt.Check( m_fullScreen );
			break;
		}
	}
}

// help menu commands
void FrmMain::onHelp(wxCommandEvent& evt)
{
	wxHelpControllerBase& hc = ShellApp::getInstance()->getHelpController();
	
	if( hc.LoadFile( ShellApp::getAppRootStatic() + wxT("esshell.chm")) )
		hc.DisplayContents();
}

void FrmMain::onHelpAbout(wxCommandEvent& WXUNUSED(evt))
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.AddDeveloper( _("Vsevolod V Gromov <vsevolod.gromov@mailfrom.ru>") );
	aboutInfo.AddTranslator( _("Vsevolod V Gromov <vsevolod.gromov@mailfrom.ru>") );
	aboutInfo.SetIcon( m_icons.GetIcon( 48 ) );
	aboutInfo.SetCopyright( COPYRIGHT );
	aboutInfo.SetVersion( PRODUCT_VERSION_STRING );
	
	aboutInfo.SetDescription( EsString::format( _("%s\nPowered by:\n\t%s\n\t%s\n\t%s"), 
		_("Ekosfera Multipurpose Extendable Application Shell").t_str(),
		wxVERSION_STRING,
		wxT("wxFreeChart: 1.6"),
		wxT("boost: 1.46.1") ));
	aboutInfo.SetWebSite( wxT("www.ekosf.ru"), _("Product web site") );
	
	EsAboutDlg dlg( this, aboutInfo );
	dlg.ShowModal();
}

// plugin selection changed
void FrmMain::onPluginActivate(wxTreeEvent& evt)
{
	wxTreeItemId item = evt.GetItem();
	if( item.IsOk() )
	{
		// find currently active plugin
		PluginSelectorData* data = dynamic_cast<PluginSelectorData*>(m_plgSelector->GetItemData(item));
		if( data )
			setActivePlugin( data->getPlugin() );
	}
}

// respond to tree branch collapsing
void FrmMain::onPluginCategoryCollapsing(wxTreeEvent& evt)
{
	wxTreeItemId item = evt.GetItem();
	if( item.IsOk() )
	{
		// get currentlu active plugin 
		wxTreeItemId plugin = findPluginLeaf( getActivePlugin() );
		if( plugin.IsOk() )
		{
			wxTreeItemId category = m_plgSelector->GetItemParent(plugin);
			// if we're trying to collapse category which active plugin belongs to,
			// veto this event
			if( category.IsOk() && category == item )
				evt.Veto();				
		}
	}
}

// status bar sizing handler
void FrmMain::onStatusBarSize(wxSizeEvent& evt)
{
	// update size of embedded notifier control
	wxRect r;
	m_statBar->GetFieldRect(0, r);
	m_notifier->SetSize(r.GetSize());

	evt.Skip();
}