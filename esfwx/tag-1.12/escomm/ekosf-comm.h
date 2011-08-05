#ifndef _ekosf_comm_h_
#define _ekosf_comm_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included prior to this header!"
#endif

// communication stuff
//
// core export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef ESCOMM_EXPORTS
#		define EKOSF_COMM_CLASS	__declspec(dllexport)
#		define EKOSF_COMM_FUNC	__declspec(dllexport)
#		define EKOSF_COMM_DATA	__declspec(dllexport)
#	else
#		define EKOSF_COMM_CLASS	__declspec(dllimport)
#		define EKOSF_COMM_FUNC	__declspec(dllimport)
#		define EKOSF_COMM_DATA	__declspec(dllimport)
#	endif
#else
#	define EKOSF_COMM_CLASS
#	define EKOSF_COMM_FUNC
#	define EKOSF_COMM_DATA
#endif

// cross-platform includes
#include <common/type.h>
#include <common/CommIntf.h>
#include <common/remoteProcCall.h>

class EsIoChannelIntf;
class EsIoChannelConfigPaneIntf;
class IoChannelEvent;
class UartIoChannel;
class FtdiDriver;
class EkonnectIoChannel;
class EsRpcMaster;
class FrmCommLog;
class CommConfigPane;
class EsProtocolConfigPane;
class CommConfigDlg;

#include <escomm/EsIoChannelIntf.h>
#include <escomm/EsCommUtils.h>
#include <escomm/IoChannelFactory.h>
#include <escomm/IoChannelEvent.h>
#include <escomm/EsProtocolConfigPane.h>
#include <escomm/CommConfigPane.h>
#include <escomm/CommConfigDlg.h>
#include <escomm/StubIoChannel.h>
#include <escomm/UartIoChannel.h>
#include <escomm/FtdiDriver.h>
#include <escomm/EkonnectIoChannel.h>
#include <escomm/EkosfRPCServices.h>
#include <escomm/SocketIoAdapter.h>
#include <escomm/SocketClientIoChannel.h>
#include <escomm/SocketServerIoChannel.h>

ES_DECL_REFLECTION_REGISTRAR(escomm)

#endif // _ekosf_comm_h_
