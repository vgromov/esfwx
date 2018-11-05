class EsWinOsInfo
{
private:
  EsWinOsInfo() :
  m_platform(EsOsPlatfrom::Windows),
  m_arch(EsOsArchitecture::Unknown),
  m_build(0),
  m_major(0),
  m_minor(0),
  m_spMajor(0),
  m_spMinor(0),
  m_name(esT("Windows"))
  {
    SYSTEM_INFO si;
    memset(&si, 0, sizeof(SYSTEM_INFO));

    GetNativeSystemInfo(&si);
    switch( si.wProcessorArchitecture )
    {
    case PROCESSOR_ARCHITECTURE_AMD64:
      m_arch = EsOsArchitecture::IntelAMDx64;
      break;
    case PROCESSOR_ARCHITECTURE_ARM:
      m_arch = EsOsArchitecture::ARM32;
      m_name = esT("Windows RT");
      break;
    case PROCESSOR_ARCHITECTURE_IA64:
      m_arch = EsOsArchitecture::IAx64;
      break;
    case PROCESSOR_ARCHITECTURE_INTEL:
      m_arch = EsOsArchitecture::IntelX86;
      break;
    }

    OSVERSIONINFOEX osvi;
    memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if( GetVersionEx( reinterpret_cast<LPOSVERSIONINFO>(&osvi) ) )
    {
      m_major = osvi.dwMajorVersion;
      m_minor = osvi.dwMinorVersion;
      m_build = osvi.dwBuildNumber;
      m_spMajor = osvi.wServicePackMajor;
      m_spMinor = osvi.wServicePackMinor;
      m_spName = osvi.szCSDVersion;
    }

    m_str = EsString::format(
      esT("%s %u.%u.%u"),
      m_name,
      m_major,
      m_minor,
      m_build
    );

    if( !m_spName.empty() )
      m_str += esT(" ") + m_spName;
  }

public:
  // Access info instance singleton
  static const EsWinOsInfo& inst()
  {
    static const EsWinOsInfo s_osInfo;

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
  return EsWinOsInfo::inst().m_str;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::nameGet()
{
  return EsWinOsInfo::inst().m_name;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::spNameGet()
{
  return EsWinOsInfo::inst().m_spName;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::architectureGet()
{
  return static_cast<ulong>(EsWinOsInfo::inst().m_arch);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::platformGet()
{
  return static_cast<ulong>(EsWinOsInfo::inst().m_platform);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::buildGet()
{
  return EsWinOsInfo::inst().m_build;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::majorGet()
{
  return EsWinOsInfo::inst().m_major;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::minorGet()
{
  return EsWinOsInfo::inst().m_minor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMajorGet()
{
  return EsWinOsInfo::inst().m_spMajor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMinorGet()
{
  return EsWinOsInfo::inst().m_spMinor;
}
//---------------------------------------------------------------------------

