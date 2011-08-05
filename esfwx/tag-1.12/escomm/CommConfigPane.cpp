#include "stdafx.h"
#pragma hdrstop

#include "CommConfigPane.h"

CommConfigPane::CommConfigPane(wxWindow* parent, 
	wxConfigBase* cfg/* = 0*/, const EsString& path /*= EsString::s_null*/) :
wxPanel(parent, wxID_ANY),
m_cfg(cfg),
m_path(path),
m_contents(new wxBoxSizer(wxVERTICAL)),
m_channel(0)
{
	wxASSERT(m_contents);
	wxBoxSizer* channel = new wxBoxSizer( wxHORIZONTAL );
	wxASSERT(channel);
	wxStaticText* lblChannel = new wxStaticText( this, wxID_ANY, _("Select channel:") );
	wxASSERT(lblChannel);
	channel->Add( lblChannel, 0, wxALL|wxEXPAND, 5 );
	
	m_channel = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	wxASSERT(m_channel);
	channel->Add( m_channel, 1, wxALL|wxEXPAND, 5 );
	m_contents->Add( channel, 0, wxEXPAND|wxALL, 5 );
	
	SetSizer( m_contents );
	Layout();
	
	// connect Events
	m_channel->Bind( wxEVT_COMMAND_COMBOBOX_SELECTED, &CommConfigPane::onChannelSelected, this );
	
	// populate channel selector
	EsString::Array channels = IoChannelFactory::getChannelNames();
	wxArrayString chLabels;
	chLabels.SetCount(channels.size());
	for(size_t idx = 0; idx < channels.size(); ++idx )
		chLabels[idx] = IoChannelFactory::getChannelLabel(channels[idx]).c_str();
	m_channel->Append(chLabels);

	if( m_channel->GetCount() )
		selectChannelByLabel(m_channel->GetString(0));
}

CommConfigPane::~CommConfigPane()
{
	// disconnect Events
	m_channel->Unbind( wxEVT_COMMAND_COMBOBOX_SELECTED, &CommConfigPane::onChannelSelected, this );
}

// try to select channel by name.
void CommConfigPane::selectChannel(const EsString& channelName)
{
	wxWindow* wnd = 0;
	if( m_pane && 0 != channelName.compare(m_pane->getChannelName()) )
	{
		// destroy active pane
		wnd = m_pane->getWindow();
		if(wnd)
			m_contents->Detach(wnd);
		m_pane.reset();
	}
	
	// create new pane
	if( !m_pane )
	{
		m_pane = IoChannelFactory::createChannelConfigPane(channelName, this);
		wxASSERT(m_pane);
		wnd = m_pane->getWindow();
		if( wnd )
		{
			m_contents->Add(wnd, 1, wxALL|wxEXPAND);
			EsUtilities::applyCtlsFont(wnd, GetFont());
			Layout();
		}
		
		// optionally update channel settings from persistent storage
		if( m_cfg && !m_path.empty() )
		{
			// select specified config path
			EsString oldPath = m_cfg->GetPath();
			m_cfg->SetPath(m_path);
			if( m_cfg->HasEntry(channelName) )
			{
				EsString chnlConfig = m_cfg->Read(channelName);
				// apply profile config to currently selected channel
				m_pane->setChannelConfigString(chnlConfig);
			}
			
			// restore config path
			m_cfg->SetPath(oldPath);
		}
		
		// update channel selection
		const ClassInfo* info = ClassInfo::getClassInfo(channelName, true);
		m_channel->SetValue( info->getDescr().c_str() );
		
		// call fit on top level, as well
		wnd = EsUtilities::getTopLevelFor(this);
		if( wnd )
			wnd->Fit();
	}
}

// channel selector helper
void CommConfigPane::selectChannelByLabel(const EsString& label)
{
	EsString channelName = IoChannelFactory::getChannelByLabel( label );
	selectChannel(channelName);
}

void CommConfigPane::onChannelSelected( wxCommandEvent& evt )
{
	// try to find channel name by its label
	EsString label = m_channel->GetValue();
	selectChannelByLabel(label);
}
