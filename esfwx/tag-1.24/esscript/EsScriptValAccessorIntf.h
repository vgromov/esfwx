#ifndef _es_val_accessors_h_
#define _es_val_accessors_h_

// misc symbol flags
namespace EsScriptSymbolFlag
{
	enum
	{
		None		    	= 0x00,
		BuiltIn	    	= 0x01,
		ReadOnly    	= 0x02,
    ExternEnum    = 0x04,
    ExternObject  = 0x08,
	};
}

// special value accessor objects, used internally to provide transparent item|field|property access
// in lhs and rhs expressions
//
ES_DECL_INTF_BEGIN2(35E7B5D4, D6D8400f, BA799EED, CC38B82E, EsScriptValAccessorIntf, EsBaseIntf)
	// checks, flags
	ES_DECL_INTF_METHOD(bool, isOk)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(bool, getReturnsTemporary)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(bool, isBuiltIn)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(bool, isReadOnly)() const ES_NOTHROW = 0;
  ES_DECL_INTF_METHOD(ulong, flagsGet)() const ES_NOTHROW = 0;
	// access
	ES_DECL_INTF_METHOD(const EsString&, nameGet)() const ES_NOTHROW = 0;
	ES_DECL_INTF_METHOD(EsVariant&, get)() = 0;
	ES_DECL_INTF_METHOD(void, set)(const EsVariant& val) = 0;
	// debug
	ES_DECL_INTF_METHOD(EsString, trace)() const = 0;
	// for member var accessors only
	ES_DECL_INTF_METHOD(void, updateSubscriberAdd)(EsScriptObjectIntf* subscriber) = 0;
ES_DECL_INTF_END

// value accessors debug trace
//
#ifdef ESSCRIPT_VALACCESS_USE_TRACE
#	define ESSCRIPT_VALACCESS_TRACE1(a)									ES_DEBUG_TRACE(a);
#	define ESSCRIPT_VALACCESS_TRACE2(a, b)							ES_DEBUG_TRACE((a), (b));
#	define ESSCRIPT_VALACCESS_TRACE3(a, b, c)						ES_DEBUG_TRACE((a), (b), (c));
#	define ESSCRIPT_VALACCESS_TRACE4(a, b, c, d)				ES_DEBUG_TRACE((a), (b), (c), (d));
#	define ESSCRIPT_VALACCESS_TRACE5(a, b, c, d, e)			ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#	define ESSCRIPT_VALACCESS_TRACE6(a, b, c, d, e, f)	ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#else
#	define ESSCRIPT_VALACCESS_TRACE1(a)
#	define ESSCRIPT_VALACCESS_TRACE2(a, b)
#	define ESSCRIPT_VALACCESS_TRACE3(a, b, c)
#	define ESSCRIPT_VALACCESS_TRACE4(a, b, c, d)
#	define ESSCRIPT_VALACCESS_TRACE5(a, b, c, d, e)
#	define ESSCRIPT_VALACCESS_TRACE6(a, b, c, d, e, f)
#endif

#ifdef DEBUG
#	define ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(v)		if( (v).isObject() ) { EsScriptValAccessorIntf::Ptr acc = (v).asObject(); if( acc ) ES_FAIL_MSG(esT("value is value accessor")); }
#else
#	define ESSCRIPT_VALACCESS_ASSERT_NOT_ACCESSOR(v)
#endif

#endif // _es_val_accessors_h_
