// reflected method info declaration macros
//
#ifndef _reflection_method_defs_h_
#define _reflection_method_defs_h_

// object methods
//
#define ES_DECL_REFLECTED_METHOD0(RetVal, MethodName) \
public: \
	typedef GEN_CALL_T0(RetVal, ThisClass::, wxCONCAT(GEN_SIG0(wxCONCAT3(MethodName, _, RetVal), Call), _T)); \
	RetVal MethodName()

#define ES_DECL_REFLECTED_METHOD1(RetVal, MethodName, Param1) \
public: \
	typedef GEN_CALL_T1(RetVal, ThisClass::, wxCONCAT(GEN_SIG1(wxCONCAT3(MethodName, _, RetVal), Call, Param1), _T), Param1); \
	RetVal MethodName(Param1 p1)

#define ES_DECL_REFLECTED_METHOD2(RetVal, MethodName, Param1, Param2) \
public: \
	typedef GEN_CALL_T2(RetVal, ThisClass::, wxCONCAT(GEN_SIG2(wxCONCAT3(MethodName, _, RetVal), Call, Param1, Param2), _T), Param1, Param2); \
	RetVal MethodName(Param1 p1, Param2 p2)

#define ES_DECL_REFLECTED_METHOD3(RetVal, MethodName, Param1, Param2, Param3) \
public: \
	typedef GEN_CALL_T3(RetVal, ThisClass::, wxCONCAT(GEN_SIG3(wxCONCAT3(MethodName, _, RetVal), Call, Param1, Param2, Param3), _T), Param1, Param2, Param3); \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3)

#define ES_DECL_REFLECTED_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4) \
public: \
	typedef GEN_CALL_T4(RetVal, ThisClass::, GEN_SIG4(wxCONCAT3(MethodName, _, RetVal), Call, Param1, Param2, Param3, Param4) ##_T, Param1, Param2, Param3, Param4); \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4)

#define ES_DECL_REFLECTED_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5) \
public: \
	typedef GEN_CALL_T5(RetVal, ThisClass::, GEN_SIG5(wxCONCAT3(MethodName, _, RetVal), Call, Param1, Param2, Param3, Param4, Param5) ##_T, Param1, Param2, Param3, Param4, Param5); \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)

#define ES_DECL_REFLECTED_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6) \
public: \
	typedef GEN_CALL_T6(RetVal, ThisClass::, GEN_SIG6(wxCONCAT3(MethodName, _, RetVal), Call, Param1, Param2, Param3, Param4, Param5, Param6) ##_T, Param1, Param2, Param3, Param4, Param5, Param6); \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)

#define ES_DECL_REFLECTED_CONST_METHOD0(RetVal, MethodName) \
public: \
	typedef GEN_CALL_T0(RetVal, ThisClass::, GEN_SIG0(wxCONCAT3(MethodName, _, RetVal), CallConst) ## _T) const; \
	RetVal MethodName() const

#define ES_DECL_REFLECTED_CONST_METHOD1(RetVal, MethodName, Param1) \
public: \
	typedef GEN_CALL_T1(RetVal, ThisClass::, GEN_SIG1(wxCONCAT3(MethodName, _, RetVal), CallConst, Param1) ## _T, Param1) const; \
	RetVal MethodName(Param1 p1) const

#define ES_DECL_REFLECTED_CONST_METHOD2(RetVal, MethodName, Param1, Param2) \
public: \
	typedef GEN_CALL_T2(RetVal, ThisClass::, GEN_SIG2(wxCONCAT3(MethodName, _, RetVal), CallConst, Param1, Param2) ## _T, Param1, Param2) const; \
	RetVal MethodName(Param1 p1, Param2 p2) const

#define ES_DECL_REFLECTED_CONST_METHOD3(RetVal, MethodName, Param1, Param2, Param3) \
public: \
	typedef GEN_CALL_T3(RetVal, ThisClass::, GEN_SIG3(wxCONCAT3(MethodName, _, RetVal), CallConst, Param1, Param2, Param3) ##_T, Param1, Param2, Param3) const; \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3) const

#define ES_DECL_REFLECTED_CONST_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4) \
public: \
	typedef GEN_CALL_T4(RetVal, ThisClass::, GEN_SIG4(wxCONCAT3(MethodName, _, RetVal), CallConst, Param1, Param2, Param3, Param4) ##_T, Param1, Param2, Param3, Param4) const; \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const

#define ES_DECL_REFLECTED_CONST_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5) \
public: \
	typedef GEN_CALL_T5(RetVal, ThisClass::, GEN_SIG5(wxCONCAT3(MethodName, _, RetVal), CallConst, Param1, Param2, Param3, Param4, Param5) ##_T, Param1, Param2, Param3, Param4, Param5) const; \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const

#define ES_DECL_REFLECTED_CONST_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6) \
public: \
	typedef GEN_CALL_T6(RetVal, ThisClass::, GEN_SIG6(wxCONCAT3(MethodName, _, RetVal), CallConst, Param1, Param2, Param3, Param4, Param5, Param6) ##_T, Param1, Param2, Param3, Param4, Param5, Param6) const; \
	RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const
	
// reflected interface methods - merely aliased object method decls
//
#define ES_DECL_REFLECTED_INTF_METHOD0(RetVal, MethodName)																		ES_DECL_REFLECTED_METHOD0(RetVal, MethodName)
#define ES_DECL_REFLECTED_INTF_METHOD1(RetVal, MethodName, Param1)														ES_DECL_REFLECTED_METHOD1(RetVal, MethodName, Param1) 
#define ES_DECL_REFLECTED_INTF_METHOD2(RetVal, MethodName, Param1, Param2)										ES_DECL_REFLECTED_METHOD2(RetVal, MethodName, Param1, Param2)
#define ES_DECL_REFLECTED_INTF_METHOD3(RetVal, MethodName, Param1, Param2, Param3)						ES_DECL_REFLECTED_METHOD3(RetVal, MethodName, Param1, Param2, Param3)
#define ES_DECL_REFLECTED_INTF_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)		ES_DECL_REFLECTED_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)
#define ES_DECL_REFLECTED_INTF_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5) ES_DECL_REFLECTED_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5)
#define ES_DECL_REFLECTED_INTF_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6) ES_DECL_REFLECTED_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD0(RetVal, MethodName)															ES_DECL_REFLECTED_CONST_METHOD0(RetVal, MethodName)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD1(RetVal, MethodName, Param1)											ES_DECL_REFLECTED_CONST_METHOD1(RetVal, MethodName, Param1)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD2(RetVal, MethodName, Param1, Param2)							ES_DECL_REFLECTED_CONST_METHOD2(RetVal, MethodName, Param1, Param2)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD3(RetVal, MethodName, Param1, Param2, Param3)			ES_DECL_REFLECTED_CONST_METHOD3(RetVal, MethodName, Param1, Param2, Param3)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4) ES_DECL_REFLECTED_CONST_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5) ES_DECL_REFLECTED_CONST_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6) ES_DECL_REFLECTED_CONST_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6)

// class methods
//

#define ES_DECL_REFLECTED_CLASS_METHOD0(RetVal, MethodName) \
public: \
	typedef GEN_CALL_T0(RetVal, NO_THIS_SPEC, GEN_SIG0(wxCONCAT3(MethodName, _, RetVal), ClassCall) ##_T); \
	static RetVal MethodName()

#define ES_DECL_REFLECTED_CLASS_METHOD1(RetVal, MethodName, Param1) \
public: \
	typedef GEN_CALL_T1(RetVal, NO_THIS_SPEC, GEN_SIG1(wxCONCAT3(MethodName, _, RetVal), ClassCall, Param1) ##_T, Param1); \
	static RetVal MethodName(Param1 p1)

#define ES_DECL_REFLECTED_CLASS_METHOD2(RetVal, MethodName, Param1, Param2) \
public: \
	typedef GEN_CALL_T2(RetVal, NO_THIS_SPEC, GEN_SIG2(wxCONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2) ##_T, Param1, Param2); \
	static RetVal MethodName(Param1 p1, Param2 p2)

#define ES_DECL_REFLECTED_CLASS_METHOD3(RetVal, MethodName, Param1, Param2, Param3) \
public: \
	typedef GEN_CALL_T3(RetVal, NO_THIS_SPEC, GEN_SIG3(wxCONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3) ##_T, Param1, Param2, Param3); \
	static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3)

#define ES_DECL_REFLECTED_CLASS_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4) \
public: \
	typedef GEN_CALL_T4(RetVal, NO_THIS_SPEC, GEN_SIG4(wxCONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4) ##_T, Param1, Param2, Param3, Param4); \
	static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4)

#define ES_DECL_REFLECTED_CLASS_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5) \
public: \
	typedef GEN_CALL_T5(RetVal, NO_THIS_SPEC, GEN_SIG5(wxCONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4, Param5) ##_T, Param1, Param2, Param3, Param4, Param5); \
	static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)

#define ES_DECL_REFLECTED_CLASS_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6) \
public: \
	typedef GEN_CALL_T6(RetVal, NO_THIS_SPEC, GEN_SIG6(wxCONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4, Param5, Param6) ##_T, Param1, Param2, Param3, Param4, Param5, Param6); \
	static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)

// special decls for reflected constructors
//
#define ES_DECL_REFLECTED_CLASS_CTOR0(RetVal) \
	ES_DECL_REFLECTED_CLASS_METHOD0(RetVal, NEW)

#define ES_DECL_REFLECTED_CLASS_CTOR1(RetVal, Param1) \
	ES_DECL_REFLECTED_CLASS_METHOD1(RetVal, NEW, Param1)
	
#define ES_DECL_REFLECTED_CLASS_CTOR2(RetVal, Param1, Param2) \
	ES_DECL_REFLECTED_CLASS_METHOD2(RetVal, NEW, Param1, Param2)

#define ES_DECL_REFLECTED_CLASS_CTOR3(RetVal, Param1, Param2, Param3) \
	ES_DECL_REFLECTED_CLASS_METHOD3(RetVal, NEW, Param1, Param2, Param3)

#define ES_DECL_REFLECTED_CLASS_CTOR4(RetVal, Param1, Param2, Param3, Param4) \
	ES_DECL_REFLECTED_CLASS_METHOD4(RetVal, NEW, Param1, Param2, Param3, Param4)

#define ES_DECL_REFLECTED_CLASS_CTOR5(RetVal, Param1, Param2, Param3, Param4, Param5) \
	ES_DECL_REFLECTED_CLASS_METHOD5(RetVal, NEW, Param1, Param2, Param3, Param4, Param5)

#define ES_DECL_REFLECTED_CLASS_CTOR6(RetVal, Param1, Param2, Param3, Param4, Param5, Param6) \
	ES_DECL_REFLECTED_CLASS_METHOD6(RetVal, NEW, Param1, Param2, Param3, Param4, Param5, Param6)

#endif // _reflection_method_defs_h_
