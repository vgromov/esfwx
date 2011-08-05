#ifndef _progress_monitor_intf_h_
#define _progress_monitor_intf_h_

// progress monitor interface
//
ES_DECL_INTF_BEGIN2( 99860C17, 23CE478a, BC91CCE4, B706483D, ProgressMonitorIntf, EsBaseIntf )
	ES_DECL_INTF_METHOD(void, startAction)(const EsString& actionName, int range) = 0;
	ES_DECL_INTF_METHOD(void, endAction)(int val, const EsString& txt) = 0;
	ES_DECL_INTF_METHOD(EsString, getName)() const = 0;
	ES_DECL_INTF_METHOD(int, getRange)() const = 0;
	ES_DECL_INTF_METHOD(void, setPos)(int pos) = 0;
	ES_DECL_INTF_METHOD(int, getPos)() const = 0;
	ES_DECL_INTF_METHOD(void, pulse)() = 0;
ES_DECL_INTF_END

// accompaining event types to provide thread-safe notifications for GUI
//
// start action notification event. command event string carries action name, int - action range
wxDECLARE_EXPORTED_EVENT( EKOSF_CORE_CLASS, evtPROGRESS_ACTION_START, EsEvent );
// set progress position for current action. position is in command event int member
wxDECLARE_EXPORTED_EVENT( EKOSF_CORE_CLASS, evtPROGRESS_SET_POS, EsEvent );
// pulse progress for current action.
wxDECLARE_EXPORTED_EVENT( EKOSF_CORE_CLASS, evtPROGRESS_PULSE, EsEvent );
// action end notification
wxDECLARE_EXPORTED_EVENT( EKOSF_CORE_CLASS, evtPROGRESS_ACTION_END, EsEvent );

// progress monitor notification event senders
//
namespace EsProgressMonitor 
{
	EKOSF_CORE_FUNC void notifyActionStart(wxEvtHandler* sink, const EsString& name, long range, int winId = wxID_ANY);
	EKOSF_CORE_FUNC void notifySetPos(wxEvtHandler* sink, long pos, const EsString& txt = EsString::s_null, int winId = wxID_ANY);
	EKOSF_CORE_FUNC void notifyPulse(wxEvtHandler* sink, long val = 0, const EsString& txt = EsString::s_null, int winId = wxID_ANY);
	EKOSF_CORE_FUNC void notifyActionEnd(wxEvtHandler* sink, long val = 0, const EsString& txt = EsString::s_null, int winId = wxID_ANY);
}

#endif 