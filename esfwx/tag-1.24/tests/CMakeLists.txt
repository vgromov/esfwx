cmake_minimum_required(VERSION 3.11)
project(esfwx-tests)

include_directories(
  gtest-1.7.0
  gtest-1.7.0/include
)

include(tests.config.cmake)

configure_file(
  "${PROJECT_SOURCE_DIR}/testsConfig.h.in"
  "${PROJECT_SOURCE_DIR}/testsConfig.h"
)

configure_file(
  "${PROJECT_SOURCE_DIR}/testsSelector.cxx.in"
  "${PROJECT_SOURCE_DIR}/testsSelector.cxx"
)
    
# project source files
set(tests_SRCS
  main.cpp
)

# Add backward.cpp - pretty stack backtrace printer
if(ESTESTS_USE_STACK_BACKTRACE AND (UNIX AND NOT WIN32))
  set(tests_SRCS
		${tests_SRCS}
		backward-cpp/backward.cpp
	)
endif()

# targets
set(staticMarker "")
if( NOT ES_BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()
set(targetName tests_${ESTESTS_VERSION_SUFFIX}_${ESCOMMON_BIN_SUFFIX}${ESCOMMON_COMPILER_VERSION}${staticMarker})
SPECIFY_HEADER_FILES(tests_SRCS headerExtensions)

add_executable(${targetName}
	${tests_SRCS}
)
target_link_libraries(${targetName}
  ${escoreLib${staticMarker}}
  ${esmathLib${staticMarker}}
  ${escommLib${staticMarker}}
  ${esscriptLib${staticMarker}}
)

set(targetProperties
	_VARIADIC_MAX=10
)

if( ES_BUILD_SHARED_LIBS )
  set(targetProperties
		${targetProperties}
		${ES_USEDLL_PREPROC_FOR_EXE}
	)
endif()

set_target_properties(
  ${targetName} 
  PROPERTIES COMPILE_DEFINITIONS "${targetProperties}"
)

if(CMAKE_COMPILER_IS_GNUCXX)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(
      TARGET ${targetName} 
      APPEND
      PROPERTY COMPILE_DEFINITIONS -g -rdynamic
    )
    
    if(ESTESTS_USE_STACK_BACKTRACE)
      target_link_libraries(
        ${targetName}
        dl
        dfd
      )
    endif()
  endif()
  
  if(MINGW)
    set_property(
      TARGET ${targetName} 
      APPEND
      PROPERTY COMPILE_DEFINITIONS "${COMPILE_DEFINITIONS} -mconsoile -municode"
    )
  endif()
endif()

message(
  STATUS
  "tests COMPILE_DEFINITIONS=>${COMPILE_DEFINITIONS}"
)