
// internal service - extract library version info
EsString EsDynamicLibrary::versionExtract(const EsString& path)
{
	EsString result;

	esU32 size = ::GetFileVersionInfoSize(path.c_str(), 0);
	if( size )
	{
		EsBinBuffer buff(size);
		if( ::GetFileVersionInfo(path.c_str(), 0, size, &buff[0]) )
		{
			UINT len;
			LPVOID val;
			// find out translation used in library resource
      if( ::VerQueryValue(&buff[0], esT("\\VarFileInfo\\Translation"), &val, &len) && val && len )
			{
				const EsString& translation = EsString::format(esT("%04X%04X"), *(esU16*)val, *((esU16*)val + 1));
				EsString qry = esT("\\StringFileInfo\\") + translation + esT("\\FileDescription");
				if( ::VerQueryValue(&buff[0], qry.c_str(), &val, &len) && val && len )
					result.append((EsString::const_pointer)val, len);
				if( ::VerQueryValue(&buff[0], esT("\\"), &val, &len) && val && sizeof(VS_FIXEDFILEINFO) == len )
				{
					VS_FIXEDFILEINFO* fi = (VS_FIXEDFILEINFO*)val;
					const EsString& ver = EsString::format(esT("%d.%d.%d.%d"),
						HIWORD(fi->dwFileVersionMS), LOWORD(fi->dwFileVersionMS),
						HIWORD(fi->dwFileVersionLS), LOWORD(fi->dwFileVersionLS));
					if( !result.empty() )
						result += esT(" ");
					result += ver;
					if( fi->dwFileFlags & VS_FF_DEBUG )
						result += esT(" DEBUG");
				}
			}
		}
	}

	return result;
}
//---------------------------------------------------------------------------

void EsDynamicLibrary::unload()
{
	if(!m_handle)
    return;

  ::FreeLibrary(m_handle);
  m_handle = 0;

  size_t idx = libsGet().itemFind(m_path);
  if( EsStringIndexedMap::npos != idx )
    libsGet().itemDelete(idx);
}
//---------------------------------------------------------------------------

EsDynamicLibrary::Ptr EsDynamicLibrary::load(const EsString& path, bool doThrow /*= true*/)
{
	EsDynamicLibraryHandle hlib = 0;
	hlib = ::LoadLibrary(path.c_str());
	if( hlib )
		return EsDynamicLibrary::Ptr(
      new EsDynamicLibrary(
        hlib,
        path
      )
    );
	else if( doThrow )
		EsException::Throw(
      _("Could not load library '%s'; reason: '%s'"),
      path,
      EsUtilities::osErrorStringGet(
        EsUtilities::osErrorCodeGet()
      )
    );

	return nullptr;
}
//---------------------------------------------------------------------------

// try to acquire exported symbol address
void* EsDynamicLibrary::symbolAddrGet(const EsString& name, bool doThrow /*= true*/)
{
	checkLoaded();
	FARPROC addr = ::GetProcAddress(m_handle, EsString::toAscii(name).c_str());
	if( !addr && doThrow )
  {
		EsException::Throw(
      _("Could not find symbol: '%s', in '%s'; reason: '%s'"),
      name,
      m_path,
      EsUtilities::osErrorStringGet(
        EsUtilities::osErrorCodeGet()
      )
    );
  }

	return (void*)addr;
}
//---------------------------------------------------------------------------

EsDynamicLibrary::Pfn EsDynamicLibrary::procAddrGetAorW(const EsString& name, bool doThrow /*= true*/)
{
	return procAddrGet(name+
#ifdef ES_UNICODE
		esT('W')
#else
		esT('A')
#endif
		, doThrow);
}
//---------------------------------------------------------------------------