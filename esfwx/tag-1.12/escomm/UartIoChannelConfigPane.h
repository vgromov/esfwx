#ifndef _uart_io_channel_config_pane_h_
#define _uart_io_channel_config_pane_h_

#include "EsEnumCommPorts.h"

class UartIoChannelConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct UartIoChannelConfigPane :	public EsBaseIntf,
																		public EsIoChannelConfigPaneIntf,
																		public EsReflectedClassIntf																	 
	{
		UartIoChannelConfigPane( UartIoChannelConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE(UartIoChannelConfigPane)
		
		// interface mapping
		ES_INTF_MAP_BEGIN( UartIoChannelConfigPane )
			ES_INTF_SUPPORTS( UartIoChannelConfigPane, EsIoChannelConfigPaneIntf )
			ES_INTF_SUPPORTS( UartIoChannelConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
	
		// reflected class services
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			UartIoChannelConfigPaneWnd* pane = new UartIoChannelConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}
	
		// EsIoChannelConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getChannelName)() const { return UartIoChannel::getClassInfoStatic().getName(); }
		ES_DECL_INTF_METHOD(EsString, getChannelConfigString)() const;
		ES_DECL_INTF_METHOD(void, setChannelConfigString)(const EsString& str);
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		
		// data members
		UartIoChannelConfigPaneWnd& m_this;
	};
	friend struct UartIoChannelConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	UartIoChannelConfigPaneWnd(wxWindow* parent);

protected:
	// selectors population helpers
	void populatePorts();
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
	wxStaticText* m_lblPort;
	wxComboBox* m_edPortName;
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
	wxStaticText* m_lblRxBuffer;
	wxSpinCtrl* m_edRxBuff;
	wxStaticText* m_lblTxBuffer;
	wxSpinCtrl* m_edTxBuff;
	wxCheckBox* m_chkResetOnRxTmo;

	EsCommPortInfos m_ports;
	UartIoChannelConfigPane m_intf;
};

#endif // _uart_io_channel_config_pane_h_