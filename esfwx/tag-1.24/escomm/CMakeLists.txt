cmake_minimum_required(VERSION 3.11)
project(escomm)

# component version definitions
set(component_DESCRIPTION_STRING "ESFWX Communication Library")
set(component_INTERNAL_NAME "escomm")
set(component_ORIGINAL_NAME "escomm.dll")
set(component_BUILD_NUM	1)	

# component version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/build.h.in"
  "${PROJECT_SOURCE_DIR}/build.h"
  )
	
# optional resource version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/res/version.rc.in"
  "${PROJECT_SOURCE_DIR}/res/version.rc"
  )

# documentation generator file  
#
configure_file (
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_SOURCE_DIR}/Doxyfile"
  )
	
# project top-level build directory
include_directories(../compatibility-headers ${escomm_CONFIG_DIR})

set(escomm_common_SRCS
						${escomm_CONFIG_DIR}/EsCommConfig.h
						escomm.h
						build.h
            protocolEkosfConfig.h
						EsRpcServices.cpp
						EsRpcServices.h
            EsNetAddressPool.cpp
            EsNetAddressPool.h
						EsCommUtils.cpp
						EsCommUtils.h
						EsChannelIoIntf.h
						EsChannelIoFactory.cpp
						EsChannelIoFactory.h
						EsProtocolMonitorUdp.cpp
						EsProtocolMonitorUdp.h
						)
source_group(common	FILES ${escomm_common_SRCS})

set(escomm_FTDI_SRCS
						EsFtdiDriver.cpp
						EsFtdiDriver.hxx
						EsFtdiDriver.h
						EsFtdiDevice.cpp
						EsFtdiDevice.h
            EsFtdiMpsseDefs.h
            EsFtdiDeviceMpsseI2c.cpp
            EsFtdiDeviceMpsseI2c.h
            EsFtdiDeviceMpsseSpi.cpp
            EsFtdiDeviceMpsseSpi.h
						)
source_group(FTDI FILES ${escomm_FTDI_SRCS})						
						
set(escomm_EkonnectIoChannel_SRCS
						EsChannelIoEkonnect.cpp
						EsChannelIoEkonnect.h
						)
source_group(ChannelIoEkonnect FILES ${escomm_EkonnectIoChannel_SRCS})						

set(escomm_Sockets_SRCS
						EsSocket.cpp
						EsSocket.h
						EsSocketClient.cpp
						EsSocketClient.h
						EsSocketServer.cpp
						EsSocketServer.h
            EsIanaRegistryDefs.h
            EsIanaRegistry.cpp
            EsIanaRegistry.h
						)
source_group(Sockets FILES ${escomm_Sockets_SRCS})

set(escomm_SocketIoChannel_SRCS
						EsChannelIoSocketClient.cpp
						EsChannelIoSocketClient.h
						EsChannelIoSocketServer.cpp
						EsChannelIoSocketServer.h
						)
source_group(ChannelIoSocket FILES ${escomm_SocketIoChannel_SRCS})

set(escomm_UartIoChannel_SRCS						
						EsChannelIoUart.cpp
						EsChannelIoUart.h
						)
source_group(ChannelIoUart FILES ${escomm_UartIoChannel_SRCS})

set(escomm_StubIoChannel_SRCS
						EsChannelIoStub.cpp
						EsChannelIoStub.h
						)
source_group(ChannelIoStub FILES ${escomm_StubIoChannel_SRCS})	

set(escomm_Bluetooth_SRCS
            EsBluetooth.MFGdefs.h
            EsBluetooth.UUID16defs.h
						EsBluetooth.cpp
						EsBluetooth.h
            EsBluetoothLEcharacteristic.defs.h
            EsBluetoothLEdescriptor.defs.h
            EsBluetoothLEservice.defs.h
            EsBluetoothLE.cpp
            EsBluetoothLE.h
						)
source_group(Bluetooth FILES ${escomm_Bluetooth_SRCS})	

set(escomm_BluetoothIoChannels_SRCS
            EsChannelIoBluetoothClient.cpp
            EsChannelIoBluetoothClient.h
            EsChannelIoBluetoothLE.cpp
            EsChannelIoBluetoothLE.h
            )
source_group(BluetoothIoChannels FILES ${escomm_BluetoothIoChannels_SRCS})	

set(escomm_locale_SRCS 
						locale/escomm.pot
						locale/escomm.ru_RU.po
						)
source_group(locale FILES ${escomm_locale_SRCS})
I18N_ADD("escomm" escomm_locale_SRCS "${binaryRoot}")

set(escomm_SRCS
						${escomm_common_SRCS}
						${escomm_StubIoChannel_SRCS}
						${escomm_UartIoChannel_SRCS}
            ${escomm_FTDI_SRCS}
						${escomm_EkonnectIoChannel_SRCS}
            ${escomm_Sockets_SRCS}
						${escomm_SocketIoChannel_SRCS}
            ${escomm_Bluetooth_SRCS}
            ${escomm_BluetoothIoChannels_SRCS}
						${escomm_locale_SRCS}
						)					
						
set(escomm_c_shared_SRCS
						#../external/esfwxe/commintf.c
						../external/esfwxe/commintf.h
						#../external/esfwxe/crc.c
						../external/esfwxe/crc.h
						crcConfig.h
						#../external/esfwxe/protocols/proprietary/protocolEkosf.c
						../external/esfwxe/protocols/proprietary/protocolEkosf.h
						../external/esfwxe/remoteProcCall.h
						#../external/esfwxe/utils.c
						../external/esfwxe/utils.h
						)
source_group(c-shared FILES ${escomm_c_shared_SRCS})
						
if(WIN32)
	set(escomm_RES ${escomm_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${escomm_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "escommLib${staticMarker}")
set(${targetName} escomm_${binarySuffix}${compilerVersion}${staticMarker}	CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("escommpch.h" "EsCommMain.cpp" escomm_SRCS)
SPECIFY_HEADER_FILES(escomm_SRCS headerExtensions)
SPECIFY_HEADER_FILES(escomm_c_shared_SRCS headerExtensions)

add_library(${${targetName}} 
	${escomm_c_shared_SRCS} 
	${escomm_SRCS} 
	${escomm_RES}
	)
target_link_libraries(${${targetName}} 
	${escoreLib${staticMarker}} 
	)

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL ESCOMM_EXPORTS)
						
# set-up crc-ed dll generation
	# set(${targetName}Crc "D29084FC")
	# get_target_property(targetDll ${${targetName}} LOCATION)
	# set(targetDll_crced ${targetDll}_crced)
	# set(${targetName}_crced ${${targetName}}_crced CACHE STRING "" FORCE)
	# add_custom_target(${${targetName}_crced}
		# COMMAND ${CMAKE_COMMAND} -E copy
						# ${targetDll} 
						# ${targetDll_crced}
		# COMMAND ${binaryRoot}/../3rd-parties/CrcTrim.exe
						# ${targetDll_crced}
						# ${${targetName}Crc}
					# )
	# add_dependencies(${${targetName}_crced} ${${targetName}})
	# set_target_properties(${${targetName}_crced} PROPERTIES
						# FOLDER setup )					
endif()
