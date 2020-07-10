// reflected method info declaration macros
//
#ifndef _reflection_method_defs_h_
#define _reflection_method_defs_h_

// internal helper macros for reflected method declaration
#define ES_INTERNAL_DECL_REFLECTED_METHOD0(VirtualM, RetVal, MethodName, CallSig, ConstM) \
public: \
  typedef GEN_CALL_T0(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG0(ES_CONCAT3(MethodName, _, RetVal), CallSig), _T)) ConstM; \
  VirtualM RetVal MethodName() ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD1(VirtualM, RetVal, MethodName, CallSig, Param1, ConstM) \
public: \
  typedef GEN_CALL_T1(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG1(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1), _T), Param1) ConstM; \
  VirtualM RetVal MethodName(Param1 p1) ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD2(VirtualM, RetVal, MethodName, CallSig, Param1, Param2, ConstM) \
public: \
  typedef GEN_CALL_T2(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG2(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1, Param2), _T), Param1, Param2) ConstM; \
  VirtualM RetVal MethodName(Param1 p1, Param2 p2) ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD3(VirtualM, RetVal, MethodName, CallSig, Param1, Param2, Param3, ConstM) \
public: \
  typedef GEN_CALL_T3(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG3(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1, Param2, Param3), _T), Param1, Param2, Param3) ConstM; \
  VirtualM RetVal MethodName(Param1 p1, Param2 p2, Param3 p3) ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD4(VirtualM, RetVal, MethodName, CallSig, Param1, Param2, Param3, Param4, ConstM) \
public: \
  typedef GEN_CALL_T4(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG4(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1, Param2, Param3, Param4), _T), Param1, Param2, Param3, Param4) ConstM; \
  VirtualM RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4) ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD5(VirtualM, RetVal, MethodName, CallSig, Param1, Param2, Param3, Param4, Param5, ConstM) \
public: \
  typedef GEN_CALL_T5(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG5(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1, Param2, Param3, Param4, Param5), _T), Param1, Param2, Param3, Param4, Param5) ConstM; \
  VirtualM RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD6(VirtualM, RetVal, MethodName, CallSig, Param1, Param2, Param3, Param4, Param5, Param6, ConstM) \
public: \
  typedef GEN_CALL_T6(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG6(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1, Param2, Param3, Param4, Param5, Param6), _T), Param1, Param2, Param3, Param4, Param5, Param6) ConstM; \
  VirtualM RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) ConstM

#define ES_INTERNAL_DECL_REFLECTED_METHOD7(VirtualM, RetVal, MethodName, CallSig, Param1, Param2, Param3, Param4, Param5, Param6, Param7, ConstM) \
public: \
  typedef GEN_CALL_T7(RetVal, ThisClassT::, ES_CONCAT(GEN_SIG7(ES_CONCAT3(MethodName, _, RetVal), CallSig, Param1, Param2, Param3, Param4, Param5, Param6, Param7), _T), Param1, Param2, Param3, Param4, Param5, Param6, Param7) ConstM; \
  VirtualM RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param6 p7) ConstM

#define ES_INTERNAL_NO_VIRTUAL
#define ES_INTERNAL_VIRTUAL                   virtual
#define ES_INTERNAL_NO_CONST
#define ES_INTERNAL_CONST                     const 

// object methods
//
// non-const
#define ES_DECL_REFLECTED_METHOD0(RetVal, MethodName)                                                                         ES_INTERNAL_DECL_REFLECTED_METHOD0(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD1(RetVal, MethodName, Param1)                                                                 ES_INTERNAL_DECL_REFLECTED_METHOD1(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD2(RetVal, MethodName, Param1, Param2)                                                         ES_INTERNAL_DECL_REFLECTED_METHOD2(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD3(RetVal, MethodName, Param1, Param2, Param3)                                                 ES_INTERNAL_DECL_REFLECTED_METHOD3(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)                                         ES_INTERNAL_DECL_REFLECTED_METHOD4(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5)                                 ES_INTERNAL_DECL_REFLECTED_METHOD5(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, Param5, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6)                         ES_INTERNAL_DECL_REFLECTED_METHOD6(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, Param5, Param6, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_METHOD7(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6, Param7)                 ES_INTERNAL_DECL_REFLECTED_METHOD7(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7, ES_INTERNAL_NO_CONST)
// const
#define ES_DECL_REFLECTED_CONST_METHOD0(RetVal, MethodName)                                                                   ES_INTERNAL_DECL_REFLECTED_METHOD0(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD1(RetVal, MethodName, Param1)                                                           ES_INTERNAL_DECL_REFLECTED_METHOD1(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD2(RetVal, MethodName, Param1, Param2)                                                   ES_INTERNAL_DECL_REFLECTED_METHOD2(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD3(RetVal, MethodName, Param1, Param2, Param3)                                           ES_INTERNAL_DECL_REFLECTED_METHOD3(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)                                   ES_INTERNAL_DECL_REFLECTED_METHOD4(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5)                           ES_INTERNAL_DECL_REFLECTED_METHOD5(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, Param5, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6)                   ES_INTERNAL_DECL_REFLECTED_METHOD6(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_CONST_METHOD7(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6, Param7)           ES_INTERNAL_DECL_REFLECTED_METHOD7(ES_INTERNAL_NO_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7, ES_INTERNAL_CONST)

// reflected interface methods
//
// non-const
#define ES_DECL_REFLECTED_INTF_METHOD0(RetVal, MethodName, ...)                                                               ES_INTERNAL_DECL_REFLECTED_METHOD0(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD1(RetVal, MethodName, Param1)                                                            ES_INTERNAL_DECL_REFLECTED_METHOD1(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD2(RetVal, MethodName, Param1, Param2)                                                    ES_INTERNAL_DECL_REFLECTED_METHOD2(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD3(RetVal, MethodName, Param1, Param2, Param3)                                            ES_INTERNAL_DECL_REFLECTED_METHOD3(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)                                    ES_INTERNAL_DECL_REFLECTED_METHOD4(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5)                            ES_INTERNAL_DECL_REFLECTED_METHOD5(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, Param5, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6)                    ES_INTERNAL_DECL_REFLECTED_METHOD6(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, Param5, Param6, ES_INTERNAL_NO_CONST)
#define ES_DECL_REFLECTED_INTF_METHOD7(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6, Param7)            ES_INTERNAL_DECL_REFLECTED_METHOD7(ES_INTERNAL_VIRTUAL, RetVal, MethodName, Call, Param1, Param2, Param3, Param4, Param5, Param6, Param7, ES_INTERNAL_NO_CONST)
// const
#define ES_DECL_REFLECTED_INTF_CONST_METHOD0(RetVal, MethodName)                                                              ES_INTERNAL_DECL_REFLECTED_METHOD0(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD1(RetVal, MethodName, Param1)                                                      ES_INTERNAL_DECL_REFLECTED_METHOD1(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD2(RetVal, MethodName, Param1, Param2)                                              ES_INTERNAL_DECL_REFLECTED_METHOD2(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD3(RetVal, MethodName, Param1, Param2, Param3)                                      ES_INTERNAL_DECL_REFLECTED_METHOD3(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4)                              ES_INTERNAL_DECL_REFLECTED_METHOD4(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5)                      ES_INTERNAL_DECL_REFLECTED_METHOD5(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, Param5, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6)              ES_INTERNAL_DECL_REFLECTED_METHOD6(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, ES_INTERNAL_CONST)
#define ES_DECL_REFLECTED_INTF_CONST_METHOD7(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6, Param7)      ES_INTERNAL_DECL_REFLECTED_METHOD7(ES_INTERNAL_VIRTUAL, RetVal, MethodName, CallConst, Param1, Param2, Param3, Param4, Param5, Param6, Param7, ES_INTERNAL_CONST)

// class methods
//

#define ES_DECL_REFLECTED_CLASS_METHOD0(RetVal, MethodName) \
public: \
  typedef GEN_CALL_T0(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG0(ES_CONCAT3(MethodName, _, RetVal), ClassCall), _T)); \
  static RetVal MethodName()

#define ES_DECL_REFLECTED_CLASS_METHOD1(RetVal, MethodName, Param1) \
public: \
  typedef GEN_CALL_T1(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG1(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1), _T), Param1); \
  static RetVal MethodName(Param1 p1)

#define ES_DECL_REFLECTED_CLASS_METHOD2(RetVal, MethodName, Param1, Param2) \
public: \
  typedef GEN_CALL_T2(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG2(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2), _T), Param1, Param2); \
  static RetVal MethodName(Param1 p1, Param2 p2)

#define ES_DECL_REFLECTED_CLASS_METHOD3(RetVal, MethodName, Param1, Param2, Param3) \
public: \
  typedef GEN_CALL_T3(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG3(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3), _T), Param1, Param2, Param3); \
  static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3)

#define ES_DECL_REFLECTED_CLASS_METHOD4(RetVal, MethodName, Param1, Param2, Param3, Param4) \
public: \
  typedef GEN_CALL_T4(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG4(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4), _T), Param1, Param2, Param3, Param4); \
  static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4)

#define ES_DECL_REFLECTED_CLASS_METHOD5(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5) \
public: \
  typedef GEN_CALL_T5(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG5(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4, Param5), _T), Param1, Param2, Param3, Param4, Param5); \
  static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)

#define ES_DECL_REFLECTED_CLASS_METHOD6(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6) \
public: \
  typedef GEN_CALL_T6(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG6(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4, Param5, Param6), _T), Param1, Param2, Param3, Param4, Param5, Param6); \
  static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)

#define ES_DECL_REFLECTED_CLASS_METHOD7(RetVal, MethodName, Param1, Param2, Param3, Param4, Param5, Param6, Param7) \
public: \
  typedef GEN_CALL_T7(RetVal, NO_THIS_SPEC, ES_CONCAT(GEN_SIG7(ES_CONCAT3(MethodName, _, RetVal), ClassCall, Param1, Param2, Param3, Param4, Param5, Param6, Param7), _T), Param1, Param2, Param3, Param4, Param5, Param6, Param7); \
  static RetVal MethodName(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)

// special decls for reflected constructors
//
#define ES_DECL_REFLECTED_CLASS_CTOR0(RetVal)                                                                                 ES_DECL_REFLECTED_CLASS_METHOD0(RetVal, NEW)
#define ES_DECL_REFLECTED_CLASS_CTOR1(RetVal, Param1)                                                                         ES_DECL_REFLECTED_CLASS_METHOD1(RetVal, NEW, Param1)
#define ES_DECL_REFLECTED_CLASS_CTOR2(RetVal, Param1, Param2)                                                                 ES_DECL_REFLECTED_CLASS_METHOD2(RetVal, NEW, Param1, Param2)
#define ES_DECL_REFLECTED_CLASS_CTOR3(RetVal, Param1, Param2, Param3)                                                         ES_DECL_REFLECTED_CLASS_METHOD3(RetVal, NEW, Param1, Param2, Param3)
#define ES_DECL_REFLECTED_CLASS_CTOR4(RetVal, Param1, Param2, Param3, Param4)                                                 ES_DECL_REFLECTED_CLASS_METHOD4(RetVal, NEW, Param1, Param2, Param3, Param4)
#define ES_DECL_REFLECTED_CLASS_CTOR5(RetVal, Param1, Param2, Param3, Param4, Param5)                                         ES_DECL_REFLECTED_CLASS_METHOD5(RetVal, NEW, Param1, Param2, Param3, Param4, Param5)
#define ES_DECL_REFLECTED_CLASS_CTOR6(RetVal, Param1, Param2, Param3, Param4, Param5, Param6)                                 ES_DECL_REFLECTED_CLASS_METHOD6(RetVal, NEW, Param1, Param2, Param3, Param4, Param5, Param6)
#define ES_DECL_REFLECTED_CLASS_CTOR7(RetVal, Param1, Param2, Param3, Param4, Param5, Param6, Param7)                         ES_DECL_REFLECTED_CLASS_METHOD7(RetVal, NEW, Param1, Param2, Param3, Param4, Param5, Param6, Param7)

#endif // _reflection_method_defs_h_
