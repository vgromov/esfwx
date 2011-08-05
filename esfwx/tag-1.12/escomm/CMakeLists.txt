cmake_minimum_required(VERSION 2.8)
project(escomm)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base net core)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)
include_directories(../compatibility-headers)

set(escomm_common_SRCS
						build.h
						ekosf-comm.h
						CommConfigDlg.cpp
						CommConfigDlg.h
						CommConfigPane.cpp
						CommConfigPane.h
						EkosfRPCServices.cpp
						EkosfRPCServices.h
						EsCommUtils.cpp
						EsCommUtils.h
						EsIoChannelIntf.h
						EsProtocolConfigPane.cpp
						EsProtocolConfigPane.h
						IoChannelEvent.cpp
						IoChannelEvent.h
						IoChannelFactory.cpp
						IoChannelFactory.h
						)
source_group(common	FILES ${escom_common_SRCS})
						
set(escomm_EkonnectIoChannel_SRCS
						EkonnectIoChannel.cpp
						EkonnectIoChannel.h
						EkonnectIoChannelConfigPane.cpp
						EkonnectIoChannelConfigPane.h
						FtdiDriver.cpp
						FtdiDriver.hxx
						FtdiDriver.h
						)
source_group(EkonnectIoChannel FILES ${escomm_EkonnectIoChannel_SRCS})						

set(escomm_SocketIoChannel_SRCS
						SocketClientIoChannel.cpp
						SocketClientIoChannel.h
						SocketClientIoChannelConfigPane.cpp
						SocketClientIoChannelConfigPane.h
						SocketIoAdapter.cpp
						SocketIoAdapter.h
						SocketServerIoChannel.cpp
						SocketServerIoChannel.h
						SocketServerIoChannelConfigPane.cpp
						SocketServerIoChannelConfigPane.h
						)
source_group(SocketIoChannel FILES ${escomm_SocketIoChannel_SRCS})

set(escomm_UartIoChannel_SRCS						
						EsEnumCommPorts.cpp
						EsEnumCommPorts.h
						UartIoChannel.cpp
						UartIoChannel.h
						UartioChannelConfigPane.cpp
						UartIoChannelConfigPane.h
						)
source_group(UartIoChannel FILES ${escomm_UartIoChannel_SRCS})

set(escomm_StubIoChannel_SRCS
						StubIoChannel.cpp
						StubIoChannel.h
						StubIoChannelConfigPane.cpp
						StubIoChannelConfigPane.h
						)
source_group(StubIoChannel FILES ${escomm_StubIoChannel_SRCS})	

set(escomm_locale_SRCS 
						locale/escomm.pot
						locale/escomm.ru_RU.po
						)
source_group(locale FILES ${escomm_locale_SRCS})
I18N_ADD("esshell" escomm_locale_SRCS "${PROJECT_BINARY_DIR}")

set(escomm_SRCS
						${escomm_common_SRCS}
						${escomm_StubIoChannel_SRCS}
						${escomm_UartIoChannel_SRCS}
						${escomm_EkonnectIoChannel_SRCS}
						${escomm_SocketIoChannel_SRCS}
						${escomm_locale_SRCS}
						)					
						
set(escomm_c_shared_SRCS
						common/commintf.c
						common/commintf.h
						common/crc.c
						common/crc.h
						crcConfig.h
						common/protocols/proprietary/protocolEkosf.c
						common/protocols/proprietary/protocolEkosf.h
						common/remoteProcCall.h
						common/utils.c
						common/utils.h
						)
source_group(c-shared FILES ${escomm_c_shared_SRCS})
						
set(escomm_RES
						res/rescan.xpm
						)
source_group(res FILES ${escomm_RES})
						
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

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" escomm_SRCS)
SPECIFY_HEADER_FILES(escomm_SRCS headerExtensions)
SPECIFY_HEADER_FILES(escomm_c_shared_SRCS headerExtensions)

add_library(${${targetName}} ${escomm_c_shared_SRCS} ${escomm_SRCS} ${escomm_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES} ${escoreLib${staticMarker}})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL ESCOMM_EXPORTS)
endif()
