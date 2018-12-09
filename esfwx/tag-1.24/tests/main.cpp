// our includes
#include <escore/escore.h>
#include <esmath/esmath.h>
#include <escomm/escomm.h>
#include <esscript/esscript.h>
#pragma hdrstop

#include "testsConfig.h"

#ifdef ESTESTS_USE_VLD
# include <vld.h>
#endif

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

// All tests are included from here
#include "testsSelector.cxx"

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

#ifdef ESTESTS_USE_VLD
    VLDEnable();
#endif

#ifdef ES_USE_CPPTEST
    Test::TextOutput output( Test::TextOutput::Verbose );
    result = g_allTests.run( output ) ? EXIT_SUCCESS : EXIT_FAILURE;
#else
    ::testing::InitGoogleTest( &argc, argv );
    result = RUN_ALL_TESTS();
#endif

#ifdef ESTESTS_USE_VLD
    VLDDisable();
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

