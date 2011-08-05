#ifndef _Io_Channel_Intf_h_
#define _Io_Channel_Intf_h_

// io operation breaker abstraction interface
//
ES_DECL_INTF_BEGIN2( 377F1490, 6CC9485b, 85D53F47, BE758A56, EsIoBreakIntf, EsBaseIntf )
	// return true if io break is signalled
	ES_DECL_INTF_METHOD(bool, isBreaking)() const = 0;
ES_DECL_INTF_END

// CommChannelBreak converter
class EsCommChannelBreakImpl : public CommChannelBreak
{
public:
	EsCommChannelBreakImpl(EsIoBreakIntf::Ptr brk) : 
	m_brk(brk) 
	{
		isBreaking = &EsCommChannelBreakImpl::cIsBreaking;
	}
	
	CommChannelBreak* getCommChannelBreakPtr()
	{
		if(m_brk)
			return this;
		
		return 0;
	}
	
	static BOOL cIsBreaking(void* This)
	{
		EsCommChannelBreakImpl* cThis = reinterpret_cast<EsCommChannelBreakImpl*>(This);
		wxASSERT(cThis->m_brk);
		return true == cThis->m_brk->isBreaking();
	}
	
	EsIoBreakIntf::Ptr m_brk;
	
private:
	EsCommChannelBreakImpl();
	EsCommChannelBreakImpl(const EsCommChannelBreakImpl&);
	const EsCommChannelBreakImpl& operator= (const EsCommChannelBreakImpl&);
};

// standard embedded threaded io breaker interface implementation
#define ES_THREADED_IO_BREAK_DECL_STD( HostClass ) \
	private: \
	 class EsIoBreakIntfImpl : public EsIoBreakIntf { public: \
		EsIoBreakIntfImpl( const HostClass& host ) : m_host(host) {} \
		ES_EMBEDDED_INTF_MAP_BEGIN(EsIoBreakIntfImpl) \
			ES_INTF_SUPPORTS(EsIoBreakIntfImpl, EsIoBreakIntf) \
		ES_EMBEDDED_INTF_MAP_END \
		ES_DECL_INTF_METHOD(EsString, getTypeName)() const { \
			return getClassName(); } \
		ES_DECL_INTF_METHOD(bool, isBreaking)() const { \
			wxASSERT(m_host.m_inst); \
			return m_host.m_inst->checkForStop(0); } \
		protected: \
			const HostClass& m_host; }; \
	friend class EsIoBreakIntfImpl; \
	EsIoBreakIntfImpl m_breaker;

#define ES_THREADED_IO_BREAK_IMPL_STD( HostClass ) \
HostClass::EsIoBreakIntfImpl::~EsIoBreakIntfImpl() {}

// io channel abstraction interface
//
ES_DECL_INTF_BEGIN2( 1D0C5F74, 516046be, B9D71D76, 6BEC48B9, EsIoChannelIntf, EsBaseIntf )
	// open-close
	ES_DECL_INTF_METHOD(bool, open)() = 0;
	ES_DECL_INTF_METHOD(void, close)() = 0;
	ES_DECL_INTF_METHOD(bool, isOpen)() const = 0;
	// byte io
	ES_DECL_INTF_METHOD(DWORD, putBytes)(const BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker) = 0;
	ES_DECL_INTF_METHOD(DWORD, getBytes)(BYTE* data, DWORD len, DWORD tmo, EsIoBreakIntf::Ptr breaker) = 0;
	ES_DECL_INTF_METHOD(ulong, putBytes)(const EsBinBuffer& data, ulong tmo, EsBaseIntf::Ptr breaker) = 0;
	ES_DECL_INTF_METHOD(EsBinBuffer, getBytes)(ulong len, ulong tmo, EsBaseIntf::Ptr breaker) = 0;
	ES_DECL_INTF_METHOD(ulong, getTxTimeEstimate)(ulong len) const = 0;
	// reset & error info access
	ES_DECL_INTF_METHOD(void, reset)() = 0;
	ES_DECL_INTF_METHOD(long, getError)() const = 0;
	// event sink access
	ES_DECL_INTF_METHOD(void, setEvtSink)(wxEvtHandler* sink) = 0;
	// initialize C CommChannel from existing EsIoChannelIntf
	ES_DECL_INTF_METHOD(void, initCommChannel)(CommChannel* chnl) = 0;
ES_DECL_INTF_END

// io channel with device boot control (normal, programming) 
ES_DECL_INTF_BEGIN2( D07C82FB, 202842c4, B3086B99, FACA18CA, BootCtlIntf, EsBaseIntf )
	// (re)boot device in normal operations node
	ES_DECL_INTF_METHOD(bool, bootNormalMode)() = 0;
	// (re)boot device in programming mode. may be executed in thread context, i.e. breakable
	ES_DECL_INTF_METHOD(bool, bootProgrammingMode)(EsThread::Worker* thread = 0) = 0;
ES_DECL_INTF_END

// io channel with rate change control
ES_DECL_INTF_BEGIN2( D630E44E, 04F14f24, 917F950D, DA53560E, EsRateCtlIntf, EsBaseIntf )
	// rate change support
	ES_DECL_INTF_METHOD(bool, isRateSupported)(long rate) const = 0;
	ES_DECL_INTF_METHOD(EsVariant, getSupportedRates)() const = 0;
	ES_DECL_INTF_METHOD(long, getDefaultRate)() const = 0;
	ES_DECL_INTF_METHOD(long, getRate)() const = 0;
	ES_DECL_INTF_METHOD(bool, setRate)(long rate) = 0;	
ES_DECL_INTF_END

// channel config pane
//
ES_DECL_INTF_BEGIN2( 8CC19CF9, D0B24b4c, 8D361982, 26E276BA, EsIoChannelConfigPaneIntf, EsBaseIntf )
	ES_DECL_INTF_METHOD(EsString, getChannelName)() const = 0;
	// access configuration currently being edited
	ES_DECL_INTF_METHOD(EsString, getChannelConfigString)() const = 0;
	ES_DECL_INTF_METHOD(void, setChannelConfigString)(const EsString& config) = 0;
	// access pane window
	ES_DECL_INTF_METHOD(wxWindow*, getWindow)() = 0;
ES_DECL_INTF_END

#endif // _Io_Channel_Intf_h_