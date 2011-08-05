#ifndef _reflection_method_info_decl_defs_h_
#define _reflection_method_info_decl_defs_h_

// reflected methods info declarations
//

#define NO_METHOD_DESCR	EsString::s_null

// allow method reflection to have alias by which method will be identified in reflection framework
// this would augment calling overloaded methods which differ only by parameter types
#define ES_DECL_REFLECTED_METHOD_INFO(OwnerT, MethodName, MethodAlias, Sig, Descr) \
static MethodInfo wxCONCAT4(s_, MethodName, _, Sig)( \
	s_class, Sig, wxCONCAT(Sig, _paramsCount), wxT( #MethodAlias ), Descr, \
	(EsReflectedClassIntf::CallT)(void ( wxCONCAT(OwnerT, ::* ) )(void))( OwnerT :: wxCONCAT4(MethodName, _, Sig, _T) )& wxCONCAT3(OwnerT, ::, MethodName));
	
#define ES_DECL_REFLECTED_METHOD_INFO_STD(OwnerT, MethodName, Sig, Descr) \
	ES_DECL_REFLECTED_METHOD_INFO(OwnerT, MethodName, MethodName, Sig, Descr)

// allow for reflection of interface methods, which have to be properly cast and called
// via proper EsBaseIntf queries
#define ES_DECL_REFLECTED_INTF_METHOD_INFO(OwnerT, IntfType, MethodName, Sig, Descr) \
static MethodInfo wxCONCAT4(s_, MethodName, _, Sig)( \
	s_class, Sig, EsIID::fromIntf< IntfType >(), wxCONCAT(Sig, _paramsCount), wxT( #MethodName ), Descr, \
	(EsReflectedClassIntf::CallT)(void (IntfType :: *)(void))(void (OwnerT :: *)(void))(OwnerT :: wxCONCAT4(MethodName, _, Sig, _T))& wxCONCAT3(OwnerT, ::, MethodName));
	
// reflected class method info delcaration macros
//
#define ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, MethodName, MethodAlias, Sig, Descr) \
static MethodInfo wxCONCAT4(s_, MethodName, _, Sig)( \
	s_class, Sig, wxCONCAT(Sig, _paramsCount), wxT( #MethodAlias ), Descr, \
	(EsReflectedClassIntf::ClassCallT)(OwnerT :: wxCONCAT4(MethodName, _, Sig, _T)) & wxCONCAT3(OwnerT, ::, MethodName), 0);
	
#define ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(OwnerT, MethodName, Sig, Descr) \
	ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, MethodName, MethodName, Sig, Descr)

// specialized CTOR declarations
//
#define ES_DECL_REFLECTED_CTOR_INFO(OwnerT, Sig, Descr) \
	ES_DECL_REFLECTED_CLASS_METHOD_INFO(OwnerT, NEW, new, Sig, Descr)

#endif // _reflection_method_info_decl_defs_h_