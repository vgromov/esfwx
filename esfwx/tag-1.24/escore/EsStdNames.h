#ifndef _es_std_names_h_
#define _es_std_names_h_

// Forward decl
class EsString;

/// Standard and|or reflection-specific object service names
///
namespace EsStdNames
{
/// Reflected constructor and destructor names
///
ESCORE_FUNC( const EsString&, reflectedCtr() ES_NOTHROW );
ESCORE_FUNC( const EsString&, reflectedDtr() ES_NOTHROW );

ESCORE_FUNC( const EsString&, _true() ES_NOTHROW  );
ESCORE_FUNC( const EsString&, _false() ES_NOTHROW );
ESCORE_FUNC( const EsByteString&, _bfalse() ES_NOTHROW );
ESCORE_FUNC( const EsString&, asString() ES_NOTHROW );
ESCORE_FUNC( const EsString&, compare() ES_NOTHROW  );
ESCORE_FUNC( const EsString&, has() ES_NOTHROW  );
ESCORE_FUNC( const EsString&, add() ES_NOTHROW  );
ESCORE_FUNC( const EsString&, add_inplace() ES_NOTHROW );
ESCORE_FUNC( const EsString&, subtract() ES_NOTHROW );
ESCORE_FUNC( const EsString&, subtract_inplace() ES_NOTHROW );
ESCORE_FUNC( const EsString&, multiply() ES_NOTHROW );
ESCORE_FUNC( const EsString&, multiply_inplace() ES_NOTHROW );
ESCORE_FUNC( const EsString&, divide() ES_NOTHROW );
ESCORE_FUNC( const EsString&, divide_inplace() ES_NOTHROW );
ESCORE_FUNC( const EsString&, countGet() ES_NOTHROW );
ESCORE_FUNC( const EsString&, countSet() ES_NOTHROW );
ESCORE_FUNC( const EsString&, itemGet() ES_NOTHROW );
ESCORE_FUNC( const EsString&, itemSet() ES_NOTHROW );
ESCORE_FUNC( const EsString&, itemDelete() ES_NOTHROW );
ESCORE_FUNC( const EsString&, itemAppend() ES_NOTHROW );
ESCORE_FUNC( const EsString&, setToNull() ES_NOTHROW );
ESCORE_FUNC( const EsString&, value() ES_NOTHROW );
ESCORE_FUNC( const EsString&, version() ES_NOTHROW );
ESCORE_FUNC( const EsString&, label() ES_NOTHROW );
ESCORE_FUNC( const EsString&, buffer() ES_NOTHROW);
}

#endif // _es_std_names_h_
