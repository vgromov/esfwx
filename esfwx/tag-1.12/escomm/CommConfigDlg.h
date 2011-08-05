#ifndef _comm_config_dlg_h_
#define _comm_config_dlg_h_
 
class EKOSF_COMM_CLASS CommConfigDlg :	public wxDialog
{
public:
	CommConfigDlg(wxWindow* parent, bool showProtocolPane = false,
		wxConfigBase* cfg = 0, const EsString& path = EsString::s_null);
	
	// transparent delegation from CommConfigPane
	//
	// access to the currently active config pane interface
	inline EsIoChannelConfigPaneIntf::Ptr getConfigPane() const { return m_cfgPane->getConfigPane(); }
	// try to select channel by name.
	inline void selectChannel(const EsString& channelName) { m_cfgPane->selectChannel(channelName); }
	
	// protocol config access
	//
	EsProtocolConfigPane* getProtocolConfigPane() { return m_protoCfgPane; }
	const EsProtocolConfigPane* getProtocolConfigPane() const { return m_protoCfgPane; }
	
protected:
	wxButton* m_btnOK;
	wxButton* m_btnCancel;
	CommConfigPane* m_cfgPane;
	EsProtocolConfigPane* m_protoCfgPane;
	bool m_showProtocolPane;
};

#endif // _comm_config_dlg_h_
