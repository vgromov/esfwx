cmake_minimum_required(VERSION 2.8)
project(escore)

# boost dependencies
find_package(Boost 1.46 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base xml core)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)
include_directories(../compatibility-headers)

# project source files
set(escore_c_shared_SRCS
						common/crc.c
						common/crc.h
						hubs/common/dataFile.c
						hubs/common/dataFile.h
						common/datetime.c
						common/datetime.h
						common/devices/descriptions.c
						common/devices/descriptions.h
						common/hwIdBase.h
						hubs/common/hwIdUtils.c
						hubs/common/hwIdUtils.h
						common/utils.c
						common/utils.h
						)
source_group(c-shared FILES ${escore_c_shared_SRCS})

set(escore_common_SRCS	
						build.h
						ekosf-core.h
						)
source_group(common FILES ${escore_common_SRCS})

set(escore_utility_SRCS	
						des.cpp
						des.h
						EsBinBuffer.cpp
						EsBinBuffer.h
						EsCompositeFile.cpp
						EsCompositeFile.h
						EsCrc.cpp
						EsCrc.h
						EsEvent.cpp
						EsEvent.h
						EsString.cpp
						EsString.h
						EsThread.cpp
						EsThread.h
						EsVariant.cpp
						EsVariant.h
						EsExceptions.cpp
						EsExceptions.h
						grammar_defs.h
						EsUtilities.cpp
						EsUtilities.h
						EsValidators.cpp
						EsValidators.h
						)
source_group(utility FILES ${escore_utility_SRCS})
						
set(escore_utility_Dictionary_SRCS
						EsDictionary.cpp
						EsDictionary.h
						EsDictionaryCompiler.hxx
						)
source_group(utility\\Dictionary FILES ${escore_utility_Dictionary_SRCS})						

set(escore_utility_DataFile_SRCS
						DataFileParser.cpp
						DataFileParser.h
						DataFileUtils.cpp
						DataFileUtils.h
						DataXmlConversion.cpp
						DataXmlConversion.h
						)
source_group(utility\\DataFile FILES ${escore_utility_DataFile_SRCS})												

set(escore_utility_Wrappers_SRCS
						Devices.cpp
						Devices.h
						EsDateTime.cpp
						EsDateTime.h
						EsGui.cpp
						EsGui.h
						EsPowerStatus.cpp
						EsPowerStatus.h
						HwIdWrappers.cpp
						HwIdWrappers.h
						)
source_group(utility\\Wrappers FILES ${escore_utility_Wrappers_SRCS})	

set(escore_utility_TernaryTree_SRCS
						tst/iterator_wrapper.hpp
						tst/structured_map.hpp
						tst/structured_set.hpp
						tst/ternary_tree.hpp
						)
source_group(utility\\TernaryTree FILES ${escore_utility_TernaryTree_SRCS})
			
set(escore_utility_SRCS
						${escore_utility_SRCS}
						${escore_utility_Dictionary_SRCS}
						${escore_utility_DataFile_SRCS}
						${escore_utility_Wrappers_SRCS}
						${escore_utility_TernaryTree_SRCS}
						)

set(escore_interfaces_SRCS
						EsAppIntf.h
						EsBaseIntf.h
						ProgressMonitorIntf.cpp
						ProgressMonitorIntf.h
						)
source_group(interfaces FILES ${escore_interfaces_SRCS})
						
set(escore_interfaces_reflection_SRCS
						Reflection.cpp
						Reflection.hxx
						Reflection.h
						ReflectionDefCalls.hxx
						ReflectionDefClassCalls.hxx
						ReflectionDefGetters.hxx
						ReflectionDefParamCount.h
						ReflectionDefSetters.hxx
						ReflectionDefSignatures.h
						ReflectionDefSigStrings.hxx
						ReflectionDefTypeCasts.h
						ReflectionGenDefs.h
						ReflectionMethodDeclDefs.h
						ReflectionMethodInfoDeclDefs.h
						ReflectionPropertyDeclDefs.h
						ReflectionReflectedClassDefs.h
						)
source_group(interfaces\\Reflection FILES ${escore_interfaces_reflection_SRCS})		
				
set(escore_interfaces_plugin_SRCS
						PluginIntf.cpp
						PluginIntf.h
						)
source_group(interfaces\\Plugin FILES ${escore_interfaces_plugin_SRCS})	

set(escore_interfaces_SRCS 
						${escore_interfaces_SRCS}
						${escore_interfaces_reflection_SRCS}
						${escore_interfaces_plugin_SRCS}
						)

set(escore_locale_SRCS	
						locale/escore.pot
						locale/escore.ru_RU.po
						)
source_group(locale FILES ${escore_locale_SRCS})
I18N_ADD("esshell" escore_locale_SRCS "${PROJECT_BINARY_DIR}")
						
set(escore_SRCS 		
						${escore_common_SRCS}
						${escore_utility_SRCS}
						${escore_interfaces_SRCS}
						${escore_locale_SRCS})						
						
set(escore_RES  		
						res/help-plugin.xpm
						res/help-plugin.xpm
						res/menu-check-checked.xpm
						res/menu-check-unchecked.xpm
						res/menu-radio-checked.xpm
						res/menu-radio-unchecked.xpm)
				
if(WIN32)
	set(escore_RES ${escore_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${escore_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "escoreLib${staticMarker}")
set(${targetName} escore_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" escore_SRCS)
SPECIFY_HEADER_FILES(escore_SRCS headerExtensions)
SPECIFY_HEADER_FILES(escore_c_shared_SRCS headerExtensions)

add_library(${${targetName}} ${escore_c_shared_SRCS} ${escore_SRCS} ${escore_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL CORE_EXPORTS)
endif()						