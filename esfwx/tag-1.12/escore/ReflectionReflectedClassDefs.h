#ifndef _reflection_reflected_class_defs_h_
#define _reflection_reflected_class_defs_h_

// reflected class declaration macro
// 
// declares this class the first one, implementing EsReflectedClassIntf in 
// the inheritance chain. if not, use ES_DECL_REFLECTED_CLASS instead
#define ES_DECL_REFLECTED_CLASS_BASE( ClassName ) \
public: \
	typedef ClassName ThisClass; \
	static const ClassInfo& getClassInfoStatic(); \
	ES_DECL_INTF_METHOD(const ClassInfo&, getClassInfo)() const { return ClassName ##::getClassInfoStatic(); } \
	ES_DECL_INTF_METHOD(EsString, getTypeName)() const { return ClassName ##::getClassInfoStatic().getName(); } \
	ES_DECL_INTF_METHOD(bool, isKindOf)(const EsReflectedClassIntf* other) const \
	{ if( other ) return ClassName ##::getClassInfoStatic().isKindOf( other->getClassInfo() ); return false; } \
	ES_DECL_INTF_METHOD(bool, isIndexed)() const { return ClassName ##::getClassInfoStatic().isIndexed(); } \
	ES_DECL_INTF_METHOD(EsString::Array, getPropertyNames)() const \
	{ return ClassName ##::getClassInfoStatic().getPropertyNames(); } \
	ES_DECL_INTF_METHOD(EsString::Array, getPersistentPropertyNames)() const \
	{ return ClassName ##::getClassInfoStatic().getPersistentPropertyNames(); } \
	ES_DECL_INTF_METHOD(bool, hasProperty)(const EsString& name) const \
	{ return ClassName ##::getClassInfoStatic().hasProperty(name); } \
	ES_DECL_INTF_METHOD(EsVariant, getProperty)(const EsString& name) const \
	{ const PropertyInfo& info = ClassName ##::getClassInfoStatic().getPropertyInfo(name); \
		return info.get(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this))); } \
	ES_DECL_INTF_METHOD(void, setProperty)(const EsString& name, const EsVariant& val) \
	{ const PropertyInfo& info = ClassName ##::getClassInfoStatic().getPropertyInfo(name); \
		return info.set(static_cast<EsBaseIntf*>(this), val); } \
	ES_DECL_REFLECTED_INTF_CONST_METHOD0(EsString, getConfigString) \
	{ EsString::Array props = getPersistentPropertyNames(); \
		EsDictionary dic( getTypeName() ); \
		for(size_t idx = 0; idx < props.size(); ++idx) { \
			const EsString& prop = props[idx]; \
			dic.setValue(prop, getProperty(prop), true); } \
		return dic.getAsString(); \
	} \
	ES_DECL_REFLECTED_INTF_METHOD2(void, setConfigString, cr_EsString, bool) \
	{ EsDictionary dic( getTypeName() ); \
		if( !p1.empty() ) { \
			dic.setAsString(p1); \
			if( dic.getName() != getTypeName() ) EsException::Throw(0, EsException::Generic, \
				_("Wrong configuration name. Expected '%s', got '%s'"), getTypeName().c_str(), dic.getName().c_str() ); \
			EsString::Array names = dic.getAllNames(); \
			for(size_t idx = 0; idx < names.size(); ++idx) { \
				const EsString& prop = names[idx]; \
				if( hasProperty(prop) ) setProperty(prop, dic.getValue(prop)); \
				else if( !p2 ) EsException::Throw(0, EsException::Generic, \
					_("Unknown configuration property '%s'"), prop.c_str()); \
			} \
		} \
	} \
	ES_DECL_INTF_METHOD(bool, hasMethod)(const MethodInfoKeyT& key) const \
	{ return ClassName ##::getClassInfoStatic().hasMethod(key); } \
	ES_DECL_INTF_METHOD(EsVariant, callMethod)(const EsString& name, const EsVariant& params) \
	{ return ClassName ##::getClassInfoStatic().callMethod(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name, params); } \
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name) \
	{ return ClassName ##::getClassInfoStatic().call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name); } \
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1) \
	{ return ClassName ##::getClassInfoStatic().call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name, param1); } \
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2) \
	{ return ClassName ##::getClassInfoStatic().call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name, param1, param2); } \
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) \
	{ return ClassName ##::getClassInfoStatic().call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name, param1, param2, param3); } \
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) \
	{ return ClassName ##::getClassInfoStatic().call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name, param1, param2, param3, param4); } \
	ES_DECL_INTF_METHOD(EsVariant, call)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) \
	{ return ClassName ##::getClassInfoStatic().call(static_cast<EsBaseIntf*>(const_cast<ThisClass*>(this)), name, param1, param2, param3, param4, param5); } \
	ES_DECL_INTF_METHOD(EsVariant, callClassMethod)(const EsString& name, const EsVariant& params) const \
	{ return ClassName ##::getClassInfoStatic().classCallMethod(name, params); } \
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name) const \
	{ return ClassName ##::getClassInfoStatic().classCall(name); } \
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1) const \
	{ return ClassName ##::getClassInfoStatic().classCall(name, param1); } \
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2) const \
	{ return ClassName ##::getClassInfoStatic().classCall(name, param1, param2); } \
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3) const \
	{ return ClassName ##::getClassInfoStatic().classCall(name, param1, param2, param3); } \
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4) const \
	{ return ClassName ##::getClassInfoStatic().classCall(name, param1, param2, param3, param4); } \
	ES_DECL_INTF_METHOD(EsVariant, classCall)(const EsString& name, const EsVariant& param1, const EsVariant& param2, const EsVariant& param3, const EsVariant& param4, const EsVariant& param5) const \
	{ return ClassName ##::getClassInfoStatic().classCall(name, param1, param2, param3, param4, param5); }

// declare only neccessary part of EsReflectedClassIntf, use in
// classes which inherit their functionality from
// base(s) already implementing EsReflectedClassIntf using ES_DECL_REFLECTED_CLASS_BASE
#define ES_DECL_REFLECTED_CLASS( ClassName ) \
public: \
	typedef ClassName ThisClass; \
	typedef void (ThisClass:: *CallT)(); \
	static const ClassInfo& getClassInfoStatic(); \
	ES_DECL_INTF_METHOD(const ClassInfo&, getClassInfo)() const { return ClassName ##::getClassInfoStatic(); }
	
// special case of reflected class - embedded reflected class, which may be passed in variant by-value
// custom macro defines all neccessary embedded support except for ClassName (const EsVariant& src) constructor, 
// which have to be defined explicitly.
//
#define ES_DECL_REFLECTED_CLASS_EMBEDDED_CUSTOM( ClassName ) \
ES_DECL_REFLECTED_CLASS_BASE( ClassName ) \
private: \
	void copyFromVariant(const EsVariant& src)	\
	{ \
		EsBaseIntf::Ptr obj = src.asObject(); \
		if( obj && is( obj->getClassName() ) ) \
		{ \
			wxASSERT(src.getType() == EsVariant::VAR_OBJECT_EMBEDDED); \
			*this = *reinterpret_cast<const ClassName*>(obj.get()); \
		} \
		else \
			EsException::ThrowObjectTypeIsIncompatibleWith( getClassName() );	\
	} \
public: \
	ClassName& operator= (const EsVariant& src) { copyFromVariant(src);	return *this; } \
	operator EsVariant () const { \
		wxCOMPILE_TIME_ASSERT(sizeof(EsVariant::ObjectByValue) >= sizeof( ClassName ), ClassName ##SizeIsLessOrEqualToEmbeddedObject); \
		return EsVariant(this, EsVariant::ACCEPT_OBJECT_EMBEDDED); }
		
// standard embedded reflected class declaration. it's ok if no custom initializer list is needed for 
// the members of ClassName's class
//
#define ES_DECL_REFLECTED_CLASS_EMBEDDED( ClassName ) \
ES_DECL_REFLECTED_CLASS_EMBEDDED_CUSTOM( ClassName ) \
ClassName (const EsVariant& src) { copyFromVariant(src);	}

// reflected namespace service definition
//
// declares reflected namespace service, as pure static class which may not be created
// only class reflected methods are allowed in service definition
#define ES_DECL_REFLECTED_SERIVES_BEGIN( NamespaceName ) \
class NamespaceName { \
private: \
	NamespaceName(); \
	NamespaceName(const NamespaceName&); \
	NamespaceName& operator= (const NamespaceName&); \
public: \
	static const ClassInfo& getClassInfoStatic();

#define ES_DECL_REFLECTED_SERIVES_END };

// reflected class info implementation macros
//
#define NO_CLASS_DESCRIPTION		EsString::s_null
// reflected ancestors declaration macros
#define NO_REFLECTED_ANCESTORS	ClassInfo::ClassInfosT()
#define ES_DECL_REFLECTED_ANCESTORS1( ReflBase1 ) ClassInfo::ClassInfosT().add( & ReflBase1 ##::getClassInfoStatic() )
#define ES_DECL_REFLECTED_ANCESTORS2( ReflBase1, ReflBase2 ) \
	ES_DECL_REFLECTED_ANCESTORS1( ReflBase1 ).add( & ReflBase2 ##::getClassInfoStatic() )
#define ES_DECL_REFLECTED_ANCESTORS3( ReflBase1, ReflBase2, ReflBase3 ) \
	ES_DECL_REFLECTED_ANCESTORS2( ReflBase1, ReflBase2 ).add( & ReflBase3 ##::getClassInfoStatic() )
#define ES_DECL_REFLECTED_ANCESTORS4( ReflBase1, ReflBase2, ReflBase3, ReflBase4 ) \
	ES_DECL_REFLECTED_ANCESTORS3( ReflBase1, ReflBase2, ReflBase3 ).add( & ReflBase4 ##::getClassInfoStatic() )
#define ES_DECL_REFLECTED_ANCESTORS5( ReflBase1, ReflBase2, ReflBase3, ReflBase4, ReflBase5 ) \
	ES_DECL_REFLECTED_ANCESTORS4( ReflBase1, ReflBase2, ReflBase3, ReflBase4 ).add( & ReflBase5 ##::getClassInfoStatic() )
#define ES_DECL_REFLECTED_ANCESTORS6( ReflBase1, ReflBase2, ReflBase3, ReflBase4, ReflBase5, ReflBase6 ) \
	ES_DECL_REFLECTED_ANCESTORS5( ReflBase1, ReflBase2, ReflBase3, ReflBase4, ReflBase5 ).add( & ReflBase6 ##::getClassInfoStatic() )

// class info opening clause
#define ES_DECL_CLASS_INFO_BEGIN( ClassName, ReflectedAncestors, Descr ) \
	const ClassInfo& ClassName ##::getClassInfoStatic() { \
		static ClassInfo s_class( wxT( #ClassName ), Descr, ReflectedAncestors );

// use in case reflected interface implementor is nested class on another class
#define ES_DECL_NESTED_CLASS_INFO_BEGIN( ClassName, ReflectedClassName, ReflectedAncestors, Descr ) \
	const ClassInfo& ClassName ##::getClassInfoStatic() { \
		static ClassInfo s_class( wxT( #ReflectedClassName ), Descr, ReflectedAncestors );

// base class info opening clause
#define ES_DECL_BASE_CLASS_INFO_BEGIN( ClassName, Descr ) \
	ES_DECL_CLASS_INFO_BEGIN( ClassName, NO_REFLECTED_ANCESTORS, Descr )

// nested base class info opening clause
#define ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN( ClassName, ReflectedClassName, Descr ) \
	ES_DECL_NESTED_CLASS_INFO_BEGIN( ClassName, ReflectedClassName, NO_REFLECTED_ANCESTORS, Descr )

#define ES_DECL_CLASS_INFO_END \
	return s_class; }

#endif // _reflection_reflected_class_defs_h_
