#ifndef _es_event_h_
#define _es_event_h_

// our own thread-safe event class, may be used as-is or 
// as base class for other event types
class EKOSF_CORE_CLASS EsEvent : public wxEvent
{
public:
	EsEvent(wxEventType type, wxWindowID id = wxID_ANY, int val = 0, const EsString& str = EsString::s_null);
	EsEvent(const EsEvent& src);
	
	// base class overrides
  virtual wxEvent* Clone() const;
  virtual wxEventCategory GetEventCategory() const { return wxEVT_CATEGORY_THREAD; }
  
  // data access
  //
  const EsString& getStr() const { return m_str; }
  void setStr(const EsString& str) { m_str = str; }
  int getVal() const { return m_val; }
  void setVal(int val) { m_val = val; } 
	
protected:
	// data members
	EsString m_str;
	int m_val;
};

#endif // _es_thread_event_h_
