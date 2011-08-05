#ifndef _ekonnect_io_channel_config_pane_h_
#define _ekonnect_io_channel_config_pane_h_

class EkonnectIoChannelConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct EkonnectIoChannelConfigPane :	public EsBaseIntf,
																				public EsIoChannelConfigPaneIntf,
																				public EsReflectedClassIntf
	{
		EkonnectIoChannelConfigPane( EkonnectIoChannelConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE( EkonnectIoChannelConfigPane )
		
		ES_INTF_MAP_BEGIN( EkonnectIoChannelConfigPane )
			ES_INTF_SUPPORTS( EkonnectIoChannelConfigPane, EsIoChannelConfigPaneIntf )
			ES_INTF_SUPPORTS( EkonnectIoChannelConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
		
		// reflected ctor
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			EkonnectIoChannelConfigPaneWnd* pane = new EkonnectIoChannelConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}

		// EsIoChannelConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getChannelName)() const { return EkonnectIoChannel::getClassInfoStatic().getName(); }
		ES_DECL_INTF_METHOD(EsString, getChannelConfigString)() const;
		ES_DECL_INTF_METHOD(void, setChannelConfigString)(const EsString& str);
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		
		// data members
		EkonnectIoChannelConfigPaneWnd& m_this;
	};
	friend struct EkonnectIoChannelConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	EkonnectIoChannelConfigPaneWnd(wxWindow* parent);

protected:
	// selectors population helpers
	void populateDevices();
	void populateBauds();
	void populateBits();
	void populateParity();
	void populateStopBits();

	// update dialog controls in accordance with m_cfg
	void updateControls( const EsDictionary& cfg );
	// update config from controls
	void updateConfig( EsDictionary& cfg ) const;
	
	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);
	// rescan button handler
	void onRescan(wxCommandEvent& evt);	

protected:
	wxButton* m_btnResetToDefaults;
	wxNotebook* m_settings;
	wxPanel* m_pnlStd;
	wxStaticText* m_lblDevice;
	wxComboBox* m_edDeviceName;
	wxBitmapButton* m_btnRescan;
	wxStaticText* m_lblBaud;
	wxComboBox* m_edBaud;
	wxStaticText* m_lblByteSize;
	wxComboBox* m_edByteSize;
	wxStaticText* m_lblParity;
	wxComboBox* m_edParity;
	wxStaticText* m_lblStopBits;
	wxComboBox* m_edStopBits;
	wxPanel* m_pnlAdvanced;
	wxStaticText* m_lblRxTmo;
	wxSpinCtrl* m_edRxTmo;
	wxStaticText* m_lblTxTmo;
	wxSpinCtrl* m_edTxTmo;
	wxStaticText* m_lblRxBuffer;
	wxSpinCtrl* m_edRxBuff;
	wxStaticText* m_lblTxBuffer;
	wxSpinCtrl* m_edTxBuff;
	wxStaticText* m_lblRebootDelay;
	wxSpinCtrl* m_edRebootDelay;
	wxCheckBox* m_chkResetOnRxTmo;
	wxCheckBox* m_chkUseStandardSocket;
	wxCheckBox* m_chkSupplyAuxPower;
	wxCheckBox* m_chkProgrammingMode;
		
	// interface implementor instance
	EkonnectIoChannelConfigPane m_intf;
};

#endif // _ekonnect_io_channel_config_pane_h_
