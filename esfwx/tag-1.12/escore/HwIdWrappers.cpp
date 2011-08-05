#include <stdafx.h>
#pragma hdrstop

#include "HwIdWrappers.h"
#include <algorithm>

// short hardware id. Embedded version
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsShortHwIdEmbedded, wxT("Read-only access to short hardware id data"))
	ES_DECL_PROP_INFO_RO(EsShortHwIdEmbedded, dev_type, ulong, _i("Device type id"))
	ES_DECL_PROP_INFO_RO(EsShortHwIdEmbedded, dev_year, ulong, _i("Device production year"))
	ES_DECL_PROP_INFO_RO(EsShortHwIdEmbedded, dev_num, ulong, _i("Device production number within year"))	
	ES_DECL_PROP_INFO_RO(EsShortHwIdEmbedded, fw_major, ulong, _i("Firmware id version"))
	ES_DECL_PROP_INFO_RO(EsShortHwIdEmbedded, fw_minor, ulong, _i("Firmware id revision"))
	// services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsShortHwIdEmbedded, asString, EsString_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsShortHwIdEmbedded, asString, EsString_CallConst_ulong, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END	

EsShortHwIdEmbedded::~EsShortHwIdEmbedded() {}

ulong EsShortHwIdEmbedded::get_dev_type() const { return m_shortId.type; }
ulong EsShortHwIdEmbedded::get_dev_year() const { return m_shortId.num.year; }
ulong EsShortHwIdEmbedded::get_dev_num() const { return m_shortId.num.ser; }
ulong EsShortHwIdEmbedded::get_fw_major() const { return m_shortId.ver.major; }
ulong EsShortHwIdEmbedded::get_fw_minor() const { return m_shortId.ver.minor; }

// short hardware id, ref-counted version
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsShortHwId, wxT("Read-only access to short hardware id data"))
	ES_DECL_PROP_INFO_RO(EsShortHwId, dev_type, ulong, _i("Device type id"))
	ES_DECL_PROP_INFO_RO(EsShortHwId, dev_year, ulong, _i("Device production year"))
	ES_DECL_PROP_INFO_RO(EsShortHwId, dev_num, ulong, _i("Device production number within year"))	
	ES_DECL_PROP_INFO_RO(EsShortHwId, fw_major, ulong, _i("Firmware id version"))
	ES_DECL_PROP_INFO_RO(EsShortHwId, fw_minor, ulong, _i("Firmware id revision"))
	// services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsShortHwId, asString, EsString_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsShortHwId, asString, EsString_CallConst_ulong, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END	

// create owned copy of shortId
EsShortHwId::EsShortHwId(const BasicFirmwareID& shortId) :
m_id(0)
{
	m_id = new BasicFirmwareID;
	wxASSERT(m_id);
	memcpy(reinterpret_cast<void*>( const_cast<BasicFirmwareID*>(m_id) ), &shortId, BasicFirmwareID_SZE);
}

// special dynamic creator for reflected usage
EsVariant EsShortHwId::create(const BasicFirmwareID& shortId)
{
	EsReflectedClassIntf::Ptr id = new EsShortHwId(shortId);
	return id;
}

EsShortHwId::~EsShortHwId() 
{
	wxDELETE(m_id);
}

ulong EsShortHwId::get_dev_type() const { return m_id->type; }
ulong EsShortHwId::get_dev_year() const { return m_id->num.year; }
ulong EsShortHwId::get_dev_num() const { return m_id->num.ser; }
ulong EsShortHwId::get_fw_major() const { return m_id->ver.major; }
ulong EsShortHwId::get_fw_minor() const { return m_id->ver.minor; }

// long hardware id. Embedded
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsLongHwIdEmbedded, wxT("Read-only access to long hardware id data"))
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, dev_type, ulong, _i("Device type id"))
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, dev_year, ulong, _i("Device production year"))
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, dev_num, ulong, _i("Device production number within year"))	
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, fw_major, ulong, _i("Firmware id version"))
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, fw_minor, ulong, _i("Firmware id revision"))
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, uid, EsString, _i("Unique identifier"))
	ES_DECL_PROP_INFO_RO(EsLongHwIdEmbedded, func_mask, ulong, _i("Functionality mask"))
	// services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsLongHwIdEmbedded, asString, EsString_CallConst, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsLongHwIdEmbedded::~EsLongHwIdEmbedded() {}

ulong EsLongHwIdEmbedded::get_dev_type() const { return m_longId.id.type; }
ulong EsLongHwIdEmbedded::get_dev_year() const { return m_longId.id.num.year; }
ulong EsLongHwIdEmbedded::get_dev_num() const { return m_longId.id.num.ser; }
ulong EsLongHwIdEmbedded::get_fw_major() const { return m_longId.id.ver.major; }
ulong EsLongHwIdEmbedded::get_fw_minor() const { return m_longId.id.ver.minor; }
EsString EsLongHwIdEmbedded::get_uid() const { return wxString(reinterpret_cast<const char*>(m_longId.key), *wxConvCurrent, UID_SZE); }
ulong EsLongHwIdEmbedded::get_func_mask() const { return m_longId.funcMask; }

// long hardware id. ref-counted version
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsLongHwId, wxT("Read-only access to long hardware id data"))
	ES_DECL_PROP_INFO_RO(EsLongHwId, dev_type, ulong, _i("Device type id"))
	ES_DECL_PROP_INFO_RO(EsLongHwId, dev_year, ulong, _i("Device production year"))
	ES_DECL_PROP_INFO_RO(EsLongHwId, dev_num, ulong, _i("Device production number within year"))	
	ES_DECL_PROP_INFO_RO(EsLongHwId, fw_major, ulong, _i("Firmware id version"))
	ES_DECL_PROP_INFO_RO(EsLongHwId, fw_minor, ulong, _i("Firmware id revision"))
	ES_DECL_PROP_INFO_RO(EsLongHwId, uid, EsString, _i("Unique identifier"))
	ES_DECL_PROP_INFO_RO(EsLongHwId, func_mask, ulong, _i("Functionality mask"))
	// services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsLongHwId, asString, EsString_CallConst, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsLongHwId::EsLongHwId(const HubFirmwareID& longId) :
m_id(0)
{
	m_id = new HubFirmwareID;
	wxASSERT( m_id );
	memcpy(reinterpret_cast<void*>( const_cast<HubFirmwareID*>(m_id) ), &longId, HubFirmwareID_SZE);
}

EsVariant EsLongHwId::create(const HubFirmwareID& longId)
{
	EsReflectedClassIntf::Ptr id = new EsLongHwId( longId );
	return id;
}

EsLongHwId::~EsLongHwId() 
{
	wxDELETE(m_id);
}

ulong EsLongHwId::get_dev_type() const { return m_id->id.type; }
ulong EsLongHwId::get_dev_year() const { return m_id->id.num.year; }
ulong EsLongHwId::get_dev_num() const { return m_id->id.num.ser; }
ulong EsLongHwId::get_fw_major() const { return m_id->id.ver.major; }
ulong EsLongHwId::get_fw_minor() const { return m_id->id.ver.minor; }
EsString EsLongHwId::get_uid() const { return wxString(reinterpret_cast<const char*>(m_id->key), *wxConvCurrent, UID_SZE); }
ulong EsLongHwId::get_func_mask() const { return m_id->funcMask; }

// UID class wrapper implementation
//
ES_DECL_BASE_CLASS_INFO_BEGIN(EsUID, wxT("UID wrapper class"))
	// properties
	ES_DECL_PROP_INFO_RO(EsUID, isDevelopment, bool, NO_PROPERTY_LABEL)
	ES_DECL_PROP_INFO_RO(EsUID, value, EsBinBuffer, NO_PROPERTY_LABEL)
	// services
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsUID, asString, EsString_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsUID, isRegistered, bool_CallConst_cr_EsVariant, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsUID, compare, int_CallConst_cr_EsVariant, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

EsUID::EsUID()
{
	memcpy(m_uid, c_emptyKey, UID_SZE);
}

EsUID::EsUID(const EsUID& other)
{
	memcpy(m_uid, other.m_uid, UID_SZE);
}

EsUID::EsUID(UID uid)
{
	memcpy(m_uid, uid, UID_SZE);
}

EsUID::EsUID(const EsString& uid)
{
	copyFromString(uid);
}

EsUID::EsUID(const EsVariant& v)
{
	memcpy(m_uid, c_emptyKey, UID_SZE);
	if( EsVariant::VAR_STRING == v.getType() )
		copyFromString( v.asString() );
	else
		copyFromVariant(v);
}

EsUID::~EsUID() {}

// assignment
EsUID& EsUID::operator= (const EsUID& other)
{
	memcpy(m_uid, other.m_uid, UID_SZE);
	return *this;
}

EsUID& EsUID::operator= (const UID uid)
{
	memcpy(m_uid, uid, UID_SZE);
	return *this;
}

EsUID& EsUID::operator= (const EsString& uid)
{
	copyFromString(uid);
	return *this;
}

// comparison
bool EsUID::operator== (const EsUID& other) const
{
	return 0 == memcmp(m_uid, other.m_uid, UID_SZE);
}

bool EsUID::operator== (const UID uid) const
{
	return 0 == memcmp(m_uid, uid, UID_SZE);
}

bool EsUID::operator== (const EsString& uid) const
{
	EsUID tmp(uid);
	return this->operator ==(tmp);
}

int EsUID::compare(const EsVariant& v) const
{
	if( operator== ( EsUID(v) ) )
		return 0;
	
	return 1;
}

// UID registration checking
bool EsUID::get_isDevelopment() const
{
	return isDevelopmentUid(m_uid) == TRUE;
}

bool EsUID::isRegistered(const HubFirmwareID& hwId) const
{
	return isUidRegistered(&hwId, m_uid) == TRUE;
}

bool EsUID::isRegistered(const EsVariant& v) const
{
	EsReflectedClassIntf::Ptr hwId = v.asExistingObject();
	if( hwId )
	{
		if( hwId->is( EsLongHwId::getClassNameStatic() ) || hwId->is( EsLongHwIdEmbedded::getClassNameStatic() ) )
		{
			EsString uid = hwId->getProperty(wxT("uid")).asString();
			return operator== (uid); 
		}
		else
			EsException::Throw( 0, EsException::Generic, _("Argument '%s' is not hardware ID class"), hwId->getClassName().c_str() );
	}
	
	return false;
}

// return UID as formatted string
EsString EsUID::getAsString(bool format /*= true*/) const
{
	EsString result;
	if( format )
	{
		char buff[36];
		fmtUID(buff, sizeof(buff), m_uid);
		result = wxString(buff);
	}
	else
		result = wxString(m_uid, UID_SZE);
			
	return result;
}

// return UID as binary buffer
EsBinBuffer EsUID::get_value() const
{
	return EsBinBuffer::fromAscii(reinterpret_cast<const char*>(m_uid), UID_SZE);
}

// internal services
//
static const EsString::value_type c_optVals[] = 
{	wxT(' '), wxT('\n'), wxT('\t'), wxT('-') };

struct IsOptionalSymbol
{
	IsOptionalSymbol()
	{}
	
	bool operator() (EsString::value_type val) const 
	{
		for( int idx = 0; idx < ES_CONST_ARRAY_COUNT(c_optVals); ++idx )
			if( c_optVals[idx] == val )
				return true;
		
		return false;
	}
};

void EsUID::copyFromString(const EsString& uid)
{
	// remove optional formatting symbols
	EsString str = uid;
	std::remove_if( str.begin(), str.end(), IsOptionalSymbol() );
	// check remaining string size
	if( str.size() != UID_SZE )
		EsException::Throw(0, EsException::Generic, _("UID string size is not valid") );
	// check if remaining string contains only valid hex symbols
	for( size_t idx = 0; idx < str.size(); ++idx )
		if( !EsString::isHexChar( str[idx] ) )
			EsException::Throw(0, EsException::Generic, _("UID contains invalid symbol(s)") );
		else
			m_uid[idx] = static_cast<char>(str[idx]);
}