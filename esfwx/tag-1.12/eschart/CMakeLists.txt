cmake_minimum_required(VERSION 2.8)
project(eschart)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base core aui)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

USE_FREECHART()
include_directories(${freechart_Include})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)

# project source files
set(eschart_common_SRCS	
						build.h
						ekosf-chart.h
						)
source_group(common FILES ${eschart_common_SRCS})

set(eschart_Charting_SRCS
						EsSpecDataset.cpp
						EsSpecDataset.h
						EsTransparentAreaDraw.cpp
						EsTransparentAreaDraw.h
						)
source_group(Charting FILES ${eschart_Charting_SRCS})

set(eschart_Misc_SRCS
						EsDsUpdateLocker.h
						EsSpecDataEvent.cpp
						EsSpecDataEvent.h
						EsSpecView.cpp
						EsSpecView.h
						)
source_group(Misc FILES ${eschart_Misc_SRCS})						

set(eschart_locale_SRCS
						locale/eschart.pot
						locale/eschart.ru_RU.po						
						)
source_group(locale FILES ${eschart_locale_SRCS})
I18N_ADD("esshell" eschart_locale_SRCS "${PROJECT_BINARY_DIR}")

set(eschart_SRCS 		
						${eschart_common_SRCS}
						${eschart_Charting_SRCS}
						${eschart_Misc_SRCS}
						${eschart_locale_SRCS}
						)						
						
set(eschart_RES
						res/spec-autoscale.xpm
						res/spec-clear.xpm
						res/spec-wnd-add.xpm
						res/spec-wnd-del.xpm
						res/spec-wnd-rename.xpm
						res/spec-zoom-in.xpm
						res/spec-zoom-out.xpm
						)
				
if(WIN32)
	set(eschart_RES ${eschart_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${eschart_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "eschartLib${staticMarker}")
set(${targetName} eschart_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" eschart_SRCS)
SPECIFY_HEADER_FILES(eschart_SRCS headerExtensions)

add_library(${${targetName}} ${eschart_SRCS} ${eschart_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES} ${freechart_Libs} ${escoreLib${staticMarker}})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL ESCHART_EXPORTS)
endif()
