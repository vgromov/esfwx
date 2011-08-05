cmake_minimum_required(VERSION 2.8)
project(esshell-base)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base core net xml aui adv html qa)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)

# project source files
set(esshell_base_common_SRCS	
						build.h
						esshell-base.h
						)
source_group(common FILES ${esshell_base_common_SRCS})

set(esshell_base_gui_SRCS
						CustomizeShellDlg.cpp
						CustomizeShellDlg.h
						DataFileView.cpp
						DataFileView.h
						EsAboutDlg.cpp
						EsAboutDlg.h
						EsTimeCtrl.cpp
						EsTimeCtrl.h
						FrmLog.cpp
						FrmLog.h
						FrmMain.cpp
						FrmMain.h
						NotifierCtl.cpp
						NotifierCtl.h
						RemoteFsTree.cpp
						RemoteFsTree.h
						SetDateTimeDlg.cpp
						SetDateTimeDlg.h
						SetDateTimePane.cpp
						SetDateTimePane.h
						ShellApp.cpp
						ShellApp.h
						)
source_group(gui FILES ${esshell_base_gui_SRCS})						

set(esshell_base_locale_SRCS
						locale/esshell-base.pot
						locale/esshell-base.ru_RU.po
						)
source_group(locale FILES ${esshell_base_locale_SRCS})
I18N_ADD("esshell" esshell_base_locale_SRCS "${PROJECT_BINARY_DIR}")

set(esshell_base_SRCS 		
						${esshell_base_common_SRCS}
						${esshell_base_gui_SRCS}
						${esshell_base_locale_SRCS}
						)						

set(esshell_base_RES
						res/actions-clear-log.xpm
						res/exit.xpm
						res/file-logoff.xpm
						res/file-selected.xpm
						res/file.xpm
						res/folder-empty.xpm
						res/folder-selected-partial.xpm
						res/folder-selected.xpm
						res/folder.xpm
						res/help.xpm
						res/info.xpm
						res/langs.xpm
						res/menu-options.xpm
						res/open.xpm
						res/stop.xpm
						res/warning.xpm
						)
				
if(WIN32)
	set(esshell_base_RES ${esshell_base_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${esshell_base_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "esshell_baseLib${staticMarker}")
set(${targetName} esshell-base_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" esshell_base_SRCS)
SPECIFY_HEADER_FILES(esshell_base_SRCS headerExtensions)

add_library(${${targetName}} ${esshell_base_SRCS} ${esshell_base_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES} ${escoreLib${staticMarker}} ${escommLib${staticMarker}})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL SHELL_BASE_EXPORTS)
endif()						
