cmake_minimum_required(VERSION 2.8)
project(esmath)

#wxWidgets dependencies
set(wxWidgets_USE_LIBS base)
find_package(wxWidgets 2.9 REQUIRED)
include(${wxWidgets_USE_FILE})

# project top-level build directory
set(PROJECT_BINARY_DIR ../../bin CACHE INTERNAL "" FORCE)

# project source files
set(esmath_common_SRCS	
						build.h
						ekosf-math.h
						EsMath.h
						EsMath.cpp
						EsMathInternals.h
						EsMathInternals.cpp
						EsMathFitConstraint.h
						EsMathFitConstraint.cpp
						EsMathSpline.h
						EsMathSpline.cpp
						EsMathPolyFit.h
						EsMathPolyFit.cpp
						EsMathCubicSplineFit.h
						EsMathCubicSplineFit.cpp
						)
source_group(common FILES ${esmath_common_SRCS})

set(esmath_alglib_SRCS
						alglib/alglibinternal.cpp
						alglib/alglibinternal.h
						alglib/alglibmisc.cpp
						alglib/alglibmisc.h
						alglib/ap.cpp
						alglib/ap.h
						alglib/dataanalysis.cpp
						alglib/dataanalysis.h
						alglib/diffequations.cpp
						alglib/diffequations.h
						alglib/fasttransforms.cpp
						alglib/fasttransforms.h
						alglib/integration.cpp
						alglib/integration.h
						alglib/interpolation.cpp
						alglib/interpolation.h
						alglib/linalg.cpp
						alglib/linalg.h
						alglib/optimization.cpp
						alglib/optimization.h
						alglib/solvers.cpp
						alglib/solvers.h
						alglib/specialfunctions.cpp
						alglib/specialfunctions.h
						alglib/statistics.cpp
						alglib/statistics.h
						)
source_group(alglib FILES ${esmath_alglib_SRCS})
						
set(esmath_SRCS 		
						${esmath_common_SRCS}
						${esmath_alglib_SRCS}
						)						
						
if(WIN32)
	set(esmath_RES ${esmath_RES} res/version.rc)
endif(WIN32)						
source_group(res FILES ${esmath_RES})

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName "esmathLib${staticMarker}")
set(${targetName} esmath_${binarySuffix}${compilerVersion}${staticMarker} CACHE STRING "" FORCE)

SPECIFY_PRECOMPILED_HEADER("stdafx.h" "stdafx.cpp" esmath_SRCS)
SPECIFY_HEADER_FILES(esmath_SRCS headerExtensions)

add_library(${${targetName}} ${esmath_SRCS} ${esmath_RES})
target_link_libraries(${${targetName}} ${wxWidgets_LIBRARIES} ${escoreLib${staticMarker}})

# customize target properties
#
# export preprocessor defines for dll
if( BUILD_SHARED_LIBS )
	set_target_properties(${${targetName}} PROPERTIES 
						COMPILE_DEFINITIONS "${dynamicLinkPreprocessorDefines}"
						DEFINE_SYMBOL ESMATH_EXPORTS)
endif()		
		