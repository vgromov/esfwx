#include "escommpch.h"
#pragma hdrstop

#include "EsChannelIoFactory.h"

#if defined(ES_COMM_USE_ANY_CHANNEL)

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

#define FMT_CHNL_S0_NOT_REGISTERED  _("Channel name '%s' is not registered")

// channel registrar helper macros. to be used in channel implementation classes
#define ES_REGISTER_CHANNEL( ChannelClass ) EsChannelIoFactory::channelsGet().push_back( ES_STRINGIZE( ChannelClass ) );

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsChannelIoFactory, esT("Communication channel factory"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoFactory, channelNamesGet, EsStringArray_ClassCall, esT("Return collection of all registered channel type names"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoFactory, channelDescriptionGet, EsString_ClassCall_cr_EsString, esT("Return registered channel description by its type name"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoFactory, channelDescriptionsGet, EsStringArray_ClassCall, esT("Return collection of all registered channels descriptions"))
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsChannelIoFactory, channelCreate, EsBaseIntfPtr_ClassCall_cr_EsString, esT("Create channel instance by its name"))

#ifdef ES_COMM_USE_CHANNEL_STUB
  ES_REGISTER_CHANNEL(EsChannelIoStub)
#endif
#ifdef ES_COMM_USE_CHANNEL_UART
  ES_REGISTER_CHANNEL(EsChannelIoUart)
#endif
#ifdef ES_COMM_USE_CHANNEL_EKONNECT
  ES_REGISTER_CHANNEL(EsChannelIoEkonnect)
#endif
#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET
  ES_REGISTER_CHANNEL(EsChannelIoSocketClient)
  ES_REGISTER_CHANNEL(EsChannelIoSocketServer)
#endif
#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH
  ES_REGISTER_CHANNEL(EsChannelIoBluetoothClient)
#endif
#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
  ES_REGISTER_CHANNEL(EsChannelIoBluetoothLE)
#endif
ES_DECL_REFLECTED_SERVICES_INFO_END

EsStringArray& EsChannelIoFactory::channelsGet()
{
  static EsStringArray s_channels;

  return s_channels;
}

// return list of all known channel class names
EsString::Array EsChannelIoFactory::channelNamesGet()
{
  return channelsGet();
}

// return label for channel class, throw exception if not registered
EsString EsChannelIoFactory::channelDescriptionGet(const EsString& channelName)
{
  EsStringArray::const_iterator cit = std::find(channelsGet().begin(), channelsGet().end(), channelName);
  if( cit != channelsGet().end() )
    return EsClassInfo::classInfoGet(channelName, true)->descriptionGet();
  else
    EsException::Throw(
      FMT_CHNL_S0_NOT_REGISTERED, 
      channelName
    );

  return EsString::null();
}

// try to find channel name by its label. throw if not found
EsString::Array EsChannelIoFactory::channelDescriptionsGet()
{
  EsStringArray result;
  result.reserve(channelsGet().size());
  for(size_t idx = 0; idx < channelsGet().size(); ++idx)
  {
    result.push_back(
      EsClassInfo::classInfoGet(channelsGet().at(idx), true)->descriptionGet());
  }

  return result;
}

// create channel instance, throw exception if class is not registered
EsBaseIntfPtr EsChannelIoFactory::channelCreate(const EsString& channelName)
{  
  EsBaseIntfPtr result;
  EsStringArray::const_iterator cit = std::find(channelsGet().begin(), channelsGet().end(), channelName);
  if( cit != channelsGet().end() )
    result = EsClassInfo::classInfoGet((*cit), true)->classCall(EsStdNames::reflectedCtr()).asExistingObject();
  else
    EsException::Throw( 
      FMT_CHNL_S0_NOT_REGISTERED, 
      channelName
    );
  
  return result;
}

#endif //< ANY Channels

