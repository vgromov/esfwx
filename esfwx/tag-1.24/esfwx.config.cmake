# Common ES framework configuration file
set(ESFWXCONFIG_STANDALONE_BUILD 0 CACHE BOOL "Build framework components as part of some project (Default - OFF), or as stand-alone (ON)")
set(ESFWXCONFIG_BUILD_SHARED_LIBS 1 CACHE BOOL "Build framework components as shared libraries (Default - ON), or as static libraries (OFF)")
set(ESFWXCONFIG_BUILD_TESTS 0 CACHE BOOL "Build ESFWX tests executable. Default=OFF. If ON, sets ESFWXCONFIG_STANDALONE_BUILD ON as well")
set(ESFWXCONFIG_USE_DYNAMIC_RUNTIME 1 CACHE BOOL "Build framework components using dynamic (Default - ON), or static libraries (OFF). When using ESFWX_CONFIG_BUILD_SHARED_LIBS=ON, it's a must")

if(ESFWXCONFIG_BUILD_TESTS)
  set(ESFWXCONFIG_STANDALONE_BUILD 1 CACHE BOOL "" FORCE)
endif()

if(ESFWXCONFIG_BUILD_SHARED_LIBS)
  set(ESFWXCONFIG_USE_DYNAMIC_RUNTIME 1 CACHE BOOL "" FORCE)
endif()

# Framework-wide version definitions
set(frameworkVendor	      "ECO-Electronics" CACHE INTERNAL "")
set(frameworkVendorShort  "ECO-E" CACHE INTERNAL "")
set(framework		          "ECO-E Framework" CACHE INTERNAL "")
set(frameworkCopyright	  "Copyright 2009-2018" CACHE INTERNAL "")
set(framework_VER_MAJOR	  1 CACHE INTERNAL "")
set(framework_VER_MINOR	  24 CACHE INTERNAL "")
set(framework_BUILD_NUM	  2 CACHE INTERNAL "")

set(ESFWX_VERSION_SUFFIX  "v${framework_VER_MAJOR}_${framework_VER_MINOR}_${framework_BUILD_NUM}" CACHE INTERNAL "")
message(
  STATUS
  "ESFWX_VERSION_SUFFIX=>${ESFWX_VERSION_SUFFIX}"
)

# Configure known global variables
set(ES_BUILD_SHARED_LIBS ${ESFWXCONFIG_BUILD_SHARED_LIBS} CACHE INTERNAL "" FORCE)
set(ES_USE_DYNAMIC_RUNTIME ${ESFWXCONFIG_USE_DYNAMIC_RUNTIME} CACHE INTERNAL "" FORCE)
