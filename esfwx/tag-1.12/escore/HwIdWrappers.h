#ifndef _hw_id_wrappers_h_
#define _hw_id_wrappers_h_

// reflected wrappers for hardware identification structures. 
// there is two implementation versions - embedded and ref-counted
//
// read-only transparent access to the BasicFirmwareID members. 
// Embedded version. Actual BasicFirmwareID instance should exist elsewhere
//
class EKOSF_CORE_CLASS EsShortHwIdEmbedded : public EsReflectedClassIntf
{
public:
	EsShortHwIdEmbedded(const BasicFirmwareID& shortId) : m_shortId(shortId) {}
	ES_DECL_REFLECTED_CLASS_BASE(EsShortHwIdEmbedded)
	
	ES_EMBEDDED_INTF_MAP_BEGIN(EsShortHwIdEmbedded)
		ES_INTF_SUPPORTS(EsShortHwIdEmbedded, EsReflectedClassIntf)
	ES_EMBEDDED_INTF_MAP_END
	
	// reflected properties
	ES_DECL_PROPERTY_RO(dev_type, ulong)
	ES_DECL_PROPERTY_RO(dev_year, ulong)
	ES_DECL_PROPERTY_RO(dev_num, ulong)	
	ES_DECL_PROPERTY_RO(fw_major, ulong)
	ES_DECL_PROPERTY_RO(fw_minor, ulong)
	// reflected services
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString) 
	{ 
		return Devices::fmtApplianceFwID(m_shortId, APPL_ID_FMT_DESCR_SHORT|APPL_ID_FMT_SERIAL|APPL_ID_FMT_FW); 
	}
	ES_DECL_REFLECTED_CONST_METHOD1(EsString, asString, ulong)
	{ 
		return Devices::fmtApplianceFwID(m_shortId, static_cast<WORD>(p1)); 
	}	

protected:
	const BasicFirmwareID& m_shortId;
};

// read-only transparent accessor to the BasicFirmwareID members. 
// ref-counted version, creates and owns copy of ID data 
//
class EKOSF_CORE_CLASS EsShortHwId : public EsReflectedClassIntf
{
protected:
	EsShortHwId(const BasicFirmwareID& shortId);

public:
	// special dynamic creator for reflected usage
	static EsVariant create(const BasicFirmwareID& shortId);
	ES_DECL_REFLECTED_CLASS_BASE(EsShortHwId)
	
	ES_INTF_MAP_BEGIN(EsShortHwId)
		ES_INTF_SUPPORTS(EsShortHwId, EsReflectedClassIntf)
	ES_INTF_MAP_END
	{ delete this; }
	
	// reflected properties
	ES_DECL_PROPERTY_RO(dev_type, ulong)
	ES_DECL_PROPERTY_RO(dev_year, ulong)
	ES_DECL_PROPERTY_RO(dev_num, ulong)	
	ES_DECL_PROPERTY_RO(fw_major, ulong)
	ES_DECL_PROPERTY_RO(fw_minor, ulong)
	// reflected services
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString) 
	{ 
		return Devices::fmtApplianceFwID(*m_id, APPL_ID_FMT_DESCR_SHORT|APPL_ID_FMT_SERIAL|APPL_ID_FMT_FW); 
	}
	ES_DECL_REFLECTED_CONST_METHOD1(EsString, asString, ulong)
	{ 
		return Devices::fmtApplianceFwID(*m_id, static_cast<WORD>(p1)); 
	}

protected:
	const BasicFirmwareID* m_id;
	
private:
	// prohibited functionality
	EsShortHwId();
	EsShortHwId(const EsShortHwId&);
	EsShortHwId& operator= (const EsShortHwId&);
};

// read-only transparent access to the HubFirmwareID members. 
// embedded version. Actual HubFirmwareID instance should exist elsewhere
//
class EKOSF_CORE_CLASS EsLongHwIdEmbedded : public EsReflectedClassIntf
{
public:
	EsLongHwIdEmbedded(const HubFirmwareID& longId) : m_longId(longId) {}
	ES_DECL_REFLECTED_CLASS_BASE(EsLongHwIdEmbedded)
	
	ES_EMBEDDED_INTF_MAP_BEGIN(EsLongHwIdEmbedded)
		ES_INTF_SUPPORTS(EsLongHwIdEmbedded, EsReflectedClassIntf)
	ES_EMBEDDED_INTF_MAP_END

	// reflected properties	
	ES_DECL_PROPERTY_RO(dev_type, ulong)
	ES_DECL_PROPERTY_RO(dev_year, ulong)
	ES_DECL_PROPERTY_RO(dev_num, ulong)	
	ES_DECL_PROPERTY_RO(fw_major, ulong)
	ES_DECL_PROPERTY_RO(fw_minor, ulong)	
	ES_DECL_PROPERTY_RO(uid, EsString)
	ES_DECL_PROPERTY_RO(func_mask, ulong)
	// reflected services
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString) 
	{ 
		EsString result = Devices::fmtApplianceFwID(m_longId.id, APPL_ID_FMT_DESCR_SHORT|APPL_ID_FMT_SERIAL|APPL_ID_FMT_FW);
		result += wxT(" ") + Devices::fmtUID(m_longId.key);
		return result; 
	}
		
protected:
	const HubFirmwareID& m_longId;
};

// read-only transparent accessor to the HubFirmwareID members. 
// ref-counted version, owns its copy of ID data.
//
class EKOSF_CORE_CLASS EsLongHwId : public EsReflectedClassIntf
{
protected:
	EsLongHwId(const HubFirmwareID& longId);

public:
	// special dynamic creator for reflected usage
	static EsVariant create(const HubFirmwareID& longId);
	ES_DECL_REFLECTED_CLASS_BASE(EsLongHwId)
	
	ES_INTF_MAP_BEGIN(EsLongHwId)
		ES_INTF_SUPPORTS(EsLongHwId, EsReflectedClassIntf)
	ES_INTF_MAP_END
	{ delete this; }

	// reflected properties	
	ES_DECL_PROPERTY_RO(dev_type, ulong)
	ES_DECL_PROPERTY_RO(dev_year, ulong)
	ES_DECL_PROPERTY_RO(dev_num, ulong)	
	ES_DECL_PROPERTY_RO(fw_major, ulong)
	ES_DECL_PROPERTY_RO(fw_minor, ulong)	
	ES_DECL_PROPERTY_RO(uid, EsString)
	ES_DECL_PROPERTY_RO(func_mask, ulong)
	// reflected services
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString) 
	{ 
		EsString result = Devices::fmtApplianceFwID(m_id->id, APPL_ID_FMT_DESCR_SHORT|APPL_ID_FMT_SERIAL|APPL_ID_FMT_FW);
		result += wxT(" ") + Devices::fmtUID(m_id->key);
		return result; 
	}
		
protected:
	const HubFirmwareID* m_id;
	
private:
	// prohibited functionality
	EsLongHwId();
	EsLongHwId(const EsLongHwId&);
	EsLongHwId& operator= (const EsLongHwId&);
};

// unique ID wrapper class
class EKOSF_CORE_CLASS EsUID : public EsReflectedClassIntf
{
public:
	EsUID();
	EsUID(const EsUID& other);
	EsUID(UID uid);
	EsUID(const EsString& uid);
	EsUID(const EsVariant& v);

	// assignment
	EsUID& operator= (const EsUID& other);
	EsUID& operator= (const UID uid);
	EsUID& operator= (const EsString& uid);
	
	// comparison
	bool operator== (const EsUID& other) const;
	bool operator== (const UID uid) const;
	bool operator== (const EsString& uid) const;

	ES_DECL_REFLECTED_CLASS_EMBEDDED_CUSTOM( EsUID )
	ES_EMBEDDED_INTF_MAP_BEGIN( EsUID )
		ES_INTF_SUPPORTS( EsUID, EsReflectedClassIntf )
	ES_EMBEDDED_INTF_MAP_END

	// properties
	//
	// return UID as binary buffer
	ES_DECL_PROPERTY_RO(value, EsBinBuffer);
	ES_DECL_PROPERTY_RO(isDevelopment, bool);
	// services
	//
	// reflected compare service
	ES_DECL_REFLECTED_CONST_METHOD1(int, compare, cr_EsVariant);
	// UID registration checking
	bool isRegistered(const HubFirmwareID& hwId) const;
	ES_DECL_REFLECTED_CONST_METHOD1(bool, isRegistered, cr_EsVariant);
	// return UID as (optionally) formatted string
	EsString getAsString(bool format = true) const;
	ES_DECL_REFLECTED_CONST_METHOD0(EsString, asString) { return getAsString(); }
	
protected:
	// internal services
	void copyFromString(const EsString& uid);
	
protected:
	UID m_uid;
};

#endif // _hw_id_wrappers_h_
