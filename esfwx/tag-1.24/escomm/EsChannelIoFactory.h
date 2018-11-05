#ifndef _io_channel_factory_h_
#define _io_channel_factory_h_

// io channel factory class
ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsChannelIoFactory, ESCOMM_CLASS)
	// reflected service infos declarations
	//
	// return list of all known channel class names
	ES_DECL_REFLECTED_CLASS_METHOD0(EsStringArray, channelNamesGet);
	// return description for channel class, throw exception if not registered
	ES_DECL_REFLECTED_CLASS_METHOD1(EsString, channelDescriptionGet, cr_EsString);
	// return descriptions for all registered channels
	ES_DECL_REFLECTED_CLASS_METHOD0(EsStringArray, channelDescriptionsGet);
	// create channel instance, throw exception if class is not registered
	ES_DECL_REFLECTED_CLASS_METHOD1(EsBaseIntfPtr, channelCreate, cr_EsString);

protected:
	static EsStringArray& channelsGet();

ES_DECL_REFLECTED_SERVICES_END


#endif // _io_channel_factory_h_