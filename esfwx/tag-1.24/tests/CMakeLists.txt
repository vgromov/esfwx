cmake_minimum_required(VERSION 3.10)
project(esfwx-tests)

include_directories(
		gtest-1.7.0
		gtest-1.7.0/include
		)

# project source files
set(tests_SRCS
		main.cpp
		)

# Add backward.cpp - pretty stack backtrace printer
if(UNIX AND NOT WIN32)
set(tests_SRCS
		${tests_SRCS}
		backward-cpp/backward.cpp
		)
endif()

# targets
set(staticMarker "")
if( NOT BUILD_SHARED_LIBS )
	set(staticMarker "_s")
endif()

set(targetName tests_${binarySuffix}${compilerVersion}${staticMarker})
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

if( BUILD_SHARED_LIBS )
set(targetProperties
		${targetProperties}
		${dynamicLinkPreprocessorDefinesExe}
		)
endif()

set_target_properties(${targetName} PROPERTIES
		COMPILE_DEFINITIONS "${targetProperties}"
		)
