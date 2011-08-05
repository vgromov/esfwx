#ifndef _global_devices_descriptions_h_
#define _global_devices_descriptions_h_

#include <hubs/common/hwIdUtils.h>

// global namespaced wrapper around device & plugin descriptions
namespace Devices
{
	// device type check
	EKOSF_CORE_FUNC bool isKnownDevice( WORD type );

	// device descriptions access
	EKOSF_CORE_FUNC EsString getCodeName( WORD type );
	EKOSF_CORE_FUNC EsString getDescrShort( WORD type );
	EKOSF_CORE_FUNC EsString getDescrLong( WORD type );
	EKOSF_CORE_FUNC bool isHub( WORD type ); 
	EKOSF_CORE_FUNC bool isLightHub( WORD type );
	EKOSF_CORE_FUNC WORD getTypeFromCodeName(const EsString& codeName);

	// registered plugin ids access
	EKOSF_CORE_FUNC size_t getRegisteredPluginsCount(const HubFirmwareID& hwId);
	EKOSF_CORE_FUNC WORD getRegisteredPluginAtIdx(const HubFirmwareID& hwId, int idx);

	// count of known plugins in specified schema
	EKOSF_CORE_FUNC size_t getPluginCount(const HubSchemaT& schema);

	// get short and long plugin descriptions by its type id
	EKOSF_CORE_FUNC EsString getPluginCodeName(const HubSchemaT& schema, WORD pluginId);
	EKOSF_CORE_FUNC EsString getPluginDescrShort(const HubSchemaT& schema, WORD pluginId);
	EKOSF_CORE_FUNC EsString getPluginDescrLong(const HubSchemaT& schema, WORD pluginId);
	// user registration checking
	EKOSF_CORE_FUNC bool isUidRegistered(const HubFirmwareID& hwId, const UID uid);
	// plugins regisrtation checking
	EKOSF_CORE_FUNC bool isPluginAllowed(const HubSchemaT& schema, const HubFirmwareID& hwId, WORD pluginId);
	// find the first plugin id by appliance type. if plugin id is not found, NO_ID is returned
	EKOSF_CORE_FUNC WORD findPluginIdByApplType(const HubSchemaT& schema, WORD applType);
	// return true if appliance with applId is allowed for plugin with pluginId
	EKOSF_CORE_FUNC bool isApplianceAllowedForPlugin(const HubSchemaT& schema, WORD applType, WORD pluginId);
	// return plugin directory & file names
	// by plugin id
	EKOSF_CORE_FUNC EsString getPluginRootDir(const HubSchemaT& schema, WORD pluginId);
	EKOSF_CORE_FUNC EsString getPluginDataDir(const HubSchemaT& schema, WORD pluginId);
	EKOSF_CORE_FUNC EsString getPluginSettingsFilePathName(const HubSchemaT& schema, WORD pluginId);
	EKOSF_CORE_FUNC EsString fmtApplianceFwID(const ApplianceFirmwareID& id, WORD flags);
	// UID key standard formatting
	EKOSF_CORE_FUNC EsString fmtUID(const UID key);

	// hub schemas map access
	EKOSF_CORE_FUNC const HubSchemaT& getHubSchema(WORD hubType);

}; // namespace Devices


#endif // _global_devices_descriptions_h_
