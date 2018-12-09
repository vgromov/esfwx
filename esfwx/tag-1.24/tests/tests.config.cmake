# Tests configuration
set(ESTESTS_USE_BASIC 1 CACHE BOOL "Include basic functionality tests")
set(ESTESTS_USE_MULTITHREAD 1 CACHE BOOL "Include MT Synchro objects and MT tests")
set(ESTESTS_USE_CRYPTO 1 CACHE BOOL "Include Crypto tests, both legacy and full")
set(ESTESTS_USE_FILE 1 CACHE BOOL "Include file tests")
set(ESTESTS_USE_STREAM 1 CACHE BOOL "Include Stream object tests")
set(ESTESTS_USE_UART 0 CACHE BOOL "Include UART tests")
set(ESTESTS_USE_BT 0 CACHE BOOL "Include Bluetooth Classic tests")
set(ESTESTS_USE_SOCKETS 0 CACHE BOOL "Include Socket tests")

# Script profiling tests
set(ESTESTS_USE_SCRIPT_PROFILE 0 CACHE BOOL "Include script profiling tests (see profiling.ess script source)")
set(ESTESTS_SCRIPT_PROFILE_REPEAT_CNT 10 CACHE STRING "How many times to repeat each script profiling test")
set(ESTESTS_SCRIPT_PROFILE_BASIC_FOR_LOOP 1 CACHE BOOL "Include basic for loop profiling") 
set(ESTESTS_SCRIPT_PROFILE_BASIC_FOR_LOOP_WITH_NAMED_CONST_CNT 1 CACHE BOOL "Include basic for loop profiling with named const is used as max count value")
set(ESTESTS_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF 1 CACHE BOOL "Include loop with buffer fill-in by adding: +=")
set(ESTESTS_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF_WITH_NAMED_CNT 1 CACHE BOOL "Include loop with buffer fill-in by adding: +=, with named const is used as max count value")
set(ESTESTS_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF_WITH_PREALLOC 1 CACHE BOOL "Include loop with preallocated buffer fill-in by direc indexed item assigning")
set(ESTESTS_SCRIPT_PROFILE_FOR_LOOP_REFLECTED_STATIC_CALL_RET_NO_PARAMS 1 CACHE BOOL "Include static call profiling, no parameters")
set(ESTESTS_SCRIPT_PROFILE_FOR_LOOP_REFLECTED_STATIC_CALL_RET_4_PARAMS 1 CACHE BOOL "Include static call profiling, with 4 parameters")
set(ESTESTS_SCRIPT_PROFILE_OBJ_COLLECTION_INIT 1 CACHE BOOL "Include script collection object initialization, simple data object is used")
set(ESTESTS_SCRIPT_PROFILE_OBJ_COLLECTION_INIT_WITH_MIDDLE_FLD_ASSIGN 1 CACHE BOOL "Include script collection object initialization, simple data object is used, single field access and assign")
set(ESTESTS_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT 1 CACHE BOOL "Include script derived object collection initialization")
set(ESTESTS_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT_WITH_THIS_FLD_ASSIGN 1 CACHE BOOL "Include script derived object collection initialization, field access and assign on derived object")
set(ESTESTS_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT_WITH_BASE_FLD_ASSIGN 1 CACHE BOOL "Include script derived object collection initialization, field access and assign on base object")
set(ESTESTS_SCRIPT_PROFILE_OBJ_ARRAY_INIT 1 CACHE BOOL "Include script object array initialization")
set(ESTESTS_SCRIPT_PROFILE_DERIVED_OBJ_ARRAY_INIT 1 CACHE BOOL "Include derived script object array initialization")
set(ESTESTS_SCRIPT_PROFILE_OBJ_ARRAY_INIT_IDX_ITERATION_FLD_ASN 1 CACHE BOOL "Include script object array initialization + iterate every item within the array and assign field value")
set(ESTESTS_SCRIPT_PROFILE_OBJ_ARRAY_INIT_IDX_ITERATION_CACHED_CNT_AND_FLD_ASN 1 CACHE BOOL "Include script object array initialization + iterate every item within the array and assign field value. Array count is cached into local variable")
set(ESTESTS_SCRIPT_PROFILE_OBJ_ARRAY_INIT_FOREACH_ITERATION_AND_FLD_ASN 1 CACHE BOOL "Include script object array initialization + foreach iterate every item within the array and assign field value.")

# Extra configs
set(ESTESTS_USE_CPPTEST 0 CACHE BOOL "Use CPP_TEST library to run testbed, otherwise, Google tests is used. Default=0, i.e Google test is used")
set(ESTESTS_USE_STACK_BACKTRACE 1 CACHE BOOL "Use backward-cpp facility to print pretty CALL and STACK backtrace on test crash. For X-OSes only")

# Prepare config vars
#
set(ES_USE_CPPTEST ${ESTESTS_USE_CPPTEST})

# Set-up Tests binary version naming
#
set(ESTESTS_VERSION_SUFFIX "${ESFWX_VERSION_SUFFIX}")
