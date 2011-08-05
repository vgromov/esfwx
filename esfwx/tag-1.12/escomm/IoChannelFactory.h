#ifndef _io_channel_factory_h_
#define _io_channel_factory_h_

// io channel factory class
class EKOSF_COMM_CLASS IoChannelFactory : public EsReflectedClassIntf
{
public:
	ES_DECL_REFLECTED_CLASS_BASE( IoChannelFactory )
	ES_NON_RC_INTF_MAP_BEGIN(IoChannelFactory)
		ES_INTF_SUPPORTS(IoChannelFactory, EsReflectedClassIntf)
	ES_NON_RC_INTF_MAP_END
	{ /* destroy does nothing */ }

	// return list of all known channel class names
	static EsString::Array getChannelNames();
	// return label for channel class, throw exception if not registered
	static EsString getChannelLabel(const EsString& channelName);
	// try to find channel name by its label. throw if not found
	static EsString getChannelByLabel(const EsString& label);
	// create channel instance, throw exception if class is not registered
	static EsIoChannelIntf::Ptr createChannel(const EsDictionary& cfg, wxEvtHandler* sink = 0);
	static EsIoChannelIntf::Ptr createChannel(const EsString& channelName, const EsString& config = EsString::s_null, wxEvtHandler* sink = 0);
	// create configuration GUI pane instance for channel class, throw exception if class is not registered
	static EsIoChannelConfigPaneIntf::Ptr createChannelConfigPane(const EsString& channelName, wxWindow* parent);

protected:
	// register channel entry
	static void registerChannel(const ClassInfo* chnlInfo, const ClassInfo* paneInfo = 0);

protected:
	struct RegEntryT
	{
		const ClassInfo* m_chnlInfo;
		const ClassInfo* m_paneInfo;
	};
	typedef containers::structured_map<EsString, RegEntryT> ChannelClassesT;
	ChannelClassesT m_reg;

	IoChannelFactory();
	static IoChannelFactory& inst();

private:
	// denied functionality
	IoChannelFactory(const IoChannelFactory&);
	IoChannelFactory& operator=(const IoChannelFactory&);
};

// channel registrar helper macros. to be used in channel implementation classes
#define ES_REGISTER_CHANNEL_NO_CONFIG_PANE( ChannelClass ) \
	IoChannelFactory::registerChannel(& ChannelClass ##::getClassInfoStatic());

#define ES_REGISTER_CHANNEL( ChannelClass, PaneClass ) \
	IoChannelFactory::registerChannel(& ChannelClass ##::getClassInfoStatic(), & PaneClass ##::getClassInfoStatic());
	
#endif // _io_channel_factory_h_