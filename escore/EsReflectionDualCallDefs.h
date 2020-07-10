#ifndef _reflection_dual_call_defs_h_
#define _reflection_dual_call_defs_h_

// specific macros family, allowing to utilize dualistic nature of classes implementing reflected interfaces
// the specific object calls on specific interfaces may be tried first on native (C++) interface,
// if one is implemented by the object, otherwise, if object implements reflection interface,
// the named service call attempt is performed
//
#define ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, NativeServiceCall, ReflectedServiceCall) \
do { if( IntfPtr ) { \
  NativeIntfT ::Ptr nativeIntf = IntfPtr; \
  if(nativeIntf) { \
    nativeIntf-> NativeServiceCall ; \
  } else { \
    EsReflectedClassIntf::Ptr reflectedIntf = IntfPtr; \
    if(reflectedIntf) { \
      reflectedIntf-> ReflectedServiceCall ; } \
    } } \
} while(0)

#define ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, NativeServiceCall, ReflectedServiceCall) \
do { if( IntfPtr ) { \
  NativeIntfT ::Ptr nativeIntf = IntfPtr; \
  if(nativeIntf) { \
    RetVar = nativeIntf-> NativeServiceCall ; \
  } else { \
    EsReflectedClassIntf::Ptr reflectedIntf = IntfPtr; \
    if(reflectedIntf) { \
      RetVar = reflectedIntf-> ReflectedServiceCall ; } \
    } } \
} while(0)

#define ES_NC_INTERNAL0(Service) Service()
#define ES_RC_INTERNAL0(Service) call( esT( #Service ) )
#define ES_NC_INTERNAL1(Service, p0) Service( p0 )
#define ES_RC_INTERNAL1(Service, p0) call( esT( #Service ), (p0) )
#define ES_NC_INTERNAL2(Service, p0, p1) Service( (p0), (p1) )
#define ES_RC_INTERNAL2(Service, p0, p1) call( esT( #Service ), (p0), (p1) )
#define ES_NC_INTERNAL3(Service, p0, p1, p2) Service( (p0), (p1), (p2) )
#define ES_RC_INTERNAL3(Service, p0, p1, p2) call( esT( #Service ), (p0), (p1), (p2) )
#define ES_NC_INTERNAL4(Service, p0, p1, p2, p3) Service( (p0), (p1), (p2), (p3) )
#define ES_RC_INTERNAL4(Service, p0, p1, p2, p3) call( esT( #Service ), (p0), (p1), (p2), (p3) )
#define ES_NC_INTERNAL5(Service, p0, p1, p2, p3, p4) Service( (p0), (p1), (p2), (p3), (p4) )
#define ES_RC_INTERNAL5(Service, p0, p1, p2, p3, p4) call( esT( #Service ), (p0), (p1), (p2), (p3), (p4) )

// dual call end-user macros
//
// no-return types
#define ES_DUAL_CALL0_NR(IntfPtr, NativeIntfT, Service) ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, ES_NC_INTERNAL0(Service), ES_RC_INTERNAL0(Service))
#define ES_DUAL_CALL1_NR(IntfPtr, NativeIntfT, Service, p0) ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, ES_NC_INTERNAL1(Service, p0), ES_RC_INTERNAL1(Service, p0))
#define ES_DUAL_CALL2_NR(IntfPtr, NativeIntfT, Service, p0, p1) ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, ES_NC_INTERNAL2(Service, p0, p1), ES_RC_INTERNAL2(Service, p0, p1))
#define ES_DUAL_CALL3_NR(IntfPtr, NativeIntfT, Service, p0, p1, p2) ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, ES_NC_INTERNAL3(Service, p0, p1, p2), ES_RC_INTERNAL3(Service, p0, p1, p2))
#define ES_DUAL_CALL4_NR(IntfPtr, NativeIntfT, Service, p0, p1, p2, p3) ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, ES_NC_INTERNAL4(Service, p0, p1, p2, p3), ES_RC_INTERNAL4(Service, p0, p1, p2, p3))
#define ES_DUAL_CALL5_NR(IntfPtr, NativeIntfT, Service, p0, p1, p2, p3, p4) ES_DUAL_CALL_INTERNAL_NR(IntfPtr, NativeIntfT, ES_NC_INTERNAL5(Service, p0, p1, p2, p3, p4), ES_RC_INTERNAL5(Service, p0, p1, p2, p4, p5))
// return types
#define ES_DUAL_CALL0(RetVar, IntfPtr, NativeIntfT, Service) ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, ES_NC_INTERNAL0(Service), ES_RC_INTERNAL0(Service))
#define ES_DUAL_CALL1(RetVar, IntfPtr, NativeIntfT, Service, p0) ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, ES_NC_INTERNAL1(Service, p0), ES_RC_INTERNAL1(Service, p0))
#define ES_DUAL_CALL2(RetVar, IntfPtr, NativeIntfT, Service, p0, p1) ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, ES_NC_INTERNAL2(Service, p0, p1), ES_RC_INTERNAL2(Service, p0, p1))
#define ES_DUAL_CALL3(RetVar, IntfPtr, NativeIntfT, Service, p0, p1, p2) ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, ES_NC_INTERNAL3(Service, p0, p1, p2), ES_RC_INTERNAL3(Service, p0, p1, p2))
#define ES_DUAL_CALL4(RetVar, IntfPtr, NativeIntfT, Service, p0, p1, p2, p3) ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, ES_NC_INTERNAL4(Service, p0, p1, p2, p3), ES_RC_INTERNAL4(Service, p0, p1, p2, p3))
#define ES_DUAL_CALL5(RetVar, IntfPtr, NativeIntfT, Service, p0, p1, p2, p3, p4) ES_DUAL_CALL_INTERNAL(RetVar, IntfPtr, NativeIntfT, ES_NC_INTERNAL5(Service, p0, p1, p2, p3, p4), ES_RC_INTERNAL5(Service, p0, p1, p2, p4, p5))

#endif //  _reflection_dual_call_defs_h_