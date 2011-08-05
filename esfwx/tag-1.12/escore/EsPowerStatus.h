#ifndef _es_power_status_h_
#define _es_power_status_h_

// EsDateTime reflected embedded class wrapper around wxDateTime && DATE_TIME
//
class EKOSF_CORE_CLASS EsPowerStatus : public EsReflectedClassIntf
{
public:
	EsPowerStatus();
	EsPowerStatus(const PowerStatus& ps) : m_ps(ps) {}
	ES_DECL_REFLECTED_CLASS_EMBEDDED(EsPowerStatus)
	
	ES_EMBEDDED_INTF_MAP_BEGIN(EsPowerStatus)
		ES_INTF_SUPPORTS(EsPowerStatus, EsReflectedClassIntf)
	ES_EMBEDDED_INTF_MAP_END
	
	// reflected properties
	ES_DECL_PROPERTY_RO(minCode, ulong)
	ES_DECL_PROPERTY_RO(maxCode, ulong)
	ES_DECL_PROPERTY_RO(curCode, ulong)	
	ES_DECL_PROPERTY_RO(flags, ulong)
	ES_DECL_PROPERTY_RO(battery, bool)
	ES_DECL_PROPERTY_RO(mainPower, bool)
	ES_DECL_PROPERTY_RO(charging, bool)
	// standard "asString" service
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString);

private:
	PowerStatus m_ps;
};


#endif // _es_power_status_h_
