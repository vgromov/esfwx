#ifndef _es_script_std_names_h_
#define _es_script_std_names_h_

/// Script standard symbol names
///
namespace EsScriptStdNames
{
ESSCRIPT_FUNC( const EsString&, startup() ES_NOTHROW );
ESSCRIPT_FUNC( const EsString&, version() ES_NOTHROW );
ESSCRIPT_FUNC( const EsString&, metaclass() ES_NOTHROW );
ESSCRIPT_FUNC( const EsString&, scriptHost() ES_NOTHROW );
ESSCRIPT_FUNC( const EsString&, null() ES_NOTHROW );
ESSCRIPT_FUNC( const EsString&, exception() ES_NOTHROW );
ESSCRIPT_FUNC( const EsString&, _this() ES_NOTHROW );
}

#endif // _es_script_std_names_h_
