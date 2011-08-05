#ifndef _plugin_intf_h_
#define _plugin_intf_h_

// plugin module interface should consist of the single 
// exported function returning PluginModule interface
// ekosfera shell plugin interface declarations
//

// plugin module
ES_DECL_INTF_BEGIN2( 5ED2298C, F8644f1a, AD046A08, 9E9827FC, PluginModuleIntf, EsBaseIntf )
	ES_DECL_INTF_METHOD(EsString, getName)() const = 0;
	ES_DECL_INTF_METHOD(void, attach)(EsAppIntf* app) = 0;
	ES_DECL_INTF_METHOD(EsAppIntf*, getApp)() const = 0;
	ES_DECL_INTF_METHOD(bool, isAttached)() const = 0;
	ES_DECL_INTF_METHOD(int, getPluginCount)() const  = 0;
	ES_DECL_INTF_METHOD(PluginIntf*, getPlugin)(int idx) = 0;
	ES_DECL_INTF_METHOD(EsString, getFileName)() const = 0;
ES_DECL_INTF_END

// plugin entry
ES_DECL_INTF_BEGIN2( 18ACD363, F12B4ff8, 8896822D, 1EC14301, PluginIntf, EsBaseIntf )
	// r/o plugin information access
	//
	ES_DECL_INTF_METHOD(AppSecurityLevel, getMinAsl)() const = 0; // return minimal security level required to access plugin
	ES_DECL_INTF_METHOD(EsString, getId)() const = 0; // string plugin identifier. do not care how, but must be unique among all plugins
	ES_DECL_INTF_METHOD(EsString, getCategory)() const = 0;
	ES_DECL_INTF_METHOD(EsString, getShortDescription)() const = 0;
	ES_DECL_INTF_METHOD(EsString, getLongDescription)() const = 0;
	ES_DECL_INTF_METHOD(wxIcon, getIcon)() const = 0;
	ES_DECL_INTF_METHOD(PluginModuleIntf*, getModule)() const = 0;
	// return full plugin settings file name
	//
	ES_DECL_INTF_METHOD(EsString, getSettingsFileName)() const = 0;
	// plugin activation|status query in application framework
	//
	ES_DECL_INTF_METHOD(void, activate)() = 0;
	ES_DECL_INTF_METHOD(bool, deActivate)(bool force = false) = 0;
	ES_DECL_INTF_METHOD(bool, isActive)() const = 0;
	// return plugin frame window, if there is one
	ES_DECL_INTF_METHOD(wxWindow*, getWindow)() = 0;
ES_DECL_INTF_END

// plugin module entry point access type
typedef PluginModuleIntf* (__cdecl *PluginModuleIntfGetter)();
// interface getter naming, for proper dll symbol search
#define PLUGIN_MODULE_INTF_GETTER	wxT("getPluginModuleIntf")

#define DEF_PLUGIN_MODULE(ClsName, Name) \
	class ClsName ## Module : public PluginModuleIntf \
	{ \
	public: \
		ClsName ## Module(); \
		static EsString getBaseName() { return wxT( Name ); } \
		ES_NON_RC_INTF_MAP_BEGIN(ClsName ## Module) \
			ES_INTF_SUPPORTS( ClsName ## Module, PluginModuleIntf ) \
		ES_NON_RC_INTF_MAP_END \
		{ /* destroy does nothing */ } \
		ES_DECL_INTF_METHOD(EsString, getTypeName)() const { \
			return getClassName(); } \
		ES_DECL_INTF_METHOD(EsString, getName)() const { return wxT( Name ) wxT( ".plug" ); } \
		ES_DECL_INTF_METHOD(void, attach)(EsAppIntf* app); \
		ES_DECL_INTF_METHOD(EsAppIntf*, getApp)() const { return m_app; } \
		ES_DECL_INTF_METHOD(bool, isAttached)() const { return m_app != 0; } \
		ES_DECL_INTF_METHOD(int, getPluginCount)() const { return static_cast<int>(m_plugins.size()); } \
		ES_DECL_INTF_METHOD(PluginIntf*, getPlugin)(int idx) \
		{ \
			wxASSERT(idx < getPluginCount()); \
			if( m_plugins[idx]->getMinAsl() <= m_app->getAsl() ) \
				return m_plugins[idx]; \
			else \
				return 0; \
		} \
		ES_DECL_INTF_METHOD(EsString, getFileName)() const { \
			wxFileName result( m_app->getPluginsRoot().c_str(), getName().c_str(), wxT("dll") ); \
			result.Normalize(); \
			return result.GetFullPath(); \
		} \
		static PluginModuleIntf* getInstance() { return &s_module; } \
	protected: \
		EsAppIntf* m_app; \
		typedef std::vector<PluginIntf*> PluginList; \
		PluginList m_plugins; \
		static ClsName ## Module s_module; \
	}

#define IMPL_PLUGIN_MODULE_BEGIN(ClsName) \
ClsName ## Module ClsName ## Module::s_module; \
ClsName ## Module::ClsName ## Module() : \
	m_app(0) { } \
ClsName ## Module::~ClsName ## Module() \
{ \
	for( PluginList::iterator it = m_plugins.begin(); it != m_plugins.end(); ++it ) \
	{ (*it)->destroy(); } \
	m_plugins.clear(); \
} \
ES_IMPL_INTF_METHOD(void, ClsName ## Module::attach)(EsAppIntf* app) \
{ \
	wxASSERT(app); m_app = app; \
	m_app->getLocale().AddCatalog( ClsName ## Module::getBaseName().c_str() );

#define ATTACH_PLUGIN( ClsName ) \
	m_plugins.push_back( new ClsName( this ) ); 

#define ATTACH_STD_PLUGIN( ClsName ) ATTACH_PLUGIN( ClsName ## Plugin )

#define IMPL_PLUGIN_MODULE_END(ClsName) \
} \
extern "C" { \
__declspec(dllexport) PluginModuleIntf* getPluginModuleIntf() \
{ return ClsName ## Module::getInstance(); } }

// standard plugin class declaration helper
#define DECL_STD_PLUGIN( ClsName, MinAsl, Id, Category, ShortDesc, LongDesc ) \
class ClsName ## Frm; \
class ClsName ## Plugin : public PluginIntf { \
public: \
	ClsName ## Plugin(PluginModuleIntf* module); \
	ES_NON_RC_INTF_MAP_BEGIN(ClsName ## Plugin) \
		ES_INTF_SUPPORTS( ClsName ## Plugin, PluginIntf ) \
	ES_NON_RC_INTF_MAP_END \
	{ deActivate(true); delete this; } \
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const { \
		return getClassName(); } \
	ES_DECL_INTF_METHOD(AppSecurityLevel, getMinAsl)() const { return MinAsl; } \
	ES_DECL_INTF_METHOD(EsString, getId)() const { return wxT( Id ); } \
	ES_DECL_INTF_METHOD(EsString, getCategory)() const { return Category; } \
	ES_DECL_INTF_METHOD(EsString, getShortDescription)() const { return ShortDesc; } \
	ES_DECL_INTF_METHOD(EsString, getLongDescription)() const { return LongDesc; } \
	ES_DECL_INTF_METHOD(wxIcon, getIcon)() const { return m_icon; } \
	ES_DECL_INTF_METHOD(PluginModuleIntf*, getModule)() const { return m_module; } \
	ES_DECL_INTF_METHOD(EsString, getSettingsFileName)() const \
	{ \
		wxFileName file(wxStandardPaths::Get().GetUserDataDir(), \
			m_module->getName(), wxT("ini") ); \
		wxASSERT( wxFileName::DirExists( file.GetPath() ) ); \
		return file.GetFullPath(); \
	} \
	ES_DECL_INTF_METHOD(void, activate)(); \
	ES_DECL_INTF_METHOD(bool, deActivate)(bool force); \
	ES_DECL_INTF_METHOD(bool, isActive)() const { return m_active; } \
	ES_DECL_INTF_METHOD(wxWindow*, getWindow)(); \
protected: \
	PluginModuleIntf* m_module; \
	wxIcon m_icon; \
	bool m_active; \
	ClsName ## Frm* m_frm; \
} 

// plugin form base class
//
class EKOSF_CORE_CLASS PluginForm : public wxPanel
{
public:
	PluginForm(PluginModuleIntf* module);
	
	// public services:
	virtual void activate();
	virtual void deactivate();
	virtual bool queryClose() const { return true; }
	MainWindowIntf* getMainWnd() { return m_mainWnd; }
	virtual const PluginIntf* getPlugin() const = 0;
	
protected:
	// internal services
	virtual void connectEvents() = 0;
	virtual void disconnectEvents() = 0;	
	virtual void installMenus();
	virtual void uninstallMenus();
	virtual void loadSettings() = 0;
	virtual void saveSettings() const = 0;
	// menu items addition facility
	virtual void addMenuItem(wxMenu* menu, wxMenuItem* item, size_t& pos);	
	void addMenuSeparator(wxMenu* menu, size_t& pos);
	// toolbar addition facility
	wxAuiToolBar* beginToolbar(wxWindowID toolbarId, bool shown = true);
	void endToolbar(wxAuiToolBar* tb);
	wxAuiToolBar* getToolbar(wxWindowID toolbarId) const;
	void showToolbar(wxWindowID toolbarId, bool show);
	bool toolbarIsShown(wxWindowID toolbarId) const;	
	// generic plugin help menu handler
	void onHelp(wxCommandEvent& evt);
	
protected:
	// data members
	MainWindowIntf* m_mainWnd;
	// logger interface
	EsLogIntf* m_logger;
	// menu items
	MenuItemsT m_items;
	// managed toolbar
	ToolBarsT m_toolBars;
	// optional help file associated with plugin form
	EsString m_helpFile;
};

#define IMPL_STD_PLUGIN(ClsName) \
	ClsName ## Plugin:: ## ClsName ## Plugin(PluginModuleIntf* module) : \
	m_module(module), m_active(false), m_frm(0), m_icon( ClsName ## Plugin_xpm ) {	wxASSERT(m_module); } \
	ES_IMPL_INTF_METHOD(void, ClsName ## Plugin::activate)() { \
	if( !m_active ) { \
		EsAppIntf* app = m_module->getApp(); \
		wxASSERT(app); \
		MainWindowIntf* wnd = app->getMainWindow(); \
		wxASSERT(wnd); \
		wxASSERT(!m_frm); \
		try{ \
			m_frm = new ClsName ## Frm(*this); \
			wxASSERT(m_frm); \
			m_frm->activate(); \
			m_active = true; \
			wnd->getLogger()->append(EsLogIntf::LevelInfo, \
				EsString::format(_("Plugin '%s' activated"), getShortDescription().c_str()) ); \
		} catch (const EsException& ex) { \
			wxDELETE( m_frm ); \
			wnd->getLogger()->append(EsLogIntf::LevelError, \
				EsString::format(_("Plugin '%s' activation error: '%s'"), getShortDescription().c_str(), ex.getMsg().c_str() ) ); \
			throw; \
		}	} } \
	ES_IMPL_INTF_METHOD(bool, ClsName ## Plugin::deActivate)(bool force) { \
	bool result = true; \
	if( m_active ) { \
		EsAppIntf* app = m_module->getApp(); \
		wxASSERT(app); \
		MainWindowIntf* wnd = app->getMainWindow(); \
		wxASSERT(wnd); \
		wxASSERT(m_frm); \
		if( force || m_frm->queryClose() ) { \
			m_frm->deactivate(); \
			m_frm = 0; \
			m_active = false; \
			wnd->getLogger()->append(EsLogIntf::LevelInfo, \
			EsString::format(_("Plugin '%s' de-activated"), getShortDescription().c_str()) ); } \
		else result = false; } \
	return result; } \
	ClsName ## Plugin::~ClsName ## Plugin() { wxASSERT( !isActive() ); } \
	ES_IMPL_INTF_METHOD(wxWindow*, ClsName ## Plugin::getWindow)() \
	{ return dynamic_cast<wxWindow*>(m_frm); }

// plugin form declaration helpers. no need to use them directly in client code
//
#define DECL_PLUGIN_FORM_BEGIN(PluginName) \
class PluginName ##Frm : public PluginForm \
{ \
public: \
	PluginName ##Frm( PluginName ##Plugin& plug ); \
	virtual const PluginIntf* getPlugin() const { return &m_plug; } \
protected: \
	PluginName ##Plugin& m_plug;
//
// plugin form declaration helpers end

#define DECL_PLUGIN_FORM_END };

// standard plugin form constructor implementation
#define IMPL_PLUGIN_FORM_CTOR( PluginName ) \
	PluginName ##Frm:: PluginName ##Frm( PluginName ##Plugin& plug ) : \
	PluginForm(plug.getModule()), \
	m_plug(plug)
	
// wrapper around wxFileConfig, to allow automatic opening the proper configuration file and path set-up
class EKOSF_CORE_CLASS PluginConfig : public wxFileConfig
{
public:
	PluginConfig( const PluginIntf* plugin ) :
		wxFileConfig(	wxEmptyString, wxEmptyString, wxFileName(plugin->getSettingsFileName().c_str()).GetFullPath(), 
			wxEmptyString, wxCONFIG_USE_LOCAL_FILE),
		m_plugin(plugin)
	{	
		SetPath( getRoot() ); 
	}
	
	EsString getRoot() const 
	{ 
		wxASSERT(m_plugin); 
		return wxT("/") + m_plugin->getId(); 
	}

protected:
	const PluginIntf* m_plugin;
};

enum {
	escoreIDM_HELP_PLUGIN = wxID_HIGHEST+1,
	
	// special identifier. must go last
	escoreID_HIGHEST
};

#endif // _plugin_intf_h_
