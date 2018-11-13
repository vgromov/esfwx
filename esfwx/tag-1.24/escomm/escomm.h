#ifndef _es_comm_h_
#define _es_comm_h_

#ifndef _es_core_h_
#	error "escore.h must be included prior to this header!"
#endif

// Communication stuff
//
// core export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#	ifdef ESCOMM_EXPORTS
#		define ESCOMM_CLASS	            ES_EXPORT_SPEC
#		define ESCOMM_FUNC(type, decl)	ES_FUNCEXPORT_SPEC(type, decl)
#		define ESCOMM_DATA(type, decl)	ES_DATAEXPORT_SPEC(type, decl)
#	else
#		define ESCOMM_CLASS	            ES_IMPORT_SPEC
#		define ESCOMM_FUNC(type, decl)	ES_FUNCIMPORT_SPEC(type, decl)
#		define ESCOMM_DATA(type, decl)	ES_DATAIMPORT_SPEC(type, decl)
#	endif
#else
#	define ESCOMM_CLASS
#	define ESCOMM_FUNC(type, decl)    type decl
#	define ESCOMM_DATA(type, decl)    type decl
#endif

// Comm library configuration
#include <EsCommConfig.h>

// Apply dependency fixups
#ifdef ES_COMM_USE_CHANNEL_UART
# ifndef ES_COMM_USE_UART
#   undef ES_COMM_USE_CHANNEL_UART
# endif
#endif

#ifdef ES_COMM_USE_CHANNEL_EKONNECT
# ifndef ES_COMM_USE_FTDI
#   undef ES_COMM_USE_CHANNEL_EKONNECT
# endif
#endif

#ifdef ES_COMM_USE_CHANNEL_BLUETOOTH
# ifndef ES_COMM_USE_BLUETOOTH
#   undef ES_COMM_USE_CHANNEL_BLUETOOTH
#   undef ES_COMM_USE_CHANNEL_BLUETOOTH_LE
# endif
#endif

#ifdef ES_COMM_USE_CHANNEL_IO_SOCKET
# ifndef ES_COMM_USE_SOCKETS
#   undef ES_COMM_USE_CHANNEL_IO_SOCKET
# endif
#endif

/// Cross-framework includes
///
#include <esfwxe/type.h>
#include <esfwxe/CommIntf.h>

#ifdef ES_COMM_USE_RPC
# include <esfwxe/remoteProcCall.h>
#endif

/// Forward declarations
///
class EsChannelIoIntf;

#ifdef ES_COMM_USE_UART
class EsUartEnumerator;
#ifdef ES_COMM_USE_CHANNEL_UART
class EsChannelIoUart;
#endif
#endif

#ifdef ES_COMM_USE_FTDI
class EsFtdiDriver;
# ifdef ES_COMM_USE_CHANNEL_EKONNECT
class EsChannelIoEkonnect;
# endif
#endif

#ifdef ES_COMM_USE_SOCKETS
class EsSocket;
class EsSocketClient;
class EsSocketServer;
#endif

#ifdef ES_COMM_USE_RPC
class EsRpcMaster;
#endif

#include <escomm/EsChannelIoIntf.h>
#include <escomm/EsCommUtils.h>
#include <escomm/EsChannelIoFactory.h>
#include <escomm/EsChannelIoStub.h>
#include <escomm/EsChannelIoUart.h>
#include <escomm/EsFtdiDriver.h>
#include <escomm/EsFtdiDevice.h>
#include <escomm/EsFtdiDeviceMpsseI2c.h>
#include <escomm/EsFtdiDeviceMpsseSpi.h>
#include <escomm/EsChannelIoEkonnect.h>
#include <escomm/EsBluetooth.h>
#include <escomm/EsBluetoothLE.h>
#include <escomm/EsIanaRegistry.h>
#include <escomm/EsSocket.h>
#include <escomm/EsSocketClient.h>
#include <escomm/EsSocketServer.h>
#include <escomm/EsChannelIoSocketClient.h>
#include <escomm/EsChannelIoSocketServer.h>
#include <escomm/EsChannelIoBluetoothClient.h>
#include <escomm/EsChannelIoBluetoothLE.h>
#include <escomm/EsRPCServices.h>
#include <escomm/EsProtocolMonitorUdp.h>
#include <escomm/EsNetAddressPool.h>

ES_DECL_REFLECTION_REGISTRAR(ESCOMM_FUNC, escomm)

#endif // _es_comm_h_
