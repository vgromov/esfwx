#ifdef ESSCRIPT_MACHINE_USE_TRACE
#  ifdef ESSCRIPT_MACHINE_USE_DATA_TRACE
#    define ESSCRIPT_MACHINE_TRACE1(a)                     ES_DEBUG_TRACE(a);
#    define ESSCRIPT_MACHINE_TRACE2(a, b)                  ES_DEBUG_TRACE((a), (b));
#    define ESSCRIPT_MACHINE_TRACE3(a, b, c)               ES_DEBUG_TRACE((a), (b), (c));
#    define ESSCRIPT_MACHINE_TRACE4(a, b, c, d)            ES_DEBUG_TRACE((a), (b), (c), (d));
#    define ESSCRIPT_MACHINE_TRACE5(a, b, c, d, e)         ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#    define ESSCRIPT_MACHINE_TRACE6(a, b, c, d, e, f)      ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#  else
#    define ESSCRIPT_MACHINE_TRACE1(a)
#    define ESSCRIPT_MACHINE_TRACE2(a, b)
#    define ESSCRIPT_MACHINE_TRACE3(a, b, c)
#    define ESSCRIPT_MACHINE_TRACE4(a, b, c, d)
#    define ESSCRIPT_MACHINE_TRACE5(a, b, c, d, e)
#    define ESSCRIPT_MACHINE_TRACE6(a, b, c, d, e, f)
#  endif
#  ifdef ESSCRIPT_MACHINE_USE_CALL_TRACE
#    define ESSCRIPT_MACHINE_CALL_TRACE1(a)                ES_DEBUG_TRACE(a);
#    define ESSCRIPT_MACHINE_CALL_TRACE2(a, b)             ES_DEBUG_TRACE((a), (b));
#    define ESSCRIPT_MACHINE_CALL_TRACE3(a, b, c)          ES_DEBUG_TRACE((a), (b), (c));
#    define ESSCRIPT_MACHINE_CALL_TRACE4(a, b, c, d)       ES_DEBUG_TRACE((a), (b), (c), (d));
#    define ESSCRIPT_MACHINE_CALL_TRACE5(a, b, c, d, e)    ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#    define ESSCRIPT_MACHINE_CALL_TRACE6(a, b, c, d, e, f) ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#  else
#    define ESSCRIPT_MACHINE_CALL_TRACE1(a)
#    define ESSCRIPT_MACHINE_CALL_TRACE2(a, b)
#    define ESSCRIPT_MACHINE_CALL_TRACE3(a, b, c)
#    define ESSCRIPT_MACHINE_CALL_TRACE4(a, b, c, d)
#    define ESSCRIPT_MACHINE_CALL_TRACE5(a, b, c, d, e)
#    define ESSCRIPT_MACHINE_CALL_TRACE6(a, b, c, d, e, f)
#  endif
#else
#  define ESSCRIPT_MACHINE_TRACE1(a)
#  define ESSCRIPT_MACHINE_TRACE2(a, b)
#  define ESSCRIPT_MACHINE_TRACE3(a, b, c)
#  define ESSCRIPT_MACHINE_TRACE4(a, b, c, d)
#  define ESSCRIPT_MACHINE_TRACE5(a, b, c, d, e)
#  define ESSCRIPT_MACHINE_TRACE6(a, b, c, d, e, f)
#  define ESSCRIPT_MACHINE_CALL_TRACE1(a)
#  define ESSCRIPT_MACHINE_CALL_TRACE2(a, b)
#  define ESSCRIPT_MACHINE_CALL_TRACE3(a, b, c)
#  define ESSCRIPT_MACHINE_CALL_TRACE4(a, b, c, d)
#  define ESSCRIPT_MACHINE_CALL_TRACE5(a, b, c, d, e)
#  define ESSCRIPT_MACHINE_CALL_TRACE6(a, b, c, d, e, f)
#endif
