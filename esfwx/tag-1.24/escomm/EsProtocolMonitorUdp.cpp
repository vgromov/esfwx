#include "escommpch.h"
#pragma hdrstop

#include "EsProtocolMonitorUdp.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

/*
const EsProtocolMonitorUdpSignature EsProtocolMonitorUdp::s_onFrameSentSig = "onFrameSent";
const EsProtocolMonitorUdpSignature EsProtocolMonitorUdp::s_onFrameSendingFailedSig = "onFrameSendingFailed";
const EsProtocolMonitorUdpSignature EsProtocolMonitorUdp::s_onFrameReceivedSig = "onFrameReceived";
const EsProtocolMonitorUdpSignature EsProtocolMonitorUdp::s_onFrameReceptionFailedSig = "onFrameReceptionFailed";

EsProtocolIoMonitorIntf::Ptr EsProtocolMonitorUdp::create(const EsString& target, unsigned short targetPort)
{
	std::unique_ptr<EsProtocolMonitorUdp> tmp(new EsProtocolMonitorUdp(target, targetPort));
	tmp->m_dynamic = true;

	return EsProtocolIoMonitorIntf::Ptr( tmp.release() );
}

EsProtocolMonitorUdp::EsProtocolMonitorUdp(const EsString& target, unsigned short targetPort) :
m_dynamic(false),
m_target(target),
m_targetPort(targetPort)
{
}

EsProtocolMonitorUdp::~EsProtocolMonitorUdp()
{
	EsCriticalSectionLocker lock(m_cs);
	m_udp.reset();
}

wxDatagramSocket* EsProtocolMonitorUdp::socketInstanceGet()
{
	if( !m_udp.get() )
	{
		wxIPV4address addr;
		addr.AnyAddress();
		addr.Service(50001);
		m_udp.reset( new wxDatagramSocket(addr) );
	}
	
	ES_ASSERT(m_udp.get());
	ES_ASSERT(m_udp->IsOk());

	return m_udp.get();
}

// internal socket helper
static bool udpSigSend(wxDatagramSocket& udp, const wxSockAddress& peer, 
											 const EsProtocolMonitorUdpSignature sig)
{
	return wxSOCKET_NOERROR == udp.SendTo( peer, sig, sizeof(EsProtocolMonitorUdpSignature)).LastError() &&
			sizeof(EsProtocolMonitorUdpSignature) == udp.LastCount();
}

static bool udpUint32Send(wxDatagramSocket& udp, const wxSockAddress& peer, esU32 val)
{
	return wxSOCKET_NOERROR == udp.SendTo(peer, &val, 4).LastError() &&
		4 == udp.LastCount();
}

static bool udpEsStringSend(wxDatagramSocket& udp, const wxSockAddress& peer, const EsString& str)
{
	esU32 len = str.size() * sizeof(EsString::value_type);
	bool ok = udpUint32Send(udp, peer, len);
	
	if( ok && len )
		ok = wxSOCKET_NOERROR == udp.SendTo(peer, str.c_str(), len).LastError() &&
				 len == udp.LastCount();

	return ok;
}

static bool udpEsBinBufferSend(wxDatagramSocket& udp, const wxSockAddress& peer, const EsBinBuffer& buff)
{
	esU32 len = buff.size();
	bool ok = wxSOCKET_NOERROR == udp.SendTo(peer, &len, 4).LastError() &&
		4 == udp.LastCount();

	if( ok && len )
		ok = wxSOCKET_NOERROR == udp.SendTo(peer, &buff[0], len).LastError() &&
			len == udp.LastCount();

	return ok;
}

// interface implementation
ES_IMPL_INTF_METHOD(void, EsProtocolMonitorUdp::onFrameSent)(const EsString& channelConfig, 
	const EsBinBuffer& frameData, size_t retry)
{
	EsCriticalSectionLocker lock(m_cs);
	wxDatagramSocket* udp = socketInstanceGet();
	wxIPV4address peer;
	peer.Hostname(m_target);
	peer.Service(m_targetPort);

	bool ok = udpSigSend(*udp, peer, s_onFrameSentSig);
	if( ok )
	{
		ok = udpEsStringSend(*udp, peer, channelConfig);
		if( ok )
			ok = udpEsBinBufferSend(*udp, peer, frameData);
		if( ok )
			udpUint32Send(*udp, peer, retry);
	}
}

ES_IMPL_INTF_METHOD(void, EsProtocolMonitorUdp::onFrameSendingFailed)(const EsString& channelConfig, const EsString& channelError, 
	const EsString& protocolError, size_t retry)
{
	EsCriticalSectionLocker lock(m_cs);
	wxDatagramSocket* udp = socketInstanceGet();
	wxIPV4address peer;
	peer.Hostname(m_target);
	peer.Service(m_targetPort);

	bool ok = udpSigSend(*udp, peer, s_onFrameSendingFailedSig);
	if( ok )
	{
		ok = udpEsStringSend(*udp, peer, channelConfig);
		if( ok )
			ok = udpEsStringSend(*udp, peer, channelError);
		if( ok )
			ok = udpEsStringSend(*udp, peer, protocolError);
		if( ok )
			udpUint32Send(*udp, peer, retry);
	}
}

ES_IMPL_INTF_METHOD(void, EsProtocolMonitorUdp::onFrameReceived)(const EsString& channelConfig, 
	const EsBinBuffer& frameData, size_t retry)
{
	EsCriticalSectionLocker lock(m_cs);
	wxDatagramSocket* udp = socketInstanceGet();
	wxIPV4address peer;
	peer.Hostname(m_target);
	peer.Service(m_targetPort);

	bool ok = udpSigSend(*udp, peer, s_onFrameReceivedSig);
	if( ok )
	{
		ok = udpEsStringSend(*udp, peer, channelConfig);
		if( ok )
			ok = udpEsBinBufferSend(*udp, peer, frameData);
		if( ok )
			udpUint32Send(*udp, peer, retry);
	}
}

ES_IMPL_INTF_METHOD(void, EsProtocolMonitorUdp::onFrameReceptionFailed)(const EsString& channelConfig, const EsString& channelError, 
	const EsString& protocolError, size_t retry)
{
	EsCriticalSectionLocker lock(m_cs);
	wxDatagramSocket* udp = socketInstanceGet();
	wxIPV4address peer;
	peer.Hostname(m_target);
	peer.Service(m_targetPort);

	bool ok = udpSigSend(*udp, peer, s_onFrameSendingFailedSig);
	if( ok )
	{
		ok = udpEsStringSend(*udp, peer, channelConfig);
		if( ok )
			ok = udpEsStringSend(*udp, peer, channelError);
		if( ok )
			ok = udpEsStringSend(*udp, peer, protocolError);
		if( ok )
			udpUint32Send(*udp, peer, retry);
	}
}*/