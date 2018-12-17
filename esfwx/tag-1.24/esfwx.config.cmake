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
