cmake_minimum_required(VERSION 3.11)
project(esmath)

include(esmath.config.cmake)

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
set(esmath_common_SRCS
						esmath.h
						build.h
            EsMathException.cpp
            EsMathException.h
            EsMathUtils.cpp
            EsMathUtils.h
            EsMathInternals.cpp
            EsMathInternals.h
            EsMathArrayReal.cpp
            EsMathArrayReal.h
            EsMathArrayComplex.cpp
            EsMathArrayComplex.h
            EsMathArrayView.h
            EsMathConvolutionReal.cpp
            EsMathConvolutionReal.h
            EsMathDelayLine.cpp
            EsMathDelayLine.h
            EsMathFft.cpp
            EsMathFft.h
            EsMathFitConstraint.cpp
            EsMathFitConstraint.h
            EsMathPolyFit.cpp
            EsMathPolyFit.h
            EsMathSpline.cpp
            EsMathSpline.h
            EsMathSplineFit.cpp
            EsMathSplineFit.h
            EsMathWindow.cpp
            EsMathWindow.h
						)

set(esmath_locale_SRCS 
						locale/esmath.pot
						locale/esmath.ru_RU.po
						)
source_group(locale FILES ${esmath_locale_SRCS})
I18N_ADD("esmath" esmath_locale_SRCS "${binaryRoot}")

set(esmath_SRCS
						${esmath_common_SRCS}
						${esmath_alglib_SRCS}
						${esmath_locale_SRCS}
						)					
						
if(WIN32)
	set(esmath_RES ${esmath_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${esmath_RES})

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "esmathLib${staticMarker}")
set(${targetName} "esmath_${ESMATH_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker}" CACHE INTERNAL "" FORCE)
message(
  STATUS
  "esmath targetName=>${targetName}:${${targetName}}"
)

SPECIFY_PRECOMPILED_HEADER("esmathpch.h" "EsMathMain.cpp" esmath_SRCS)
SPECIFY_HEADER_FILES(esmath_SRCS headerExtensions)

add_library(${${targetName}} 
	${esmath_SRCS} 
	${esmath_RES}
)
  
# Set-up library dependencies
set(ESMATH_EXTRA_LIBRARIES ${escoreLib${staticMarker}})
target_link_libraries( 
  ${${targetName}}
  ${ESMATH_EXTRA_LIBRARIES}
)
message(
  STATUS
  "ESMATH_EXTRA_LIBRARIES=>${ESMATH_EXTRA_LIBRARIES}"
)

# customize target properties
#
# export preprocessor defines for dll
if( ES_BUILD_SHARED_LIBS )
	set_property(
    TARGET ${${targetName}} 
    APPEND
    PROPERTY COMPILE_DEFINITIONS "${ES_USEDLL_PREPROC_FOR_DYNLIB}" ESMATH_EXPORTS
  )					
endif()
