#ifndef _es_comm_config_h_
#define _es_comm_config_h_

// Used Communication channels
//
#define ES_COMM_USE_UART
#ifdef _WIN32
# define ES_COMM_USE_FTDI
#endif

/* #undef ES_COMM_USE_CHANNEL_STUB */
#define ES_COMM_USE_CHANNEL_UART

#define ES_COMM_USE_ANCIENT_PC_DELAY

#ifdef _WIN32
# define ES_COMM_USE_CHANNEL_EKONNECT
#endif

#define ES_COMM_USE_SOCKETS

#ifdef ES_COMM_USE_SOCKETS

#ifndef __APPLE__
/* #undef ES_COMM_USE_BLUETOOTH */
# if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WINNT_WIN32_WINNT_WIN8
/* #undef ES_COMM_USE_BLUETOOTH_LE */
# endif
#endif

#ifndef __APPLE__
# define ES_COMM_USE_CHANNEL_IO_SOCKET
/* #undef ES_COMM_USE_CHANNEL_BLUETOOTH */
# if defined(_WIN32_WINNT) && _WIN32_WINNT >= _WINNT_WIN32_WINNT_WIN8
/* #undef ES_COMM_USE_CHANNEL_BLUETOOTH_LE */
# endif
#endif

#endif

#endif // _es_comm_config_h_
