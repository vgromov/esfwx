#ifndef _frm_log_h_
#define _frm_log_h_

#include <wx/file.h> 
#include <wx/wfstream.h>
#include <wx/txtstrm.h> 
#include <wx/listctrl.h>
#include <deque>

// virtual list control class
class EKOSF_SHELL_BASE_CLASS LogView : public wxListCtrl
{
protected:
	struct EKOSF_SHELL_BASE_CLASS LogIntfImpl : public EsLogIntf
	{
		LogIntfImpl(LogView& This) : m_this(This) {}
	
		// interface map
		ES_NON_RC_INTF_MAP_BEGIN( LogIntfImpl )
			ES_INTF_SUPPORTS( LogIntfImpl, EsLogIntf )
		ES_NON_RC_INTF_MAP_END
		{ /* destroy does nothing */ }
		
		// EsBaseIntf override
		//
		ES_DECL_INTF_METHOD(EsString, getTypeName)() const 
		{ 
			return getClassName(); 
		}
		// logger interface
		ES_DECL_INTF_METHOD(void, clear)();
		ES_DECL_INTF_METHOD(void, append)(EsLogIntf::Level lvl, const EsString& msg);
		ES_DECL_INTF_METHOD(EsString, getAll)() const { return m_this.getAll(); }
		ES_DECL_INTF_METHOD(bool, isEmpty)() const { return m_this.isEmpty(); }
		ES_DECL_INTF_METHOD(wxEvtHandler*, getAsEventSink)() const;	
		ES_DECL_INTF_METHOD(bool, isFileLoggingEnabled)() const;
		ES_DECL_INTF_METHOD(void, enableFileLogging)(bool enable);
				
		LogView& m_this;
	};
	friend struct LogIntfImpl;
	
public:
	enum { 
		MAX_LOG_DEPTH = 32000, 
		MIN_LOG_DEPTH = 100,
		COL_TS = 0,
		COL_LVL = 1,
		COL_DATA = 2,
	};

public:
	LogView( wxWindow* parent, int logDepth = MAX_LOG_DEPTH );
	virtual ~LogView();
	// interface implementation access
	EsLogIntf* getIntf() const { return &const_cast<LogIntfImpl&>(m_intf); }

	// virtual list interface
	virtual wxString OnGetItemText(long item, long column) const;
	virtual wxListItemAttr* OnGetItemAttr(long item) const;
	
	// disable|enable file logging
	bool fileLoggingEnabled() const { return m_doFileLogging; }
	void enableFileLogging(bool enable);
	
protected:
	// event handlers
	void onRefresh(wxTimerEvent& evt);
	void onClear(EsEvent& evt);
	void onAppend(EsEvent& evt);
	// helper services
	EsString getAll() const;
	bool isEmpty() const;
	
protected:
	class EKOSF_SHELL_BASE_CLASS LogData
	{
	public:
		LogData() : m_lvl(EsLogIntf::LevelInfo) {}
		LogData(EsLogIntf::Level lvl, const EsString& msg);
		LogData(EsLogIntf::Level lvl, const EsString& msg, const EsBinBuffer& data);
		
		wxDateTime getTs() const { return m_timestamp; }
		EsLogIntf::Level getLvl() const { return m_lvl; }
		EsString getMsg() const;
		const EsBinBuffer& getData() const { return m_data; }
		EsString getAsString() const;
		static EsString lvlString(EsLogIntf::Level lvl);
	
	protected:
		wxDateTime m_timestamp;
		EsLogIntf::Level m_lvl;
		EsString m_msg;
#pragma warning(push)
#pragma warning(disable: 4251)		
		EsBinBuffer m_data;
#pragma warning(pop)
	};

protected:
	EsString constructNewLogFileName() const;

protected:
	// mt guard
	mutable wxCriticalSection m_cs;
	// local typedef
	typedef std::deque<LogData> LogItems;
#pragma warning(push)
#pragma warning(disable: 4251)
	LogItems m_items;
	LogIntfImpl m_intf;
#pragma warning(pop)
	size_t m_logDepth;
	static wxListItemAttr s_attrErr;
	static wxListItemAttr s_attrWarn;
	static wxListItemAttr s_attrTX;
	static wxListItemAttr s_attrRX;
	// refresh timer
	wxTimer m_refresh;
	bool m_needRefresh;
	// file logging
	bool m_doFileLogging;
	wxFile m_flog;
	wxFileOutputStream m_fout;
	wxTextOutputStream m_out;
};

// comunications log frame
class EKOSF_SHELL_BASE_CLASS FrmLog : public wxFrame
{
public:
	FrmLog(wxWindow* parent, int logDepth);
	virtual ~FrmLog();

public:
	EsLogIntf* getLogIntf() const { wxASSERT(m_log); return m_log->getIntf(); }

	// settings persistence
	void loadSettings();
	void saveSettings();
	
	// events
	void onClose(wxCloseEvent& evt);
	void onIoChannelEvent(IoChannelEvent& evt);
	void onClear(wxCommandEvent& evt);
	void onLogToFile(wxCommandEvent& evt);
	void onUpdateUi(wxUpdateUIEvent& evt);

protected:
	LogView* m_log;
	wxToolBar* m_tb;
	bool m_debugData;
};

#endif // _frm_log_h_
