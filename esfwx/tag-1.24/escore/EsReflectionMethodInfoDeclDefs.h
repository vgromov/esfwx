#ifndef _reflection_method_info_decl_defs_h_
#define _reflection_method_info_decl_defs_h_

// reflected methods info declarations
//

#define NO_METHOD_DESCR  EsString::null()

#define ES_INTERNAL_METHOD_CAST(OwnerT, MethodName, Sig, CallT) \
  esCallCastToCall< ES_CONCAT4(MethodName, _, Sig, _T), EsMemberCallT >(CallT)

#define ES_INTERNAL_INTF_METHOD_CAST(OwnerT, IntfType, MethodName, Sig, CallT) \
  esCallCastToCall< ES_CONCAT4(MethodName, _, Sig, _T), EsMemberCallT >(CallT)

// allow method reflection to have alias by which method will be identified in reflection framework
// this would augment calling overloaded methods which differ only by parameter types
#define ES_DECL_REFLECTED_METHOD_INFO(OwnerT, MethodName, MethodAlias, Sig, Descr) \
static EsMethodInfo ES_CONCAT4(s_, MethodName, _, Sig)( \
  s_class, Sig, ES_CONCAT(Sig, _paramsCount), ES_STRINGIZE( MethodAlias ), Descr, \
  ES_INTERNAL_METHOD_CAST(OwnerT, MethodName, Sig, & OwnerT :: MethodName));

#define ES_DECL_REFLECTED_METHOD_INFO_STD(OwnerT, MethodName, Sig, Descr) \
  ES_DECL_REFLECTED_METHOD_INFO(OwnerT, MethodName, MethodName, Sig, Descr)

// allow for reflection of interface methods, which have to be properly cast and called
// via proper EsBaseIntf queries
#define ES_DECL_REFLECTED_INTF_METHOD_INFO(OwnerT, IntfType, MethodName, Sig, Descr) \
static EsMethodInfo ES_CONCAT4(s_, MethodName, _, Sig)( \
  s_class, Sig, EsIID::fromIntf< IntfType >(), ES_CONCAT(Sig, _paramsCount), ES_STRINGIZE(MethodName), Descr, \
  ES_INTERNAL_INTF_METHOD_CAST(OwnerT, IntfType, MethodName, Sig, & OwnerT :: MethodName));

// reflected class method info delcaration macros
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
# define ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, MethodName, MethodAlias, Sig, Descr) \
  __pragma(warning(disable:4191)) \
  static EsMethodInfo ES_CONCAT4(s_, MethodName, _, Sig)( \
    s_class, Sig, ES_CONCAT(Sig, _paramsCount), ES_STRINGIZE(MethodAlias), Descr, \
    (EsClassCallT)( ES_CONCAT4(MethodName, _, Sig, _T) ) & OwnerT :: MethodName, \
    0);
#else
# define ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, MethodName, MethodAlias, Sig, Descr) \
  static EsMethodInfo ES_CONCAT4(s_, MethodName, _, Sig)( \
    s_class, Sig, ES_CONCAT(Sig, _paramsCount), ES_STRINGIZE(MethodAlias), Descr, \
    (EsClassCallT)( ES_CONCAT4(MethodName, _, Sig, _T) ) & OwnerT :: MethodName, \
    0);
#endif

#define ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(OwnerT, MethodName, Sig, Descr) \
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, MethodName, MethodName, Sig, Descr)

// specialized CTOR declarations
//
#define ES_DECL_REFLECTED_CTOR_INFO(OwnerT, Sig, Descr) \
  ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, NEW, new, Sig, Descr)

#endif // _reflection_method_info_decl_defs_h_
