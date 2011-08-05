#ifndef _comm_config_pane_h_
#define _comm_config_pane_h_

// embeddable channel configuration GUI pane
//
class EKOSF_COMM_CLASS CommConfigPane : public wxPanel
{
public:
	// optional parameters - cfg, path, key may point to the config entry,
	// which is used when specific channel is selected, to update 
	// property values from persistent storage profiles saved for specific channel name
	CommConfigPane(wxWindow* parent, 
		wxConfigBase* cfg = 0, const EsString& path = EsString::s_null);
	virtual ~CommConfigPane();

	// access to the currently active config pane interface
	EsIoChannelConfigPaneIntf::Ptr getConfigPane() const { return m_pane; }
	// try to select channel by name.
	void selectChannel(const EsString& channelName);

protected:
	// event handling
	void onChannelSelected( wxCommandEvent& evt );
	// channel selector helper
	void selectChannelByLabel(const EsString& label);

protected:
	// data members
	wxConfigBase* m_cfg;
	EsString m_path;
	wxBoxSizer* m_contents;
	wxComboBox* m_channel;
	EsIoChannelConfigPaneIntf::Ptr m_pane;	
};

#endif // _comm_config_pane_h_
