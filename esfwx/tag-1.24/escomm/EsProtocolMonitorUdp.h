#ifndef _es_protocol_monitor_udp_h_
#define _es_protocol_monitor_udp_h_
/*
// udp packet signature
typedef esU8 EsProtocolMonitorUdpSignature[32];

class ESCOMM_CLASS ES_INTF_IMPL1(EsProtocolMonitorUdp, EsProtocolIoMonitorIntf)

public:
	enum {
		defPort = 60000,
	};

public:
	static EsProtocolIoMonitorIntf::Ptr create(const EsString& target, unsigned short targetPort);
	EsProtocolMonitorUdp(const EsString& target, unsigned short targetPort);

	// EsBaseIntf interface implementation
	ES_DECL_INTF_METHOD(EsString, typeNameGet)() const { return getClassNameStatic(); }
	// EsProtocolIoMonitorIntf interface implementation
	ES_DECL_INTF_METHOD(void, onFrameSent)(const EsString& channelConfig, 
		const EsBinBuffer& frameData, size_t retry);
	ES_DECL_INTF_METHOD(void, onFrameSendingFailed)(const EsString& channelConfig, const EsString& channelError, 
		const EsString& protocolError, size_t retry);
	ES_DECL_INTF_METHOD(void, onFrameReceived)(const EsString& channelConfig, 
		const EsBinBuffer& frameData, size_t retry);	
	ES_DECL_INTF_METHOD(void, onFrameReceptionFailed)(const EsString& channelConfig, const EsString& channelError, 
		const EsString& protocolError, size_t retry);

public:
	static const EsProtocolMonitorUdpSignature s_onFrameSentSig;
	static const EsProtocolMonitorUdpSignature s_onFrameSendingFailedSig;
	static const EsProtocolMonitorUdpSignature s_onFrameReceivedSig;
	static const EsProtocolMonitorUdpSignature s_onFrameReceptionFailedSig;

protected:
	wxDatagramSocket* socketInstanceGet();
	
protected:
	bool m_dynamic;
	EsCriticalSection m_cs;
	// socket options
	EsString m_target;
	unsigned short m_targetPort;
	std::unique_ptr<wxDatagramSocket> m_udp;
};
*/
#endif // _es_protocol_monitor_udp_h_
