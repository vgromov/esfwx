#ifndef _ekosf_core_h_
#define _ekosf_core_h_

// consolidated include for escore defines and types
//
#if defined(DEBUG) || defined(_DEBUG)
#	ifndef _DEBUG
#		define _DEBUG
#	endif
#	ifndef DEBUG
#		define DEBUG
#	endif
#	undef NDEBUG 
#endif

// use advanced memleak detection
#if defined(_DEBUG) && !defined(USE_MEMLEAK_DETECTION)
#	define USE_MEMLEAK_DETECTION 1
#endif

// memory leak detection
#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION)
# define _CRTDBG_MAP_ALLOC
# include <stdlib.h>
# include <crtdbg.h>
#endif

// STL stuff
#include <limits>
#include <vector>
#include <list>
#include <map>

// TST containers stuff
#include <escore/tst/structured_map.hpp>
#include <escore/tst/structured_set.hpp>
#include <escore/tst/iterator_wrapper.hpp>

// wxwidgets stuff
#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION)
#	include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif
#if defined(_DEBUG) && (1 == USE_MEMLEAK_DETECTION) && ( !defined(_INC_CRTDBG) || !defined(_CRTDBG_MAP_ALLOC) )
#	error Debug CRT functions have not been included!
#endif
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/treectrl.h>
#include <wx/aui/aui.h>
#include <wx/fileconf.h>
#include <wx/dynlib.h>
#include <wx/wupdlock.h>
#include <wx/regex.h>
#include <wx/socket.h>
#include <wx/xml/xml.h>
#include <wx/html/htmprint.h>
#include <wx/help.h>
#include <wx/cshelp.h>

// escore export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef CORE_EXPORTS
#		define EKOSF_CORE_CLASS	__declspec(dllexport)
#		define EKOSF_CORE_FUNC	__declspec(dllexport)
#		define EKOSF_CORE_DATA	__declspec(dllexport)
#	else
#		define EKOSF_CORE_CLASS	__declspec(dllimport)
#		define EKOSF_CORE_FUNC	__declspec(dllimport)
#		define EKOSF_CORE_DATA	__declspec(dllimport)
#	endif
#else
#	define EKOSF_CORE_CLASS
#	define EKOSF_CORE_FUNC
#	define EKOSF_CORE_DATA
#endif

#define ES_ABSTRACT __declspec(novtable)
#define ES_NOTHROW	throw()

// configuration groups & keys definition macros
//
#define DEF_CFG_SECTION( Name ) static const wxChar* CFG_ ## Name ## _GROUP = wxT( "/") wxT( #Name )
#define DEF_CFG_KEY( Name ) static const wxChar* CFG_ ## Name ## _KEY = wxT( #Name )

#if defined(_UNICODE) || defined(UNICODE) 
#	define ES_UNICODE
#endif

#include "../version.h"

// escore forward declarations
class EsBaseIntf;
class EsReflectedClassIntf;
class PluginModuleIntf;
class PluginIntf;
class EsAppIntf;
class EsBinBuffer;
class EsString;
class EsVariant;
class EsEvent;
class EsException; 
class EsShortHwId;
class EsThread;
class DataFileParser;

// cross-platform includes
#include <common/type.h>
#include <common/hwIdBase.h>
#include <common/datetime.h>

// menu items list
typedef std::vector<wxMenuItem*> MenuItemsT;
typedef std::map<wxWindowID, wxAuiToolBar*> ToolBarsT;

// disable C4584 warning which occurs in many places due to our multiple interface inheritance technique
#pragma warning( disable: 4584 )

// escore includes
#include <escore/EsBinBuffer.h>
#include <escore/EsString.h>
#include <escore/EsBaseIntf.h>
#include <escore/EsVariant.h>
#include <escore/EsEvent.h>
#include <escore/Reflection.h>
#include <escore/ProgressMonitorIntf.h>
#include <escore/EsAppIntf.h>
#include <escore/PluginIntf.h>
#include <escore/EsUtilities.h>
#include <escore/EsCRC.h>
#include <escore/des.h>
#include <escore/EsDictionary.h>
#include <escore/EsCompositeFile.h>
#include <escore/Devices.h>
#include <escore/EsExceptions.h>
#include <escore/HwIdWrappers.h>
#include <escore/EsPowerStatus.h>
#include <escore/EsGui.h>
#include <escore/EsDateTime.h>
#include <escore/EsThread.h>
#include <escore/EsValidators.h>
#include <escore/DataFileUtils.h>
#include <escore/DataFileParser.h>

ES_DECL_REFLECTION_REGISTRAR(escore)

#endif _ekosf_core_h_
