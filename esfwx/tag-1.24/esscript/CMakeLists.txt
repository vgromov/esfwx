cmake_minimum_required(VERSION 3.7)
project(esscript)

include(esscript.config.cmake)

# Generate configuration header file
#
configure_file (
  "${PROJECT_SOURCE_DIR}/EsScriptConfig.h.in"
  "${ES_PROJECT_ROOT}/EsScriptConfig.h"
)

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
  
# Explicitly use new XXX_ROOT policy
if( POLICY CMP0074 )
  cmake_policy(SET CMP0074 NEW)
endif()
  
# boost dependencies
find_package(Boost 1.54 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS} ${ES_PROJECT_ROOT})
link_directories(${Boost_LIBRARY_DIRS})

# project source files
set(esscript_common_SRCS
            ${ES_PROJECT_ROOT}/EsScriptConfig.h
            build.h
            esscript.h
            EsScriptMain.h
#           EsScriptMain.cpp NB! Got included as pch generator later in this file
            )
source_group(common FILES ${esscript_common_SRCS})

set(esscript_ScriptCore_SRCS
            EsScriptObjectDataBuffer.cpp
            EsScriptObjectDataBuffer.h
            EsScriptObjectPropertyInfo.cpp
            EsScriptObjectPropertyInfo.h
            EsScriptObjectIntf.cpp
            EsScriptObjectIntf.h
            EsScriptValAccessorIntf.h
            EsScriptDataStack.cpp
            EsScriptDataStack.h
            EsScriptInstruction.cpp
            EsScriptInstruction.h  
            EsScriptCodeSection.cpp
            EsScriptCodeSection.h
            EsScriptException.cpp
            EsScriptException.h
            EsScriptCompiler.cpp
            EsScriptCompiler.h
            EsScriptCompilerRuleNames.hxx
            EsScriptCompiledBinary.cpp
            EsScriptCompiledBinary.h
            EsScriptMachine.cpp
            EsScriptMachine.h
            EsScriptlet.cpp
            EsScriptlet.h
            EsScriptSymbolTable.cpp
            EsScriptSymbolTable.h
            EsScriptEnumeration.cpp
            EsScriptEnumeration.h
            )
source_group(ScriptCore FILES ${esscript_ScriptCore_SRCS})

set(esscript_Objects_SRCS
            EsScriptObject.cpp
            EsScriptObject.h
            EsScriptPodObjects.cpp
            EsScriptPodObjects.h
            EsScriptArrayObject.cpp
            EsScriptArrayObject.h
            EsScriptIfObject.cpp
            EsScriptIfObject.h
            )
source_group(ScriptObjects FILES ${esscript_Objects_SRCS})

set(esscript_Utilities_SRCS
            EsScriptDebug.cpp
            EsScriptDebug.h
            EsVar.cpp
            EsVar.h
            EsScriptMetaclass.cpp
            EsScriptMetaclass.h
            EsScriptMetainfoLookup.cpp
            EsScriptMetainfoLookup.h
            EsStr.cpp
            EsStr.h
            EsExceptionInfo.cpp  
            EsExceptionInfo.h
            EsScriptHost.cpp
            EsScriptHost.h
            EsScriptThreadContext.cpp
            EsScriptThreadContext.h
            EsCore.cpp
            EsCore.h
            EsScriptStdNames.cpp
            EsScriptStdNames.h
            EsEventHandler.cpp
            EsEventHandler.h
            EsThreadWorker.cpp
            EsThreadWorker.h
            EsScriptParser.cpp
            EsScriptParser.h
            )
source_group(Utilities FILES ${esscript_Utilities_SRCS})
            
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
if( NOT ES_BUILD_SHARED_LIBS )
  set(staticMarker "_s")
endif()
set(targetName "esscriptLib${staticMarker}")
set(${targetName} "esscript_${ESSCRIPT_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker}" CACHE INTERNAL "" FORCE)
message(
  STATUS
  "esscript targetName=>${targetName}:${${targetName}}"
)

ES_SPECIFY_PRECOMPILED_HEADER("esscriptpch.h" "EsScriptMain.cpp" esscript_SRCS)
ES_SPECIFY_HEADER_FILES(esscript_SRCS ${ES_HEADER_EXTENSIONS})

add_library(${${targetName}} ${esscript_SRCS} ${esscript_RES})

# Set-up library dependencies
set(ESSCRIPT_EXTRA_LIBRARIES ${escoreLib${staticMarker}})
target_link_libraries(
  ${${targetName}} 
  ${ESSCRIPT_EXTRA_LIBRARIES}
)
message(
  STATUS
  "ESSCRIPT_EXTRA_LIBRARIES=>${ESSCRIPT_EXTRA_LIBRARIES}"
)

# customize target properties
#
# export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${${targetName}} 
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESSCRIPT_EXPORTS
  )
endif()
