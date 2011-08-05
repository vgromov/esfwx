#ifndef _es_spec_data_event_
#define _es_spec_data_event_

// data event
//
class EKOSF_CHART_CLASS EsEvtSpecData : public EsEvent
{
public:
	EsEvtSpecData(wxEventType eventType, EsBinBuffer::const_pointer data, size_t len, int id = wxID_ANY);
	EsEvtSpecData(wxEventType eventType, const EsBinBuffer& data, int id = wxID_ANY);
  // implement the base class pure virtual
  virtual wxEvent* Clone() const;
  
  const EsBinBuffer& getData() const;

protected:	
	EsEvtSpecData(const EsEvtSpecData& src);
	
protected:
	EsBinBuffer m_data;
};

#endif // _es_spec_data_event_