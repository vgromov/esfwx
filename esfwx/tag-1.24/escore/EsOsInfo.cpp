#include "escorepch.h"
#pragma hdrstop

#include "EsOsInfo.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

namespace EsReflection
{
  ES_DECL_ENUMERATION_INFO(EsOsPlatfrom, NO_CLASS_DESCR)
  ES_DECL_ENUMERATION_INFO(EsOsArchitecture, NO_CLASS_DESCR)
}
//---------------------------------------------------------------------------

ES_DECL_REFLECTED_SERVICES_INFO_BEGIN(EsOsInfo, NO_CLASS_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, compare, long_ClassCall_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, compare, long_ClassCall_ulong_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, compare, long_ClassCall_ulong_ulong_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, compare, long_ClassCall_ulong_ulong_ulong_ulong, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, asString, cr_EsString_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, nameGet, cr_EsString_ClassCall, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, spNameGet, cr_EsString_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, architectureGet, ulong_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, platformGet, ulong_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, buildGet, ulong_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, majorGet, ulong_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, minorGet, ulong_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, spMajorGet, ulong_ClassCall, NO_METHOD_DESCR)
  ES_DECL_REFLECTED_CLASS_METHOD_INFO_STD(EsOsInfo, spMinorGet, ulong_ClassCall, NO_METHOD_DESCR)
ES_DECL_REFLECTED_SERVICES_INFO_END
//---------------------------------------------------------------------------

long EsOsInfo::compare(ulong major)
{
  ulong _major = majorGet();
  return (_major < major) ?
    -1 :
    ((_major > major) ?
      1 :
      0
    );
}
//---------------------------------------------------------------------------

long EsOsInfo::compare(ulong major, ulong minor)
{
  long result = compare(major);
  if( 0 == result )
  {
    ulong _minor = minorGet();
    result = (_minor < minor) ?
      -1 :
      ((_minor > minor) ?
        1 :
        0
      );
  }

  return result;
}
//---------------------------------------------------------------------------

long EsOsInfo::compare(ulong major, ulong minor, ulong spMajor)
{
  long result = compare(major, minor);
  if( 0 == result )
  {
    ulong _spMajor = spMajorGet();
    result = (_spMajor < spMajor) ?
      -1 :
      ((_spMajor > spMajor) ?
        1 :
        0
      );
  }

  return result;
}
//---------------------------------------------------------------------------

long EsOsInfo::compare(ulong major, ulong minor, ulong spMajor, ulong spMinor)
{
  long result = compare(major, minor, spMajor);
  if( 0 == result )
  {
    ulong _spMinor = spMinorGet();
    result = (_spMinor < spMinor) ?
      -1 :
      ((_spMinor > spMinor) ?
        1 :
        0
      );
  }

  return result;
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsOsInfo.win.cxx"
#elif ES_OS == ES_OS_MAC
# include "EsOsInfo.mac.cxx"
#elif ES_OS == ES_OS_IOS
# include "EsOsInfo.ios.cxx"
#elif ES_OS == ES_OS_ANDROID
# include "EsOsInfo.android.cxx"
#elif ES_OS == ES_OS_LINUX
# include "EsOsInfo.linux.cxx"
#endif
