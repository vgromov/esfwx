#include <dlfcn.h>
//---------------------------------------------------------------------------

// internal service - extract library version info
EsString EsDynamicLibrary::versionExtract(const EsString& path)
{
  EsString result;

  return result;
}
//---------------------------------------------------------------------------

void EsDynamicLibrary::unload()
{
  if(m_handle)
  {
    dlclose(m_handle);
    m_handle = 0;

    ulong idx = libsGet().itemFind(m_path);
    if( EsStringIndexedMap::npos != idx )
      libsGet().itemDelete(idx);
  }
}
//---------------------------------------------------------------------------

EsDynamicLibrary::Ptr EsDynamicLibrary::load(const EsString& path, bool doThrow /*= true*/)
{
  EsDynamicLibraryHandle hlib = 0;
  hlib = dlopen( EsString::toUtf8(path).c_str(), RTLD_LAZY);
  if( hlib )
    return EsDynamicLibrary::Ptr(new EsDynamicLibrary(hlib, path));
  else if( doThrow )
    EsException::Throw(
      _("Could not load library '%s'; reason: '%s'"),
      path,
      EsString::fromUtf8( dlerror() )
    );

  return EsDynamicLibrary::Ptr();
}
//---------------------------------------------------------------------------

void* EsDynamicLibrary::symbolAddrGet(const EsString& name, bool doThrow /*= true*/)
{
  checkLoaded();
  void* addr = dlsym(m_handle, EsString::toUtf8(name).c_str());
  if( !addr && doThrow )
  {
    EsException::Throw(
      _("Could not find symbol: '%s', in '%s'; reason: '%s'"),
      name,
      m_path,
      EsString::fromUtf8( dlerror() )
    );
  }

  return (void*)addr;
}
//---------------------------------------------------------------------------

EsDynamicLibrary::Pfn EsDynamicLibrary::procAddrGetAorW(const EsString& name, bool doThrow /*= true*/)
{
  return procAddrGet(
    name,
    doThrow
  );
}
//---------------------------------------------------------------------------
