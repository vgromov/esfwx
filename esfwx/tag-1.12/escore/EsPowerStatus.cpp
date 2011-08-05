#include "stdafx.h"
#pragma hdrstop

#include "EsPowerStatus.h"

ES_DECL_BASE_CLASS_INFO_BEGIN(EsPowerStatus, NO_CLASS_DESCRIPTION)
	// properties
	ES_DECL_PROP_INFO_RO(EsPowerStatus, minCode, ulong, _i("Minimum power code"))
	ES_DECL_PROP_INFO_RO(EsPowerStatus, maxCode, ulong, _i("Maximum power code"))
	ES_DECL_PROP_INFO_RO(EsPowerStatus, curCode, ulong, _i("Current power code"))	
	ES_DECL_PROP_INFO_RO(EsPowerStatus, flags, ulong, _i("Power status flags"))
	ES_DECL_PROP_INFO_RO(EsPowerStatus, battery, bool, _i("Battery power"))
	ES_DECL_PROP_INFO_RO(EsPowerStatus, mainPower, bool, _i("Mains power"))
	ES_DECL_PROP_INFO_RO(EsPowerStatus, charging, bool, _i("Charging"))
	// methods
	ES_DECL_REFLECTED_METHOD_INFO_STD(EsPowerStatus, asString, EsString_CallConst, wxT("Return power status string"))
ES_DECL_CLASS_INFO_END

EsPowerStatus::EsPowerStatus()
{
	m_ps.minCode = m_ps.maxCode = m_ps.curCode = m_ps.flags = 0; 
}

EsPowerStatus::~EsPowerStatus() {}

EsString EsPowerStatus::asString() const
{
	ulong percent = 0;
	if( m_ps.curCode > m_ps.minCode && m_ps.maxCode > m_ps.minCode )
		percent = ((m_ps.curCode - m_ps.minCode)*100) / (m_ps.maxCode-m_ps.minCode);
		
	if( percent > 100 )
		percent = 100;
		
	return EsString::format(_("Power status: %d%%"), percent);	
}

ulong EsPowerStatus::get_minCode() const
{
	return static_cast<ulong>(m_ps.minCode);
}

ulong EsPowerStatus::get_maxCode() const
{
	return static_cast<ulong>(m_ps.maxCode);
}

ulong EsPowerStatus::get_curCode() const
{
	return static_cast<ulong>(m_ps.curCode);
}

ulong EsPowerStatus::get_flags() const
{
	return static_cast<ulong>(m_ps.flags);
}

bool EsPowerStatus::get_battery() const
{
	return powerBattery == (m_ps.flags & powerBattery);
}

bool EsPowerStatus::get_mainPower() const
{
	return powerMains == (m_ps.flags & powerMains);
}

bool EsPowerStatus::get_charging() const
{
	return powerCharging == (m_ps.flags & powerCharging);
}
