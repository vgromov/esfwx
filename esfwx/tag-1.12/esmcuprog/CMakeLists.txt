cmake_minimum_required(VERSION 2.8)
project(esmcuprog)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)

# project source files
set(esmcuprog_common_SRCS	
						build.h
						esmcuprog.h
						EsFwMoniker.cpp
						EsFwMoniker.h
						)
source_group(common FILES ${esmcuprog_common_SRCS})

set(esmcuprog_isp_SRCS
						McuProgrammer.cpp
						McuProgrammer.h
						)
source_group(isp FILES ${esmcuprog_isp_SRCS})

set(esmcuprog_isp_nxp_SRCS						
						McuLpcProgrammer.cpp
						McuLpcProgrammer.h
						)
source_group(isp\\nxp FILES ${esmcuprog_isp_nxp_SRCS})
						
set(esmcuprog_isp_nxp_lpc2000_SRCS
						lpc21isp.h
						lpcprog.c
						lpcprog.h
						)
source_group(isp\\nxp\\lpc2000 FILES ${esmcuprog_isp_nxp_lpc2000_SRCS})						
						
set(esmcuprog_SRCS 		
						${esmcuprog_common_SRCS}
						${esmcuprog_isp_SRCS}
						${esmcuprog_isp_nxp_SRCS}
						)						
						
set(esmcuprog_locale_SRCS
						)
source_group(locale FILES ${esmcuprog_locale_SRCS})

#set(esmcuprog_RES  		
#)
				
if(WIN32)
	set(esmcuprog_RES ${esmcuprog_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${esmcuprog_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "esmcuprogLib${staticMarker}")
set(${targetName} esmcuprog_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" esmcuprog_SRCS)
SPECIFY_HEADER_FILES(esmcuprog_SRCS headerExtensions)
# do not use files from esmcuprog_isp_nxp_lpc2000_SRCS in explicit build (they got included where appropriate)
set_source_files_properties(${esmcuprog_isp_nxp_lpc2000_SRCS} PROPERTIES
						HEADER_FILE_ONLY ON)

add_library(${${targetName}} ${esmcuprog_SRCS} ${esmcuprog_isp_nxp_lpc2000_SRCS} ${esmcuprog_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES} ${escoreLib${staticMarker}})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL MCUPROG_EXPORTS)
endif()	
					