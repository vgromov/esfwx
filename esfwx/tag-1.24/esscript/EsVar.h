#ifndef _es_variant_services_h_
#define _es_variant_services_h_

// variant reflected services
//
ES_DECL_REFLECTED_SERVICES_BEGIN( EsVar )

  /// Typed constructor (s)
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, as, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, as, ulong, ulong);
  /// Generic services
  ES_DECL_REFLECTED_CLASS_METHOD1(ulong, typeGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(ulong, countGet, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, itemGet, cr_EsVariant, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD3(void, itemSet, cr_EsVariant, ulong, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, itemDelete, cr_EsVariant, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, setToNull, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, value, cr_EsVariant);  
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, asString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsString, asEscapedString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsBinBuffer, asBinBuffer, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, asBool, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(esU8, asByte, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(EsVariant, asChar, cr_EsVariant);  
  ES_DECL_REFLECTED_CLASS_METHOD1(long, asLong, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(ulong, asULong, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(llong, asLLong, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(ullong, asULLong, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(double, asDouble, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isIndexed, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isCollection, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isObject, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isNumeric, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isEmpty, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(bool, isString, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(bool, isKindOf, cr_EsVariant, cr_EsString);
  /// Collection manipulation
  ES_DECL_REFLECTED_CLASS_METHOD2(void, countSet, cr_EsVariant, ulong);
  ES_DECL_REFLECTED_CLASS_METHOD3(EsVariant, sliceGet, cr_EsVariant, long, long);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, find, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, findFirstOf, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(EsVariant, findLastOf, cr_EsVariant, cr_EsVariant);
  /// Replace part of sequence, with another one, startimg from specified offset
  ES_DECL_REFLECTED_CLASS_METHOD3(void, replace, cr_EsVariant, cr_EsVariant, ulong);
  /// Append an element to the end of the sequence. Behaves the same way as += operator
  /// but for variant collections, in which case it does not concat sequences, but
  /// inserts right sequence as an element of left one
  ES_DECL_REFLECTED_CLASS_METHOD2(void, append, cr_EsVariant, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD2(void, prepend, cr_EsVariant, cr_EsVariant);
  /// Append unique element to the end of collection
  ES_DECL_REFLECTED_CLASS_METHOD2(void, appendUnique, cr_EsVariant, cr_EsVariant);
  /// Sort elements collection
  ES_DECL_REFLECTED_CLASS_METHOD1(void, sortAscending, cr_EsVariant);
  ES_DECL_REFLECTED_CLASS_METHOD1(void, sortDescending, cr_EsVariant);
  /// Return collection whose elements are in reverse order
  ES_DECL_REFLECTED_CLASS_METHOD1(void, reverse, cr_EsVariant);

ES_DECL_REFLECTED_SERVICES_END

#endif // _es_variant_services_h_