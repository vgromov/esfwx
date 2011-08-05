#include "stdafx.h"
#pragma hdrstop

#include "IoChannelFactory.h"
#include "StubIoChannelConfigPane.h"
#include "UartIoChannelConfigPane.h"
#include "EkonnectIoChannelConfigPane.h"
#include "SocketClientIoChannelConfigPane.h"
#include "SocketServerIoChannelConfigPane.h"

#define FMT_CHNL_S0_NOT_REGISTERED  _("Channel name '%s' is not registered")

ES_DECL_BASE_CLASS_INFO_BEGIN(IoChannelFactory, _i("IO channel factory class"))
ES_DECL_CLASS_INFO_END

IoChannelFactory::IoChannelFactory()
{
	ES_REGISTER_CHANNEL(StubIoChannel, StubIoChannelConfigPaneWnd::StubIoChannelConfigPane)
	ES_REGISTER_CHANNEL(UartIoChannel, UartIoChannelConfigPaneWnd::UartIoChannelConfigPane)
	ES_REGISTER_CHANNEL(EkonnectIoChannel, EkonnectIoChannelConfigPaneWnd::EkonnectIoChannelConfigPane)
	ES_REGISTER_CHANNEL(SocketClientIoChannel, SocketClientIoChannelConfigPaneWnd::SocketClientIoChannelConfigPane)
	ES_REGISTER_CHANNEL(SocketServerIoChannel, SocketServerIoChannelConfigPaneWnd::SocketServerIoChannelConfigPane)
}

IoChannelFactory::~IoChannelFactory() {}

IoChannelFactory& IoChannelFactory::inst()
{
	static IoChannelFactory s_factory;
	return s_factory;
}

// return list of all known channel class names
EsString::Array IoChannelFactory::getChannelNames()
{
	EsString::Array result;
	result.reserve(inst().m_reg.size());
	for( ChannelClassesT::const_iterator cit = inst().m_reg.begin(); cit != inst().m_reg.end(); ++cit )
		result.push_back( cit->first );

	return result;
}

// return label for channel class, throw exception if not registered
EsString IoChannelFactory::getChannelLabel(const EsString& channelName)
{
	ChannelClassesT::const_iterator cit = inst().m_reg.find(channelName);
	if( cit != inst().m_reg.end() )
		return cit->second.m_chnlInfo->getDescr();
	else
		EsException::Throw(0, EsException::Generic, FMT_CHNL_S0_NOT_REGISTERED, channelName);

	return EsString::s_null;
}

// try to find channel name by its label. throw if not found
EsString IoChannelFactory::getChannelByLabel(const EsString& label)
{
	for( ChannelClassesT::const_iterator cit = inst().m_reg.begin(); cit != inst().m_reg.end(); ++cit )
		if( 0 == label.compare(cit->second.m_chnlInfo->getDescr()) )
			return cit->first;

	EsException::Throw(0, EsException::Generic, _("Cannot find channel name for label \"%s\""), label.c_str());
	return EsString::s_null;
}

// create channel instance, throw exception if class is not registered
EsIoChannelIntf::Ptr IoChannelFactory::createChannel(const EsDictionary& cfg, wxEvtHandler* sink /*= 0*/)
{
	return createChannel(cfg.getName(), cfg.getAsString(), sink);
}

EsIoChannelIntf::Ptr IoChannelFactory::createChannel(const EsString& channelName, const EsString& config /*= EsString::s_null*/, wxEvtHandler* sink /*= 0*/)
{	
	EsIoChannelIntf::Ptr result;
	ChannelClassesT::const_iterator cit = inst().m_reg.find(channelName);
	if( cit != inst().m_reg.end() )
	{
		result = cit->second.m_chnlInfo->classCall(c_reflectedCtrName).
			asExistingObject().request<EsIoChannelIntf>();
		wxASSERT(result);
		if( sink )
			result->setEvtSink(sink);
		if( !config.empty() )
		{
			EsReflectedClassIntf::Ptr ref = result;
			if( ref )
				ref->setConfigString(config, true);
		}
	}
	else
		EsException::Throw(0, EsException::Generic, FMT_CHNL_S0_NOT_REGISTERED, channelName);

	return result;
}

// create configuration GUI pane instance for channel class, throw exception if class is not registered
EsIoChannelConfigPaneIntf::Ptr IoChannelFactory::createChannelConfigPane(const EsString& channelName, wxWindow* parent)
{
	EsIoChannelConfigPaneIntf::Ptr result;
	ChannelClassesT::const_iterator cit = inst().m_reg.find(channelName);
	if( cit != inst().m_reg.end() )
	{
		if( cit->second.m_paneInfo )
		{
			EsVariant p( parent );
			result = cit->second.m_paneInfo->classCall(c_reflectedCtrName, p).
				asExistingObject().request<EsIoChannelConfigPaneIntf>();
			wxASSERT(result);
		}
	}
	else
		EsException::Throw(0, EsException::Generic, FMT_CHNL_S0_NOT_REGISTERED, channelName.c_str());

	return result;
}

// register channel entry
void IoChannelFactory::registerChannel(const ClassInfo* chnlInfo, const ClassInfo* paneInfo/* = 0*/)
{
	wxASSERT(chnlInfo);

	RegEntryT entry;
	entry.m_chnlInfo = chnlInfo;
	entry.m_paneInfo = paneInfo;
	inst().m_reg[chnlInfo->getName()] = entry;
}
