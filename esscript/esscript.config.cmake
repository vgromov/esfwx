# ESSCRIPT component version definitions
set(component_DESCRIPTION_STRING "ESFWX Script Library")
set(component_INTERNAL_NAME "esscript")
set(component_ORIGINAL_NAME "esscript")
set(component_BUILD_NUM	10)

# Configuration parameters
#
set(ESSCRIPT_USE_COMPILER 1 CACHE BOOL "Use script && scriptlet compiler functionality")

set(ESSCRIPT_TRACE_COMPILER 0 CACHE BOOL "Use compiler functionality trace in Debug mode")
set(ESSCRIPT_TRACE_VMACHINE 0 CACHE BOOL "Use virtual machine trace in Debug mode")
set(ESSCRIPT_TRACE_INSTRUCTION 0 CACHE BOOL "Use virtual machine instruction trace in Debug mode")
set(ESSCRIPT_TRACE_USAGE 0 CACHE BOOL "Use virtual machine internal data trace in Debug mode")
set(ESSCRIPT_TRACE_CALLS 0 CACHE BOOL "Use methods calls trace in Debug mode")
set(ESSCRIPT_TRACE_CTX_DOCALL 0 CACHE BOOL "Use Script Context doCall gateway trace in Debug mode")
set(ESSCRIPT_TRACE_CODESECTION 0 CACHE BOOL "Use Script Code Section operations trace in Debug mode")
set(ESSCRIPT_TRACE_STACK 0 CACHE BOOL "Use script data stack trace in Debug mode")
set(ESSCRIPT_TRACE_OBJECTS 0 CACHE BOOL "Trace scripted object operations in Debug mode")
set(ESSCRIPT_TRACE_OBJECT_BUFFER 0 CACHE BOOL "Trace script object buffer updates in Debug mode")
set(ESSCRIPT_TRACE_VALACCESS 0 CACHE BOOL "Trace script value accessors operations in Debug mode")
set(ESSCRIPT_TRACE_COMPILED 0 CACHE BOOL "Trace compiled AST and binary structure in Debug mode")
set(ESSCRIPT_DEBUG_LOG_TRACE 0 CACHE BOOL "If TRUE, EsScriptDebug::log messages got broadcasted to IDE's debugger output, if available")

# Prepare configuration variables
#
set(ES_USE_COMPILER_IMPLEMENTATION ${ESSCRIPT_USE_COMPILER})

set(ESSCRIPT_COMPILER_USE_TRACE ${ESSCRIPT_TRACE_COMPILER})
set(ESSCRIPT_MACHINE_USE_TRACE ${ESSCRIPT_TRACE_VMACHINE})
set(ESSCRIPT_INSTRUCTION_USE_TRACE ${ESSCRIPT_TRACE_INSTRUCTION})
set(ESSCRIPT_MACHINE_USE_DATA_TRACE ${ESSCRIPT_TRACE_USAGE})
set(ESSCRIPT_MACHINE_USE_CALL_TRACE ${ESSCRIPT_TRACE_CALLS})
set(ESSCRIPT_CONTEXT_DOCALL_TRACE ${ESSCRIPT_TRACE_CTX_DOCALL})
set(ESSCRIPT_CODESECTION_USE_TRACE ${ESSCRIPT_TRACE_CODESECTION})
set(ESSCRIPT_DATA_STACK_USE_TRACE ${ESSCRIPT_TRACE_STACK})
set(ESSCRIPT_OBJECT_USE_TRACE ${ESSCRIPT_TRACE_OBJECTS})
set(ESSCRIPT_OBJECT_BUFFER_USE_TRACE ${ESSCRIPT_TRACE_OBJECT_BUFFER})
set(ESSCRIPT_VALACCESS_USE_TRACE ${ESSCRIPT_TRACE_VALACCESS})
set(ESSCRIPT_COMPILED_USE_TRACE ${ESSCRIPT_TRACE_COMPILED})
set(ESSCRIPT_USE_DEBUG_LOG_TRACE ${ESSCRIPT_DEBUG_LOG_TRACE})

# Set-up ESSCRIPT binary version naming
#
set(ESSCRIPT_VERSION_SUFFIX "${ESFWX_VERSION_SUFFIX}_${component_BUILD_NUM}" CACHE INTERNAL "")
