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

# Set-up extra libraries
set(
  ESFWX_TESTS_EXTRA_LIBRARIES 
  ${escoreLib${staticMarker}}
  ${esmathLib${staticMarker}}
  ${escommLib${staticMarker}}
  ${esscriptLib${staticMarker}}
)
if(CMAKE_COMPILER_IS_GNUCXX AND NOT MINGW)
  if(ESTESTS_USE_STACK_BACKTRACE)
    list(
      APPEND
      ESFWX_TESTS_EXTRA_LIBRARIES
      dl bfd
    )
  endif()
endif()
  
target_link_libraries(
  ${targetName}
  ${ESFWX_TESTS_EXTRA_LIBRARIES}
)
message(
  STATUS
  "ESFWX_TESTS_EXTRA_LIBRARIES=>${ESFWX_TESTS_EXTRA_LIBRARIES}"
)

# Set-up definitions and options
set( 
  ESFWX_TESTS_DEFS 
  _VARIADIC_MAX=10 
)
if( ES_BUILD_SHARED_LIBS )
  list(
    APPEND 
    ESFWX_TESTS_DEFS ${ES_USEDLL_PREPROC_FOR_EXE}
  )
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
  if(NOT MINGW AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    set(ESFWX_TESTS_OPTS "-rdynamic")
  endif()
endif()
if(MINGW)
  if(ESFWX_TESTS_OPTS)
    set(
      ESFWX_TESTS_OPTS
      "${ESFWX_TESTS_OPTS} -mconsole -municode"
    )
  else()
    set(
      ESFWX_TESTS_OPTS
      "-mconsole -municode"
    )
  endif()
endif()


set_target_properties(
  ${targetName}
  PROPERTIES
  LINK_FLAGS "${ESFWX_TESTS_OPTS}"
  COMPILE_DEFINITIONS "${ESFWX_TESTS_DEFS}"
)

message(
  STATUS
  "${targetName} ESFWX_TESTS_DEFS=>${ESFWX_TESTS_DEFS} ESFWX_TESTS_OPTS=>${ESFWX_TESTS_OPTS}"
)
