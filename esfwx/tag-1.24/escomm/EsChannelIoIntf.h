#ifndef _Io_Channel_Intf_h_
#define _Io_Channel_Intf_h_

// Channel monitor interface, along with native implementation,
// may be any reflected class, implementing the same named services, as ones declared in an interface.
// idGet string is _any_ string, uniquely identifying the channel instance which posts its messages to monitor
// 
ES_DECL_INTF_BEGIN( E78A80DC, 86194C4F, 9E2E4078, B5A22EF2, EsChannelIoMonitorIntf )
  ES_DECL_INTF_METHOD(void, channelOpened)(const EsString& idGet) = 0;
  ES_DECL_INTF_METHOD(void, channelClosed)(const EsString& idGet) = 0;
  ES_DECL_INTF_METHOD(void, channelBytesReceived)(const EsString& idGet, const EsBinBuffer& data) = 0;
  ES_DECL_INTF_METHOD(void, channelReceptionError)(const EsString& idGet, const EsString& error) = 0;
  ES_DECL_INTF_METHOD(void, channelBytesSent)(const EsString& idGet, const EsBinBuffer& data) = 0;
  ES_DECL_INTF_METHOD(void, channelSendingError)(const EsString& idGet, const EsString& error) = 0;
  ES_DECL_INTF_METHOD(void, channelError)(const EsString& idGet, const EsString& error) = 0;
  ES_DECL_INTF_METHOD(void, channelCustomEvent)(const EsString& idGet, const EsString& msg) = 0;
ES_DECL_INTF_END

// io channel abstraction interface
//
ES_DECL_INTF_BEGIN( 1D0C5F74, 516046BE, B9D71D76, 6BEC48B9, EsChannelIoIntf )
  // channel unique id string
  ES_DECL_INTF_METHOD(EsString, idGet)() const = 0;
  // monitor interface access.
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, monitorGet)() const = 0;
  ES_DECL_INTF_METHOD(void, monitorSet)(const EsBaseIntf::Ptr& monitor) = 0;
  // open-close
  ES_DECL_INTF_METHOD(bool, open)() = 0;
  ES_DECL_INTF_METHOD(void, close)() = 0;
  ES_DECL_INTF_METHOD(bool, isOpen)() const = 0;
  // io breaker access
  ES_DECL_INTF_METHOD(EsBaseIntf::Ptr, breakerGet)() const = 0;
  ES_DECL_INTF_METHOD(void, breakerSet)(const EsBaseIntf::Ptr& breaker) = 0;
  // Byte io
  ES_DECL_INTF_METHOD(esU32, bytesPut)(const esU8* data, esU32 len, esU32 tmo) = 0;
  ES_DECL_INTF_METHOD(esU32, bytesGet)(esU8* data, esU32 len, esU32 tmo) = 0;
  ES_DECL_INTF_METHOD(ulong, bytesPut)(const EsBinBuffer& data, ulong tmo) = 0;
  ES_DECL_INTF_METHOD(EsBinBuffer, bytesGet)(ulong len, ulong tmo) = 0;
  ES_DECL_INTF_METHOD(ulong, txTimeEstimateGet)(ulong len) const = 0;
  // reset & error info access
  ES_DECL_INTF_METHOD(void, reset)() = 0;
  ES_DECL_INTF_METHOD(long, errorGet)() const = 0;
  ES_DECL_INTF_METHOD(EsString, errorStringGet)() const = 0;  
  // initialize C EseChannelIo from existing EsChannelIoIntf
  ES_DECL_INTF_METHOD(void, commChannelInit)(EseChannelIo* chnl) = 0;
ES_DECL_INTF_END

// device power control && mode configuration (normal, programming) 
ES_DECL_INTF_BEGIN( D07C82FB, 202842C4, B3086B99, FACA18CA, EsPowerCtlIntf )
  // remove|set device power
  ES_DECL_INTF_METHOD(void, devicePowerSet)(bool powerOn) = 0;
  ES_DECL_INTF_METHOD(bool, devicePowerGet)() const = 0;
  // set|reset device programming mode pin
  ES_DECL_INTF_METHOD(void, deviceProgrammingModeSet)(bool modeOn) = 0;
  ES_DECL_INTF_METHOD(bool, deviceProgrammingModeGet)() const = 0;
ES_DECL_INTF_END

// io channel with rate change control
ES_DECL_INTF_BEGIN( D630E44E, 04F14F24, 917F950D, DA53560E, EsRateCtlIntf )
  // rate change support
  ES_DECL_INTF_METHOD(bool, isRateSupported)(ulong rate) const = 0;
  ES_DECL_INTF_METHOD(EsVariant, supportedRatesGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, defaultRateGet)() const = 0;
  ES_DECL_INTF_METHOD(ulong, rateGet)() const = 0;
  ES_DECL_INTF_METHOD(bool, rateSet)(ulong rate) = 0;
ES_DECL_INTF_END

#endif // _Io_Channel_Intf_h_