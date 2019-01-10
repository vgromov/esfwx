cmake_minimum_required(VERSION 3.11)
project(escore)

include(escore.config.cmake)

# escore configuration header generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/EsCoreConfig.h.in"
  "${ES_PROJECT_ROOT}/EsCoreConfig.h"
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

include_directories(${ES_PROJECT_ROOT})

set(escore_common_SRCS
      ${ES_PROJECT_ROOT}/EsCoreConfig.h
      build.h
      EsCoreDefs.h
      EsCoreTypes.h
      EsCoreEndiannessDefs.h
      escore.h
      EsCoreRtlDefs.h
      EsCoreRtl.cpp
      EsCoreRtl.h
      )
source_group(common FILES ${escore_common_SRCS})

set(escore_utility_SRCS
      EsAssocContainer.cpp
      EsAssocContainer.h
      EsBinBuffer.cpp
      EsBinBuffer.h
      EsCompositeFile.cpp
      EsCompositeFile.h
      EsCrc.cpp
      EsCrc.h
      EsDateTime.cpp
      EsDateTime.h
      EsDynamicLibrary.cpp
      EsDynamicLibrary.h
      EsEnumeration.cpp
      EsEnumeration.h
      EsExceptions.cpp
      EsExceptions.h
      EsHash.cpp
      EsHash.h
      EsVariant.cpp
      EsVariant.h
      EsVariantType.cpp
      EsVariantType.h
      EsScriptDebugInfo.cpp
      EsScriptDebugInfo.h
      EsRange.cpp
      EsRange.h
      EsUtilities.cpp
      EsUtilities.h
      EsProgressMonitor.cpp
      EsProgressMonitor.h
      EsOsInfo.cpp
      EsOsInfo.h
      EsScriptDebugInfo.cpp
      EsScriptDebugInfo.h
      EsUUID.cpp
      EsUUID.h
      EsXml.cpp
      EsXml.h
      EsZip.cpp
      EsZip.h
      EsVector.h
      EsGrammarDefs.h
      EsNearestGreater2n.h
    )
source_group(utility FILES ${escore_utility_SRCS})

set(escore_crypto_SRCS
      EsCryptoAes.cpp
      EsCryptoAes.h
      EsCryptoDes.cpp
      EsCryptoDes.h
      EsCryptoDesLegacy.cpp
      EsCryptoDesLegacy.h
      EsCryptoHash.cpp
      EsCryptoHash.h
      EsCryptoInternal.cpp
    )
source_group(crypto FILES ${escore_crypto_SRCS})

set(escore_event_SRCS
      EsEvent.cpp
      EsEvent.h
      EsEventDispatcher.cpp
      EsEventDispatcher.h
    )
source_group(event FILES ${escore_event_SRCS})

set(escore_file_SRCS
      EsFile.cpp
      EsFile.h
      EsPath.cpp
      EsPath.h
      EsPathEnumerator.cpp
      EsPathEnumerator.h
    )
source_group(file FILES ${escore_file_SRCS})

set(escore_thread_SRCS
      EsAtomic.cpp
      EsAtomic.h
      EsThread.cpp
      EsThread.h
      EsSyncObjects.cpp
      EsSyncObjects.h
    )
source_group(thread FILES ${escore_thread_SRCS})

set(escore_string_SRCS
      EsStringDefs.h
      EsString.cpp
      EsString.h
      EsStringStd.cpp
      EsStringStd.h
      EsStringI18n.cpp
      EsStringI18n.h
      EsStringConverter.cpp
      EsStringConverter.h
      EsStringIndexedMap.cpp
      EsStringIndexedMap.h
      EsStringTokenizer.cpp
      EsStringTokenizer.h
      EsRegEx.cpp
      EsRegEx.h
      EsLocale.cpp
      EsLocale.h
    )
source_group(string FILES ${escore_string_SRCS})

set(escore_interfaces_SRCS
      EsBaseIntf.h
      EsBreakIntf.h
      EsLogIntf.h
    )
source_group(interfaces FILES ${escore_interfaces_SRCS})

set(escore_reflection_SRCS
      EsReflection.cpp
      EsReflection.hxx
      EsReflection.h
      EsReflectionDefCallProxies.hxx
      EsReflectionDefCalls.hxx
      EsReflectionDefClassCallProxies.hxx
      EsReflectionDefClassCalls.hxx
      EsReflectionDefGetters.hxx
      EsReflectionDefParamCount.h
      EsReflectionDefSetters.hxx
      EsReflectionDefSignatures.h
      EsReflectionDefSigStrings.hxx
      EsReflectionDefTypeCasts.h
      EsReflectionDualCallDefs.h
      EsReflectionGenDefs.h
      EsReflectionMethodDeclDefs.h
      EsReflectionMethodInfoDeclDefs.h
      EsReflectionPropertyDeclDefs.h
      EsReflectionReflectedClassDefs.h
      EsMetaclass.cpp
      EsMetaclass.h
      EsStdNames.cpp
      EsStdNames.h
    )
source_group(reflection FILES ${escore_reflection_SRCS})

set(escore_streams_SRCS
      EsStream.cpp
      EsStream.h
      EsStreamBlock.cpp
      EsStreamBlock.h
      EsStreamBinary.cpp
      EsStreamBinary.h
      EsStreamXml.cpp
      EsStreamXml.h
    )
source_group(streams FILES ${escore_streams_SRCS})

set(escore_locale_SRCS
      locale/escore.pot
      locale/escore.ru_RU.po
      locale/escore.de_DE.po
    )
source_group(locale FILES ${escore_locale_SRCS})
ESI18N_ADD("escore" escore_locale_SRCS "${binaryRoot}")

set(escore_SRCS
      ${escore_common_SRCS}
      ${escore_utility_SRCS}
      ${escore_crypto_SRCS}
      ${escore_interfaces_SRCS}
      ${escore_reflection_SRCS}
      ${escore_string_SRCS}
      ${escore_file_SRCS}
      ${escore_thread_SRCS}
      ${escore_streams_SRCS}
      ${escore_event_SRCS}
      ${escore_locale_SRCS}
    )

if(WIN32)
    set(escore_RES ${escore_RES} res/version.rc)
endif(WIN32)
source_group(res FILES ${escore_RES})

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
  set(staticMarker "_s")
endif()
set(targetName "escoreLib${staticMarker}")
set(${targetName} "escore_${ESCORE_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker}" CACHE INTERNAL "" FORCE)
message(
  STATUS
  "escore targetName=>${targetName}:${${targetName}}"
)

ES_SPECIFY_PRECOMPILED_HEADER("escorepch.h" "EsCoreMain.cpp" escore_SRCS)
ES_SPECIFY_HEADER_FILES(escore_SRCS ${ES_HEADER_EXTENSIONS})
ES_SPECIFY_HEADER_FILES(escore_c_shared_SRCS ${ES_HEADER_EXTENSIONS})

add_library( ${${targetName}}
  ${escore_SRCS}
  ${escore_RES}
)

# Set-up library dependencies
if(WIN32)
  set(ESCORE_EXTRA_LIBRARIES version winmm rpcrt4)
else(WIN32)
  set(ESCORE_EXTRA_LIBRARIES dl pthread)
endif(WIN32)
target_link_libraries( 
  ${${targetName}} 
  ${ESCORE_EXTRA_LIBRARIES}
)
message(
  STATUS
  "ESCORE_EXTRA_LIBRARIES=>${ESCORE_EXTRA_LIBRARIES}"
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
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESCORE_EXPORTS
  )
endif()
