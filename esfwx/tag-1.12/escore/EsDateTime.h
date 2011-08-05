#ifndef _es_date_time_h_
#define _es_date_time_h_

// EsDateTime reflected embedded class wrapper around wxDateTime && DATE_TIME
//
class EKOSF_CORE_CLASS EsDateTime : public EsReflectedClassIntf
{
public:
	EsDateTime() : m_dt(EsUtilities::DATETIME2wxDateTime(0)) {}
	EsDateTime(const DATETIME& dt) : m_dt( EsUtilities::DATETIME2wxDateTime(dt) ) {}
	EsDateTime(const wxDateTime& dt) : m_dt(dt) {}
	ES_DECL_REFLECTED_CLASS_EMBEDDED(EsDateTime)
	
	// convert to embedded DATETIME type
	operator DATETIME () const 
	{ 
		return EsUtilities::wxDateTime2DATETIME(m_dt); 
	}
	// convert to wxDateTime
	operator wxDateTime () const 
	{ 
		return m_dt; 
	}
	
	ES_EMBEDDED_INTF_MAP_BEGIN(EsDateTime)
		ES_INTF_SUPPORTS(EsDateTime, EsReflectedClassIntf)
	ES_EMBEDDED_INTF_MAP_END
	
	// standard "asString" service
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString) { return m_dt.FormatISOCombined(wxT(' ')); }
	
	// construction methods
	ES_DECL_REFLECTED_CLASS_METHOD0(EsVariant, now);
	ES_DECL_REFLECTED_CLASS_CTOR0(EsVariant) { return EsDateTime(); }
	
	// operators
	EsDateTime& operator= (const DATETIME& src) 
	{
		m_dt = EsUtilities::DATETIME2wxDateTime(src);
		return *this;
	}
	bool operator== (const EsDateTime& other) const
	{
		return m_dt.IsEqualTo(other.m_dt);
	}
	bool operator< (const EsDateTime& other) const
	{
		return m_dt.IsEarlierThan(other.m_dt);
	}
	
private:
	wxDateTime m_dt;
};

#endif // _es_date_time_h_
 