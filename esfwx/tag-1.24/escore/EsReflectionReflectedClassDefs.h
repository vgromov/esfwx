#ifndef _reflection_reflected_class_defs_h_
#define _reflection_reflected_class_defs_h_

// reflected class declaration macro
//
#define ES_THISBASE_CAST  asBaseIntf()

// declares this class the first one, implementing EsReflectedClassIntf in
// the inheritance chain. if not, use ES_DECL_REFLECTED_CLASS instead
#define ES_DECL_REFLECTED_CLASS_BASE_CUSTOM( ClassName ) \
public: \
  typedef ClassName ThisClassT; \
  static const EsClassInfo& classInfoGetStatic() ES_NOTHROW; \
  ES_DECL_INTF_METHOD(const EsClassInfo&, classInfoGet)() const ES_NOTHROW ES_OVERRIDE { return ClassName ::classInfoGetStatic(); } \
  ES_DECL_INTF_METHOD(EsMetaclassIntf::Ptr, metaGet)() const ES_NOTHROW ES_OVERRIDE { return EsMetaclass::instance(); } \
  ES_DECL_INTF_METHOD(EsString, typeNameGet)() const ES_NOTHROW ES_OVERRIDE { return classInfoGet().nameGet(); } \
  ES_DECL_INTF_METHOD(bool, isKindOf)(const EsReflectedClassIntf::Ptr& other) const ES_NOTHROW ES_OVERRIDE \
  { if( other ) return classInfoGet().isKindOf( other->classInfoGet() ); return false; } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, isKindOf, cr_EsString) ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().isKindOf(p1); } \
  ES_DECL_INTF_METHOD(bool, isKindOf)(const EsClassInfo& other) const ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().isKindOf(other); } \
  ES_DECL_REFLECTED_INTF_METHOD1(void, copyFrom, cr_EsBaseIntfPtr) ES_OVERRIDE \
  { EsReflectedClassIntf::Ptr obj = p1; ES_ASSERT(obj); \
    if( isKindOf(obj) ) { const EsStringArray& propNames = persistentPropertyNamesGet(); \
    for(size_t idx = 0; idx < propNames.size(); ++idx) { \
      const EsString& prop = propNames[idx]; \
      propertySet(prop, obj->propertyGet(prop)); } \
    } else EsException::Throw(esT("Could not copy from object of different type")); \
  } \
   ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsBaseIntfPtr, asWeakReference) ES_NOTHROW ES_OVERRIDE \
  { return const_cast<ThisClassT*>(this)->asBaseIntfPtrDirectWeak(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isIndexed) ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().isIndexed(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD0(bool, isScripted) ES_NOTHROW ES_OVERRIDE \
  { return false; } \
  ES_DECL_INTF_METHOD(EsString::Array, propertyNamesGet)() const ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().propertyNamesGet(); } \
  ES_DECL_INTF_METHOD(EsString::Array, persistentPropertyNamesGet)() const ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().persistentPropertyNamesGet(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasProperty, cr_EsString) ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().hasProperty(p1); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, propertyCanRead, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    return info.canRead(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, propertyCanWrite, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    return info.canWrite(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, propertyGet, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    return info.get( ES_THISBASE_CAST ); } \
  ES_DECL_REFLECTED_INTF_METHOD2(void, propertySet, cr_EsString, cr_EsVariant) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    return info.set( ES_THISBASE_CAST, p2); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsStringArray, propertyAttributeNamesGet, cr_EsString) ES_OVERRIDE \
  { EsStringArray result; const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    EsAttributesIntf::Ptr attrs = info.attributesAccess(); if( attrs ) { result = attrs->allNamesGet(); } \
    return result; } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD2(bool, propertyHasAttribute, cr_EsString, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    EsAttributesIntf::Ptr attrs = info.attributesAccess(); if( attrs ) return attrs->attributeExists(p2); \
    return false; } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, propertyAttributeGet, cr_EsString, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    EsAttributesIntf::Ptr attrs = info.attributesAccess(); if( attrs ) return attrs->attributeGet(p2); \
    EsException::ThrowAttributeDoesNotExist(p2, p1); return EsVariant::null(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, propertyLabelGet, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    return info.labelGet(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsString, propertyDescriptionGet, cr_EsString) ES_OVERRIDE \
  { const EsPropertyInfo& info = classInfoGet().propertyInfoGet(p1); \
    return info.descriptionGet(); } \
  ES_DECL_INTF_METHOD(bool, hasMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().hasMethod(key); } \
  ES_DECL_INTF_METHOD(bool, hasClassMethod)(const EsMethodInfoKeyT& key) const ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().hasClassMethod(key); } \
  ES_DECL_PROPERTY_RO(type, EsString) \
  ES_DECL_PROPERTY_RO(attributeNames, EsStringArray) \
  ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, attribute, cr_EsString); \
  ES_DECL_PROPERTY_RO(propertyNames, EsStringArray) \
  ES_DECL_PROPERTY_RO(persistentPropertyNames, EsStringArray) \
  ES_DECL_PROPERTY_RO(weakReference, EsVariant) \
  ES_DECL_REFLECTED_CONST_METHOD2(bool, hasMethod, cr_EsString, ulong); \
  ES_DECL_REFLECTED_CONST_METHOD2(bool, hasClassMethod, cr_EsString, ulong);

#define ES_DECL_REFLECTED_CLASS_BASE( ClassName ) \
  ES_DECL_REFLECTED_CLASS_BASE_CUSTOM( ClassName ) \
  ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, callClassMethod, cr_EsString, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, callMethod, cr_EsString, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD1(EsVariant, call, cr_EsString); \
  ES_DECL_REFLECTED_INTF_METHOD2(EsVariant, call, cr_EsString, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD3(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD4(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD5(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD6(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_METHOD7(EsVariant, call, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, classCall, cr_EsString); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD2(EsVariant, classCall, cr_EsString, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD3(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD4(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD5(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD6(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant); \
  ES_DECL_REFLECTED_INTF_CONST_METHOD7(EsVariant, classCall, cr_EsString, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant, cr_EsVariant);

#define ES_DECL_ATTR_HANDLING_STD \
  ES_DECL_INTF_METHOD(void, attributesAssign)(const EsAttributesIntf::Ptr& attrs) ES_OVERRIDE \
  { const_cast<EsClassInfo&>(classInfoGet()).attributesAssign(attrs); } \
  ES_DECL_INTF_METHOD(void, attributeAdd)(const EsString& name, const EsVariant& val = EsVariant::null()) ES_OVERRIDE \
  { const_cast<EsClassInfo&>(classInfoGet()).attributeAdd(name, val); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(bool, hasAttribute, cr_EsString) ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().attributesAccess()->attributeExists(p1); } \
  ES_DECL_INTF_METHOD(EsString::Array, attributeNamesGet)() const ES_NOTHROW ES_OVERRIDE \
  { return classInfoGet().attributesAccess()->allNamesGet(); } \
  ES_DECL_REFLECTED_INTF_CONST_METHOD1(EsVariant, attributeGet, cr_EsString) ES_OVERRIDE \
  { EsAttributesIntf::Ptr attrs = classInfoGet().attributesAccess(); \
    if( !attrs->attributeExists(p1) ) EsException::ThrowAttributeDoesNotExist(p1, typeNameGet()); \
    return attrs->attributeGet(p1); }

struct EsClassAttrInitializer
{
  inline EsClassAttrInitializer(EsClassInfo& ci, const EsString& name, const EsVariant& val = EsVariant::null()) ES_NOTHROW
  {
    ci.attributeAdd(name, val);
  }

private:
  EsClassAttrInitializer() ES_REMOVEDECL;
  EsClassAttrInitializer( const EsClassAttrInitializer& ) ES_REMOVEDECL;
  EsClassAttrInitializer& operator=( const EsClassAttrInitializer& ) ES_REMOVEDECL;
};

#define ES_DECL_REFLECTED_CLASS_ATTR( Attr ) \
  static EsClassAttrInitializer s_##Attr( s_class, ES_STRINGIZE(Attr), EsVariant::null() );

#define ES_DECL_REFLECTED_ATTR_VAL( Attr, AttrVal ) \
  static EsClassAttrInitializer s_##Attr( s_class, ES_STRINGIZE(Attr), (AttrVal) );

// declare only neccessary part of EsReflectedClassIntf, use in
// classes which inherit their functionality from
// base(s) already implementing EsReflectedClassIntf using ES_DECL_REFLECTED_CLASS_BASE
#define ES_DECL_REFLECTED_CLASS( ClassName, BaseName ) \
public: \
  typedef ClassName ThisClassT; \
  static const EsClassInfo& classInfoGetStatic() ES_NOTHROW; \
  ES_DECL_INTF_METHOD(const EsClassInfo&, classInfoGet)() const ES_NOTHROW ES_OVERRIDE { return ClassName ::classInfoGetStatic(); }

// Reflected namespace service definition
//

// Declares reflected namespace service, as pure static class which may not be created
// only class reflected methods are allowed in service definition
#define ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN( NamespaceName, ExportSpec ) \
class ExportSpec NamespaceName { \
private: \
  NamespaceName() ES_REMOVEDECL; \
  NamespaceName(const NamespaceName&) ES_REMOVEDECL; \
  NamespaceName& operator= (const NamespaceName&) ES_REMOVEDECL; \
public: \
  static const EsClassInfo& classInfoGetStatic() ES_NOTHROW;

// Allow to use existing class as reflected service
// only class reflected methods are allowed in service definition
#define ES_USE_AS_EXISTING_REFLECTED_CLASS_SERVICE \
public: \
  static const EsClassInfo& classInfoGetStatic() ES_NOTHROW;

#define ES_NO_EXPORT_SPEC

#define ES_DECL_REFLECTED_SERVICES_BEGIN( NamespaceName ) \
  ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN( NamespaceName, ES_NO_EXPORT_SPEC )

#define ES_DECL_REFLECTED_SERVICES_END };

// reflected class info implementation macros
//
#define NO_CLASS_DESCR    EsString::null()

// class info opening clause
#define ES_DECL_CLASS_INFO_BEGIN( ClassName, Descr ) \
  const EsClassInfo& ClassName ::classInfoGetStatic() ES_NOTHROW { \
    static EsClassInfo s_class( ES_STRINGIZE( ClassName ), Descr, nullptr );

#define  ES_DECL_CLASS_INFO_DERIVED_BEGIN( ClassName, BaseClass, Descr ) \
  const EsClassInfo& ClassName ::classInfoGetStatic() ES_NOTHROW { \
    static EsClassInfo s_class( ES_STRINGIZE( ClassName ), Descr, (& BaseClass ::classInfoGetStatic()) );

// use in case reflected interface implementor is nested class on another class
#define ES_DECL_NESTED_CLASS_INFO_BEGIN( ClassName, ReflectedClassName, Descr ) \
  const EsClassInfo& ClassName ::classInfoGetStatic() ES_NOTHROW { \
    static EsClassInfo s_class( ES_STRINGIZE( ReflectedClassName ), Descr, nullptr );

#define ES_DECL_NESTED_CLASS_INFO_DERIVED_BEGIN( ClassName, ReflectedClassName, BaseClass, Descr ) \
  const EsClassInfo& ClassName ::classInfoGetStatic() ES_NOTHROW { \
    static EsClassInfo s_class( ES_STRINGIZE( ReflectedClassName ), Descr, (& BaseClass ::classInfoGetStatic()) );

#define ES_IMPL_REFLECTED_PROPERTIES(ClassName) \
  EsString ClassName ::get_type() const { return typeNameGet(); } \
  EsStringArray ClassName ::get_attributeNames() const { return attributeNamesGet(); } \
  EsStringArray ClassName ::get_propertyNames() const { return propertyNamesGet(); } \
  EsStringArray ClassName ::get_persistentPropertyNames() const { return persistentPropertyNamesGet(); } \
  EsVariant ClassName ::get_weakReference() const { return asWeakReference(); }

#define ES_IMPL_REFLECTED_CALLS_CUSTOM(ClassName) \
  bool ClassName ::hasMethod(const EsString& name, ulong paramsCnt) const { return hasMethod( EsMethodInfoKeyT(paramsCnt, name) ); } \
  bool ClassName ::hasClassMethod(const EsString& name, ulong paramsCnt) const { return hasClassMethod( EsMethodInfoKeyT(paramsCnt, name) ); }

#define ES_IMPL_REFLECTED_CALLS(ClassName) \
  ES_IMPL_REFLECTED_CALLS_CUSTOM(ClassName) \
  EsVariant ClassName ::callClassMethod(const EsString& name, const EsVariant& params) const { return classInfoGet().classCallMethod(name, params); } \
  EsVariant ClassName ::callMethod(const EsString& name, const EsVariant& params) { return classInfoGet().callMethod(ES_THISBASE_CAST, name, params); } \
  EsVariant ClassName ::call(const EsString& name) { return classInfoGet().call(ES_THISBASE_CAST, name); } \
  EsVariant ClassName ::call(const EsString& name, const EsVariant& param1) { return classInfoGet().call(ES_THISBASE_CAST, name, param1); } \
  EsVariant ClassName ::call(const EsString& name, const EsVariant& param1, const EsVariant& param2) { return classInfoGet().call(ES_THISBASE_CAST, name, param1, param2); } \
  EsVariant ClassName ::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) { return classInfoGet().call(ES_THISBASE_CAST, name, param1, param2, param3); } \
  EsVariant ClassName ::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) { return classInfoGet().call(ES_THISBASE_CAST, name, param1, param2, param3, param4); } \
  EsVariant ClassName ::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) { return classInfoGet().call(ES_THISBASE_CAST, name, param1, param2, param3, param4, param5); } \
  EsVariant ClassName ::call(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) { return classInfoGet().call(ES_THISBASE_CAST, name, param1, param2, param3, param4, param5, param6); } \
  EsVariant ClassName ::classCall(const EsString& name) const { return classInfoGet().classCall(name); } \
  EsVariant ClassName ::classCall(const EsString& name, const EsVariant& param1) const { return classInfoGet().classCall(name, param1); } \
  EsVariant ClassName ::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2) const { return classInfoGet().classCall(name, param1, param2); } \
  EsVariant ClassName ::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const { return classInfoGet().classCall(name, param1, param2, param3); } \
  EsVariant ClassName ::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const { return classInfoGet().classCall(name, param1, param2, param3, param4); } \
  EsVariant ClassName ::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const { return classInfoGet().classCall(name, param1, param2, param3, param4, param5); } \
  EsVariant ClassName ::classCall(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5, const EsVariant& param6) const { return classInfoGet().classCall(name, param1, param2, param3, param4, param5, param6); }

#define ES_DECL_DEFAULT_REFLECTED_METHOD_INFOS(ClassName) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, isKindOf, bool_CallConst_cr_EsString, esT("Return true if objects are of the same kinds")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, copyFrom, void_Call_cr_EsBaseIntfPtr, esT("Copy object from another object of the same type")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, asWeakReference, EsBaseIntfPtr_CallConst, esT("Return weak reference to the object itself")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, isIndexed, bool_CallConst, esT("Check if object supports indexing")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, isScripted, bool_CallConst, esT("Check if object is implemented in script")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, hasProperty, bool_CallConst_cr_EsString, esT("Check property existence")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyCanRead, bool_CallConst_cr_EsString, esT("Check property is readable")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyCanWrite, bool_CallConst_cr_EsString, esT("Check property is writeable")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyGet, EsVariant_CallConst_cr_EsString, esT("Get property value")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertySet, void_Call_cr_EsString_cr_EsVariant, esT("Set property value")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyAttributeNamesGet, EsStringArray_CallConst_cr_EsString, esT("Acquire property attribute names")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyHasAttribute, bool_CallConst_cr_EsString_cr_EsString, esT("Check if attribute declared for property")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyAttributeGet, EsVariant_CallConst_cr_EsString_cr_EsString, esT("Get property attribute value")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyLabelGet, EsString_CallConst_cr_EsString, esT("Get property label")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, propertyDescriptionGet, EsString_CallConst_cr_EsString, esT("Get property description")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, hasAttribute, bool_CallConst_cr_EsString, esT("Return true if attribute with this name exists, false otherwise")) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, attributeGet, EsVariant_CallConst_cr_EsString, esT("Return attribute by name. Throw Exception if attribute could not found")) \
  ES_DECL_PROP_INFO_RO(ClassName, type, EsString, esT("Type name"), NO_PROPERTY_DESCR) \
  ES_DECL_PROP_INFO_RO(ClassName, attributeNames, EsStringArray, esT("Object attribute names"), NO_PROPERTY_DESCR) \
  ES_DECL_PROP_INFO_RO(ClassName, propertyNames, EsStringArray, esT("Object property names"), NO_PROPERTY_DESCR) \
  ES_DECL_PROP_INFO_RO(ClassName, persistentPropertyNames, EsStringArray, esT("Object persistent property names"), NO_PROPERTY_DESCR) \
  ES_DECL_PROP_INFO_RO(ClassName, weakReference, EsVariant, esT("Weak reference to the object itself"), NO_PROPERTY_DESCR) \
  ES_DECL_REFLECTED_METHOD_INFO_STD(ClassName, hasMethod, bool_CallConst_cr_EsString_ulong, esT("Check if reflected method declared"))  \
  ES_DECL_REFLECTED_METHOD_INFO_STD(ClassName, hasClassMethod, bool_CallConst_cr_EsString_ulong, esT("Check if reflected class method declared"))  \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, callMethod, EsVariant_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, call, EsVariant_Call_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, callClassMethod, EsVariant_CallConst_cr_EsString_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR) \
  ES_DECL_REFLECTED_INTF_METHOD_INFO(ClassName, EsReflectedClassIntf, classCall, EsVariant_CallConst_cr_EsString_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant_cr_EsVariant, NO_METHOD_DESCR)

// base class info opening clause
#define ES_DECL_BASE_CLASS_INFO_BEGIN( ClassName, Descr ) \
  ES_IMPL_REFLECTED_PROPERTIES(ClassName) \
  ES_IMPL_REFLECTED_CALLS(ClassName) \
  ES_DECL_CLASS_INFO_BEGIN( ClassName, Descr ) \
  ES_DECL_DEFAULT_REFLECTED_METHOD_INFOS(ClassName)

#define ES_DECL_BASE_CLASS_INFO_CUSTOM_BEGIN( ClassName, Descr ) \
  ES_IMPL_REFLECTED_PROPERTIES(ClassName) \
  ES_IMPL_REFLECTED_CALLS_CUSTOM(ClassName) \
  ES_DECL_CLASS_INFO_BEGIN( ClassName, Descr ) \
  ES_DECL_DEFAULT_REFLECTED_METHOD_INFOS(ClassName)

// nested base class info opening clause
#define ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN( ClassName, ReflectedClassName, Descr ) \
  ES_IMPL_REFLECTED_PROPERTIES(ClassName) \
  ES_IMPL_REFLECTED_CALLS(ClassName) \
  ES_DECL_NESTED_CLASS_INFO_BEGIN( ClassName, ReflectedClassName, Descr ) \
  ES_DECL_DEFAULT_REFLECTED_METHOD_INFOS(ClassName)

#define ES_DECL_CLASS_INFO_END \
  return s_class; }

#define ES_DECL_REFLECTED_SERVICES_INFO_BEGIN( NamespaceName, Descr ) \
  ES_DECL_CLASS_INFO_BEGIN( NamespaceName, Descr )

#define ES_DECL_REFLECTED_SERVICES_INFO_END    ES_DECL_CLASS_INFO_END

#endif // _reflection_reflected_class_defs_h_
