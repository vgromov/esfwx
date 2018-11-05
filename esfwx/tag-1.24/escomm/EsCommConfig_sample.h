#ifndef _es_comm_config_h_
#define _es_comm_config_h_

// escomm library components configuration sample file
//
// uncomment to include sockets implementation
// and master-slave channels based on sockets
//
//#define ES_COMM_USE_SOKECTS

// uncomment to use channel for ekonnect communication device
// it also enables ftdi driver api support
//
#define ES_COMM_USE_EKONNECT

// uncomment to enable channel based on plain old uart
//
#define ES_COMM_USE_DIRECT_COM

#endif // _es_comm_config_h_
