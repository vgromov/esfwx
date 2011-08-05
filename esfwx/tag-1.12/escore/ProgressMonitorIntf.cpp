#include "stdafx.h"
#pragma hdrstop

#include "ProgressMonitorIntf.h"

// accompaining event types to provide thread-safe notifications for GUI
//
// start action notification event. command event string carries action name, int - action range
wxDEFINE_EVENT( evtPROGRESS_ACTION_START, EsEvent );
// set progress position for current action. position is in command event int member
wxDEFINE_EVENT( evtPROGRESS_SET_POS, EsEvent );
// pulse progress for current action.
wxDEFINE_EVENT( evtPROGRESS_PULSE, EsEvent );
// action ended event notification
wxDEFINE_EVENT( evtPROGRESS_ACTION_END, EsEvent );

// progress monitor notification event senders
//
void EsProgressMonitor::notifyActionStart(wxEvtHandler* sink, const EsString& name, long range, int winId)
{
	wxQueueEvent(sink, EsEvent(evtPROGRESS_ACTION_START, winId, range, name).Clone());
}

void EsProgressMonitor::notifySetPos(wxEvtHandler* sink, long pos, const EsString& txt /*= EsString::s_null*/, int winId)
{
	wxQueueEvent(sink, EsEvent(evtPROGRESS_SET_POS, winId, pos, txt).Clone());
}

void EsProgressMonitor::notifyPulse(wxEvtHandler* sink, long val /*= 0*/, const EsString& txt /*= EsString::s_null*/, int winId)
{
	wxQueueEvent(sink, EsEvent(evtPROGRESS_PULSE, winId, val, txt).Clone());
}

void EsProgressMonitor::notifyActionEnd(wxEvtHandler* sink, long val /*= 0*/, const EsString& txt /*= EsString::s_null*/, int winId)
{
	wxQueueEvent(sink, EsEvent(evtPROGRESS_ACTION_END, winId, val, txt).Clone());	
}
