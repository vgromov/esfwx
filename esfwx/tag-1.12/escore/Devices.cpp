#include "stdafx.h"
#pragma hdrstop

#include <common/devices/descriptions.h>
#include <common/utils.h>
#include <hubs/common/hwIdUtils.h>
#include "Devices.h"

#define UNKNOWN_DEVICE _("Unknown device.")

// device type check
bool Devices::isKnownDevice( WORD type )
{
	return ::isKnownDevice(type) != 0;
}

// device description wrappers
//
EsString Devices::getCodeName( WORD type )
{
	wxString result(getDeviceCodeName(type), *wxConvCurrent);
	if( result.empty() )
		result = UNKNOWN_DEVICE;
	return result;
}

EsString Devices::getDescrShort( WORD type )
{
	wxString result(getDeviceDescrShort(type), *wxConvCurrent);
	if( result.empty() )
		result = UNKNOWN_DEVICE;
	return result;
}

EsString Devices::getDescrLong( WORD type )
{
	wxString result(getDeviceDescrLong(type), *wxConvCurrent);
	if( result.empty() )
		result = UNKNOWN_DEVICE;
	return result;
}

bool Devices::isHub( WORD type )
{
	return deviceIsHub(type) != 0;
}

bool Devices::isLightHub( WORD type )
{
	return deviceIsLightHub(type) != 0;
}

WORD Devices::getTypeFromCodeName(const EsString& codeName)
{
	wxString _codeName = codeName.c_str();
	return getDeviceTypeFromCodeName(_codeName.ToAscii());
}

// device functional schema wrappers
//
#define INVALID_REGISTERED_PLUGIN_IDX_D0	_("Invalid registered plugin index: %d.")
#define UNKNOWN_PLUGIN_ID_D0							_("Unknown plugin ID: %d.")
#define UNKNOWN_DEVICE_D0									_("Unknown device: %d.")
#define DEVICE_TYPE_D0_IS_NOT_AN_APPLIANCE _("Device type %d is not an appliance.")

// registered plugin ids access
size_t Devices::getRegisteredPluginsCount(const HubFirmwareID& hwId)
{
	return ::getRegisteredPluginsCount(&hwId);
}

WORD Devices::getRegisteredPluginAtIdx(const HubFirmwareID& hwId, int idx)
{
	if( 0 > idx || idx >= static_cast<int>(getRegisteredPluginsCount(hwId)) )
		EsException::Throw(0, EsException::Generic, INVALID_REGISTERED_PLUGIN_IDX_D0, idx);
	return ::getRegisteredPluginAtIdx(&hwId, idx);
}

size_t Devices::getPluginCount(const HubSchemaT& schema)
{
	return schema.pluginInfosCnt;
}

// get short and long plugin descriptions by its type id
EsString Devices::getPluginCodeName(const HubSchemaT& schema, WORD pluginId)
{
	const char* str = ::getPluginCodeName(&schema, pluginId);
	if( !str )
		return EsString::format(UNKNOWN_PLUGIN_ID_D0, static_cast<int>(pluginId));
	else
		return wxString(str, *wxConvCurrent);
}

EsString Devices::getPluginDescrShort(const HubSchemaT& schema, WORD pluginId)
{
	const char* str = ::getPluginDescrShort(&schema, pluginId);
	if( !str )
		return EsString::format(UNKNOWN_PLUGIN_ID_D0, static_cast<int>(pluginId));
	else
		return wxString(str, *wxConvCurrent);
}

EsString Devices::getPluginDescrLong(const HubSchemaT& schema, WORD pluginId)
{
	const char* str = ::getPluginDescrLong(&schema, pluginId);
	if( !str )
		return EsString::format(UNKNOWN_PLUGIN_ID_D0, static_cast<int>(pluginId));
	else
		return wxString(str, *wxConvCurrent);
}

// user registration checking
bool Devices::isUidRegistered(const HubFirmwareID& hwId, const UID uid)
{
	return ::isUidRegistered(&hwId, uid) != 0;
}

// plugins regisrtation checking
bool Devices::isPluginAllowed(const HubSchemaT& schema, const HubFirmwareID& hwId, WORD pluginId)
{
	return ::isPluginAllowed(&schema, &hwId, pluginId) != 0;
}

// internal type check helper
static inline void checkApplType(WORD applType)
{
	if( !Devices::isKnownDevice(applType) )
		EsException::Throw(0, EsException::Generic, UNKNOWN_DEVICE_D0, static_cast<int>(applType));
	if( Devices::isHub(applType) )
		EsException::Throw(0, EsException::Generic, DEVICE_TYPE_D0_IS_NOT_AN_APPLIANCE, static_cast<int>(applType));
}

// internal plugin id checker
static inline void checkPluginId(const HubSchemaT& schema, WORD pluginId)
{
	if( pluginId >= schema.pluginInfosCnt )
		EsException::Throw(0, EsException::Generic, UNKNOWN_PLUGIN_ID_D0, static_cast<int>(pluginId));
}

// find the first plugin id by appliance type. if plugin id is not found, NO_ID is returned
WORD Devices::findPluginIdByApplType(const HubSchemaT& schema, WORD applType)
{
	checkApplType(applType);
	return ::findPluginIdByApplType(&schema, applType);
}

// return true if appliance with applId is allowed for plugin with pluginId
bool Devices::isApplianceAllowedForPlugin(const HubSchemaT& schema, WORD applType, WORD pluginId)
{
	checkApplType(applType);
	return ::isApplianceAllowedForPlugin(&schema, applType, pluginId) != 0;
}

// return plugin directory & file names
// by plugin id
EsString Devices::getPluginRootDir(const HubSchemaT& schema, WORD pluginId)
{
	checkPluginId(schema, pluginId);
	return wxString(::getPluginRootDir(&schema, pluginId), *wxConvCurrent);
}

EsString Devices::getPluginDataDir(const HubSchemaT& schema, WORD pluginId)
{
	checkPluginId(schema, pluginId);
	return wxString(::getPluginDataDir(&schema, pluginId), *wxConvCurrent);
}

EsString Devices::getPluginSettingsFilePathName(const HubSchemaT& schema, WORD pluginId)
{
	checkPluginId(schema, pluginId);
	return wxString(::getPluginSettingsFilePathName(&schema, pluginId), *wxConvCurrent);
}

EsString Devices::fmtApplianceFwID(const ApplianceFirmwareID& id, WORD flags)
{
	char buff[256];
	::fmtApplianceFwID(buff, sizeof(buff)-1, &id, flags);
	return wxString(buff, *wxConvCurrent);
}

// UID key standard formatting
EsString Devices::fmtUID(const UID key)
{
	char buff[38];
	::fmtUID(buff, sizeof(buff), key);
	return wxString(buff, *wxConvCurrent);
}

// hub schemas map generation. each schema is wrapped in its unique namespace
// to avoid ambiguous names. namespace name must be the same as base name 
// used for corresponding device type in device descriptions.cc
//
namespace BOI1 {
// generate appliances applicable to the plugins from "funcSchema.cc" file
#include <hubs/common/schemaApplsToPluginsBegin.cc>
#include <hubs/boi-01/def_registration/funcSchema.cc>
#include <hubs/common/schemaElementEnd.cc>

// generate all known plugin information from "funcSchema.cc" file
#include <hubs/common/schemaPluginInfosBegin.cc>
#include <hubs/boi-01/def_registration/funcSchema.cc>
#include <hubs/common/schemaElementEnd.cc>

static const HubSchemaT c_schema = { c_all_plugins, ES_CONST_ARRAY_COUNT(c_all_plugins) };
};

// schema map registration halper macro
#define DEF_SCHEMA_ENTRY( HubName ) \
	m_schemas[HUB_TYPE_ ## HubName ] = & HubName ::c_schema;

struct SchemaMap 
{
	typedef std::map<WORD, const HubSchemaT*> SchemasT;

	SchemaMap()
	{
		// for each hub schema add registration entry
		DEF_SCHEMA_ENTRY( BOI1 )
	}

	SchemasT m_schemas;
};
// automatic map registration
static const SchemaMap c_schemaMap;

// hub schemas map access
const HubSchemaT& Devices::getHubSchema(WORD hubType)
{
	if( !Devices::isKnownDevice(hubType) )
		EsException::Throw(0, EsException::Generic, UNKNOWN_DEVICE_D0, static_cast<int>(hubType));
	if( !isHub(hubType) )
		EsException::Throw(0, EsException::Generic, _("Device %d is not a hub."), static_cast<int>(hubType));

	SchemaMap::SchemasT::const_iterator cit = c_schemaMap.m_schemas.find(hubType);
	if( cit == c_schemaMap.m_schemas.end() )
		EsException::Throw(0, EsException::Generic, _("No schema entry is registered for hub %d."), static_cast<int>(hubType));
	
	return *(cit->second);
}
