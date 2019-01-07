// reflected property declaration, setter & getter prototypes defined.
//
#ifndef _reflection_property_decl_defs_h_
#define _reflection_property_decl_defs_h_

#define ES_DECL_PROPERTY(PropName, NativeT) \
public: \
  virtual NativeT get_## PropName() const; \
  virtual void set_## PropName(cr_## NativeT val);

#define ES_DECL_PROPERTY_STD(PropName, NativeT) \
  ES_DECL_PROPERTY(PropName, NativeT) \
protected: \
  NativeT m_## PropName;
  
#define ES_DECL_PROPERTY_RO(PropName, NativeT) \
public: \
  virtual NativeT get_ ## PropName() const;

#define ES_DECL_PROPERTY_RO_STD(PropName, NativeT) \
  ES_DECL_PROPERTY_RO(PropName, NativeT) \
protected: \
  NativeT m_## PropName;  

#define ES_DECL_PROPERTY_WO(PropName, NativeT) \
public: \
  virtual void set_ ## PropName(cr_## NativeT val);

#define ES_DECL_PROPERTY_WO_STD(PropName, NativeT) \
  ES_DECL_PROPERTY_WO(PropName, NativeT) \
protected: \
  NativeT m_## PropName;  

// property info definition helpers
// must not be used in header files
//

#define NO_DEFAULT_VAL        EsVariant::null()
#define NO_PROPERTY_LABEL     EsString::null()
#define NO_PROPERTY_DESCR     NO_PROPERTY_LABEL

#define ES_INTERNAL_PROPCALL_CAST_GETTER(OwnerT, NativeT, CallT) \
  esCallCastToCall< NativeT (OwnerT:: *)() const, EsMemberCallT >(CallT)
#define ES_INTERNAL_PROPCALL_CAST_SETTER(OwnerT, NativeT, CallT) \
  esCallCastToCall< void (OwnerT:: *)(ES_CONCAT(cr_, NativeT)), EsMemberCallT >(CallT)

#define ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr, Restriction, Persistent) \
  static EsPropertyInfo s_## PropName ##_info( \
  s_class, ES_STRINGIZE(PropName), Label, Descr, (DefVal), \
  ES_INTERNAL_PROPCALL_CAST_GETTER(OwnerT, NativeT, & OwnerT ::get_## PropName), GEN_SIG0(NativeT, CallConst), \
  ES_INTERNAL_PROPCALL_CAST_SETTER(OwnerT, NativeT, & OwnerT ::set_## PropName), GEN_SIG1(void, Call, ES_CONCAT(cr_, NativeT) ), \
  Restriction, Persistent );

#define ES_DECL_PROP_INFO(OwnerT, PropName, NativeT, Label, DefVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr, EsVariant::null(), false)

#define ES_DECL_PROP_INFO_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr, EsVariant::null(), true)

#define ES_DECL_PROP_INFO_RO(OwnerT, PropName, NativeT, Label, Descr) \
  static EsPropertyInfo s_## PropName ##_info( \
  s_class, ES_STRINGIZE(PropName), Label, Descr, NO_DEFAULT_VAL, \
  ES_INTERNAL_PROPCALL_CAST_GETTER(OwnerT, NativeT, & OwnerT ::get_## PropName), GEN_SIG0(NativeT, CallConst), \
  0, invalidSignature, \
  EsVariant::null());

#define ES_DECL_PROP_INFO_WO(OwnerT, PropName, NativeT, Label, Descr) \
  static EsPropertyInfo s_## PropName ##_info( \
  s_class, ES_STRINGIZE(PropName), Label, Descr, NO_DEFAULT_VAL, \
  0, invalidSignature, \
    ES_INTERNAL_PROPCALL_CAST_SETTER(OwnerT, NativeT, & OwnerT ::set_## PropName), GEN_SIG1(void, Call, ES_CONCAT(cr_, NativeT) ), \
  EsVariant::null());

#define ES_DECL_PROP_INFO_MIN(OwnerT, PropName, NativeT, Label, DefVal, MinVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinIncl(static_cast<NativeT>(MinVal)), false)

#define ES_DECL_PROP_INFO_MIN_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MinVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinIncl(static_cast<NativeT>(MinVal)), true)

#define ES_DECL_PROP_INFO_MIN_EXCL(OwnerT, PropName, NativeT, Label, DefVal, MinVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinExcl(static_cast<NativeT>(MinVal)), false)

#define ES_DECL_PROP_INFO_MIN_EXCL_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MinVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinExcl(static_cast<NativeT>(MinVal)), true)

#define ES_DECL_PROP_INFO_MAX(OwnerT, PropName, NativeT, Label, DefVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMaxIncl(static_cast<NativeT>(MaxVal)), false)

#define ES_DECL_PROP_INFO_MAX_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMaxIncl(static_cast<NativeT>(MaxVal)), true)

#define ES_DECL_PROP_INFO_MAX_EXCL(OwnerT, PropName, NativeT, Label, DefVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMaxExcl(static_cast<NativeT>(MaxVal)), false)

#define ES_DECL_PROP_INFO_MAX_EXCL_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMaxExcl(static_cast<NativeT>(MaxVal)), true)

#define ES_DECL_PROP_INFO_RANGE(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinInclMaxIncl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), false)

#define ES_DECL_PROP_INFO_RANGE_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinInclMaxIncl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), true)

#define ES_DECL_PROP_INFO_RANGE_MIN_EXCL(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinExclMaxIncl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), false)

#define ES_DECL_PROP_INFO_RANGE_MIN_EXCL_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinExclMaxIncl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), true)

#define ES_DECL_PROP_INFO_RANGE_MAX_EXCL(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinInclMaxExcl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), false)

#define ES_DECL_PROP_INFO_RANGE_MAX_EXCL_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinInclMaxExcl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), true)

#define ES_DECL_PROP_INFO_RANGE_EXCL(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinExclMaxExcl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), false)

#define ES_DECL_PROP_INFO_RANGE_EXCL_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, MinVal, MaxVal, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  EsRange::createMinExclMaxExcl( static_cast<NativeT>(MinVal), static_cast<NativeT>(MaxVal) ), true)

#define ES_DECL_PROP_INFO_RESTRICTED(OwnerT, PropName, NativeT, Label, DefVal, Restriction, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  (Restriction), false)

#define ES_DECL_PROP_INFO_RESTRICTED_PERSISTENT(OwnerT, PropName, NativeT, Label, DefVal, Restriction, Descr) \
  ES_DECL_PROP_INFO_INTERNAL(OwnerT, PropName, NativeT, Label, DefVal, Descr,  (Restriction), true)

#endif // _reflection_property_decl_defs_h_