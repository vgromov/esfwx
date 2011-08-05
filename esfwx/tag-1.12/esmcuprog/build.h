#ifndef _build_h_
#define _build_h_

#include <../version.h>

// increment this value when publishing new changes in release
#define COMPONENT_BUILD_NO						43

#ifdef _DEBUG
#	define COMPONENT_VERSION_STRING			wxSTRINGIZE_T(PRODUCT_VER_MAJOR) wxT(".") wxSTRINGIZE_T(PRODUCT_VER_MINOR) wxT(".") wxSTRINGIZE_T(COMPONENT_BUILD_NO) wxT(" DEBUG")
#else
#	define COMPONENT_VERSION_STRING			wxSTRINGIZE_T(PRODUCT_VER_MAJOR) wxT(".") wxSTRINGIZE_T(PRODUCT_VER_MINOR) wxT(".") wxSTRINGIZE_T(COMPONENT_BUILD_NO)
#endif 

#define COMPONENT_FILE_VERSION				PRODUCT_VER_MAJOR, PRODUCT_VER_MINOR, COMPONENT_BUILD_NO, 0
#define COMPONENT_FILE_VERSION_STRING	wxSTRINGIZE_T(PRODUCT_VER_MAJOR) wxT(", ") wxSTRINGIZE_T(PRODUCT_VER_MINOR) wxT(", ") wxSTRINGIZE_T(COMPONENT_BUILD_NO) wxT(", 0")
#define COMPONENT_DESCRIPTION_STRING	wxT("MCU Programmer Library")
#define COMPONENT_INTERNAL_NAME				wxT("esmcuprog")
#define COMPONENT_ORIGINAL_NAME				wxT("esmcuprog.dll")

#endif 
