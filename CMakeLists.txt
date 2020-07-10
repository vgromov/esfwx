# top-level cmakefile for ekosfera framework
cmake_minimum_required(VERSION 3.16)
project(framework)

if( NOT ESFWX_IS_PART_OF_PROJECT )
  include(${PROJECT_SOURCE_DIR}/escmake/escommon.defs.cmake)

  # Must be placed here, after escommon.defs, but before other stuff
  include(esfwx.config.cmake)
endif()

include(esfwx.defs.cmake)

if( NOT ESFWX_IS_PART_OF_PROJECT )
  include(${PROJECT_SOURCE_DIR}/escmake/escompiler.defs.cmake)
  include(${PROJECT_SOURCE_DIR}/escmake/eslinker.defs.cmake)
endif()

# framework version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_SOURCE_DIR}/version.h"
  )

# build framework components in standalone mode
# otherwise, host project defines which components to build
if( ESFWXCONFIG_STANDALONE_BUILD )

	# global directories
	set(binaryRoot ${ES_PROJECT_ROOT}/bin CACHE INTERNAL "Build binary root")
	set(EXECUTABLE_OUTPUT_PATH ${binaryRoot})
	set(LIBRARY_OUTPUT_PATH ${binaryRoot})
 
	# for each subproject, add project current source and build dirs to the include directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# reset i18n aggregation variables
	ESI18N_RESET()
	include_directories(	
    ${PROJECT_SOURCE_DIR}
		${PROJECT_SOURCE_DIR}/compatibility-headers
		${PROJECT_SOURCE_DIR}/external
	)
  ESI18N_TRACE()
endif()

# set-up framework source root
set(ESFWX_SOURCE_ROOT ${PROJECT_SOURCE_DIR} CACHE INTERNAL "")

# framework components
#
add_subdirectory(escore)

if(ESFWXCONFIG_BUILD_MATH)
  add_subdirectory(esmath)
endif()

if(ESFWXCONFIG_BUILD_COMM)
  add_subdirectory(escomm)
endif()

if(ESFWXCONFIG_BUILD_SCRIPT)
  add_subdirectory(esscript)
endif()

if(ESFWXCONFIG_BUILD_MDI)
  add_subdirectory(esmdi)
endif()

if(ESFWXCONFIG_BUILD_TESTS)
  add_subdirectory(tests)
endif()