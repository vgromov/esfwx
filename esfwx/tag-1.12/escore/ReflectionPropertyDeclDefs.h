// reflected property declaration, setter & getter prototypes defined. 
//
#ifndef _reflection_property_decl_defs_h_
#define _reflection_property_decl_defs_h_

#define ES_DECL_PROPERTY(PropName, NativeT) \
public: \
	typedef GEN_CALL_T0(NativeT, ThisClass::, GEN_SIG0(get_## PropName ##_## NativeT, CallConst) ##_T) const; \
	typedef GEN_CALL_T1(void, ThisClass::, GEN_SIG1(set_## PropName ##_## void, Call, cr_## NativeT) ##_T, cr_## NativeT); \
	virtual NativeT get_ ## PropName() const; \
	virtual void set_ ## PropName(const NativeT& val);

#define ES_DECL_PROPERTY_STD(PropName, NativeT) \
	ES_DECL_PROPERTY(PropName, NativeT) \
protected: \
	NativeT m_## PropName;
	
#define ES_DECL_PROPERTY_RO(PropName, NativeT) \
public: \
	typedef GEN_CALL_T0(NativeT, ThisClass::, GEN_SIG0(get_## PropName ##_## NativeT, CallConst) ##_T) const; \
	virtual NativeT get_ ## PropName() const;

#define ES_DECL_PROPERTY_RO_STD(PropName, NativeT) \
	ES_DECL_PROPERTY_RO(PropName, NativeT) \
protected: \
	NativeT m_## PropName;	

// property info definition helpers
// must not be used in header files
//

#define NO_DEFAULT_VAL			EsVariant::s_null 
#define NO_PROPERTY_LABEL		EsString::s_null

#define ES_DECL_PROP_INFO(OwnerT, PropName, NativeT, Descr, DefVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction());

#define ES_DECL_PROP_INFO_RO(OwnerT, PropName, NativeT, Descr) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, NO_DEFAULT_VAL, \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	0, invalidSignature, \
	PropertyInfo::Restriction());

#define ES_DECL_PROP_INFO_MIN(OwnerT, PropName, NativeT, Descr, DefVal, MinVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction(static_cast<NativeT>(MinVal), PropertyInfo::rxtMin));

#define ES_DECL_PROP_INFO_MIN_EXCL(OwnerT, PropName, NativeT, Descr, DefVal, MinVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction(static_cast<NativeT>(MinVal), PropertyInfo::rxtMinExcl) );

#define ES_DECL_PROP_INFO_MAX(OwnerT, PropName, NativeT, Descr, DefVal, MaxVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction(static_cast<NativeT>(MaxVal), PropertyInfo::rxtMax));

#define ES_DECL_PROP_INFO_MAX_EXCL(OwnerT, PropName, NativeT, Descr, DefVal, MaxVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction(static_cast<NativeT>(MaxVal), PropertyInfo::rxtMaxExcl));

#define ES_DECL_PROP_INFO_RANGE(OwnerT, PropName, NativeT, Descr, DefVal, MinVal, MaxVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction( EsVariant(EsVariant::VAR_VARIANT_COLLECTION).addToVariantCollection(static_cast<NativeT>(MinVal)).addToVariantCollection(static_cast<NativeT>(MaxVal)), PropertyInfo::rxtRange ));

#define ES_DECL_PROP_INFO_RANGE_MIN_EXCL(OwnerT, PropName, NativeT, Descr, DefVal, MinVal, MaxVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction( EsVariant(EsVariant::VAR_VARIANT_COLLECTION).addToVariantCollection(static_cast<NativeT>(MinVal)).addToVariantCollection(static_cast<NativeT>(MaxVal)), PropertyInfo::rxtRangeMinExcl ));

#define ES_DECL_PROP_INFO_RANGE_MAX_EXCL(OwnerT, PropName, NativeT, Descr, DefVal, MinVal, MaxVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction( EsVariant(EsVariant::VAR_VARIANT_COLLECTION).addToVariantCollection(static_cast<NativeT>(MinVal)).addToVariantCollection(static_cast<NativeT>(MaxVal)), PropertyInfo::rxtRangeMaxExcl ));

#define ES_DECL_PROP_INFO_RANGE_EXCL(OwnerT, PropName, NativeT, Descr, DefVal, MinVal, MaxVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction( EsVariant(EsVariant::VAR_VARIANT_COLLECTION).addToVariantCollection(static_cast<NativeT>(MinVal)).addToVariantCollection(static_cast<NativeT>(MaxVal)), PropertyInfo::rxtRangeExcl ));

#define ES_DECL_PROP_INFO_ENUM_BEGIN(OwnerT, PropName, NativeT, Descr, DefVal) \
	static PropertyInfo s_## PropName ##_info( \
	s_class, wxT(#PropName), Descr, (DefVal), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::get_## PropName, GEN_SIG0(NativeT, CallConst), \
	(EsReflectedClassIntf::CallT)(void (OwnerT:: *)(void))& OwnerT ##::set_## PropName, GEN_SIG1(void, Call, cr_## NativeT), \
	PropertyInfo::Restriction( EsVariant(EsVariant::VAR_VARIANT_COLLECTION)

#define ES_DECL_PROP_INFO_ENUM_LABELLED_ENTRY( Val, Label ) \
	.addToVariantCollection( EsVariant(EsVariant::VAR_VARIANT_COLLECTION).addToVariantCollection(Val).addToVariantCollection(Label) )
#define ES_DECL_PROP_INFO_ENUM_ENTRY( Val ) .addToVariantCollection(Val)
#define ES_DECL_PROP_INFO_ENUM_END		,PropertyInfo::rxtEnum ) );

#endif // _reflection_property_decl_defs_h_