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
