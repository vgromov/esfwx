#include "stdafx.h"
#pragma hdrstop

#include "PluginIntf.h"
#include "res/help-plugin.xpm"

const wxSize c_pluginFrmMinSize(640, 480);

PluginForm::PluginForm(PluginModuleIntf* module) :
wxPanel(&module->getApp()->getMainWindow()->getViewContainer()),
m_mainWnd(module->getApp()->getMainWindow()),
m_logger(m_mainWnd->getLogger())
{
	m_items.reserve(32);
	Hide();
	SetSizeHints(c_pluginFrmMinSize);
}

void PluginForm::activate()
{
	try
	{
		installMenus();
		loadSettings();
		
		connectEvents();

		Fit();
		wxSize frameSize = GetSize();
		frameSize.x = wxMax(frameSize.x, c_pluginFrmMinSize.x);
		frameSize.y = wxMax(frameSize.y, c_pluginFrmMinSize.y);
		SetMinSize(frameSize);
				
		wxScrolledWindow& container = m_mainWnd->getViewContainer();
		container.SetScrollbars(1, 1, frameSize.x, frameSize.y);
		
		Show();
	}
	catch(...)
	{
		// cleanup stuff
		disconnectEvents();
		uninstallMenus();
		m_mainWnd->getViewContainer().SetScrollbars(0,0,0,0);
		
		// rethrow
		throw;
	}
}

void PluginForm::deactivate()
{
	m_mainWnd->getNotifier()->clear();
	disconnectEvents();
	
	saveSettings();
	uninstallMenus();
	
	Hide();
	m_mainWnd->getViewContainer().SetScrollbars(0,0,0,0);
	Destroy();	
}

void PluginForm::addMenuItem(wxMenu* menu, wxMenuItem* item, size_t& pos)
{
	wxASSERT(menu);
	wxASSERT(item);
	menu->Insert( pos++, item );
	m_items.push_back( item );
}

void PluginForm::addMenuSeparator(wxMenu* menu, size_t& pos)
{
	wxMenuItem* item = menu->InsertSeparator( pos++ );
	m_items.push_back(item);
}

void PluginForm::installMenus()
{
	// install plugin help, if any
	if( !m_helpFile.empty() )
	{
		wxMenu* hostMenu = &m_mainWnd->getHelpMenu();
		size_t pos = 0;	
		addMenuItem(hostMenu,
			EsUtilities::createNormalMenuItem(hostMenu, escoreIDM_HELP_PLUGIN, 
				EsString::format(_("Help for plugin '%s'"), getPlugin()->getShortDescription().c_str()), 
				wxEmptyString, wxBitmap(help_plugin_xpm, wxBITMAP_TYPE_XPM)),
			pos );
		addMenuSeparator(hostMenu, pos);
		
		Bind(wxEVT_COMMAND_MENU_SELECTED, &PluginForm::onHelp, this, escoreIDM_HELP_PLUGIN);
	}
}

void PluginForm::uninstallMenus()
{
	// disconnect help handler
	if( !m_helpFile.empty() )
		Unbind(wxEVT_COMMAND_MENU_SELECTED, &PluginForm::onHelp, this, escoreIDM_HELP_PLUGIN);
		
	// destroy menu items
	for( MenuItemsT::iterator it = m_items.begin(); it != m_items.end(); ++it )
	{
		wxMenuItem* item = *it;
		item->GetMenu()->Destroy( item );
	}
	m_items.clear();
	
	// destroy toolbars
	for( ToolBarsT::iterator it = m_toolBars.begin(); it != m_toolBars.end(); ++it )
	{
		wxAuiToolBar* tb = (*it).second;
		m_mainWnd->deleteToolbar( tb );
	}
	m_toolBars.clear();
}

// toolbar addition facility
wxAuiToolBar* PluginForm::beginToolbar(wxWindowID toolbarId, bool shown /*= true*/)
{
	// find if toolbar with such ID exists
	ToolBarsT::const_iterator cit = m_toolBars.find(toolbarId);
	if( cit != m_toolBars.end() )
		EsException::Throw(0, EsException::Generic, _("Toolbar with id: '%d' already exists"), toolbarId);
	
	wxAuiToolBar* tb = m_mainWnd->createToolbar(toolbarId, shown);
	wxASSERT(tb);
	m_toolBars[toolbarId] = tb;
	
	return tb;
}

void PluginForm::endToolbar(wxAuiToolBar* tb)
{
	wxASSERT(tb);
	m_mainWnd->finalizeToolbar(tb);
}

wxAuiToolBar* PluginForm::getToolbar(wxWindowID toolbarId) const
{
	ToolBarsT::const_iterator cit = m_toolBars.find(toolbarId);
	if(cit != m_toolBars.end())
		return (*cit).second;
	
	return 0;
}

void PluginForm::showToolbar(wxWindowID toolbarId, bool show)
{
	wxAuiToolBar* tb = getToolbar(toolbarId);
	if( tb )
		m_mainWnd->showToolbar(tb, show);
}

bool PluginForm::toolbarIsShown(wxWindowID toolbarId) const
{
	wxAuiToolBar* tb = getToolbar(toolbarId);
	if( tb )
		return m_mainWnd->toolbarIsShown(tb);
	
	return false;
}

// generic plugin help menu handler
void PluginForm::onHelp(wxCommandEvent& WXUNUSED(evt))
{
	wxASSERT(!m_helpFile.empty());
	wxHelpControllerBase& hc = 
		getPlugin()->getModule()->getApp()->getHelpController();
	if( hc.LoadFile(m_helpFile) )
		hc.DisplayContents();
}
