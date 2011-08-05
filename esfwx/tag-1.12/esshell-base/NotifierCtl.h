#ifndef _notifier_ctl_h_
#define _notifier_ctl_h_

class EKOSF_SHELL_BASE_CLASS NotifierCtl :	public wxControl
{
protected:
	struct NotifierIntfImpl :	public EsNotifierIntf
	{
		NotifierIntfImpl(NotifierCtl& This) : m_this(This) {}

		// interface map
		ES_NON_RC_INTF_MAP_BEGIN( NotifierIntfImpl )
			ES_INTF_SUPPORTS( NotifierIntfImpl, EsNotifierIntf )
		ES_NON_RC_INTF_MAP_END
		{ /* destroy does nothing */ }
		
		// EsBaseIntf override
		//
		ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
		{ 
			return getClassName(); 
		}
		// EsNotifierIntf implementation
		//
		ES_DECL_INTF_METHOD(void, clear)();
		ES_DECL_INTF_METHOD(void, notify)(EsNotifierIntf::Level lvl, const EsString& msg, const wxBitmap& icon = wxNullBitmap);
	
		NotifierCtl& m_this; 
	};
	friend struct NotifierIntfImpl;
	
public:
	NotifierCtl();
	NotifierCtl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~NotifierCtl();
	// implemented interface access
	EsNotifierIntf* getIntf() const { return &const_cast<NotifierIntfImpl&>(m_intf); }	
	// paint event handler
	void onPaint(wxPaintEvent& evt);
	// internal idle override
	virtual void OnInternalIdle();

protected:
	EsNotifierIntf::Level m_lvl;
	wxBitmap m_bm;
	EsString m_lbl;
	bool m_needUpdate;
	// multithreading guard
	wxCriticalSection m_cs;
#pragma warning(push)
#pragma warning(disable: 4251)
	// interface implementor
	NotifierIntfImpl m_intf;
#pragma warning(pop)	
};

#endif // _notifier_ctl_h_
