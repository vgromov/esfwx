#include "StdAfx.h"
#pragma hdrstop

#include "FrmLog.h"
#include <wx/tokenzr.h>
#include <commctrl.h>
#include "res/actions-clear-log.xpm"
#include "res/actions-log-to-file.xpm"

// comm event types
wxDEFINE_EVENT(evtLOG_APPEND, EsEvent);
wxDEFINE_EVENT(evtLOG_CLEAR, EsEvent);

wxListItemAttr LogView::s_attrErr(*wxBLACK, wxColour(255,128,128), wxNullFont);
wxListItemAttr LogView::s_attrWarn(*wxBLACK, wxColour(255,200,0), wxNullFont);
wxListItemAttr LogView::s_attrTX(*wxBLACK, wxColour(200,200,255), wxNullFont);
wxListItemAttr LogView::s_attrRX(*wxBLACK, wxColour(200,255,200), wxNullFont);

LogView::LogIntfImpl::~LogIntfImpl()
{
}

// log intf implementation
//
ES_IMPL_INTF_METHOD(void, LogView::LogIntfImpl::clear)()
{
	// send clear event
	//
	wxQueueEvent(&m_this, EsEvent(evtLOG_CLEAR).Clone());
}

ES_IMPL_INTF_METHOD(void, LogView::LogIntfImpl::append)(EsLogIntf::Level lvl, const EsString& msg)
{
	// append item to the input queue
	// all other processing is done in internal idle
	//
	// send append event
	wxQueueEvent(&m_this, EsEvent(evtLOG_APPEND, wxID_ANY, lvl, msg).Clone());	
}

ES_IMPL_INTF_METHOD(wxEvtHandler*, LogView::LogIntfImpl::getAsEventSink)() const
{
	return m_this.GetParent();
}

ES_IMPL_INTF_METHOD(bool, LogView::LogIntfImpl::isFileLoggingEnabled)() const
{
	return m_this.fileLoggingEnabled();
}

ES_IMPL_INTF_METHOD(void, LogView::LogIntfImpl::enableFileLogging)(bool enable)
{
	m_this.enableFileLogging(enable);
}

// --------------------------------------------------------------------------------
LogView::LogView(wxWindow *parent, int logDepth /*= MAX_LOG_DEPTH*/) :
wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_VIRTUAL|wxLC_VRULES),
m_intf(*this),
m_logDepth( wxMax(MIN_LOG_DEPTH, wxMin(MAX_LOG_DEPTH, logDepth)) ),
m_needRefresh(false),
m_doFileLogging(false),
m_fout(m_flog),
m_out(m_fout)
{
	EsUtilities::setAppLogger( &m_intf );

	m_refresh.SetOwner(this);
	InheritAttributes();
	// set-up columns
	long col = InsertColumn(COL_TS, _("Timestamp"));
	SetColumnWidth(col, 100);
	col = InsertColumn(COL_LVL, _("Level"));
	SetColumnWidth(col, 60);
	col = InsertColumn(COL_DATA, _("Data"));
	SetColumnWidth(col, 300);

	SetItemCount(0);
	
	Bind(evtLOG_CLEAR, &LogView::onClear, this);
	Bind(evtLOG_APPEND, &LogView::onAppend, this);
	Bind(wxEVT_TIMER, &LogView::onRefresh, this);	
	m_refresh.Start(1000);
}

LogView::~LogView()
{
	m_refresh.Stop();
	Unbind(evtLOG_CLEAR, &LogView::onClear, this);
	Unbind(evtLOG_APPEND, &LogView::onAppend, this);
	Unbind(wxEVT_TIMER, &LogView::onRefresh, this);	

	EsUtilities::setAppLogger( 0 );
}

void LogView::enableFileLogging(bool enable)
{
	if( enable )
	{
		if( !m_doFileLogging )
			m_flog.Create( constructNewLogFileName(), true );
	}
	else if( m_doFileLogging )
		m_flog.Close();
	
	m_doFileLogging = enable;
}

void LogView::onRefresh(wxTimerEvent& WXUNUSED(evt))
{
	if( m_needRefresh )
	{
		size_t size = m_items.size();
		if( GetItemCount() != size )
		{
			SetItemCount( size );
			if( size )
				EnsureVisible( size-1 );
		}
		
		Refresh();
		m_needRefresh = false;
	}
}

void LogView::onAppend(EsEvent& evt)
{
	const LogData& data = LogData(static_cast<EsLogIntf::Level>(evt.getVal()), evt.getStr().c_str());
	m_items.push_back( data );
	
	if( m_flog.IsOpened() )
		m_out << data.getAsString() << endl;
	
	if( m_items.size() > m_logDepth )
		m_items.pop_front();

	m_needRefresh = true;
}

void LogView::onClear(EsEvent& WXUNUSED(evt))
{
	m_items.clear();
	SetItemCount( 0 );
	m_needRefresh = true;
	m_flog.Close();
	// create new log file
	if( m_doFileLogging )
		m_flog.Create( constructNewLogFileName(), true );
}

EsString LogView::getAll() const
{
	EsString result;
	
	wxCriticalSectionLocker lock(m_cs);
	for( LogItems::const_iterator cit = m_items.begin(); cit != m_items.end(); ++cit )
	{
		if( !result.empty() )
			result += wxT("\n");
		
		result += cit->getAsString();
	}

	return result;
}

bool LogView::isEmpty() const
{
	wxCriticalSectionLocker lock(m_cs);
	return m_items.empty();
}

EsString LogView::constructNewLogFileName() const
{
	wxFileName fn(wxStandardPaths::Get().GetDocumentsDir(), 
								EsString::makeSafeFileName(wxTheApp->GetAppName() + wxDateTime::Now().FormatISOCombined(wxT('-'))),
								wxT("log"));
								
	return fn.GetFullPath();
}

LogView::LogData::LogData(EsLogIntf::Level lvl, const EsString& msg) :
m_timestamp( wxDateTime::UNow() ), 
m_lvl(lvl),
m_msg(msg)
{
}

LogView::LogData::LogData(EsLogIntf::Level lvl, const EsString& msg, const EsBinBuffer& data) :
m_timestamp( wxDateTime::UNow() ),
m_lvl(lvl), 
m_msg(msg),
m_data(data)
{
}

EsString LogView::LogData::getMsg() const
{
	return m_msg;
}

EsString LogView::LogData::lvlString( EsLogIntf::Level lvl )
{
	switch( lvl )
	{
	case EsLogIntf::LevelInfo:
		return wxT("INFO");
	case EsLogIntf::LevelRX:
		return wxT("RX <<");
	case EsLogIntf::LevelTX:
		return wxT("TX >>");
	case EsLogIntf::LevelWarning:
		return wxT("WARN");
	case EsLogIntf::LevelError:
		return wxT("ERR");
	default: // LevelDebug:
		return wxT("DBG");
	}
}

EsString LogView::LogData::getAsString() const
{
	wxString ts = m_timestamp.FormatISOCombined(wxT('-'));
	ts << wxT(".") << m_timestamp.GetMillisecond();
		
	return ts + wxT("|") +
			LogData::lvlString( m_lvl ) + wxT("|") +
			m_msg;
}

// virtual list interface
wxString LogView::OnGetItemText(long item, long column) const
{
	LogData data;
	wxASSERT(item < static_cast<long>(m_items.size()));
	wxASSERT(column <= COL_DATA);
	data = m_items[item];

	if( COL_TS == column )
	{
		wxDateTime dt = data.getTs();
		wxString ts = dt.FormatISOCombined(wxT('-'));
		ts << wxT(".") << dt.GetMillisecond();
		return ts;
	}
	else if( COL_LVL == column )
		return LogData::lvlString( data.getLvl() ).c_str();
	else
		return data.getMsg().c_str();
}

wxListItemAttr* LogView::OnGetItemAttr(long item) const
{
	LogData data;
	wxASSERT(item < static_cast<long>(m_items.size()));
	data = m_items[item];

	switch(data.getLvl())
	{
	case EsLogIntf::LevelError:
		return &s_attrErr;
	case EsLogIntf::LevelWarning:
		return &s_attrWarn;
	case EsLogIntf::LevelTX:
		return &s_attrTX;
	case EsLogIntf::LevelRX:
		return &s_attrRX;
	default:
		return 0;
	}
}

// --------------------------------------------------------------------------------
// minimal & default log frame size
static const wxSize c_minSize(240, 480);

DEF_CFG_SECTION(LOG);
	DEF_CFG_KEY(DEBUG_DATA);
	DEF_CFG_KEY(FILE_LOGGING);
	
// --------------------------------------------------------------------------------
FrmLog::FrmLog(wxWindow* parent, int logDepth) :
wxFrame(parent, wxID_ANY, _("Activities Log")),
m_log(0),
#ifdef DEBUG
m_debugData(true)
#else
m_debugData(false)
#endif
{
	Hide();

	SetSizeHints( c_minSize, wxDefaultSize );
	wxBoxSizer* contents = new wxBoxSizer( wxVERTICAL );
	wxASSERT(contents);
	m_tb = new wxToolBar(this, wxID_ANY);
	wxASSERT(m_tb);
	m_tb->AddTool(FrmMain::IDM_ACTIONS_CLEAR_LOG, _("Clear log"), 
		wxBitmap(actions_clear_log_xpm, wxBITMAP_TYPE_XPM), wxNullBitmap, wxITEM_NORMAL,
		_("Clear activities log") );
	m_tb->AddTool(FrmMain::IDM_ACTIONS_LOG_TO_FILE, _("Log to file"), 
		wxBitmap(actions_log_to_file_xpm, wxBITMAP_TYPE_XPM), wxNullBitmap, wxITEM_CHECK,
		_("Enable logging to file") );
	m_tb->Realize();
	contents->Add(m_tb, wxSizerFlags().Expand());
	
	m_log = new LogView(this, logDepth);
	wxASSERT(m_log);
	contents->Add(m_log, wxSizerFlags(1).Expand());
	SetSizer( contents );
	Layout();
	
	// Connect Events
	Bind(wxEVT_CLOSE_WINDOW, &FrmLog::onClose, this);
	Bind(evtIO_CHANNEL, &FrmLog::onIoChannelEvent, this);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmLog::onClear, this, FrmMain::IDM_ACTIONS_CLEAR_LOG);
	Bind(wxEVT_COMMAND_MENU_SELECTED, &FrmLog::onLogToFile, this, FrmMain::IDM_ACTIONS_LOG_TO_FILE);
	Bind(wxEVT_UPDATE_UI, &FrmLog::onUpdateUi, this, FrmMain::IDM_ACTIONS_CLEAR_LOG, FrmMain::IDM_ACTIONS_LOG_TO_FILE);
	
	loadSettings();
}

FrmLog::~FrmLog()
{
	// Disconnect Events
	Unbind(wxEVT_CLOSE_WINDOW, &FrmLog::onClose, this);
	Unbind(evtIO_CHANNEL, &FrmLog::onIoChannelEvent, this);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmLog::onClear, this, FrmMain::IDM_ACTIONS_CLEAR_LOG);
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &FrmLog::onLogToFile, this, FrmMain::IDM_ACTIONS_LOG_TO_FILE);
	Unbind(wxEVT_UPDATE_UI, &FrmLog::onUpdateUi, this, FrmMain::IDM_ACTIONS_CLEAR_LOG, FrmMain::IDM_ACTIONS_LOG_TO_FILE);
	
	saveSettings();
}

void FrmLog::loadSettings()
{
	wxConfigBase* cfg = wxConfigBase::Get();
	wxASSERT(cfg);
	
	// comm log form
	cfg->SetPath( CFG_LOG_GROUP );
	// form pos & size
	EsUtilities::loadWindowPos(cfg, this, c_minSize);
	
	// read column width values
	for( long idx = 0; idx < m_log->GetColumnCount(); ++idx )
	{
		wxString colKey = wxT("COL_");
		colKey << idx;
		
		m_log->SetColumnWidth(idx, cfg->ReadLong( colKey, m_log->GetColumnWidth(idx) ) );
	}
	
	cfg->Read( CFG_DEBUG_DATA_KEY, &m_debugData, 
#ifdef _DEBUG
	true
#else		
	false 
#endif
	);
	
	m_log->enableFileLogging( cfg->ReadBool(CFG_FILE_LOGGING_KEY, false) );
}

void FrmLog::saveSettings()
{
	wxConfigBase* cfg = wxConfigBase::Get();
	wxASSERT(cfg);
	
	// comm log form
	cfg->SetPath( CFG_LOG_GROUP );	
	EsUtilities::saveWindowPos(cfg, this);
	
	// write column width values	
	for( long idx = 0; idx < m_log->GetColumnCount(); ++idx )
	{
		wxString colKey = wxT("COL_");
		colKey << idx;
		
		cfg->Write( colKey, m_log->GetColumnWidth(idx) );
	}
	
	cfg->Write(CFG_FILE_LOGGING_KEY, m_log->fileLoggingEnabled() );
}

void FrmLog::onClose(wxCloseEvent& evt)
{
	// just hide window
	Hide();
}

// handle IoChannel events
void FrmLog::onIoChannelEvent(IoChannelEvent& evt)
{
	switch( evt.getType() )
	{
	case IoChannelEvent::TypeError:
		m_log->getIntf()->append(EsLogIntf::LevelError, evt.getMsg());
		break;
	case IoChannelEvent::TypeOpen:
	case IoChannelEvent::TypeClose:
	case IoChannelEvent::TypeInfo:
		m_log->getIntf()->append(EsLogIntf::LevelInfo, evt.getMsg());
		break;
	case IoChannelEvent::TypeRX:
		{
			EsString msg;
			if( m_debugData )
				msg = EsString::format(wxT("%s [%s]"), evt.getMsg().c_str(), 
					EsString::binToHex( evt.getData() ).c_str() );
			else
				msg = evt.getMsg();
				
			m_log->getIntf()->append(EsLogIntf::LevelRX, msg);
		}
		break;
	case IoChannelEvent::TypeTX:
		{
			EsString msg;
			if( m_debugData )
				msg = EsString::format(wxT("%s [%s]"), evt.getMsg().c_str(), 
					EsString::binToHex( evt.getData() ).c_str() );
			else
				msg = evt.getMsg();

			m_log->getIntf()->append(EsLogIntf::LevelTX, msg);
		}
		break;
	}

	evt.Skip();
}

void FrmLog::onClear(wxCommandEvent& evt)
{
	m_log->getIntf()->clear();
}

void FrmLog::onLogToFile(wxCommandEvent& WXUNUSED(evt))
{
	m_log->enableFileLogging( !m_log->fileLoggingEnabled() );
}

void FrmLog::onUpdateUi(wxUpdateUIEvent& evt)
{
	switch(evt.GetId())
	{
	case FrmMain::IDM_ACTIONS_CLEAR_LOG:
		evt.Enable( !m_log->getIntf()->isEmpty() );
		break;
	case FrmMain::IDM_ACTIONS_LOG_TO_FILE:
		evt.Check( m_log->fileLoggingEnabled() );
		break;
	}
}
