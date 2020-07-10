//---------------------------------------------------------------------------

class EsAndroidOsInfo
{
private:
  EsAndroidOsInfo() :
  m_platform(EsOsPlatfrom::Android),
  m_arch(EsOsArchitecture::Unknown),
  m_build(0),
  m_major(0),
  m_minor(0),
  m_spMajor(0),
  m_spMinor(0),
  m_name(esT("Android"))
  {
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
    switch( TOSVersion::Architecture )
    {
    case TOSVersion::TArchitecture::arIntelX86:
      m_arch = EsOsArchitecture::IntelX86;
      break;
    case TOSVersion::TArchitecture::arIntelX64:
      m_arch = EsOsArchitecture::IntelAMDx64;
      break;
    case TOSVersion::TArchitecture::arARM32:
      m_arch = EsOsArchitecture::ARM32;
      break;
    case TOSVersion::TArchitecture::arARM64:
      m_arch = EsOsArchitecture::ARM64;
      break;
    }

    m_build = TOSVersion::Build;
    m_major = TOSVersion::Major;
    m_minor = TOSVersion::Minor;
    m_spMajor = TOSVersion::ServicePackMajor;
    m_spMinor = TOSVersion::ServicePackMinor;

    m_str = TOSVersion::ToString().c_str();
#else
# error Not implemented yet!
#endif
  }

public:
  // Access info instance singleton
  static const EsAndroidOsInfo& inst()
  {
    static const EsAndroidOsInfo s_osInfo;

    return s_osInfo;
  }

public:
  EsOsPlatfrom m_platform;
  EsOsArchitecture m_arch;
  ulong m_build;
  ulong m_major;
  ulong m_minor;
  ulong m_spMajor;
  ulong m_spMinor;

  EsString m_name;
  EsString m_spName;
  EsString m_str;
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

const EsString& EsOsInfo::asString()
{
  return EsAndroidOsInfo::inst().m_str;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::nameGet()
{
  return EsAndroidOsInfo::inst().m_name;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::spNameGet()
{
  return EsAndroidOsInfo::inst().m_spName;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::architectureGet()
{
  return static_cast<ulong>(EsAndroidOsInfo::inst().m_arch);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::platformGet()
{
  return static_cast<ulong>(EsAndroidOsInfo::inst().m_platform);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::buildGet()
{
  return EsAndroidOsInfo::inst().m_build;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::majorGet()
{
  return EsAndroidOsInfo::inst().m_major;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::minorGet()
{
  return EsAndroidOsInfo::inst().m_minor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMajorGet()
{
  return EsAndroidOsInfo::inst().m_spMajor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMinorGet()
{
  return EsAndroidOsInfo::inst().m_spMinor;
}
//---------------------------------------------------------------------------


