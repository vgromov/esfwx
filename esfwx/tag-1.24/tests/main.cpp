// our includes
#include <escore/escore.h>
#include <esmath/esmath.h>
#include <escomm/escomm.h>
#include <esscript/esscript.h>
#pragma hdrstop

#include <sstream>

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#include "autolink.cxx"

// test engine includes
#ifdef ES_USE_CPPTEST
#  include "cpptest.h"
  Test::Suite g_allTests;
#else
#  define GTEST_HAS_TR1_TUPLE 0
#  include "src/gtest-all.cc"
# define PRINTF(...)  do { testing::internal::ColoredPrintf(testing::internal::COLOR_GREEN, "[          ] "); testing::internal::ColoredPrintf(testing::internal::COLOR_YELLOW, __VA_ARGS__); } while(0)
#endif

#if ES_OS == ES_OS_WINDOWS
static TCHAR** s_argv;
#else
static char** s_argv;
#endif

static EsString binRootGet()
{
  const EsPath& path = EsPath::createFromFilePath( s_argv[0] );
  EsString fpath = path.pathGet();
  size_t pos = fpath.find(esT("bin"));
  if( pos != EsString::npos )
  {
    fpath = fpath.substr(0, pos+3);
    fpath += EsPath::c_nativeSeparator;
  }

  return fpath;
}


// test includes
#include "testEsOsInfo.cc"
#include "testEsRegEx.cc"
#include "testBasicTypes.cc"
#include "testEsUtilities.cc"
#include "testEsReflection.cc"
#include "testEsDateTime.cc"
#include "testEsString.cc"
#include "testEsCryptoDesLegacy.cc"

#ifdef ES_USE_CRYPTO
# include "testEsCryptoDes.cc"
# include "testEsCryptoAes.cc"
#endif

#if ES_OS == ES_OS_WINDOWS
# include "testEsFile.cc"
#elif ES_OS == ES_OS_MAC
# include "testMacosFile.cc"
#endif

#include "testEsXml.cc"
#include "testEsStreamBinary.cc"
#include "testEsSyncObjects.cc"
#include "testEsThread.cc"

// Uncomment to repeat each script profiling test ES_SCRIPT_REPEAT_CNT times
//
#define ES_TEST_DO_SCRIPT_PROFILING
#define ES_SCRIPT_REPEAT_CNT 10

// Selective script profiling tests
//
//#define ES_SCRIPT_PROFILE_BASIC_FOR_LOOP
//#define ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_WITH_NAMED_CONST_CNT
//#define ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF
//#define ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF_WITH_NAMED_CNT
//#define ES_SCRIPT_PROFILE_BASIC_FOR_LOOP_FILL_BUFF
//#define ES_SCRIPT_PROFILE_FOR_LOOP_REFLECTED_STATIC_CALL_RET_NO_PARAMS
//#define ES_SCRIPT_PROFILE_FOR_LOOP_REFLECTED_STATIC_CALL_RET_4_PARAMS
//#define ES_SCRIPT_PROFILE_OBJ_COLLECTION_INIT
//#define ES_SCRIPT_PROFILE_OBJ_COLLECTION_INIT_WITH_MIDDLE_FLD_ASSIGN
//#define ES_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT
//#define ES_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT_WITH_THIS_FLD_ASSIGN
//#define ES_SCRIPT_PROFILE_DERIVED_OBJ_COLLECTION_INIT_WITH_BASE_FLD_ASSIGN
//#define ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT
//#define ES_SCRIPT_PROFILE_DERIVED_OBJ_ARRAY_INIT
//#define ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT_IDX_ITERATION_FLD_ASN
//#define ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT_IDX_ITERATION_CACHED_CNT_AND_FLD_ASN
//#define ES_SCRIPT_PROFILE_OBJ_ARRAY_INIT_FOREACH_ITERATION_CACHED_CNT_AND_FLD_ASN
//#include "testEsScriptProfile.cc"

#ifdef ES_COMM_USE_UART
# include "testUart.cc"
#endif

#if ES_OS != ES_OS_MAC
#  ifdef ES_USE_BT_TESTS
#   ifdef ES_COMM_USE_BLUETOOTH
#     include "testBtConfig.cc"
#     include "testEsBluetooth.cc"
#   endif
#  endif
#endif

#ifdef ES_COMM_USE_SOCKETS
# include "testEsSocket.cc"
# include "testEsSocketChannel.cc"
# include "testEsSocketServer.cc"
#endif

#if ES_OS == ES_OS_WINDOWS
int _tmain(int argc, TCHAR **argv)
#else
int main(int argc, char **argv)
#endif
{
  ES_DEBUG_TRACE_INIT;

  s_argv = argv;

  escore::reflectionInit();
  esmath::reflectionInit();
  escomm::reflectionInit();
  esscript::reflectionInit();

  int result = 0;

  EsVariant vss = esVT("strt");
  EsVariant vs0 = esVT("00strtest00");
  EsVariant vdt = EsDateTime::toVariant(
    EsDateTime::now()
  );
  EsVariant vs1 = esVT("11strtest11");

  EsVariant::Array va;
  va.reserve(8);

  va.insert(
    va.begin(),
    vdt
  );
  va.insert(
    va.begin(),
    vss
  );
  va.insert(
    va.begin(),
    vs0
  );
  va.insert(
    va.begin(),
    vs1
  );
  const EsString& trace = EsScriptMachine::traceVariant(va);

  ES_DEBUG_TRACE(esT("EsCriticalSection size: %d"), sizeof(EsCriticalSection));
  ES_DEBUG_TRACE(esT("EsString size: %d"), sizeof(EsString));
  ES_DEBUG_TRACE(esT("EsVariant size: %d"), sizeof(EsVariant));
  ES_DEBUG_TRACE(esT("EsBinBuffer size: %d"), sizeof(EsBinBuffer));
  ES_DEBUG_TRACE(esT("EsStringIndexedMap size: %d"), sizeof(EsStringIndexedMap));
  ES_DEBUG_TRACE(esT("EsStringIndexedMap::EsStringAssocMapT size: %d"), sizeof(EsStringIndexedMap::EsStringAssocMapT));
  ES_DEBUG_TRACE(esT("EsStringIndexedMap::EsStringAssocVectorT size: %d"), sizeof(EsStringIndexedMap::EsStringAssocVectorT));
  ES_DEBUG_TRACE(esT("EsReflectedClassIntf size: %d"), sizeof(EsReflectedClassIntf));
  ES_DEBUG_TRACE(esT("EsAttributesIntf::Ptr size: %d"), sizeof(EsAttributesIntf::Ptr));
  ES_DEBUG_TRACE(esT("EsEnumeration size: %d"), sizeof(EsEnumeration));
  ES_DEBUG_TRACE(esT("EsDateTime size: %d"), sizeof(EsDateTime));
  ES_DEBUG_TRACE(esT("EsMathArrayReal size: %d"), sizeof(EsMathArrayReal));

  try
  {

#ifdef ES_COMM_USE_SOCKETS
  // socket initializer will initialize BT as well, if it's enabled
  EsSockets::initialize();
#endif

#ifdef ES_USE_CPPTEST
    Test::TextOutput output( Test::TextOutput::Verbose );
    result = g_allTests.run( output ) ? EXIT_SUCCESS : EXIT_FAILURE;
#else
    ::testing::InitGoogleTest( &argc, argv );
    result = RUN_ALL_TESTS();
#endif
  }
  catch( EsException& ex )
  {
    PRINTF(
      EsString::toUtf8(
        ex.messageGet()
      ).c_str()
    );
  }
  catch(...)
  {
    PRINTF( "Unknown exception occurred" );
  }

#ifdef ES_COMM_USE_SOCKETS
  EsSockets::uninitialize();
#endif

  return result;
}

