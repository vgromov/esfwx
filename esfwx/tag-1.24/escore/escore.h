#ifndef _es_core_h_
#define _es_core_h_

// Configuration defines
#include <EsCoreConfig.h>

// Global defines
#include <escore/EsCoreDefs.h>

// escore export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#  ifdef ESCORE_EXPORTS
#    define ESCORE_CLASS      ES_EXPORT_SPEC
#   define ESCORE_FUNC      ES_FUNCEXPORT_SPEC
#    define ESCORE_DATA      ES_DATAEXPORT_SPEC
#  else
#    define ESCORE_CLASS      ES_IMPORT_SPEC
#    define ESCORE_FUNC      ES_FUNCIMPORT_SPEC
#    define ESCORE_DATA      ES_DATAIMPORT_SPEC
#  endif
#else
#  define ESCORE_CLASS
#  define ESCORE_FUNC(type, decl) type decl
#  define ESCORE_DATA(type, decl) type decl
#endif

// STL stuff
#include <limits>
#include <memory>
#include <cstring>
#include <locale>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#ifdef ES_MODERN_CPP
#include <functional>
#endif

// core typedefs
#include <escore/EsCoreTypes.h>
#include <esfwxe/type.h>

// custom RTL
#include <escore/EsCoreRtl.h>

// escore includes
#include "../version.h"
#include <escore/EsAtomic.h>
#include <escore/EsStringStd.h>
#include <escore/EsSyncObjects.h>
#include <escore/EsNearestGreater2n.h>
#include <escore/EsVector.h>
#include <escore/EsBinBuffer.h>
#include <escore/EsZip.h>
#include <escore/EsHash.h>
#include <escore/EsStdNames.h>
#include <escore/EsString.h>
#include <escore/EsLocale.h>
#include <escore/EsBaseIntf.h>
#include <escore/EsBreakIntf.h>
#include <escore/EsVariant.h>
#include <escore/EsStringIndexedMap.h>
#include <escore/EsStringConverter.h>
#include <escore/EsReflection.h>
#include <escore/EsStringI18n.h>
#include <escore/EsLogIntf.h>
#include <escore/EsScriptDebugInfo.h>
#include <escore/EsExceptions.h>
#include <escore/EsMetaclass.h>
#include <escore/EsEnumeration.h>
#include <escore/EsVariantType.h>
#include <escore/EsPath.h>
#include <escore/EsPathEnumerator.h>
#include <escore/EsDateTime.h>
#include <escore/EsEvent.h>
#include <escore/EsEventDispatcher.h>
#include <escore/EsRange.h>
#include <escore/EsFile.h>
#include <escore/EsDynamicLibrary.h>
#include <escore/EsStreamBlock.h>
#include <escore/EsStream.h>
#include <escore/EsStreamBinary.h>
#include <escore/EsXml.h>
#include <escore/EsStreamXml.h>
#include <escore/EsProgressMonitor.h>
#include <escore/EsUtilities.h>
#include <escore/EsUUID.h>
#include <escore/EsCrc.h>
#include <escore/EsCryptoHash.h>
#include <escore/EsCryptoDes.h>
#include <escore/EsCryptoAes.h>
#include <escore/EsCompositeFile.h>
#include <escore/EsThread.h>
#include <escore/EsAssocContainer.h>
#include <escore/EsStringTokenizer.h>
#include <escore/EsRegEx.h>
#include <escore/EsOsInfo.h>

ES_DECL_REFLECTION_REGISTRAR(ESCORE_FUNC, escore)

#endif // _es_core_h_
