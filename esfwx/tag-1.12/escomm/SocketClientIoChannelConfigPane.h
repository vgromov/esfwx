#ifndef _socket_client_io_channel_config_pane_
#define _socket_client_io_channel_config_pane_

#include <wx/spinctrl.h>

class SocketClientIoChannelConfigPaneWnd : public wxPanel
{
public:
	// configuration pane interface implementor
	struct SocketClientIoChannelConfigPane :	public EsBaseIntf,
																						public EsIoChannelConfigPaneIntf,
																						public EsReflectedClassIntf
	{
		SocketClientIoChannelConfigPane( SocketClientIoChannelConfigPaneWnd& This ) : m_this(This) {}
		
		// reflection support
		ES_DECL_REFLECTED_CLASS_BASE(SocketClientIoChannelConfigPane)
		
		ES_INTF_MAP_BEGIN( SocketClientIoChannelConfigPane )
			ES_INTF_SUPPORTS( SocketClientIoChannelConfigPane, EsIoChannelConfigPaneIntf )
			ES_INTF_SUPPORTS( SocketClientIoChannelConfigPane, EsReflectedClassIntf )
		ES_INTF_MAP_END
		{
			// destroy implementation
			m_this.Hide();	
			m_this.Destroy();
		}
		
		// reflected constructor
		ES_DECL_REFLECTED_CLASS_CTOR1(EsBaseIntfPtr, p_wxObject) 
		{ 
			SocketClientIoChannelConfigPaneWnd* pane = new SocketClientIoChannelConfigPaneWnd( dynamic_cast<wxWindow*>(p1) ); 
			wxASSERT(pane); 
			return EsBaseIntf::Ptr(&pane->m_intf); 
		}		
	
		// EsIoChannelConfigPaneIntf implementation
		ES_DECL_INTF_METHOD(EsString, getChannelName)() const { return SocketClientIoChannel::getClassInfoStatic().getName(); }
		ES_DECL_INTF_METHOD(EsString, getChannelConfigString)() const;
		ES_DECL_INTF_METHOD(void, setChannelConfigString)(const EsString& str);
		ES_DECL_INTF_METHOD(wxWindow*, getWindow)() { return &m_this; }
		
		// data members
		SocketClientIoChannelConfigPaneWnd& m_this;
	};
	friend struct SocketClientIoChannelConfigPane;

protected:
	// create pane window via reflected constructor "NEW" only
	SocketClientIoChannelConfigPaneWnd(wxWindow* parent);

protected:
	virtual ~SocketClientIoChannelConfigPaneWnd();

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
	SocketClientIoChannelConfigPane m_intf;
	// controls
	wxButton* m_reset;
	wxTextCtrl* m_addr;
	wxSpinCtrl* m_port;
	wxSpinCtrl* m_tmo;
};

#endif // _socket_client_io_channel_config_pane_
