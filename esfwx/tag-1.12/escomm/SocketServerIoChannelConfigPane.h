#ifndef _Socket_Server_Io_Channel_Config_Pane_h_
#define _Socket_Server_Io_Channel_Config_Pane_h_

#include <wx/spinctrl.h>

class EKOSF_COMM_CLASS SocketServerIoChannelConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct SocketServerIoChannelConfigPane :	public EsBaseIntf,
																						public EsIoChannelConfigPaneIntf,
																						public EsReflectedClassIntf
	{
		SocketServerIoChannelConfigPane( SocketServerIoChannelConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE( SocketServerIoChannelConfigPane )
		
		ES_INTF_MAP_BEGIN( SocketServerIoChannelConfigPane )
			ES_INTF_SUPPORTS( SocketServerIoChannelConfigPane, EsIoChannelConfigPaneIntf )
			ES_INTF_SUPPORTS( SocketServerIoChannelConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();
			m_this.Destroy();
		}
		
		// reflected constructor
				// class services
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			SocketServerIoChannelConfigPaneWnd* pane = new SocketServerIoChannelConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}

		// EsIoChannelConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getChannelName)() const { return SocketServerIoChannel::getClassInfoStatic().getName(); }
		ES_DECL_INTF_METHOD(EsString, getChannelConfigString)() const;
		ES_DECL_INTF_METHOD(void, setChannelConfigString)(const EsString& cfg);
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		
		// data members
		SocketServerIoChannelConfigPaneWnd& m_this;
	};
	friend struct SocketServerIoChannelConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	SocketServerIoChannelConfigPaneWnd(wxWindow* parent);

protected:
	virtual ~SocketServerIoChannelConfigPaneWnd();

	// update dialog controls in accordance with m_cfg
	void updateControls( const EsDictionary& cfg );
	// update config from controls
	void updateConfig( EsDictionary& cfg ) const;
	// set default values
	void setDefaults();
	// reset button click handler
	void onResetToDefaults(wxCommandEvent& evt);

protected:
	// interface implementor instance
	SocketServerIoChannelConfigPane m_intf;
	// controls
	wxButton* m_reset;
	wxSpinCtrl* m_port;
	wxSpinCtrl* m_tmo;	
};

#endif // _Socket_Server_Io_Channel_Config_Pane_h_
