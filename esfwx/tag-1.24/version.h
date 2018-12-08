#ifndef _framework_version_h_
#define _framework_version_h_

// this file is automatically generated by CMake during configuration stage
// do not make changes to this file

// product version
#define FRAMEWORK_VER_MAJOR				      	1
#define FRAMEWORK_VER_MINOR				      	24
#define FRAMEWORK_BUILD_NUM				      	2					
      
#define FRAMEWORK_VENDOR_SHORT            esT("ECO-E")
#define FRAMEWORK_VENDOR					      	esT("ECO-Electronics")
#define FRAMEWORK									      	esT("ECO-E Framework")
#define FRAMEWORK_COPYRIGHT				      	esT("Copyright 2009-2018")
      
#define FRAMEWORK_VERSION_STRING          esT("1.24.2")

#if defined(_DEBUG) || defined(DEBUG)
#	define FRAMEWORK_VERSION_STRING_SHORT		FRAMEWORK esT(" ") FRAMEWORK_VERSION_STRING esT(" DEBUG")
#	define FRAMEWORK_VERSION_STRING_LONG		FRAMEWORK esT(" ") FRAMEWORK_COPYRIGHT esT(" ") FRAMEWORK_VERSION_STRING esT(" DEBUG")
#else
#	define FRAMEWORK_VERSION_STRING_SHORT   FRAMEWORK esT(" ") FRAMEWORK_VERSION_STRING
#	define FRAMEWORK_VERSION_STRING_FULL		FRAMEWORK esT(" ") FRAMEWORK_COPYRIGHT esT(" ") FRAMEWORK_VERSION_STRING
#endif

#endif // _framework_version_h_
