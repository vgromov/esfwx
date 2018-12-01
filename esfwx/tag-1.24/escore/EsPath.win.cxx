long EsPath::isExisting(const EsString& obj)
{
  long result = 0;

  esU32 dw = ::GetFileAttributes(obj.c_str());
  if( INVALID_FILE_ATTRIBUTES == dw )
  {
    ulong error = EsUtilities::osErrorCodeGet();
    // handle non expected errors, like not enough ACL to access file|folder, etc
    if(  ERROR_FILE_NOT_FOUND != error &&
        ERROR_PATH_NOT_FOUND != error )
      EsException::ThrowOsError(error);
  }
  else
    result = (dw & FILE_ATTRIBUTE_DIRECTORY) ? 2 : 1;

  return result;
}
//---------------------------------------------------------------------------

EsString EsPath::cwdGet()
{
  EsString result;

  result.resize(MAX_PATH);
  esU32 cnt = ::GetCurrentDirectory(
    static_cast<DWORD>(result.size()-1), 
    const_cast<EsString::pointer>(result.c_str())
  );
  if( 0 < cnt )
    result.resize(cnt);
  else
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );

  return result;
}
//---------------------------------------------------------------------------

// set current working directory
void EsPath::cwdSet(const EsString& cwd)
{
  if( !SetCurrentDirectory(cwd.c_str()) )
    EsException::ThrowOsError( EsUtilities::osErrorCodeGet() );
}
//---------------------------------------------------------------------------

void EsPath::directoryCreate(const EsString& path)
{
  if( !::CreateDirectory(path.c_str(), 0) )
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

void EsPath::fileRemove(const EsString& path)
{
  if( !::DeleteFile(path.c_str()) )
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

void EsPath::dirRemove(const EsString& path)
{
  if( !::RemoveDirectory(path.c_str()) )
    EsException::ThrowOsError(EsUtilities::osErrorCodeGet());
}
//---------------------------------------------------------------------------

#include <shlobj.h>

static EsString stdPathGet( int pathId )
{
  EsString::value_type path[MAX_PATH];
  path[0] = 0;

  ::SetLastError(ERROR_SUCCESS);

  HRESULT hresult = ::SHGetFolderPath(0, pathId, 0, 0, path);
  if( S_OK != hresult &&
      S_FALSE != hresult
  )
    EsException::ThrowOsError( HRESULT_CODE(hresult) );

  return path;
}
//---------------------------------------------------------------------------

EsString EsPath::stdAppPathGet()
{
  EsString::value_type buff[MAX_PATH];
  DWORD len = GetModuleFileName(0, buff, MAX_PATH);

  return EsString(buff, len);
}
//---------------------------------------------------------------------------

EsString EsPath::stdAppDataGet()
{
  return stdPathGet(CSIDL_APPDATA);
}
//---------------------------------------------------------------------------

EsString EsPath::stdLocalAppDataGet()
{
  return stdPathGet(CSIDL_LOCAL_APPDATA);
}
//---------------------------------------------------------------------------

EsString EsPath::stdHomeGet()
{
  return stdPathGet(CSIDL_APPDATA);
}
//---------------------------------------------------------------------------

EsString EsPath::stdDocsGet()
{
  return stdPathGet(CSIDL_PERSONAL);
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDocsGet()
{
  return stdPathGet(CSIDL_COMMON_DOCUMENTS);
}
//---------------------------------------------------------------------------

EsString EsPath::stdDownloadsGet()
{
  return stdPathGet(CSIDL_LOCAL_APPDATA);
}
//---------------------------------------------------------------------------

EsString EsPath::stdPublicDownloadsGet()
{
  return stdPathGet(CSIDL_COMMON_APPDATA);
}
//---------------------------------------------------------------------------

EsString EsPath::stdSysRootGet()
{
  const EsStringIndexedMap& env = EsUtilities::environmentVarsListGet();
  return env.valueGet(esT("SystemRoot")).asString();
}
//---------------------------------------------------------------------------

EsStringArray EsPath::stdDriveNamesGet()
{
  EsStringArray names;

  DWORD result = GetLogicalDriveStrings(0, NULL);
  if( 0 < result )
  {
    std::vector<TCHAR> buff(result);
    result = GetLogicalDriveStrings(
      static_cast<DWORD>(buff.size()), 
      &buff[0]
    );

    if( 0 < result )
    {
      const TCHAR* pname = &buff[0];
      while( *pname )
      {
        names.push_back( pname );
        // get the next drive
        pname += esStrlen(pname) + 1;
      }
    }
    else
      EsException::ThrowOsError(
          EsUtilities::osErrorCodeGet()
        );
  }
  else
    EsException::ThrowOsError(
        EsUtilities::osErrorCodeGet()
      );

  return names;
}
//---------------------------------------------------------------------------

EsStringArray EsPath::stdDataRootsGet()
{
  return stdDriveNamesGet();
}
//---------------------------------------------------------------------------


