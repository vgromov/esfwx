#ifndef _ekosf_script_h_
#define _ekosf_script_h_

#ifndef _ekosf_core_h_
#	error "ekosf-core.h must be included prior to this header!"
#endif

// communication stuff
//
// core export-import defines
#ifdef EKOSF_USEDLLS
#	ifdef ESSCRIPT_EXPORTS
#		define EKOSF_SCRIPT_CLASS	__declspec(dllexport)
#		define EKOSF_SCRIPT_FUNC	__declspec(dllexport)
#		define EKOSF_SCRIPT_DATA	__declspec(dllexport)
#	else
#		define EKOSF_SCRIPT_CLASS	__declspec(dllimport)
#		define EKOSF_SCRIPT_FUNC	__declspec(dllimport)
#		define EKOSF_SCRIPT_DATA	__declspec(dllimport)
#	endif
#else
#	define EKOSF_SCRIPT_CLASS
#	define EKOSF_SCRIPT_FUNC
#	define EKOSF_SCRIPT_DATA
#endif

// standard includes
#include <stack>

// boost stuff
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/bimap.hpp>

// forward decls
class EsAccessorIntf;
class EsSymtable;
class EsInstruction;
class EsScriptObjectIntf;
class EsScriptMachine;
class EsScript;

#include <esscript/EsValAccessors.h>
#include <esscript/EsSymtable.h>
#include <esscript/EsInstruction.h>
#include <esscript/EsCodeSection.h>
#include <esscript/EsScriptMachine.h>
#include <esscript/EsScriptObject.h>
#include <esscript/EsScriptPodObjects.h>
#include <esscript/EsScriptArrayObject.h>
#include <esscript/EsScriptDebug.h>
#include <esscript/EsScript.h>

ES_DECL_REFLECTION_REGISTRAR(esscript)

#endif // _ekosf_script_h_
