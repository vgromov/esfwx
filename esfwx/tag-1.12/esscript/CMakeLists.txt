cmake_minimum_required(VERSION 2.8)
project(esscript)

# wxWidgets dependencies
set(wxWidgets_USE_LIBS base core)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)

# project source files
set(esscript_common_SRCS	
						build.h
						ekosf-script.h
						)
source_group(common FILES ${esscript_common_SRCS})

set(esscript_ScriptCore_SRCS
						EsCodeSection.cpp
						EsCodeSection.h
						EsInstruction.cpp
						EsInstruction.h
						EsScript.cpp
						EsScript.h
						EsScriptCompiler.hxx
						EsScriptCompilerRuleNames.hxx
						EsScriptMachine.cpp
						EsScriptMachine.h
						EsSymtable.cpp
						EsSymtable.h
						EsValAccessors.cpp
						EsValAccessors.h
						)
source_group(ScriptCore FILES ${esscript_ScriptCore_SRCS})

set(esscript_Objects_SRCS
						EsScriptArrayObject.cpp
						EsScriptArrayObject.h
						EsScriptObject.cpp
						EsScriptObject.h
						EsScriptPodObjects.cpp
						EsScriptPodObjects.h
						)
source_group(ScriptObjects FILES ${esscript_Objects_SRCS})

set(esscript_Utilities_SRCS
						EsScriptDebug.cpp
						EsScriptDebug.h
						)
source_group(EsUtilities FILES ${esscript_Utilities_SRCS})
						
set(esscript_SRCS 		
						${esscript_common_SRCS}
						${esscript_ScriptCore_SRCS}
						${esscript_Objects_SRCS}
						${esscript_Utilities_SRCS}
						)						
						
set(esscript_locale_SRCS
						)
source_group(locale FILES ${esscript_locale_SRCS})

#set(esscript_RES  		
#)
				
if(WIN32)
	set(esscript_RES ${esscript_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${esscript_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "esscriptLib${staticMarker}")
set(${targetName} esscript_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" esscript_SRCS)
SPECIFY_HEADER_FILES(esscript_SRCS headerExtensions)

add_library(${${targetName}} ${esscript_SRCS} ${esscript_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES} ${escoreLib${staticMarker}})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL ESSCRIPT_EXPORTS)
endif()		
		