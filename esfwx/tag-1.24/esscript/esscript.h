#ifndef _es_script_h_
#define _es_script_h_

#ifndef _es_core_h_
#  error "escore.h must be included prior to this header!"
#endif

// Script configuration
#include <EsScriptConfig.h>

/// Disable compiler implementation on ECC under MAC OS (iOS) and Android
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND && \
    ((ES_OS == ES_OS_MAC) || (ES_OS == ES_OS_ANDROID)) && \
    1 == ES_USE_COMPILER_IMPLEMENTATION
# undef ES_USE_COMPILER_IMPLEMENTATION
# undef ES_USE_SCRIPTLETS
# define ES_USE_COMPILER_IMPLEMENTATION 0
# define ES_USE_SCRIPTLETS 0
#endif

// Core export-import defines
#ifdef ES_USE_DLLS
#  ifdef ESSCRIPT_EXPORTS
#    define ESSCRIPT_CLASS              ES_EXPORT_SPEC
#    define ESSCRIPT_FUNC(type, decl)  ES_FUNCEXPORT_SPEC(type, decl)
#    define ESSCRIPT_DATA(type, decl)  ES_DATAEXPORT_SPEC(type, decl)
#  else
#    define ESSCRIPT_CLASS              ES_IMPORT_SPEC
#    define ESSCRIPT_FUNC(type, decl)  ES_FUNCIMPORT_SPEC(type, decl)
#    define ESSCRIPT_DATA(type, decl)  ES_DATAIMPORT_SPEC(type, decl)
#  endif
#else
#  define ESSCRIPT_CLASS
#  define ESSCRIPT_FUNC(type, decl)    type decl
#  define ESSCRIPT_DATA(type, decl)    type decl
#endif

// Standard includes
#include <vector>

// forward decls
class ES_ABSTRACT EsScriptValAccessorIntf;
class ES_ABSTRACT EsScriptObjectIntf;
class EsScriptCompiledBinaryWriter;
class EsScriptCompiledBinaryReader;
class EsScriptMachine;
class EsScriptlet;
class EsScript;

#include <esscript/EsScriptStdNames.h>
#include <esscript/EsScriptValAccessorIntf.h>
#include <esscript/EsScriptSymbolTable.h>
#include <esscript/EsScriptInstruction.h>
#include <esscript/EsScriptDataStack.h>
#include <esscript/EsScriptException.h>
#include <esscript/EsScriptCodeSection.h>
#include <esscript/EsScriptThreadContext.h>
#include <esscript/EsScriptObjectDataBuffer.h>
#include <esscript/EsScriptObjectPropertyInfo.h>
#include <esscript/EsScriptObjectIntf.h>
#include <esscript/EsScriptEnumeration.h>
#include <esscript/EsScriptMetaclass.h>
#include <esscript/EsScriptMetainfoLookup.h>
#include <esscript/EsScriptHost.h>
#include <esscript/EsScriptMachine.h>
#include <esscript/EsScriptObject.h>
#include <esscript/EsScriptPodObjects.h>
#include <esscript/EsScriptArrayObject.h>
#include <esscript/EsScriptIfObject.h>
#include <esscript/EsScriptlet.h>
#include <esscript/EsScriptMain.h>
#include <esscript/EsScriptDebug.h>
#include <esscript/EsExceptionInfo.h>
#include <esscript/EsScriptCompiledBinary.h>
#include <esscript/EsEventHandler.h>
#include <esscript/EsThreadWorker.h>
#include <esscript/EsScriptParser.h>

ES_DECL_REFLECTION_REGISTRAR(ESSCRIPT_FUNC, esscript)

#endif // _es_script_h_
