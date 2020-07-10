#ifndef _es_variant_type_h_
#define _es_variant_type_h_

///  @file EsVariantType.h
///  Reflected varinat types enumeration
///

/// Reflected variant types enumeration
/// we have to explicitly cast values to the int, or empty variant will be created for EsVariant::VAR_EMPTY,
/// instead of actual '0' value, contained in variant
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsVariantType)
  ES_DECL_ENUMERATION_ITEM(VAR_EMPTY,             static_cast<ulong>(EsVariant::VAR_EMPTY))
  ES_DECL_ENUMERATION_ITEM(VAR_BOOL,              static_cast<ulong>(EsVariant::VAR_BOOL))
  ES_DECL_ENUMERATION_ITEM(VAR_BYTE,              static_cast<ulong>(EsVariant::VAR_BYTE))
  ES_DECL_ENUMERATION_ITEM(VAR_CHAR,              static_cast<ulong>(EsVariant::VAR_CHAR))
  ES_DECL_ENUMERATION_ITEM(VAR_UINT,              static_cast<ulong>(EsVariant::VAR_UINT))
  ES_DECL_ENUMERATION_ITEM(VAR_INT,               static_cast<ulong>(EsVariant::VAR_INT))
  ES_DECL_ENUMERATION_ITEM(VAR_UINT64,            static_cast<ulong>(EsVariant::VAR_UINT64))
  ES_DECL_ENUMERATION_ITEM(VAR_INT64,             static_cast<ulong>(EsVariant::VAR_INT64))
  ES_DECL_ENUMERATION_ITEM(VAR_DOUBLE,            static_cast<ulong>(EsVariant::VAR_DOUBLE))
  ES_DECL_ENUMERATION_ITEM(VAR_BIN_BUFFER,        static_cast<ulong>(EsVariant::VAR_BIN_BUFFER))
  ES_DECL_ENUMERATION_ITEM(VAR_STRING,            static_cast<ulong>(EsVariant::VAR_STRING))
  ES_DECL_ENUMERATION_ITEM(VAR_STRING_COLLECTION, static_cast<ulong>(EsVariant::VAR_STRING_COLLECTION))
  ES_DECL_ENUMERATION_ITEM(VAR_POINTER,           static_cast<ulong>(EsVariant::VAR_POINTER))
  ES_DECL_ENUMERATION_ITEM(VAR_OBJECT,            static_cast<ulong>(EsVariant::VAR_OBJECT))
  ES_DECL_ENUMERATION_ITEM(VAR_VARIANT_COLLECTION,static_cast<ulong>(EsVariant::VAR_VARIANT_COLLECTION))
  ES_DECL_ENUMERATION_ITEM(TypeInvalid,           static_cast<ulong>(EsVariant::TypeInvalid))
ES_DECL_ENUMERATION_END

#endif // _es_variant_type_h_
