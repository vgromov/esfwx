# Component version definitions
set(component_DESCRIPTION_STRING "ESFWX Communication Library")
set(component_INTERNAL_NAME "escomm")
set(component_ORIGINAL_NAME "escomm")
set(component_BUILD_NUM	1)

#  Comm components configuration
set(ESCOMM_USE_UART 1 CACHE BOOL "Use serial uart IO, virtual ones included")
set(ESCOMM_USE_ANCIENT_PC_DELAY 1 CACHE BOOL "Use an extra delay when openin serial channel for virtual USB devices on ancient Windows PCs")
set(ESCOMM_USE_FTDI 1 CACHE BOOL "Use FTDI device IO driver wrapper")
set(ESCOMM_USE_SOCKETS 1 CACHE BOOL "Use Bluetooth and Network Sockets IO support")
set(ESCOMM_USE_BLUETOOTH 0 CACHE BOOL "Use classic bluetooth IO")
set(ESCOMM_USE_BLUETOOTH_LE 0 CACHE BOOL "Use BluetoothLE IO")

set(ESCOMM_USE_CHANNEL_STUB 0 CACHE BOOL "Use stub (dummy) IO channel implementation")
set(ESCOMM_USE_CHANNEL_UART 1 CACHE BOOL "Use UART channel implementation")
set(ESCOMM_USE_CHANNEL_EKONNECT 1 CACHE BOOL "Use Ekonnect channel over FTDI IO devices")
set(ESCOMM_USE_CHANNEL_IO_SOCKET 1 CACHE BOOL "Use Network && Bluetooth socket channel")
set(ESCOMM_USE_CHANNEL_BLUETOOTH 0 CACHE BOOL "Use Bluetooth classic channel")
set(ESCOMM_USE_CHANNEL_BLUETOOTH_LE 0 CACHE BOOL "Use Bluertooth LE IO channel")

# Prepare variables for core defines
#
set(ES_COMM_USE_UART ${ESCOMM_USE_UART})
set(ES_COMM_USE_ANCIENT_PC_DELAY ${ESCOMM_USE_ANCIENT_PC_DELAY})
set(ES_COMM_USE_FTDI ${ESCOMM_USE_FTDI})
set(ES_COMM_USE_SOCKETS ${ESCOMM_USE_SOCKETS})
set(ES_COMM_USE_BLUETOOTH ${ESCOMM_USE_BLUETOOTH})
set(ES_COMM_USE_BLUETOOTH_LE ${ESCOMM_USE_BLUETOOTH_LE})

set(ES_COMM_USE_CHANNEL_STUB ${ESCOMM_USE_CHANNEL_STUB})
set(ES_COMM_USE_CHANNEL_UART ${ESCOMM_USE_CHANNEL_UART})
set(ES_COMM_USE_CHANNEL_EKONNECT ${ESCOMM_USE_CHANNEL_EKONNECT})
set(ES_COMM_USE_CHANNEL_IO_SOCKET ${ESCOMM_USE_CHANNEL_IO_SOCKET})
set(ES_COMM_USE_CHANNEL_BLUETOOTH ${ESCOMM_USE_CHANNEL_BLUETOOTH})
set(ES_COMM_USE_CHANNEL_BLUETOOTH_LE ${ESCOMM_USE_CHANNEL_BLUETOOTH_LE})

# Set-up ESCOMM binary version naming
#
set(ESCOMM_VERSION_SUFFIX "${ESFWX_VERSION_SUFFIX}_${component_BUILD_NUM}")
