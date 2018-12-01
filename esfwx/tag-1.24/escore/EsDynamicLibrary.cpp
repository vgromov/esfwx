#include "escorepch.h"
#pragma hdrstop

#include "EsDynamicLibrary.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif

#ifdef EsDynamicLibrary_SUPPORTED
//---------------------------------------------------------------------------

EsDynamicLibrary::EsDynamicLibrary(EsDynamicLibraryHandle handle, const EsString& path) :
m_handle(handle),
m_path(path)
{
  ES_ASSERT(handle);
  m_version = versionExtract(path);

  libsGet().itemAdd(
    path,
    m_version,
    false
  );
}
//---------------------------------------------------------------------------

EsDynamicLibrary::~EsDynamicLibrary()
{
  unload();
}
//---------------------------------------------------------------------------

// library info (version) indexed by its full path
EsStringIndexedMap& EsDynamicLibrary::libsGet()
{
  static EsStringIndexedMap s_libs;

  return s_libs;
}
//---------------------------------------------------------------------------

// return information of all libraries, loaded by the current process
const EsStringIndexedMap& EsDynamicLibrary::loadedLibrariesGet()
{
  return libsGet();
}
//---------------------------------------------------------------------------

void EsDynamicLibrary::checkLoaded() const
{
  if( !isOk() )
    EsException::Throw(esT("Dynamic library is not loaded"));
}
//---------------------------------------------------------------------------

// try to acquire procedure address by its name
// if address is not found, and doThrow is true (default)
// an exception is thrown
EsDynamicLibrary::Pfn EsDynamicLibrary::procAddrGet(const EsString& name, bool doThrow /*= true*/)
{
  return (EsDynamicLibrary::Pfn)symbolAddrGet(name, doThrow);
}
//---------------------------------------------------------------------------

#if ES_OS == ES_OS_WINDOWS
# include "EsDynamicLibrary.win.cxx"
#elif defined(ES_POSIX_COMPAT)
# include "EsDynamicLibrary.posix.cxx"
#endif

#endif // EsDynamicLibrary_SUPPORTED

