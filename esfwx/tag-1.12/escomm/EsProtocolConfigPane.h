#ifndef _es_protocol_config_pane_h_
#define _es_protocol_config_pane_h_

// protocol parameters configuration GUI
//

class EKOSF_COMM_CLASS EsProtocolConfigPane : public wxPanel
{
public:
	EsProtocolConfigPane(wxWindow& parent);
	// configuration string access	
	EsString getProtocolConfigString() const;	
	void setProtocolConfigString(const EsString& cfg);
	
protected:
	// helper methods
	//
	void updateControls(const EsDictionary& cfg);
	void updateConfig(EsDictionary& cfg) const;
	
	// event handlers
	//
	void onResetToDefaults(wxCommandEvent& evt);
	
protected:
	// data members	
	//
	wxSpinCtrl* m_packetTmo;	
	wxSpinCtrl* m_packetRetries;
	wxSpinCtrl* m_slaveAddr;
};

#endif // _es_protocol_config_pane_h_
