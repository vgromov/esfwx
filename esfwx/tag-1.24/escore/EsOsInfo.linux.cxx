//---------------------------------------------------------------------------
#include <sys/utsname.h>

class EsLinuxOsInfo
{
private:
  EsLinuxOsInfo() :
  m_platform(EsOsPlatfrom::Linux),
  m_arch(EsOsArchitecture::Unknown),
  m_build(0),
  m_major(0),
  m_minor(0),
  m_spMajor(0),
  m_spMinor(0),
  m_name(esT("Linux"))
  {
    utsname info;
    if( 0 != uname(&info) )
      return;

    const EsString& name = EsString::fromUtf8(info.sysname);
    if( !name.empty() )
      m_name = name;

    m_spName = EsString::fromUtf8(info.version);

    const EsString& release = EsString::fromUtf8(info.release);
    if( !release.empty() )
    {
      EsRegEx re( esT("([0-9]+)\\.([0-9]+)(?:\\.([0-9]+))?") );

      re.set_text(release);
      if( re.get_matches() )
      {
        const EsString& majstr = re.matchGet(1);
        if( !majstr.empty() )
          m_major = EsString::toULong(majstr, 10);

        const EsString& minstr = re.matchGet(2);
        if( !minstr.empty() )
          m_minor = EsString::toULong(minstr, 10);

        const EsString& buildstr = re.matchGet(3);
        if( !buildstr.empty() )
          m_build = EsString::toULong(buildstr, 10);
      }
    }

    const EsString& machine = EsString::fromUtf8(info.machine);
    if( !machine.empty() )
    {
      if( esT("x86_64") == machine )
        m_arch = EsOsArchitecture::IntelAMDx64;
      // TODO: add other arch identification else if( esT("") )
    }

    m_str = release;
    if( !m_str.empty() )
      m_str += esT(" ");

    m_str += m_spName;
  }

public:
  // Access info instance singleton
  static const EsLinuxOsInfo& inst()
  {
    static const EsLinuxOsInfo s_osInfo;

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
  return EsLinuxOsInfo::inst().m_str;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::nameGet()
{
  return EsLinuxOsInfo::inst().m_name;
}
//---------------------------------------------------------------------------

const EsString& EsOsInfo::spNameGet()
{
  return EsLinuxOsInfo::inst().m_spName;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::architectureGet()
{
  return static_cast<ulong>(EsLinuxOsInfo::inst().m_arch);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::platformGet()
{
  return static_cast<ulong>(EsLinuxOsInfo::inst().m_platform);
}
//---------------------------------------------------------------------------

ulong EsOsInfo::buildGet()
{
  return EsLinuxOsInfo::inst().m_build;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::majorGet()
{
  return EsLinuxOsInfo::inst().m_major;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::minorGet()
{
  return EsLinuxOsInfo::inst().m_minor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMajorGet()
{
  return EsLinuxOsInfo::inst().m_spMajor;
}
//---------------------------------------------------------------------------

ulong EsOsInfo::spMinorGet()
{
  return EsLinuxOsInfo::inst().m_spMinor;
}
//---------------------------------------------------------------------------


