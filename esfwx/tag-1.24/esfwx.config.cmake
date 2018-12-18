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

# If esfwx is being built as a part of external project, these cache variables may be set elsewhere internally
if(ESFWXCONFIG_STANDALONE_BUILD OR ESFWXCONFIG_BUILD_TESTS)
  set(ESFWXCONFIG_BUILD_MATH 1 CACHE BOOL "Build esmath-math processing library")
	set(ESFWXCONFIG_BUILD_COMM 1 CACHE BOOL "Build escomm-communication library")
  set(ESFWXCONFIG_BUILD_SCRIPT 1 CACHE BOOL "Build esscript-scripting engine library")
endif()  