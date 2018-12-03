#ifndef _es_script_config_h_
#define _es_script_config_h_

/// Use script compiler functionality
/// if 0 or not defined, compiler and scriptlet components will not be
/// included in target buid. but, there still will be ability to load
/// and execute pre-compiled script binaries
///
#define ES_USE_COMPILER_IMPLEMENTATION 0

// Debug trace definitions
//#define ESSCRIPT_COMPILER_USE_TRACE
#define ESSCRIPT_MACHINE_USE_TRACE
//#	define ESSCRIPT_INSTRUCTION_USE_TRACE
#	define ESSCRIPT_MACHINE_USE_DATA_TRACE
#	define ESSCRIPT_MACHINE_USE_CALL_TRACE
//#define ESSCRIPT_CODESECTION_USE_TRACE
//#define ESSCRIPT_DATA_STACK_USE_TRACE
//#define ESSCRIPT_OBJECT_USE_TRACE
//#define ESSCRIPT_OBJECT_BUFFER_USE_TRACE
#define ESSCRIPT_VALACCESS_USE_TRACE
//#define ESSCRIPT_COMPILED_USE_TRACE

#endif // _es_script_config_h_