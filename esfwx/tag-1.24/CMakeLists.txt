# top-level cmakefile for ekosfera framework
cmake_minimum_required(VERSION 3.11)
project(framework)

include(${PROJECT_BINARY_DIR}/escmake/escommon.defs.cmake)
# Must be placed here, after escommon.defs, but before other stuff
include(esfwx.config.cmake)

include(${PROJECT_BINARY_DIR}/escmake/escompiler.defs.cmake)
include(${PROJECT_BINARY_DIR}/escmake/eslinker.defs.cmake)

# framework version file generation
#
configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_SOURCE_DIR}/version.h"
  )

# build framework components in standalone mode
# otherwise, host project defines which components to build
if( ESFWXCONFIG_STANDALONE_BUILD )
  
  # ESFWX-specific macros
  include(esframework-macros.cmake)

	# global directories
	set(binaryRoot ${ES_PROJECT_ROOT}/bin CACHE INTERNAL "Build binary root")
	set(EXECUTABLE_OUTPUT_PATH ${binaryRoot})
	set(LIBRARY_OUTPUT_PATH ${binaryRoot})
  set(escore_CONFIG_DIR	${PROJECT_SOURCE_DIR} CACHE INTERNAL "escore configuration directory" FORCE)
  set(escomm_CONFIG_DIR ${PROJECT_SOURCE_DIR} CACHE INTERNAL "escomm configuration directory" FORCE)
  set(esscript_CONFIG_DIR ${PROJECT_SOURCE_DIR} CACHE INTERNAL "esscript configuration directory" FORCE)

	# global preprocessor defines
	#
	# for dynamic linkage to ekosf core libraries
	set(dynamicLinkPreprocessorDefinesExe ES_USE_DLLS)
	# for dynamic linkage to ekosf core libraries and proper dll interface generation
	set(dynamicLinkPreprocessorDefines ${dynamicLinkPreprocessorDefinesExe} _USRDLL)
	# for each subproject, add project current source and build dirs to the include directories
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# reset i18n aggregation variables
	I18N_RESET()
	include_directories(	${PROJECT_SOURCE_DIR}
												${PROJECT_SOURCE_DIR}/compatibility-headers
												${PROJECT_SOURCE_DIR}/external
												)

	# framework components
	#
	add_subdirectory(escore)
	add_subdirectory(esmath)
	add_subdirectory(escomm)
	add_subdirectory(esscript)

	# tests
	#
	add_subdirectory(tests)
  
  I18N_TRACE()
 endif()
