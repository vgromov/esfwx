cmake_minimum_required(VERSION 3.16)
project(esmdi)

include(esmdi.config.cmake)

# esmdi configuration header generation
#
#configure_file (
#  "${PROJECT_SOURCE_DIR}/EsMdiConfig.h.in"
#  "${ES_PROJECT_ROOT}/EsMdiConfig.h"
#)

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
find_package(Doxygen 
  REQUIRED dot
)
configure_file(
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_SOURCE_DIR}/Doxyfile"
)

include_directories(${ES_PROJECT_ROOT})

set(esmdi_common_SRCS
  #${ES_PROJECT_ROOT}/EsMdiConfig.h
  build.h
  esmdi.h
)
source_group(common FILES ${esmdi_common_SRCS})

set(esmdi_docman_SRCS
  EsDocument.h
  EsDocument.cpp
  EsDocumentManagerIntf.h
  EsDocumentManager.h
  EsDocumentManager.cpp
)
source_group(DocManager FILES ${esmdi_docman_SRCS})

#set(esmdi_locale_SRCS
#  locale/esmdi.pot
#  locale/esmdi.ru_RU.po
#  locale/esmdi.de_DE.po
#)
#source_group(locale FILES ${esmdi_locale_SRCS})
#ESI18N_ADD("esmdi" esmdi_locale_SRCS "${binaryRoot}")

set(esmdi_SRCS
  ${esmdi_common_SRCS}
  ${esmdi_docman_SRCS}
)

if(WIN32)
    set(esmdi_RES ${esmdi_RES} res/version.rc)
endif(WIN32)
source_group(res FILES ${esmdi_RES})

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
  set(staticMarker "_s")
endif()
set(targetName "esmdiLib${staticMarker}")
set(${targetName} "esmdi_${ESMDI_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker}" CACHE INTERNAL "" FORCE)
set(ESMDI_LIB_TARGET ${${targetName}} CACHE INTERNAL "")
message(
  STATUS
  "esmdi targetName=>${targetName}:${ESMDI_LIB_TARGET}"
)

ES_SPECIFY_PRECOMPILED_HEADER("esmdipch.h" "EsMdiMain.cpp" esmdi_SRCS)
ES_SPECIFY_HEADER_FILES(esmdi_SRCS ${ES_HEADER_EXTENSIONS})

# MDI library
add_library( 
  ${ESMDI_LIB_TARGET}
  ${esmdi_SRCS}
  ${esmdi_RES}
#  ${esmdi_locale_SRCS}
)

target_link_libraries(
  ${ESMDI_LIB_TARGET} 
  ${ESCORE_LIB_TARGET} 
)

# Customize target properties
#
# export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
  if(BORLAND AND EMBARCADERO)
    set_embt_target(Package)
  endif()

  set_property(
    TARGET ${ESMDI_LIB_TARGET}
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESMDI_EXPORTS
  )
endif()
