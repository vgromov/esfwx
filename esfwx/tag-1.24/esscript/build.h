#ifndef _build_h_
#define _build_h_

// esscript component version file
// DO NOT CHANGE!, this file is automatically generated by CMake 
// during build configueration stage

#include <../version.h>

// increment this value when publishing new changes in release
#define COMPONENT_BUILD_NUM						    3
#define COMPONENT_DESCRIPTION_STRING	    esT("ESFWX Script Library")
#define COMPONENT_INTERNAL_NAME				    esT("esscript")
#define COMPONENT_ORIGINAL_NAME				    esT("esscript.dll")
#define COMPONENT_VERSION_STRING			    esT("1.24.3")

#ifdef _DEBUG
#	define COMPONENT_VERSION_STRING_LONG		COMPONENT_DESCRIPTION_STRING esT(" ") COMPONENT_VERSION_STRING esT(" DEBUG")
#else
#	define COMPONENT_VERSION_STRING_LONG		COMPONENT_DESCRIPTION_STRING esT(" ") COMPONENT_VERSION_STRING
#endif

#endif 
