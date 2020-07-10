#ifndef _es_mdi_h_
#define _es_mdi_h_

#ifndef _es_core_h_
#  error "escore.h must be included prior to this header!"
#endif

// math export-import defines
#ifdef ES_DYNAMIC_LINKAGE
#  ifdef ESMDI_EXPORTS
#    define ESMDI_CLASS              ES_EXPORT_SPEC
#    define ESMDI_FUNC(type, decl)   ES_FUNCEXPORT_SPEC(type, decl)
#    define ESMDI_DATA(type, decl)   ES_DATAEXPORT_SPEC(type, decl)
#    define ESMDI_PKG                PACKAGE
#  else
#    define ESMDI_CLASS              ES_IMPORT_SPEC
#    define ESMDI_FUNC(type, decl)   ES_FUNCIMPORT_SPEC(type, decl)
#    define ESMDI_DATA(type, decl)   ES_DATAIMPORT_SPEC(type, decl)
#    define ESMDI_PKG                PACKAGE
#  endif
#else
#  define ESMDI_CLASS
#  define ESMDI_FUNC(type, decl)     type decl
#  define ESMDI_DATA(type, decl)     type decl
#  define ESMDI_PKG                  PACKAGE
#endif

/// Document manager notification events declarations
///
#define ES_EVTC_MDI esT("es_mdi")

enum class EsMdiEvt : ulong
{
  DocumentCreated,              ///< Event Payload: key
  DocumentOpened,               ///< Event Payload: [key, <parameters associative container (optional, null, if not set)>]
  DocumentChanged,              ///< Event Payload: [key, <property name, or emtpy string for unspecified change>, <optional sender object pointer, to prevent unneeded processing>]
  DocumentKeyChanged,           ///< Event Payload: [old key, new key]
  DocumentStateChanged,         ///< Event Payload: key
  DocumentSideloadChanged,      ///< Event Payload: [key, <sideload property name, or empty string for unspecified sideload>, <optional sender object pointer, to prevent unneeded processing>]
  DocumentClosed,               ///< Event Payload: [key, document type]
  AllDocumentsClosed,           ///< Event Payload: none
  DocumentsCloseRequest         ///< Request to close one or more documents. Event Payload: [key0, ..., keyN]
};

#include <esmdi/EsDocumentManagerIntf.h>
#include <esmdi/EsDocument.h>
#include <esmdi/EsDocumentManager.h>

ES_DECL_REFLECTION_REGISTRAR(ESMDI_FUNC, esmdi)

#endif // _es_mdi_h_
