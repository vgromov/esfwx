//---------------------------------------------------------------------------
#include <IOKit/IOKitKeys.h>
#include <sys/sysctl.h>

class EsMacOsInfo
{
private:
  EsMacOsInfo() :
  m_platform(EsOsPlatfrom::MacOS),
  m_arch(EsOsArchitecture::Unknown),
  m_build(0),
  m_major(0),
  m_minor(0),
  m_spMajor(0),
  m_spMinor(0),
  m_name(esT("Mac OS"))
  {
    const EsString& osrel = EsUtilities::sysctlStrGet(CTL_KERN, KERN_OSRELEASE);
    EsStringTokenizer tok(osrel, esT("."));

    if( tok.get_moreTokens() )
    {
      m_major = 10;
      m_minor = EsString::toULong(tok.get_nextToken(), 10);
      if( m_minor >= 4 )
        m_minor -= 4;
    }

    if( tok.get_moreTokens() )
      m_spMajor = EsString::toULong(tok.get_nextToken(), 10);

    if( tok.get_moreTokens() )
      m_spMinor = EsString::toULong(tok.get_nextToken(), 10);

    const EsString& ver = EsUtilities::sysctlStrGet(CTL_KERN, KERN_VERSION);
    if( EsString::npos != ver.find(esT("X86_64")) ||
      EsString::npos != ver.find(esT("x86_64"))
    )
      m_arch = EsOsArchitecture::IntelAMDx64;
    else if( EsString::npos != ver.find(esT("I386")) ||
      EsString::npos != ver.find(esT("i386"))
    )
      m_arch = EsOsArchitecture::IntelX86;

    io_registry_entry_t rootReg = IORegistryGetRootEntry(kIOMasterPortDefault);
    if( rootReg )
    {
      const EsVariant& build = EsUtilities::macosSvcRegValGet(rootReg, CFSTR(kOSBuildVersionKey));
      IOObjectRelease( rootReg );

      if( !build.isEmpty() )
        m_build = EsString::toULong(
          build.asString(),
          16
        );
    }

    m_str = EsString::format(
      esT("%s %u.%u.%u"),
      m_name,
      m_major,
      m_minor,
      m_spMajor
    );

    if( 0 != m_build )
      m_str += EsString::format(esT(" build: %X"), m_build);
  }

public:
  // Access info instance singleton
  static const EsMacOsInfo& inst()
  {
    static const EsMacOsInfo s_osInfo;

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
  return EsMacOsInfo::inst().m_str;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::nameGet()
{
  return EsMacOsInfo::inst().m_name;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::spNameGet()
{
  return EsMacOsInfo::inst().m_spName;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::architectureGet()
{
  return static_cast<ulong>(EsMacOsInfo::inst().m_arch);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::platformGet()
{
  return static_cast<ulong>(EsMacOsInfo::inst().m_platform);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::buildGet()
{
  return EsMacOsInfo::inst().m_build;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::majorGet()
{
  return EsMacOsInfo::inst().m_major;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::minorGet()
{
  return EsMacOsInfo::inst().m_minor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMajorGet()
{
  return EsMacOsInfo::inst().m_spMajor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMinorGet()
{
  return EsMacOsInfo::inst().m_spMinor;
}
//---------------------------------------------------------------------------

