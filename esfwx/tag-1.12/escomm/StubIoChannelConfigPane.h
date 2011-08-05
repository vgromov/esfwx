#ifndef _stub_io_channel_config_pane_h_
#define _stub_io_channel_config_pane_h_

class StubIoChannelConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct StubIoChannelConfigPane :	public EsBaseIntf,
																		public EsIoChannelConfigPaneIntf,
																		public EsReflectedClassIntf																	 
	{
		StubIoChannelConfigPane( StubIoChannelConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE(StubIoChannelConfigPane)
		
		// interface mapping
		ES_INTF_MAP_BEGIN( StubIoChannelConfigPane )
			ES_INTF_SUPPORTS( StubIoChannelConfigPane, EsIoChannelConfigPaneIntf )
			ES_INTF_SUPPORTS( StubIoChannelConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
	
		// reflected class services
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			StubIoChannelConfigPaneWnd* pane = new StubIoChannelConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}
	
		// EsIoChannelConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getChannelName)() const { return StubIoChannel::getClassInfoStatic().getName(); }
		ES_DECL_INTF_METHOD(EsString, getChannelConfigString)() const;
		ES_DECL_INTF_METHOD(void, setChannelConfigString)(const EsString& str);
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		
		// data members
		StubIoChannelConfigPaneWnd& m_this;
	};
	friend struct StubIoChannelConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	StubIoChannelConfigPaneWnd(wxWindow* parent);

protected:
	// update dialog controls in accordance with m_cfg
	void updateControls( const EsDictionary& cfg );
	// update config from controls
	void updateConfig( EsDictionary& cfg ) const;
	
	// reset button click handler
	//void onResetToDefaults(wxCommandEvent& evt);

protected:
	//wxButton* m_btnResetToDefaults;
	StubIoChannelConfigPane m_intf;
};

#endif // _stub_io_channel_config_pane_h_