#ifndef _es_os_info_h_
#define _es_os_info_h_

/// @file EsOsInfo.h
/// Reflected OS and platform identification, and checking utilities
///

/// Platform types
enum class EsOsPlatfrom : ulong
{
  Unknown,
  Windows,
  MacOS,
  iOS,
  Android,
  Linux
};

/// Platform architectures
enum class EsOsArchitecture : ulong
{
  Unknown,
  IntelX86,
  IntelAMDx64,
  IAx64,
  ARM32,
  ARM64
};

ES_DECL_EXPORTED_REFLECTED_SERVICES_BEGIN(EsOsInfo, ESCORE_CLASS)

public:
  /// Compliance check services
  ///

  /// Check partial compliance, for major version
	ES_DECL_REFLECTED_CLASS_METHOD1(long, compare, ulong);
  /// Check partial compliance, for major and minor version
	ES_DECL_REFLECTED_CLASS_METHOD2(long, compare, ulong, ulong);
  /// Check partial compliance, for major, minor versions, and SP major version (if latter makes sence)
	ES_DECL_REFLECTED_CLASS_METHOD3(long, compare, ulong, ulong, ulong);
  /// Check full compliance, for major, minor versions, and SP major and minor versions (if latter makes sence)
	ES_DECL_REFLECTED_CLASS_METHOD4(long, compare, ulong, ulong, ulong, ulong);

  /// Return OS info string
	ES_DECL_REFLECTED_CLASS_METHOD0(cr_EsString, asString);

  /// Return OS name
 	ES_DECL_REFLECTED_CLASS_METHOD0(cr_EsString, nameGet);

  /// Return OS SP name
 	ES_DECL_REFLECTED_CLASS_METHOD0(cr_EsString, spNameGet);

  /// Info access services
  ///

  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, architectureGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, platformGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, buildGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, majorGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, minorGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, spMajorGet);
  ES_DECL_REFLECTED_CLASS_METHOD0(ulong, spMinorGet);

ES_DECL_REFLECTED_SERVICES_END

namespace EsReflection
{

/// Reflected platform type
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsOsPlatfrom)
	ES_DECL_ENUMERATION_ITEM(Unknown,							  static_cast<ulong>(::EsOsPlatfrom::Unknown))
	ES_DECL_ENUMERATION_ITEM(Windows,							  static_cast<ulong>(::EsOsPlatfrom::Windows))
	ES_DECL_ENUMERATION_ITEM(MacOS,							    static_cast<ulong>(::EsOsPlatfrom::MacOS))
	ES_DECL_ENUMERATION_ITEM(iOS,							      static_cast<ulong>(::EsOsPlatfrom::iOS))
	ES_DECL_ENUMERATION_ITEM(Android,							  static_cast<ulong>(::EsOsPlatfrom::Android))
	ES_DECL_ENUMERATION_ITEM(Linux,							    static_cast<ulong>(::EsOsPlatfrom::Linux))
ES_DECL_ENUMERATION_END

/// Reflected architectures
///
ES_DECL_EXPORTED_ENUMERATION_BEGIN(ESCORE_CLASS, EsOsArchitecture)
	ES_DECL_ENUMERATION_ITEM(Unknown,							  static_cast<ulong>(::EsOsArchitecture::Unknown))
  ES_DECL_ENUMERATION_ITEM(IntelX86,              static_cast<ulong>(::EsOsArchitecture::IntelX86))
  ES_DECL_ENUMERATION_ITEM(IntelAMDx64,           static_cast<ulong>(::EsOsArchitecture::IntelAMDx64))
  ES_DECL_ENUMERATION_ITEM(IAx64,                 static_cast<ulong>(::EsOsArchitecture::IAx64))
	ES_DECL_ENUMERATION_ITEM(ARM32, 							  static_cast<ulong>(::EsOsArchitecture::ARM32))
	ES_DECL_ENUMERATION_ITEM(ARM64, 							  static_cast<ulong>(::EsOsArchitecture::ARM64))
ES_DECL_ENUMERATION_END

} // namespace EsReflection

#endif // _es_os_info_h_
